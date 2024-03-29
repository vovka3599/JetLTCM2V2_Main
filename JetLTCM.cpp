#include "JetLTCM.h"

namespace JETDEV
{
    JetLTCM::JetLTCM(std::string _device_c2h,
                    std::string _device_user, 
                    uint32_t _dma_buf_count, 
                    uint32_t _dma_buf_size) 
                    : 
                    device_c2h(_device_c2h), 
                    device_user(_device_user)
    {
        dma_buf_params.buf_size = _dma_buf_size;
        dma_buf_params.buf_count = _dma_buf_count;
        reg = nullptr;
        start_dma = false;
    }

    JetLTCM::~JetLTCM()
    {
        for(int i = 0; i < dma_buf_params.buf_count; i++)
            munmap(dma_mem_array[i], dma_buf_params.buf_size);
        
        munmap(reg, 32*1024U);

        close(fpga_fd);
        delete[] dma_mem_array;
    }

    int JetLTCM::Init()
    {
        int rc;

        fpga_fd = open(device_c2h.c_str(), O_RDWR | O_NONBLOCK);
        if(fpga_fd < 0) 
        {
            printf("Error open device.\n");
            return -1;
        }
        printf("Opened device: %s\n", device_c2h.c_str());

        rc = ioctl(fpga_fd, IOCTL_HFR4_SET_BUFFERS, &dma_buf_params);
        if(rc != 0) 
        {
            printf("Error send ioctl IOCTL_HFR4_SET_BUFFERS. buffer size: %u, buffer count: %u\n",
                dma_buf_params.buf_size, dma_buf_params.buf_count);
            return -1;
        }
        printf("Set buffer size: %u, buffer count: %u\n", dma_buf_params.buf_size, dma_buf_params.buf_count);

        struct hfr4_buffer_params temp_buf;;
        rc = ioctl(fpga_fd, IOCTL_HFR4_GET_BUFFERS, &temp_buf);
        if(rc != 0)
        {
            printf("Error send ioctl IOCTL_HFR4_GET_BUFFERS.\n");
            return -1;
        }
        printf("--- Got new buffer params: dma_buffer_size=%d, dma_buffer_count=%d ---\n", temp_buf.buf_size, temp_buf.buf_count);

        dma_mem_array = new(std::nothrow) void*[dma_buf_params.buf_count];
        if (dma_mem_array == nullptr)
        {
            printf("Memory allocation error\n");
            return -1;
        }
        for(int i = 0; i < dma_buf_params.buf_count; i++) 
        {
            dma_mem_array[i] = mmap(0, dma_buf_params.buf_size, PROT_READ, MAP_SHARED, fpga_fd, i*dma_buf_params.buf_size);
            if (dma_mem_array[i] == MAP_FAILED) return -1;
        }
        printf("--- Mapped %d buffers %u size ---\n", dma_buf_params.buf_count, dma_buf_params.buf_size);

        int user_fd = open(device_user.c_str(), O_RDWR | O_NONBLOCK);
        if(user_fd < 0) 
        {
            printf(" Error open user device.\n");
            return -1;
        }

        void* status_reg = mmap(NULL, 32*1024U, PROT_READ | PROT_WRITE, MAP_SHARED, user_fd, 0);
        if (status_reg == MAP_FAILED) return -1;
        reg = (struct Registers*)status_reg;
        reg->control.reset = 0;
        close(user_fd);

        return 0;
    }

    int JetLTCM::SetParam(uint8_t _samp_freq,
                        uint32_t _center_freq, 
                        uint32_t _freq,
                        uint32_t _ifreq,
                        bool _adc_clk_use)
    {
        if (reg)
        {
            int rc;
            if(start_dma == true)
            {
                    rc = ioctl(fpga_fd, IOCTL_HFR4_DMA_START, 0);
                    if(rc != 0) return -1;
            }
            else
                start_dma = true;

            rc = ioctl(fpga_fd, IOCTL_HFR4_DMA_RESET);
            if(rc != 0) return -1;

            reg->control.reset = 0;
            reg->real_date = (REAL_DATE << 31) | CONST_VALUE;

            double pch = ((double)_ifreq - ((double)_freq - (double)_center_freq)) / 1000000.0;
            uint32_t dds_value = ((DDS_SYSTEM_CLOCK_MHz*2-pch)/DDS_SYSTEM_CLOCK_MHz)*(1<<29);
            printf("DDS = %x\n", dds_value);

            reg->dds_freq = dds_value;
            reg->control.samp_freq = _samp_freq;
            reg->control.adc_clk_use = _adc_clk_use;
            usleep(1000);
            reg->control.reset = 1;

            rc = ioctl(fpga_fd, IOCTL_HFR4_DMA_START, 1);
            if(rc != 0) return -1;

            return 0;
        }
        else
            return -1;
    }

    IQ_DEV<int>* JetLTCM::GetData()
    {
        int rc;
        rc = ioctl(fpga_fd, IOCTL_HFR4_DMA_READ, &dma_buf);
        if(rc < 0)
        {
            printf("Error read dma data\n");
            return nullptr;
        }
        else
            return (IQ_DEV<int>*)dma_mem_array[dma_buf.tail];
    }
}
