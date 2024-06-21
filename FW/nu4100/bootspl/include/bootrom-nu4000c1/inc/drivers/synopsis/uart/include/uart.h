/****************************************************************************
 *
 *   FileName: uart.h

 *
 *   Author:  Konstantin S.
 *
 *   Date: 	
 *
 *   Description: UART primitives header
 *   
 ****************************************************************************/
#ifndef __UART_H__
#define __UART_H__

#include "DW_common.h"


typedef struct dw_device  dw_device_t;

extern dw_device_t *uart_boot_dev;

dw_device_t  *uart_boot_init();
dw_device_t  *uart_debug_init();

int uart_read(dw_device_t *uart, unsigned char *byte);
int uart_write(dw_device_t *uart, unsigned char byte);
int uart_is_ready(dw_device_t *uart);
void uart_print(dw_device_t *uart, const char *ptr);

static inline int uart_boot_read(unsigned char* byte)
{
	return uart_read(uart_boot_dev, byte);
}

static inline int uart_boot_write(unsigned char byte)
{
	return uart_write(uart_boot_dev, byte);
}

static inline int uart_boot_is_ready()
{
	return uart_is_ready(uart_boot_dev);
}


#endif /*__UART_H__*/
