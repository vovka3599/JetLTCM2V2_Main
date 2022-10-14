#ifndef CONFIG_H
#define CONFIG_H

#include "register.h"

// Device config
#define DEVICE_C2H                  "/dev/hfrdma0_c2h_0"
#define DEVICE_USER                 "/dev/hfrdma0_user"
#define DEFAULT_SIZE                2048
#define IQ_SIZE_BYTE                16
#define DMA_BUF_SIZE                DEFAULT_SIZE*IQ_SIZE_BYTE
#define DMA_BUF_COUNT               4

// DDS ADC config
const bool REAL_DATE                = true;
const uint16_t CONST_VALUE          = 0x0;
const double DDS_SYSTEM_CLOCK_MHz   = 120.000;

// Control config
#define FILE_SAVE                   0
#define UDP_SEND                    1
#define TIME_ON                     0
#define TEST_CUDA                   0
#define CAN_REC                     1

#endif // CONFIG_H
