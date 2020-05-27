#include <fifi.h>
#include <pthread.h>

#define min(a,b) (((a) < (b)) ? (a) : (b))

static pthread_mutex_t mutex;

int kfifo_init(kfifo *fifo, unsigned char *buffer, unsigned int size)
{
    pthread_mutex_init(&mutex,NULL);
    if((size & (size - 1)) != 0)
        return -1;
    if (!buffer)
        return -1;
    fifo->buffer = buffer;
    fifo->size = size;
    fifo->in = fifo->out = 0;
    return 0;
}

static inline void __kfifo_reset(kfifo *fifo)
{
    fifo->in = fifo->out = 0;
}

static inline void kfifo_reset(void)
{
    pthread_mutex_lock(&mutex);
    __kfifo_reset(&console_kfifo);
    pthread_mutex_unlock(&mutex);
}

static unsigned int __kfifo_put(kfifo *fifo,
            unsigned char *buffer, unsigned int len)
{
    unsigned int l;
    len = min(len, fifo->size - fifo->in + fifo->out);
    l = min(len, fifo->size - (fifo->in & (fifo->size - 1)));
    memcpy(fifo->buffer + (fifo->in & (fifo->size - 1)), buffer, l);
    memcpy(fifo->buffer, buffer + l, len - l);
    fifo->in += len;
    return len;
}

unsigned int kfifo_put(unsigned char *buffer, unsigned int len)
{
    unsigned int ret;
    pthread_mutex_lock(&mutex);
    ret = __kfifo_put(&console_kfifo, buffer, len);
    pthread_mutex_unlock(&mutex);
    return ret;
}

static unsigned int __kfifo_get(kfifo *fifo,
            unsigned char *buffer, unsigned int len)
{
    unsigned int l;
    len = min(len, fifo->in - fifo->out);
    l = min(len, fifo->size - (fifo->out & (fifo->size - 1)));
    memcpy(buffer, fifo->buffer + (fifo->out & (fifo->size - 1)), l);
    memcpy(buffer + l, fifo->buffer, len - l);
    fifo->out += len;
    return len;
}

unsigned int kfifo_get(unsigned char *buffer, unsigned int len)
{
    unsigned int ret;
    kfifo *fifo;
    pthread_mutex_lock(&mutex);
    fifo = &console_kfifo;
    ret = __kfifo_get(fifo, buffer, len);
    if (fifo->in == fifo->out)
        fifo->in = fifo->out = 0;
    pthread_mutex_unlock(&mutex);
    return ret;
}


static inline unsigned int __kfifo_len(kfifo *fifo)
{
    return fifo->in - fifo->out;
}

unsigned int kfifo_len(void)
{
    unsigned int ret;
    pthread_mutex_lock(&mutex);
    ret = __kfifo_len(&console_kfifo);
    pthread_mutex_unlock(&mutex);
    return ret;
}
