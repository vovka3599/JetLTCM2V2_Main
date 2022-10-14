#ifndef REGISTER_H
#define REGISTER_H

#include <iostream>

enum sample_rate
{
    SAMP_RATE_10_MHz    = 0,
    SAMP_RATE_1_MHz     = 1,
    SAMP_RATE_500_kHz   = 2,
    SAMP_RATE_250_kHz   = 3,
    SAMP_RATE_125_kHz   = 4,
    SAMP_RATE_62_5_kHz  = 5
};

enum adc_clk_use{
    ADC_CLK_1 = 0,
    ADC_CLK_2
};

/**
 * @brief 
 * 
 * Control
 *      reset       : alctive low
 *      samp_freq   : 0 - 10 MHz, 1 - 1 MHz, 2 - 500 kHz, 3 - 250 kHz, 4 - 125 kHz, 5 - 62.5 kHz
 *      adc_clk_use : 0 - use adc_1 clock, 1 - use adc_2 clock
 * 
 * real date        : 31 bit - 1 = real date, 0 = const; 15-0 bit - const value (if 31 bit = 0, else ignored); 30-16 bit reserved.
 * dds_freq         : Output frequency dds. Active 28:0 bit.
 */
struct Registers
{
    struct Control
    {
        uint32_t reset : 1;
        uint32_t samp_freq : 3;
        uint32_t adc_clk_use : 1;
        uint32_t : 27;
    };

    volatile uint32_t real_date;
    volatile uint32_t dds_freq;
    volatile Control control;
};

#endif // REGISTER_H