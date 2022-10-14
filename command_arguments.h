#ifndef COMMAND_ARGUMENTS_H
#define COMMAND_ARGUMENTS_H

#include <string.h>
#include <getopt.h>
#include <iostream>

struct command_arguments
{
    char saddr[128] = "192.168.1.193";
    uint16_t port = 1234;
    int samp_rate = 1;
    int channel = 1;
    int adc_clk_use = 2;
    uint32_t frequency = 868000000;
    uint32_t center_frequency = 868000000;
    uint32_t intermediate_frequency = 137500000;
};


static struct option const long_opts[] =
{
    {"address", required_argument, NULL, 'a'},
    {"port", required_argument, NULL, 'p'},
    {"samp_rate", required_argument, NULL, 's'},
    {"channel", required_argument, NULL, 'x'},
    {"adc_clk_use", required_argument, NULL, 'm'},
    {"frequency", required_argument, NULL, 'f'},
    {"center_frequency", required_argument, NULL, 'c'},
    {"intermediate_frequency", required_argument, NULL, 'i'},
    {"help", no_argument, NULL, 'h'}
};

static uint32_t getopt_integer(char* optarg)
{
    int rc;
    uint32_t value;
    rc = sscanf(optarg, "0x%x", &value);
    if (rc <= 0)
        rc = sscanf(optarg, "%ul", &value);

    return value;
}

static int getopt_string(char* optarg, char* value)
{
    int rc;
    rc = sscanf(optarg, "%s", value);
    return rc;
}

static void usage(const char* name)
{
    int i = 0;
    printf("%s\n\n", name);
    printf("usage: %s [OPTIONS]\n\n", name);

    printf("  -%c (--%s) ip address to UDP send\n", long_opts[i].val, long_opts[i].name); i++;
    printf("  -%c (--%s) port to UDP send\n", long_opts[i].val, long_opts[i].name); i++;
    printf("  -%c (--%s) sample rate, default is %d (1MHz).\n", long_opts[i].val, long_opts[i].name, 1); i++;
    printf("\tsamp_rate:\n");
    printf("\t\tSAMP_RATE_10_MHz     = 0\n");
    printf("\t\tSAMP_RATE_1_MHz      = 1\n");
    printf("\t\tSAMP_RATE_500_kHz    = 2\n");
    printf("\t\tSAMP_RATE_250_kHz    = 3\n");
    printf("\t\tSAMP_RATE_125_kHz    = 4\n");
    printf("\t\tSAMP_RATE_62_5_kHz   = 5\n");
    printf("  -%c (--%s) number of channel (1 or 2), default is %d.\n", long_opts[i].val, long_opts[i].name, 1); i++;
    printf("  -%c (--%s) adc clk use (1 or 2), default is %d.\n", long_opts[i].val, long_opts[i].name, 2); i++;
    printf("  -%c (--%s) frequency in Hz, default is %d.\n", long_opts[i].val, long_opts[i].name, 868000000); i++;
    printf("  -%c (--%s) center frequency in Hz, default is %d.\n", long_opts[i].val, long_opts[i].name, 868000000); i++;
    printf("  -%c (--%s) intermediate frequency in Hz, default is %d.\n", long_opts[i].val, long_opts[i].name, 137500000); i++;
    printf("  -%c (--%s) print usage help and exit\n", long_opts[i].val, long_opts[i].name); i++;

    printf("\nExample: ./JetLTCM2V2_Main -a 192.168.1.193 -p 1234 -s 1 -x 2 -f 868000000 -c 868000000 -i 150000000\n");
}

void parse_arguments(int argc, char* argv[], struct command_arguments *ca)
{
    int cmd_opt;
    while((cmd_opt = getopt_long(argc, argv, "a:p:s:x:m:f:c:i:h", long_opts, NULL)) != -1)
    {
        switch(cmd_opt)
        {
            case 0:
                break;

            case 'a':
                if(getopt_string(optarg, ca->saddr) <= 0) 
                {
                    printf("unable get string optarg \n");
                    exit(0);
                }
                break;

            case 'p':
                ca->port = getopt_integer(optarg);
                break;

            case 's':
                ca->samp_rate = getopt_integer(optarg);
                if (ca->samp_rate < 0 || ca->samp_rate > 5)
                {
                    printf("Error set sampling frequency\n");
                    ca->samp_rate = 1;
                }
                break;

            case 'x':
                ca->channel = getopt_integer(optarg);
                if (ca->channel < 1 || ca->channel > 2)
                {
                    printf("Error set channel\n");
                    ca->channel = 1;
                }
                break;

            case 'm':
                ca->adc_clk_use = getopt_integer(optarg);
                if (ca->channel < 1 || ca->channel > 2)
                {
                    printf("Error set adc_clk_use\n");
                    ca->channel = 1;
                }
                break;

            case 'f':
                ca->frequency = getopt_integer(optarg);
                if (ca->frequency == 0.0)
                {
                    printf("Error set frequency\n");
                    ca->frequency = 868000000;
                }
                break;

            case 'c':
                ca->center_frequency = getopt_integer(optarg);
                if (ca->center_frequency == 0.0)
                {
                    printf("Error set center_frequency\n");
                    ca->center_frequency = 868000000;
                }
                break;

            case 'i':
                ca->intermediate_frequency = getopt_integer(optarg);
                if (ca->intermediate_frequency == 0.0)
                {
                    printf("Error set center_frequency\n");
                    ca->intermediate_frequency = 137500000;
                }
                break;

            case 'h':
                usage(argv[0]);
                exit(0);
                break;
            default:
                break;
        }
    }
}

void get_arguments(struct command_arguments *ca)
{
    printf("Get_arguments:\r\n");
    printf("\tIP_addr = %s\n", ca->saddr);
    printf("\tIP_port = %u\n", ca->port);
    printf("\tSample rate = %d\n", ca->samp_rate); 
    printf("\tChannel = %d\n", ca->channel); 
    printf("\tADC clk use = %d\n", ca->adc_clk_use); 
    printf("\tFrequency = %u\n", ca->frequency); 
    printf("\tCenter frequency = %u\n", ca->center_frequency); 
    printf("\tIntermediate frequency = %u\n", ca->intermediate_frequency);   
}

#endif // COMMAND_ARGUMENTS_H