#ifndef __FIFO_H__
#define __FIFO_H__

typedef struct {
    unsigned char *buffer;  
    unsigned int size;      
    unsigned int in;        
    unsigned int out;       
}kfifo;

#ifdef __cplusplus
extern "C"{
#endif

int kfifo_init(kfifo *fifo, unsigned char *buffer, unsigned int size);
unsigned int kfifo_put(unsigned char *buffer, unsigned int len);
unsigned int kfifo_get(unsigned char *buffer, unsigned int len);
unsigned int kfifo_len(void);

#ifdef __cplusplus
}
#endif
#endif
