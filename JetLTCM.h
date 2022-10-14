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
#include "config.h"

#include "register.h"
#include "hfr4dma-ctrl.h"

namespace JETDEV
{
    template<typename T>
    struct IQ{
        T I;
        T Q;
    };

    template<typename T>
    struct IQ_DEV{
        T I_1;
        T Q_1;

        T I_2;
        T Q_2;
    };

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
        
    public:
        JetLTCM(std::string _device_c2h,
                std::string _device_user, 
                uint32_t _dma_buf_count, 
                uint32_t _dma_buf_size);

        ~JetLTCM();

        /**
         * @brief       Device initialization
         * 
         * @return int  execution result
         */
        int Init();

        /**
         * @brief Set the Param object
         * 
         * @param _samp_freq    sampling frequency
         * @param _real_date    real data or constant
         * @param _const_value  constant value. if _real_date = true - ignored
         * @param _dds_freq     synthesizer frequency    
         * @return int          execution result
         */
        int SetParam(uint8_t _samp_freq,
                    uint32_t _center_freq, 
                    uint32_t _freq,
                    uint32_t _ifreq,
                    bool _adc_clk_use);

        /**
         * @brief Get the Data object
         * 
         * @return IQ_DEV<T>* pointer to data
         */
        IQ_DEV<int>* GetData();
    };
}

#endif // JETLTCM_H