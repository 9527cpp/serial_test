#include "serial.h"
#include <stdio.h>
#include <stdlib.h>    
#include <string.h>
#include <unistd.h>    
#include <sys/types.h>  
#include <sys/stat.h>   
#include <fcntl.h>      
#include <termios.h>  
#include <errno.h>
#include <pthread.h>


typedef void (*serial_irq)(int,int);
serial_irq g_irq = NULL;
fd_set rd;
#define BUFFER_SIZE 8192
unsigned char buffer[BUFFER_SIZE];
int g_buf_len = 0;
int g_buf_len1 = 0;

void * serial_proc(void * args)
{
    pthread_detach(pthread_self());
    int fd = *(int *)args;
    if(fd == -1)return NULL;
    printf("fd:%d\r\n",fd);
    while(1){
        FD_ZERO(&rd);
        FD_SET(fd,&rd);
        if(select(fd +1 ,&rd,NULL,NULL,NULL) > 0){
            if(FD_ISSET(fd,&rd)){
                memset(buffer,0,BUFFER_SIZE);
                int recv_len = 0;
                g_buf_len = 0;
                g_buf_len1 = 0;
                while((recv_len = read(fd,buffer + g_buf_len,BUFFER_SIZE - g_buf_len)) > 0){
                    g_buf_len += recv_len;
                    g_buf_len1 = g_buf_len;
                    printf("recv_len:%d,g_buf_len:%d\r\n",recv_len,g_buf_len);
                }
                if(g_irq)g_irq(fd,RxIrq);
            }
        }
    }
    return NULL;
}

int serial_open(const char* portname, int baudrate, char parity, char databit, char stopbit)
{
    int fd = -1;
    fd = open(portname,O_RDWR|O_NOCTTY|O_NONBLOCK);
    if(fd == -1)
    {
        printf(" open failed , may be you need 'sudo' permission.\r\n");
        goto FAILED;
    }
    struct termios options;
    if(tcgetattr(fd,&options) < 0)
    {
        printf(" open failed ,get serial port attributes failed.\r\n");
        goto FAILED;
    }
    switch(baudrate)
    {
        case 4800:
            cfsetispeed(&options,B4800);
            cfsetospeed(&options,B4800);
            break;
        case 9600:
            cfsetispeed(&options,B9600);
            cfsetospeed(&options,B9600);
            break;   
        case 19200:
            cfsetispeed(&options,B19200);
            cfsetospeed(&options,B19200);
            break;
        case 38400:
            cfsetispeed(&options,B38400);
            cfsetospeed(&options,B38400);
            break;
        case 57600:
            cfsetispeed(&options,B57600);
            cfsetospeed(&options,B57600);
            break;
        case 115200:
            cfsetispeed(&options,B115200);
            cfsetospeed(&options,B115200);
            break;
        default:
            printf(" open failed ,unkown baudrate , only support 4800,9600,19200,38400,57600,115200.\r\n");
            goto FAILED;
    }
    switch(parity)
    {
        case 0:
            options.c_cflag &= ~PARENB;
            options.c_cflag &= ~INPCK;
            break;
        case 1:
            options.c_cflag |= PARENB;
            options.c_cflag |= PARODD;
            options.c_cflag |= INPCK;
            options.c_cflag |= ISTRIP;
            break;
        case 2:
            options.c_cflag |= PARENB;
            options.c_cflag &= ~PARODD;
            options.c_cflag |= INPCK;
            options.c_cflag |= ISTRIP;
            break;
        default:
            printf(" open failed ,unkown parity.\r\n");
            goto FAILED;
    }
    switch(databit)
    {
        case 5:
            options.c_cflag &= ~CSIZE;
            options.c_cflag |= CS5;
            break;
        case 6:
            options.c_cflag &= ~CSIZE;
            options.c_cflag |= CS6;
            break;
        case 7:
            options.c_cflag &= ~CSIZE;
            options.c_cflag |= CS7;
            break;
        case 8:
            options.c_cflag &= ~CSIZE;
            options.c_cflag |= CS8;
            break;
        default:
            printf(" open failed ,unkown databit.\r\n");
            goto FAILED;
    }
    switch(stopbit)
    {
        case 1:
            options.c_cflag &= ~CSTOPB;
            break;
        case 2:
            options.c_cflag |= CSTOPB;
            break;
        default:
            printf(" open failed ,unkown stopbit.\r\n");
            goto FAILED;
    }
    if((tcsetattr(fd,TCSANOW,&options))!=0) 
    { 
        printf(" open failed , can not complete set attributes.\r\n");
        goto FAILED;
    } 
    pthread_t thread_serial;
    pthread_create(&thread_serial,NULL,serial_proc,(void *)&fd);
    usleep(1000*20);
    printf("serial fd:%d\r\n",fd);
    return fd;
FAILED:
    if(fd != -1)
        close(fd);
    return -1;
}

void serial_close(int fd)
{
    if(fd != -1)
    {
        close(fd);
    }
}

int serial_readable(int fd){
    if(g_buf_len)
    {
        return 1;
    }
    return 0;
}

void serial_setirq(int fd,void (*irqhandler)(int,int)){
    g_irq = irqhandler; 
}

unsigned char serial_getc(int fd){
    unsigned char ch = buffer[g_buf_len1 - g_buf_len];
    g_buf_len --;
    return ch;
}

void serial_putc(int fd,char ch)
{
    write(fd,&ch,1);
}

int serial_send(int fd,const void *buf,int len)
{
    int sendCount = 0;
    if(fd != -1)
    {   
        const char *buffer = (char*)buf;
        size_t length = len;
        ssize_t tmp;
        while(length > 0)
        {
            if((tmp = write(fd, buffer, length)) <= 0)
            {
                if(tmp < 0&&errno == EINTR)
                {
                    tmp = 0;
                }
                else
                {
                    break;
                }
            }
            length -= tmp;
            buffer += tmp;
        }
        sendCount = len - length;
    }
    return sendCount;
}

int serial_receive(int fd,void *buf,int maxlen)
{
    int receiveCount = 0;
    if(fd != -1){
        receiveCount = read(fd,buf,maxlen);
        if(receiveCount < 0)
        {
            receiveCount = 0;
        }
    }
    return receiveCount;
}
