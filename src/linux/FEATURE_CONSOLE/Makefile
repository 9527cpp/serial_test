
#COMPILIE_PREFIX = arm-openwrt-linux-muslgnueabi-
CC = $(COMPILIE_PREFIX)gcc
AR = $(COMPILIE_PREFIX)ar
SRCS += ${wildcard *.c}
OBJS := ${patsubst %.c, %.o, $(SRCS)}


CFLAGS = -Wall -g -I.
LDFLAGS = -lrt -lpthread
TARGET := uart_rda

all:$(TARGET)

$(TARGET):$(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
$(OBJS):%.o:%.c
	$(CC) $(CFLAGS) -c $^ -o $@
clean:
	rm -f $(OBJS) $(TARGET) 
