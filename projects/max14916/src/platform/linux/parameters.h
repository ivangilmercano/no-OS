/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definition of FTD2XX platform data used by max14916 project.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#ifdef FTD2XX
#include "ftd2xx_spi.h"
#include "ftd2xx_uart.h"
#else
#include "linux_spi.h"
#include "linux_uart.h"
#endif

#define UART_IRQ_ID	0
#define UART_DEVICE_ID	0
#define UART_BAUDRATE	0
#define UART_EXTRA      NULL

#ifdef FTD2XX
extern struct ftd2xx_spi_init max14916_spi_extra;
#define SPI_EXTRA	&max14916_spi_extra
#define SPI_OPS		&ftd2xx_spi_ops
#define UART_OPS	&ftd2xx_uart_ops
#else
#define SPI_EXTRA	NULL
#define SPI_OPS		&linux_spi_ops
#define UART_OPS	&linux_uart_ops
#endif

#define SPI_DEVICE_ID	0
#define SPI_CS		0
#define SPI_BAUDRATE	100000

#endif  /* __PARAMETERS_H__ */
