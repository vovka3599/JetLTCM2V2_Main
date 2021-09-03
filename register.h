#ifndef REGISTER_H
#define REGISTER_H

#include <iostream>

    enum
    {
        SAMP_FREQ_1_MHz = 0,
        SAMP_FREQ_500_kHz = 1,
        SAMP_FREQ_250_kHz = 2,
        SAMP_FREQ_125_kHz = 3
    };


/**
 * @brief 
 * 
 * Control
 *      reset       : alctive low
 *      samp_freq   : 0 - 1 MHz, 1 - 500 kHz, 2 - 250 kHz, 3 - 125 kHz (do not change in real time).
 * 
 * real date        : 31 bit - 1 = real date, 0 = const; 15-0 bit - const value (if 31 bit = 0, else ignored); 30-16 bit reserved.
 * dds_freq         : Output frequency dds. Active 28:0 bit.
 */
struct Registers
{
    struct Control
    {
        uint32_t reset : 1;
        uint32_t samp_freq : 2;
        uint32_t : 29;
    };

    volatile uint32_t real_date;
    volatile uint32_t dds_freq;
    volatile Control control;
};

#endif // REGISTER_H