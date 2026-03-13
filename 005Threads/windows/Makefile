# Makefile for Windows (using MinGW or similar)

CC = gcc
CFLAGS = -Wall -O2
TARGET = log_processor.exe
SRCS = main.c log_processor.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	del /Q $(OBJS) $(TARGET)

.PHONY: all clean