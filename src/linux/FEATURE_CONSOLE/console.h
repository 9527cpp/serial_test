/*****************************************************************************
* @file     console.h
* @date     26.07.2016
* @note
*
*****************************************************************************/
#ifndef _CONSOLE_H
#define _CONSOLE_H

#include <stdint.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include "rda_console.h"
#include <pthread.h>
#include <semaphore.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void console_init(void);
extern void console_puts(const char *s);
extern void console_putc(char c);
extern void console_set_baudrate(unsigned int baudrate);

#define console_cmd_usage   show_cmd_usage
#define console_cmd_add     add_cmd_to_list
#define console_fifo_len    kfifo_len
#define console_fifo_get    kfifo_get

#ifdef __cplusplus
}
#endif


#endif

