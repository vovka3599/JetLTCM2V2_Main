#include "udp.h"

void Create_udp_socket(int *s)
{
    char saddr[128];
    strcpy(saddr, ADDRESS_DEFAULT);

    struct addr_params arg;
    arg.saddr = &saddr[0];  // IP адрес UDP Source в виде строки.
    arg.port = PORT_DEFAULT;        // Порт, который слушает UDP Source
    arg.channel = 0;
    arg.fft_size = FFT_SIZE_DEFAULT; // Количество комплексных отсчетов для FFT, которые будут записаны в UDP пакет.

    struct sockaddr_in sin;
    struct protoent* ppe;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;

    if((sin.sin_addr.s_addr = inet_addr(arg.saddr)) == INADDR_NONE) 
    {
        printf("Can't get ip address %s  \n", arg.saddr);
        exit(0);
    }

    if((sin.sin_port = htons(arg.port)) == 0) 
    {
        printf("Can't get ip port %d \n", arg.port);
        exit(0);
    }

    if((ppe = getprotobyname("udp")) == 0) 
    {
        printf("Can't get protocol entry for udp \n");
        exit(0);
    }

    *s = socket(PF_INET, SOCK_DGRAM, ppe->p_proto);
    if(*s < 0) 
    {
        printf("Can't create socket: %s \n", strerror(errno));
        exit(0);
    }
    printf("created UDP socket... \n");

    if(connect(*s, (struct sockaddr*)&sin, sizeof(sin)) < 0) 
    {
        printf("Can't connect socket to %s, port %d: %s \n", arg.saddr, arg.port, strerror(errno));
        close(*s);
        exit(0);
    }

    printf("connected to UDP socket. address: %s, port %d \n", arg.saddr, arg.port);
}