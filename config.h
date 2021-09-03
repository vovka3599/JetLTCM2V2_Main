#ifndef CONFIG_H
#define CONFIG_H

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

// File config
#define FILE_SAVE           0

#endif // CONFIG_H