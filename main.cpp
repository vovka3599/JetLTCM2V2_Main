#include <iostream>
#include <time.h>
#include <thread>

#include "can_h/can.h"
#include "can_h/scan.h"
#include "can_h/control.h"
#include "JetLTCM.h"
#include "config.h"

#if UDP_SEND
#include "udp.h"
#endif

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

void work()
{
    int rc;
    IQ *rb;
    IQ_one_ch *send_buf = new(std::nothrow) IQ_one_ch[DEFAULT_SIZE];
    if (send_buf == nullptr)
    {
        printf("Memory allocation error\n");
        exit(0);
    }

#if UDP_SEND
	int udp_socket;
    rc = сreate_udp_socket(&udp_socket);
    if(rc < 0)
    {
        printf("Create udp socket fail\n");
        exit(0);
    }
#endif
    
    JetLTCM *jet = new(std::nothrow) JetLTCM(DEVICE_C2H, DEVICE_USER, DMA_BUF_COUNT, DMA_BUF_SIZE);
    if (jet == nullptr)
    {
        printf("Memory allocation error\n");
        exit(0);
    }
    rc = jet->Init();
    if(rc < 0)
    {
        printf("Init JetLTCM fail\n");
        exit(0);
    }
    rc = jet->SetParam(SAMP_FREQ, REAL_DATE, CONST_VALUE, DDS_FREQ);
    if(rc < 0)
    {
        printf("Set parameter JetLTCM fail\n");
        exit(0);
    }

    std::thread t(wait_exit);
    t.detach();

#if FILE_SAVE
    std::ofstream file("test.complex", std::ios::app | std::ios::binary);
#endif

    printf("Start receive data\n");
    while(exit_thread == 0)
	{
        rb = jet->GetData();
        if (rb)
        {
            for(int i = 0; i < DEFAULT_SIZE; i++)
            {
                send_buf[i].I = rb[i].I_1;
                send_buf[i].Q = rb[i].Q_1;
            }

            // User code

            // User code

#if FILE_SAVE
            file.write((char*)send_buf, DEFAULT_SIZE*sizeof(IQ_one_ch));
#endif

#if UDP_SEND
            ssize_t ret = send(udp_socket, send_buf, FFT_SIZE_DEFAULT, 0);
            if(ret != FFT_SIZE_DEFAULT) 
            {
                printf("Error send data to UDP. Expected %d, send %d\n", (int)FFT_SIZE_DEFAULT, (int)ret);
                if(ret < 0)
                    printf("Error send UDP: %s\n", strerror(errno));
            }
            usleep(3000);
#endif
        }
	}
#if FILE_SAVE
    file.close();
#endif

    delete[] send_buf;
    delete jet;
    printf("Exit from work loop\n");
}

int main()
{
	// Can bus address
    uint8_t haddr = 126;
    Node can("can0");

    // Receiver address by name
   	ScanAdapter scan(can, haddr, "RCV433");
    uint8_t daddr = scan.GetValue();

    // Array of commands
    ControlAdapters controls(can, haddr, daddr);
    controls.Insert(Control::FREQ);
    controls.Insert(Control::ATT);

    // Setting the frequency and attenuation
    controls.SetValue<uint32_t>(Control::FREQ, 868);    // in МГц
    controls.SetValue<uint16_t>(Control::ATT,  0);      // step = 20. value = step*0.1

    printf("Freq [MHz]: %u\n", controls.GetValue<uint32_t>(Control::FREQ));
    printf("Att  [dB]: %f\n", 0.1 * controls.GetValue<uint16_t>(Control::ATT));

    std::thread t(work);
    t.join();

	return 0;
}