#include <iostream>
#include <time.h>
#include <thread>
#include <csignal>

#include "can_h/can.h"
#include "can_h/scan.h"
#include "can_h/control.h"
#include "JetLTCM.h"
#include "config.h"

#if TEST_CUDA
#include "cuda_f.h"
#endif

#if UDP_SEND
#include "udp.h"
#endif

#if FILE_SAVE
#include <fstream>
#endif

static volatile std::sig_atomic_t exit_thread;
void sigintHandler(int sig)
{
    exit_thread = 1;
}

void work(char *IP_addr, int samp_freq, int channel)
{
    std::signal(SIGINT, sigintHandler);
    int rc;
    IQ_DEV<int> *rb;
    IQ<float> *channel_one = new(std::nothrow) IQ<float>[DEFAULT_SIZE];
    if (channel_one == nullptr)
    {
        printf("Memory allocation error\n");
        exit(0);
    }

    IQ<float> *channel_two = new(std::nothrow) IQ<float>[DEFAULT_SIZE];
    if (channel_two == nullptr)
    {
        printf("Memory allocation error\n");
        exit(0);
    }

#if UDP_SEND
	int udp_socket;
    rc = create_udp_socket(&udp_socket, IP_addr);
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
    rc = jet->SetParam(samp_freq, REAL_DATE, CONST_VALUE, DDS_FREQ);
    if(rc < 0)
    {
        printf("Set parameter JetLTCM fail\n");
        exit(0);
    }

#if FILE_SAVE
    std::ofstream ch_1("jetltcm2v2_s_1.complex", std::ios::app | std::ios::binary);
    std::ofstream ch_2("jetltcm2v2_s_2.complex", std::ios::app | std::ios::binary);
#endif

#if TIME_ON
    uint64_t start, end;
    std::time_t result1;
#endif

#if TEST_CUDA
    test();
#endif

    printf("Start receive data\n");
    while(exit_thread == 0)
	{
#if TIME_ON
        start = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
#endif
        rb = jet->GetData();
        if (rb)
        {
            for(int i = 0; i < DEFAULT_SIZE; i++)
            {
                channel_one[i].I = rb[i].I_1;
                channel_one[i].Q = rb[i].Q_1;

                channel_two[i].I = rb[i].I_2;
                channel_two[i].Q = rb[i].Q_2;
            }

            // User code

            // User code

#if FILE_SAVE
            ch_1.write((char*)channel_one, DEFAULT_SIZE*sizeof(IQ<float>));
            ch_2.write((char*)channel_two, DEFAULT_SIZE*sizeof(IQ<float>));
#endif

#if UDP_SEND
            ssize_t ret;
            if (channel == 1)
                ret = send(udp_socket, channel_one, FFT_SIZE_DEFAULT, 0);
            else
                ret = send(udp_socket, channel_two, FFT_SIZE_DEFAULT, 0);
            if(ret != FFT_SIZE_DEFAULT) 
            {
                printf("Error send data to UDP. Expected %d, send %d\n", (int)FFT_SIZE_DEFAULT, (int)ret);
                if(ret < 0)
                    printf("Error send UDP: %s\n", strerror(errno));
            }
            usleep(2700);
#endif
        }
#if TIME_ON
        end = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        result1 = end - start;
        printf("time = %d\n", (int)result1);
#endif
	}
#if FILE_SAVE
    ch_1.close();
    ch_2.close();
#endif

    delete[] channel_one;
    delete[] channel_two;
    delete jet;
    printf("Exit from work loop\n");
}

int main(int argc, char* argv[])
{
    if (argc != 4) 
    {
       printf("usage: ./JetLTCM2V2_Main IP_addr_to_send_udp samp_freq channel\n");
       printf("\tsamp_freq:\n");
       printf("\t\tSAMP_FREQ_10_MHz     = 0\n");
       printf("\t\tSAMP_FREQ_1_MHz      = 1\n");
       printf("\t\tSAMP_FREQ_500_kHz    = 2\n");
       printf("\t\tSAMP_FREQ_250_kHz    = 3\n");
       printf("\t\tSAMP_FREQ_125_kHz    = 4\n");
       printf("\t\tSAMP_FREQ_62_5_kHz   = 5\n");
       printf("example: ./JetLTCM2V2_Main 192.168.1.240 1 1\n");
       return 0;
    }

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
    controls.SetValue<uint32_t>(Control::FREQ, 868);    // МГц
    controls.SetValue<uint16_t>(Control::ATT,  0);      // step = 20. value = step*0.1

    printf("Freq [MHz]: %u\n", controls.GetValue<uint32_t>(Control::FREQ));
    printf("Att  [dB]: %f\n", 0.1 * controls.GetValue<uint16_t>(Control::ATT));

    std::thread t(work, argv[1], atoi(argv[2]), atoi(argv[3]));
    t.join();

	return 0;
}