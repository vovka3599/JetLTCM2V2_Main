#ifndef CONFIG_H
#define CONFIG_H

#include "register.h"

// UDP config
#define UDP_SEND_ADDR               "192.168.1.193"
#define UDP_SEND_PORT               1234
#define FFT_SIZE                    2048*8

// Device config
#define DEVICE_C2H                  "/dev/hfrdma0_c2h_0"
#define DEVICE_USER                 "/dev/hfrdma0_user"
#define DEFAULT_SIZE                2048
#define IQ_SIZE_BYTE                16
#define DMA_BUF_SIZE                DEFAULT_SIZE*IQ_SIZE_BYTE
#define DMA_BUF_COUNT               4

// CLK ADC config
enum adc_clk_use{
    ADC_CLK_1 = 0,
    ADC_CLK_2
};
const bool ADC_CLK_USE              = ADC_CLK_2;

// DDS ADC config
const uint8_t SAMP_FREQ             = SAMP_FREQ_1_MHz;
const bool REAL_DATE                = true;
const uint16_t CONST_VALUE          = 0x0;
const double DDS_OUT_FREQ_MHz       = 120-(137.5-120.0);
const double DDS_SYSTEM_CLOCK_MHz   = 120.000;
const uint32_t DDS_VALUE            = (DDS_OUT_FREQ_MHz/DDS_SYSTEM_CLOCK_MHz)*(1<<29);

// Control config
#define FILE_SAVE                   0
#define UDP_SEND                    1
#define TIME_ON                     0
#define TEST_CUDA                   0
#define CAN_REC                     0

#endif // CONFIG_H