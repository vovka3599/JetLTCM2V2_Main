#ifndef JETLTCM_H
#define JETLTCM_H

#include <iostream>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <memory.h>
#include <assert.h>
#include <unistd.h>

#include "register.h"
#include "hfr4dma-ctrl.h"

typedef struct{
	int I_1;
	int Q_1;

	int I_2;
	int Q_2;
}IQ;

class JetLTCM
{
private:
    struct hfr4_buffer_params dma_buf_params;
    struct hfr4_transaction dma_buf;
    void** dma_mem_array;
    bool start_dma;

    struct Registers *reg;

    std::string device_c2h;
    std::string device_h2c;
    std::string device_user;

    int fpga_fd;

    IQ *data;
public:
    JetLTCM(std::string _device_c2h,
            std::string _device_user, 
            uint32_t _dma_buf_count, 
            uint32_t _dma_buf_size);

    ~JetLTCM();

    int Init();
    int SetParam(uint8_t _samp_freq, 
                bool _real_date, 
                uint16_t _const_value,
                uint32_t _dds_freq);
    IQ* GetData();
};



#endif // JETLTCM_H