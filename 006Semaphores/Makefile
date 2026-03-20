CC = gcc
CFLAGS = -Wall -Wextra -O2 -pthread
TARGET = semaforos

all: $(TARGET)

$(TARGET): 006Semaphores.c parkingGUI.c
	$(CC) $(CFLAGS) $^ -o $(TARGET) `pkg-config --cflags --libs sdl2 SDL2_ttf`

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)