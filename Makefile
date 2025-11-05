
TARGET = mymemory

CC = gcc

CFLAGS = -Wall -std=c99

all: $(TARGET)

$(TARGET): mymemory.c main.c
	$(CC) $(CFLAGS) mymemory.c main.c -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
