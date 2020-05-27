/*****************************************************************************
* @file     console.cpp
* @date     26.07.2016
* @note
*
*****************************************************************************/
#include "console.h"

extern int stdio_uart_inited;
extern uint32_t g_cnt_uart0;
extern char console_buffer[CMD_CBSIZE];
extern char lastcommand[CMD_CBSIZE];

void show_cmd_usage(const cmd_tbl_t *cmd)
{
    printf("Usage:\r\n%s %d\r\n -%s\r\n", cmd->name, cmd->maxargs, cmd->usage);
}

int add_cmd_to_list(const cmd_tbl_t *cmd)
{
    cmd_tbl_t *tmp_cmd;
    if(CMD_LIST_COUNT <= cmd_cntr) {
        printf("No more cmds supported.\r\n");
        return -1;
    }
    tmp_cmd = &(cmd_list[cmd_cntr]);
    cmd_cntr++;
    memcpy((char *)tmp_cmd, (char *)cmd, sizeof(cmd_tbl_t));
    return 0;
}

void console_putc(char c) {
    /* serial_putc(&stdio_uart, c); */
}

void console_puts(const char *s) {
    while(*s) {
        /* serial_putc(&stdio_uart, *s++); */
    }
}

void console_set_baudrate(unsigned int baudrate)
{

    /* serial_baud(&stdio_uart,baudrate); */
    /* serial_clear(&stdio_uart); */
    /* stdio_uart.uart->IER |= 1 << 0; */

}

void console_cmd_exec(void) {
    uint8_t  ch;
    int16_t len;
    while(kfifo_get(&ch, 1)) {
        len = handle_char(ch, 0);
        if (len >= 0) {
            strcpy(lastcommand, console_buffer);
            if (len > 0) {
                if(run_command(lastcommand) < 0) {
                }
            }
            handle_char('0', "r");
        }
    }
}


static void console_rxisr_Callback(void) {
    
    /* while(serial_readable(&stdio_uart)) */
    /* { */
    /*     unsigned char c = serial_getc(&stdio_uart); */
    /*     { */
    /*         kfifo_put(&c, 1); */
    /*         if((c == '\r') || (c == '\n') || (c == 0x03)) { */
    /*             p_rda_console.cmd_cnt++; */
    /*         } */
    /*     } */ 
    /* } */
    
}

static void console_irq_handler(uint32_t id, int event) {

    /* if(RxIrq == event) { */
    /*     console_rxisr_Callback(); */
    /* } */

}

static void * console_task(const void *arg) {
    pthread_detach(pthread_self());

    while (1) {
        /* sem_wait(&p_rda_console.sem_id); */
        console_cmd_exec();
    }
}

void console_init(void) {

    if (!stdio_uart_inited) {
        /* do serial init */
    }

    /* serial_irq_handler(&stdio_uart, console_irq_handler, (uint32_t)(&p_rda_console)); */
    /* serial_irq_set(&stdio_uart, RxIrq, 1); */

    init_console_irq_buffer();
    pthread_t thread_serial;
    pthread_create(&thread_serial,NULL,console_task,NULL);
}

int main(){
    console_init();
    while(1){
        sleep(1);
    }
    return 0;
}

