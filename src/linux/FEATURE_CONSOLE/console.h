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
#include <pthread.h>
#include <semaphore.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef struct cmd_tbl_s {
    char    *name;                                     /* Command Name */
    int     maxargs;                                   /* maximum number of arguments */
    int     (*cmd)(struct cmd_tbl_s *, int, char *[]);
    char    *usage;                                    /* Usage message(short)*/
} cmd_tbl_t;

int console_add_cmd(const cmd_tbl_t *cmd,int counts);
int console_init(void);

#ifdef __cplusplus
}
#endif


#endif

