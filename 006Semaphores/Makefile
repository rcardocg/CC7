CC = gcc
CFLAGS = -Wall -Wextra -O2 -pthread
TARGET = parkingGUI

all: $(TARGET)

$(TARGET): 006Semaphores.c parkingGUI.c parkingGUI.h
	$(CC) $(CFLAGS) $^ -o $(TARGET) `pkg-config --cflags --libs sdl2`

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)