#include <fifo.h>
#include <pthread.h>

#define min(a,b) (((a) < (b)) ? (a) : (b))

static pthread_mutex_t mutex;

unsigned int kfifo_init(struct kfifo *fifo, unsigned char *buffer, unsigned int size)
{
    pthread_mutex_init(&mutex,NULL);
    if((size & (size - 1)) != 0)
        return 0;
    if (!buffer)
        return 0;
    fifo->buffer = buffer;
    fifo->size = size;
    fifo->in = fifo->out = 0;
    return 1;
}

void kfifo_reset(struct kfifo *fifo)
{
    pthread_mutex_lock(&mutex);
    fifo->in = fifo->out = 0;
    pthread_mutex_unlock(&mutex);
}

unsigned int kfifo_put(struct kfifo *fifo,unsigned char *buffer, unsigned int len)
{
    unsigned int l;
    pthread_mutex_lock(&mutex);
    len = min(len, fifo->size - fifo->in + fifo->out);
    l = min(len, fifo->size - (fifo->in & (fifo->size - 1)));
    memcpy(fifo->buffer + (fifo->in & (fifo->size - 1)), buffer, l);
    memcpy(fifo->buffer, buffer + l, len - l);
    fifo->in += len;
    pthread_mutex_unlock(&mutex);
    return len;
}

unsigned int kfifo_get(struct kfifo * fifo,unsigned char *buffer, unsigned int len)
{
    unsigned int l;
    pthread_mutex_lock(&mutex);
    len = min(len, fifo->in - fifo->out);
    l = min(len, fifo->size - (fifo->out & (fifo->size - 1)));
    memcpy(buffer, fifo->buffer + (fifo->out & (fifo->size - 1)), l);
    memcpy(buffer + l, fifo->buffer, len - l);
    fifo->out += len;
    if (fifo->in == fifo->out)
        fifo->in = fifo->out = 0;
    pthread_mutex_unlock(&mutex);
    return len;
}


unsigned int kfifo_len(struct kfifo * fifo)
{
    unsigned int ret;
    pthread_mutex_lock(&mutex);
    ret = fifo->in - fifo->out;
    pthread_mutex_unlock(&mutex);
    return ret;
}
