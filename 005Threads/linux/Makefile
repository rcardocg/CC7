CC = gcc
CFLAGS = -Wall -Wextra -O2 -pthread
TARGET = log_analyzer

all: $(TARGET)

$(TARGET): main.c log_processor.c log_processor.h
	$(CC) $(CFLAGS) main.c log_processor.c -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)