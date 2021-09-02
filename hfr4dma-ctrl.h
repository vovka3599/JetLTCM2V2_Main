/*
 * This file is part of the Xilinx DMA IP Core driver for Linux
 *
 * Copyright (c) 2016-present,  Xilinx, Inc.
 * All rights reserved.
 *
 * This source code is licensed under both the BSD-style license (found in the
 * LICENSE file in the root directory of this source tree) and the GPLv2 (found
 * in the COPYING file in the root directory of this source tree).
 * You may select, at your option, one of the above-listed licenses.
 */

#ifndef __HFR4_DMA_CTRL_H__
#define __HFR4_DMA_CTRL_H__

#include <linux/ioctl.h>


/*----------------------------------------------------------------------------
 * IOCTL Interface
 *----------------------------------------------------------------------------*/

/* Use 'x' as magic number */
#define HFR4_IOC_MAGIC	'x'

/*
 * S means "Set" through a ptr,
 * T means "Tell" directly with the argument value
 * G means "Get": reply by setting through a pointer
 * Q means "Query": response is on the return value
 * X means "eXchange": switch G and S atomically
 * H means "sHift": switch T and Q atomically
 *
 * _IO(type,nr)		    no arguments
 * _IOR(type,nr,datatype)   read data from driver
 * _IOW(type,nr.datatype)   write data to driver
 * _IORW(type,nr,datatype)  read/write data
 *
 * _IOC_DIR(nr)		    returns direction
 * _IOC_TYPE(nr)	    returns magic
 * _IOC_NR(nr)		    returns number
 * _IOC_SIZE(nr)	    returns size
 */

enum HFR4_IOC_TYPES {
        HFR4_IOC_NOP = 0,
        HFR4_IOC_INFO,
        HFR4_IOC_OFFLINE,
        HFR4_IOC_ONLINE,
        HFR4_IOC_GET_BUFFERS,
        HFR4_IOC_SET_BUFFERS,
        HFR4_IOC_DMA_READ,
        HFR4_IOC_DMA_RESET,
        HFR4_IOC_DMA_START,
        HFR4_IOC_MAX
};

struct hfr4_ioc_base {
	unsigned int magic;
	unsigned int command;
};

struct hfr4_ioc_info {
        struct hfr4_ioc_base	base;
        unsigned short		vendor;
        unsigned short		device;
        unsigned short		subsystem_vendor;
        unsigned short		subsystem_device;
        unsigned int		dma_engine_version;
        unsigned int		driver_version;
	unsigned short		domain;
	unsigned char		bus;
	unsigned char		dev;
	unsigned char		func;
};

//------------------------------------------------------------------------
struct hfr4_buffer_params {
    unsigned int buf_size;  /* Размер одного DMA буфера в байтах  */
    unsigned int buf_count; /* Количество DMA буферов */
};

struct hfr4_transaction {
    unsigned int head;  /* Индекс последнего заполненного буфера */
    unsigned int tail;  /* Индекс первого заполненного буфера */
    unsigned int buf_overrun; /* Счетчик перезаписанных буферов */
};
//-------------------------------------------------------------------------

/* IOCTL коды */
#define IOCTL_HFR4_IOCINFO		_IOWR(HFR4_IOC_MAGIC, HFR4_IOC_INFO, \
                                        struct hfr4_ioc_info)
#define IOCTL_HFR4_IOCOFFLINE		_IO(HFR4_IOC_MAGIC, HFR4_IOC_OFFLINE)
#define IOCTL_HFR4_IOCONLINE		_IO(HFR4_IOC_MAGIC, HFR4_IOC_ONLINE)

/**
*  Получить информацию о количестве буферов, выделенных драйвером для DMA транзакций
*  и информации о размере этих буферов
*/
#define IOCTL_HFR4_GET_BUFFERS    _IOR(HFR4_IOC_MAGIC, HFR4_IOC_GET_BUFFERS, \
                                        struct hfr4_buffer_params*)

/**
*   Задать размер и количество выделяемых драйвером буферов,
*   используемых для DMA транзакций.
*/
#define IOCTL_HFR4_SET_BUFFERS    _IOW(HFR4_IOC_MAGIC, HFR4_IOC_SET_BUFFERS, \
                                        struct hfr4_buffer_params*)

/**
*   Получить информацию о заполненности DMA буферов
*/
#define IOCTL_HFR4_DMA_READ       _IOR(HFR4_IOC_MAGIC, HFR4_IOC_DMA_READ, \
                                        struct hfr4_transaction*)

/**
*   Сбросить в драйвере информацию о заполненности DMA буферов
*/
#define IOCTL_HFR4_DMA_RESET      _IO(HFR4_IOC_MAGIC, HFR4_IOC_DMA_RESET)

/**
*   Старт/стоп данных в драйвере информацию о заполненности DMA буферов
*/
#define IOCTL_HFR4_DMA_START      _IOW(HFR4_IOC_MAGIC, HFR4_IOC_DMA_START, int)

//---------------------------------------------------------------------------------



#endif /* __HFR4_DMA_CTRL_H__ */
