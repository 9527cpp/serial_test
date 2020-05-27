/*****************************************************************************
* @file     rda_console.h
* @date     26.07.2016
* @note
*
*****************************************************************************/
#ifndef _RDA_CONSOLE_H
#define _RDA_CONSOLE_H

#ifdef __cplusplus
extern "C" {
#endif

#define CMD_CBSIZE       128
#define CMD_PROMPT       "mbed91h> "
#define CMD_MAXARGS      16
#define CMD_LIST_COUNT   60
#define RING_BUFF_SIZE  2048


typedef struct cmd_tbl_s {
    char    *name;                                     /* Command Name */
    int     maxargs;                                   /* maximum number of arguments */
    int     (*cmd)(struct cmd_tbl_s *, int, char *[]);
    char    *usage;                                    /* Usage message(short)*/
} cmd_tbl_t;


int run_command (char *cmd);
int handle_char (const char c, char *prompt);
void show_cmd_usage(const cmd_tbl_t *cmd);
int add_cmd_to_list(const cmd_tbl_t *cmd);

#ifdef __cplusplus
}
#endif

#endif

