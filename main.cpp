#include <iostream>
#include <time.h>
#include <thread>
#include <csignal>

#include "can_h/can.h"
#include "can_h/scan.h"
#include "can_h/control.h"

#include "JetLTCM.h"
#include "config.h"
#include "command_arguments.h"

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

void work(struct command_arguments *ca)
{
    std::signal(SIGINT, sigintHandler);
    int rc;
    JETDEV::IQ_DEV<int> *rb;
    JETDEV::IQ<float> *channel_one = new(std::nothrow) JETDEV::IQ<float>[DEFAULT_SIZE];
    if (channel_one == nullptr)
    {
        printf("Memory allocation error\n");
        exit(0);
    }

    JETDEV::IQ<float> *channel_two = new(std::nothrow) JETDEV::IQ<float>[DEFAULT_SIZE];
    if (channel_two == nullptr)
    {
        printf("Memory allocation error\n");
        exit(0);
    }

#if UDP_SEND
    UDP::Udp udp(ca->saddr, ca->port);
#endif
    
    JETDEV::JetLTCM *jet = new(std::nothrow) JETDEV::JetLTCM(DEVICE_C2H, DEVICE_USER, DMA_BUF_COUNT, DMA_BUF_SIZE);
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
    rc = jet->SetParam(ca->samp_rate, ca->center_frequency, ca->frequency, ca->intermediate_frequency, ca->adc_clk_use);
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
            if (ca->channel == 1)
                udp.send_data(channel_one, DEFAULT_SIZE);
            else
                udp.send_data(channel_two, DEFAULT_SIZE);
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
    struct command_arguments ca;
    parse_arguments(argc, argv, &ca);
    get_arguments(&ca);

#if CAN_REC
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
#endif
    std::thread t(work, &ca);
    t.join();

	return 0;
}
