#include <iostream>
#include <time.h>
#include <thread>

#include "can_h/can.h"
#include "can_h/scan.h"
#include "can_h/control.h"
#include "udp.h"

#include "JetLTCM.h"
#include "config.h"

#if FILE_SAVE
#include <fstream>
#endif

typedef struct{
	float I;
	float Q;
}IQ_one_ch;

int exit_thread = 0;

void wait_exit()
{
    getchar();
    exit_thread = 1;
}

int main()
{
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

    std::thread t(wait_exit);
    t.detach();

    JetLTCM *jet = new JetLTCM(DEVICE_C2H, DEVICE_USER, DMA_BUF_COUNT, DMA_BUF_SIZE);
    jet->Init();
    jet->SetParam(SAMP_FREQ_250_kHz, true, 0x0, 0x18000000);

#if FILE_SAVE
    std::ofstream only("test_1MHz.complex", std::ios::app | std::ios::binary);
#endif

    IQ *rb;
    while(exit_thread == 0)
	{
        rb = jet->GetData();
        for(int i = 0; i < 2048; i++)
        {
            send_buf[i].I = rb[i].I_1;
            send_buf[i].Q = rb[i].Q_1;
        }

#if FILE_SAVE
        only.write((char*)send_buf, 2048*sizeof(IQ_one_ch));
#endif

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

#if FILE_SAVE
    only.close();
#endif

    delete[] send_buf;
    delete jet;
    printf(" Exit from sending udp data loop \n");

	return 0;
}