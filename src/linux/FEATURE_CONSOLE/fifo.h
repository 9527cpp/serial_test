#ifndef __FIFO_H__
#define __FIFO_H__

struct kfifo{
    unsigned char *buffer;  
    unsigned int size;      
    unsigned int in;        
    unsigned int out;       
};

#ifdef __cplusplus
extern "C"{
#endif

unsigned int kfifo_init(struct kfifo *fifo, unsigned char *buffer, unsigned int size);
void kfifo_reset(struct kfifo *fifo);
unsigned int kfifo_put(struct kfifo *fifo, unsigned char *buffer, unsigned int len);
unsigned int kfifo_get(struct kfifo *fifo, unsigned char *buffer, unsigned int len);
unsigned int kfifo_len(struct kfifo *fifo);

#ifdef __cplusplus
}
#endif
#endif
