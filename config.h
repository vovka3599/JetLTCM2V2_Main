#ifndef CONFIG_H
#define CONFIG_H

#include "register.h"

// UDP config
#define ADDRESS_DEFAULT     "192.168.1.158"
#define PORT_DEFAULT        1234
#define FFT_SIZE_DEFAULT    2048*8

// Device config
#define DEVICE_C2H          "/dev/hfrdma0_c2h_0"
#define DEVICE_USER         "/dev/hfrdma0_user"
#define DEFAULT_SIZE        2048
#define IQ_SIZE_BYTE        16
#define DMA_BUF_SIZE        DEFAULT_SIZE*IQ_SIZE_BYTE
#define DMA_BUF_COUNT       4

// DDS config
const uint8_t SAMP_FREQ     = SAMP_FREQ_500_kHz;
const bool REAL_DATE        = true;
const uint16_t CONST_VALUE  = 0x0;
const uint32_t DDS_FREQ     = 0x18000000;

// Control config
#define FILE_SAVE           0
#define UDP_SEND            0

#endif // CONFIG_H