#COMPILIE_PREFIX = arm-openwrt-linux-muslgnueabi-
CC = $(COMPILIE_PREFIX)g++
AR = $(COMPILIE_PREFIX)ar
SRCS += ${wildcard *.cpp}
OBJS := ${patsubst %.cpp, %.o, $(SRCS)}


CFLAGS = -Wall -g -I.
LDFLAGS = -lrt 
TARGET := uart

all:$(TARGET)

$(TARGET):$(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
$(OBJS):%.o:%.cpp
	$(CC) $(CFLAGS) -c $^ -o $@
clean:
	rm -f $(OBJS) $(TARGET) 
