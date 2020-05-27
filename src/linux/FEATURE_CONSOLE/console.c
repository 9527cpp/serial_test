/*****************************************************************************
* @file     console.cpp
* @date     26.07.2016
* @note
*
*****************************************************************************/
#include "console.h"
#include "fifo.h"
#include "serial.h"

#define CMD_CBSIZE       128
#define CMD_MAXARGS      16
#define CMD_LIST_COUNT   60
#define RING_BUFF_SIZE  2048

static char erase_seq[] = "\b \b";       /* erase sequence         */
static char tab_seq[] = "    ";        /* used to expand TABs    */
static struct kfifo g_fifo;
static unsigned char console_recv_ring_buffer[RING_BUFF_SIZE] = {0};
static stdio_uart_inited = 0;
char console_buffer[CMD_CBSIZE];
char lastcommand[CMD_CBSIZE] = { 0, };
unsigned int echo_flag = 1;
static cmd_tbl_t g_cmd_list[CMD_LIST_COUNT] = {};
static unsigned int cmd_cntr = 0;
int serial_fd = -1;


static void show_cmd_usage(const cmd_tbl_t *cmd)
{
    printf("Usage:\r\n%s %d\r\n -%s\r\n", cmd->name, cmd->maxargs, cmd->usage);
}

int console_add_cmd(const cmd_tbl_t *cmd,int counts)
{
    cmd_tbl_t *tmp_cmd;
    int i = 0;
    for(i =0;i< counts;i++){
        if(CMD_LIST_COUNT <= cmd_cntr) {
            printf("No more cmds supported.\r\n");
            return -1;
        }
        tmp_cmd = &(g_cmd_list[cmd_cntr]);
        cmd_cntr++;
        memcpy((char *)tmp_cmd, (char *)cmd + i*sizeof(cmd_tbl_t), sizeof(cmd_tbl_t));
    }
    return 0;
}

static int parse_line (char *line, char *argv[])
{
    int nargs = 0;
    while (nargs < CMD_MAXARGS) {
        while ((*line == ' ') || (*line == '\t') || (*line == ',')) {
            ++line;
        }
        if (*line == '\0') {    /* end of line, no more args    */
            argv[nargs] = 0;
            return (nargs);
        }
        if(*line == '\"') {
            line++;
            argv[nargs++] = line;
            while(*line && (*line != '\"')) {
                ++line;
            }
        } else {
            argv[nargs++] = line;    /* begin of argument string    */
            while(*line && (*line != ',') && (*line != '=')) {
                ++line;
            }
        }
        if (*line == '\0') {    /* end of line, no more args    */
            argv[nargs] = 0;
            return (nargs);
        }
        *line++ = '\0';         /* terminate current arg     */
    }
    return (nargs);
}

static cmd_tbl_t *find_cmd (const char *cmd)
{
    cmd_tbl_t *cmdtp;
    cmd_tbl_t *cmdtp_temp = &g_cmd_list[0];    /* Init value */
    uint32_t len;
    int n_found = 0;
    int i;

    len = strlen(cmd);
    for (i = 0;i < (int)cmd_cntr;i++) {
        cmdtp = &g_cmd_list[i];
        if (strncmp(cmd, cmdtp->name, len) == 0) {
            if (len == strlen(cmdtp->name))
                return cmdtp;      /* full match */
            cmdtp_temp = cmdtp;    /* abbreviated command ? */
            n_found++;
        }
    }
    if (n_found == 1) {  /* exactly one match */
        return cmdtp_temp;
    }
    return 0;   /* not found or ambiguous command */
}


static int run_command(char *cmd)
{
    cmd_tbl_t *cmdtp;
    char *argv[CMD_MAXARGS + 1];    /* NULL terminated    */
    int argc;
    if ((argc = parse_line(cmd, argv)) == 0) {
        return -1;
    }
    if ((cmdtp = find_cmd(argv[0])) == 0) {
        return -1;
    }
    if (argc > cmdtp->maxargs) {
        return -1;
    }
    if ((cmdtp->cmd) (cmdtp, argc, argv) != 0) {
        return -1;
    }
    return 0;
}

static void console_putc(char c) {
    serial_putc(serial_fd, c);
}

static void console_puts(const char *s) {
    while(*s) {
        serial_putc(serial_fd, *s++);
    }
}


static char *delete_char(char *buffer, char *p, int *colp, int *np, int plen)
{
    char *s;
    if (*np == 0) {
        return (p);
    }
    if (*(--p) == '\t') {            /* will retype the whole line    */
        while (*colp > plen) {
            console_puts(erase_seq);
            (*colp)--;
        }
        for (s=buffer; s<p; ++s) {
            if (*s == '\t') {
                console_puts(tab_seq+((*colp) & 07));
                *colp += 8 - ((*colp) & 07);
            } else {
                ++(*colp);
                console_putc(*s);
            }
        }
    } else {
        console_puts(erase_seq);
        (*colp)--;
    }
    (*np)--;
    return (p);
}


int handle_char(const char c, char *prompt) {
    static char   *p   = console_buffer;
    static int    n    = 0;              /* buffer index        */
    static int    plen = 0;           /* prompt length    */
    static int    col;                /* output column cnt    */

    if (prompt) {
        plen = strlen(prompt);
        if(plen == 1 && prompt[0] == 'r')
            plen = 0;
        else
            console_puts(prompt);
        p = console_buffer;
        n = 0;
        return 0;
    }
    col = plen;
    /* Special character handling */
    switch (c) {
        case '\r':                /* Enter        */
        case '\n':
            *p = '\0';
            console_puts("\r\n");
            return (p - console_buffer);
        case '\0':                /* nul            */
            return -1;
        case 0x03:                /* ^C - break        */
            console_buffer[0] = '\0';    /* discard input */
            return 0;
        case 0x15:                /* ^U - erase line    */
            while (col > plen) {
                console_puts(erase_seq);
                --col;
            }
            p = console_buffer;
            n = 0;
            return -1;
        case 0x17:                /* ^W - erase word     */
            p=delete_char(console_buffer, p, &col, &n, plen);
            while ((n > 0) && (*p != ' ')) {
                p=delete_char(console_buffer, p, &col, &n, plen);
            }
            return -1;
        case 0x08:                /* ^H  - backspace    */
        case 0x7F:                /* DEL - backspace    */
            p=delete_char(console_buffer, p, &col, &n, plen);
            return -1;
        default:
         /*  Must be a normal character then  */
            if (n < CMD_CBSIZE-2) {
                if (c == '\t') {    /* expand TABs        */
                    console_puts(tab_seq+(col&07));
                    col += 8 - (col&07);
                } else {
                    if(echo_flag == 1){
                        ++col;         /* echo input        */
                        console_putc(c);
                    }
                }
                *p++ = c;
                ++n;
            } else {          /* Buffer full        */
                console_putc('\a');
            }
    }
    return -1;
}

void console_cmd_exec(void) {
    uint8_t  ch;
    int16_t len;
    while(kfifo_get(&g_fifo,&ch, 1)) {
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
    
    while(serial_readable(serial_fd))
    {
        unsigned char c = serial_getc(serial_fd);
    //printf("%x,",c);
        {
            kfifo_put(&g_fifo, &c, 1);
            if((c == '\r') || (c == '\n') || (c == 0x03)) {
                /* p_rda_console.cmd_cnt++; */
            }
        } 
    }
}

static void console_irq_handler(uint32_t id, int event) {
    if(RxIrq == event) {
        console_rxisr_Callback();
    }
}

static void * console_task(const void *arg) {
    pthread_detach(pthread_self());

    while (1) {
        /* sem_wait(&p_rda_console.sem_id); */
        console_cmd_exec();
    }
}

int console_init(void) {
    if (stdio_uart_inited) {
        printf("has already init\r\n");
        return 0;
    }
    
    /* do serial init */
    serial_fd = serial_open("/dev/ttyS1",9600,0,8,1);
    if(serial_fd != -1)
        serial_setirq(serial_fd,console_irq_handler);

    if(0 == kfifo_init(&g_fifo, console_recv_ring_buffer, RING_BUFF_SIZE)){
        printf("fifo init failed so return\r\n");
        return -1;
    }
    pthread_t thread_console;
    pthread_create(&thread_console,NULL,console_task,NULL);
    stdio_uart_inited = 1;
    return 0;
}




/* THIS_IS_JUST_DO_EXE_TEST: */

int do_test1(cmd_tbl_t * cmd,int argc,char *argv[])
{
    printf("this is do test_1\r\n");
}

int do_test2(cmd_tbl_t * cmd,int argc,char *argv[])
{
    printf("this is do test_2\r\n");
}

cmd_tbl_t cmd_list[] = {
    {
        "test1",1,do_test1,
        "test1 --- just do test1"
    },
    {
        "test2",1,do_test2,
        "test2 --- just do test2"
    },
};

int main(){
    console_init();
    console_add_cmd(cmd_list,sizeof(cmd_list)/sizeof(cmd_tbl_t));
    while(1){
        sleep(1);
    }
    return 0;
}

