CC = gcc
CFLAGS = -Wall -Wextra -std=c99
TARGET = septica

all: $(TARGET)

$(TARGET): main.c septica.c septica.h
	$(CC) $(CFLAGS) -o $(TARGET) main.c septica.c

clean:
	rm -f $(TARGET)
