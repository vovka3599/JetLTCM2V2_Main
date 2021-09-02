#include <iostream>

#define _BSD_SOURCE
#include <assert.h>
#include <fcntl.h>
#include <getopt.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <fstream>

#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <math.h>
#include <thread>

#include <ctype.h>
#include <memory.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <sys/ioctl.h>
#include <sys/mman.h>

#include "can_h/can.h"
#include "can_h/scan.h"
#include "can_h/control.h"
#include "hfr4dma-ctrl.h"
#include "udp.h"

#define DEFAULT_SIZE        2048
#define DEVICE_C2H          "/dev/hfrdma0_c2h_0"
#define DEVICE_USER         "/dev/hfrdma0_user"

#define MAP_SIZE (32*1024U)

static const int DMA_BUF_SIZE = 2048*16;    // Размер DMA буфера, выделяемого в драйвере
static const int DMA_BUF_COUNT = 4;         // Количество DMA буферов.

typedef struct{
	int I_1;
	int Q_1;

	int I_2;
	int Q_2;
}IQ;

typedef struct{
	float I;
	float Q;
}IQ_one_ch;

struct Registers{
    struct Control
    {
        uint32_t reset : 1;
        uint32_t samp_freq : 2;
        uint32_t : 29;
    };

    volatile uint32_t real_date;
    volatile uint32_t dds_phase;
    volatile Control control;
};

int fpga_fd;

int exit_thread = 0;

void wait_exit()
{
    getchar();
    exit_thread = 1;
}

int main()
{
	printf("PCI Driver get data. %lu\n", sizeof(Registers));
	IQ_one_ch *send_buf = new IQ_one_ch[DEFAULT_SIZE];

	int udp_socket;
    Create_udp_socket(&udp_socket);

	// Адрес на шине CAN хоста.
    uint8_t haddr = 126;
    Node can("can0");

    // Ищем адрес приёмника по имени.
   	ScanAdapter scan(can, haddr, "RCV433");
    uint8_t daddr = scan.GetValue();

    // Формируем массив команд.
    ControlAdapters controls(can, haddr, daddr);
    controls.Insert(Control::FREQ);
    controls.Insert(Control::ATT);

    // Устанвливаем значение частоты и аттенюации
    controls.SetValue<uint32_t>(Control::FREQ, 868); // 433 МГц
    controls.SetValue<uint16_t>(Control::ATT,  0); // step = 20. value = *0.1

    std::cout << "Freq [MHz]: "<< controls.GetValue<uint32_t>(Control::FREQ) << std::endl;
    std::cout << "Att  [dB]: " << 0.1 * controls.GetValue<uint16_t>(Control::ATT) << std::endl;

    int rc;
    int i;
    struct hfr4_buffer_params dma_buf_params;
    struct hfr4_transaction dma_buf;
    void* dma_mem_array[DMA_BUF_COUNT];

    // Открываем файл устройства, созданый драйвером платы HFR4
    int fpga_fd = open(DEVICE_C2H, O_RDWR | O_NONBLOCK);
    if(fpga_fd < 0) {
        printf(" Error open hfr4 device. \n");
        exit(0);
    }
    printf("Opened hfr4 device: %s \n", DEVICE_C2H);

    dma_buf_params.buf_size = DMA_BUF_SIZE;
    dma_buf_params.buf_count = DMA_BUF_COUNT;

    rc = ioctl(fpga_fd, IOCTL_HFR4_SET_BUFFERS, &dma_buf_params);
    if(rc != 0) {
        printf(" Error send ioctl IOCTL_NUTDMA_SET_BUFFERS. buffer size: %u, buffer count: %u \n",
               dma_buf_params.buf_size, dma_buf_params.buf_count);
        exit(0);
    }
    printf("Set buffer size: %u, buffer count: %u \n", dma_buf_params.buf_size, dma_buf_params.buf_count);

    memset(&dma_buf_params, 0, sizeof(dma_buf_params));
    rc = ioctl(fpga_fd, IOCTL_HFR4_GET_BUFFERS, &dma_buf_params);
    if(rc != 0){
        printf(" Error send ioctl IOCTL_NUTDMA_GET_BUFFERS. \n");
        exit(0);
    }
    printf("--- Got new buffer params: dma_buffer_size=%d, dma_buffer_count=%d ---\n", dma_buf_params.buf_size, dma_buf_params.buf_count);

    for(i = 0; i < DMA_BUF_COUNT; i++) {
        dma_mem_array[i] = mmap(0, DMA_BUF_SIZE, PROT_READ, MAP_SHARED, fpga_fd, i*DMA_BUF_SIZE);
        assert(dma_mem_array[i] != MAP_FAILED);
    }
    printf("--- Mapped %d buffers %u size  ---\n", i, DMA_BUF_SIZE);

    int user_fd = open(DEVICE_USER, O_RDWR | O_NONBLOCK);
    if(user_fd < 0) {
        printf(" Error open user hfr4 device. \n");
        exit(0);
    }

    struct Registers *reg;
	void* status_reg;
    status_reg = mmap(NULL, 32*1024U, PROT_READ | PROT_WRITE, MAP_SHARED, user_fd, 0);
    reg = (struct Registers*)status_reg;

    reg->real_date = 0x80000FFF;
    reg->dds_phase = 0x18000000; // 0x6D3A0 0x70FF000
    reg->control.samp_freq = 1; // 0 - 1 MHz, 1 - 500 kHz, 2 - 250 kHz, 3 - 125 kHz
    reg->control.reset = 0;

    rc = ioctl(fpga_fd, IOCTL_HFR4_DMA_RESET);
    assert(rc == 0);

    std::thread t(wait_exit);
    t.detach();

    rc = ioctl(fpga_fd, IOCTL_HFR4_DMA_START, 1);
    assert(rc == 0);

    IQ *rb;
    reg->control.reset = 1;
    //std::ofstream only("test_1MHz.complex", std::ios::app | std::ios::binary);
    while(exit_thread == 0)
	{
        rc = ioctl(fpga_fd, IOCTL_HFR4_DMA_READ, &dma_buf);
        if(rc < 0) {
            printf(" error read dma data \n");
            continue;
        }

        rb = (IQ*)dma_mem_array[dma_buf.tail];

        for(int i = 0; i < 2048; i++)
        {
            send_buf[i].I = rb[i].I_1;
            send_buf[i].Q = rb[i].Q_1;
        }

        
        //only.write((char*)send_buf, 2048*sizeof(IQ_one_ch));
        
        
        for(int j = 0; j < 1; j++)
        {
            ssize_t ret = send(udp_socket, send_buf+j*2048, FFT_SIZE_DEFAULT, 0);
            if(ret != FFT_SIZE_DEFAULT) 
            {
                printf("Error send data to UDP. Expected %d, send %d \n", (int)FFT_SIZE_DEFAULT, (int)ret);
                if(ret < 0)
                printf("error send UDP: %s \n", strerror(errno));
            }
            usleep(3000);
        }
	}

    rc = ioctl(fpga_fd, IOCTL_HFR4_DMA_START, 0);
    assert(rc == 0);
    reg->control.reset = 0;

    //only.close();

    delete send_buf;
    close(fpga_fd);
    printf(" Exit from sending udp data loop \n");

	return 0;
}