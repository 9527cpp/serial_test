
#ifndef _WZSERIALPORT_H
#define _WZSERIALPORT_H

enum{
    RxIrq = 0,
    TxIrq
};

int serial_open(const char* portname, int baudrate, char parity, char databit, char stopbit);
void serial_setirq(int fd,void (*irqhandler)(int,int));
int serial_readalbe(int fd);
unsigned char serial_getc(int fd);
void serial_putc(int fd,char ch);
void serial_close(int fd);
int serial_send(int fd,const void *buf,int len);
int serial_receive(int fd,void *buf,int maxlen);

#endif
