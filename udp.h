#ifndef UDP_H
#define UDP_H

#include <ctype.h>
#include <iostream>
#include <memory.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

#include "config.h"

struct addr_params 
{
    char* saddr;            // IP адрес UDP Source в виде строки.
    uint16_t port;          // Порт, который слушает UDP Source
    int channel;            // Номер канала HFR
    int dma_buffer_size;    // Размер одного блока памяти для DMA транзакции
    int dma_buffer_count;   // Количество блоков памяти, выделяемых для DMA транзакции.
    int fft_size;           // Количество комплексных отсчетов для FFT, которые будут записаны в UDP пакет.
};

typedef struct info_get_data
{
    int loop_count;
    int size_buf;
    struct file *filp;
    char *userbuf;
}INFO;

/**
 * @brief Create_udp_socket Create udp socket
 * @param s                 udp socket
 */
void Create_udp_socket(int *s);

#endif // UDP_H