
TARGET = mymemory

all: $(TARGET)

$(TARGET): mymemory.c main.c
	gcc -Wall -std=c99 mymemory.c main.c -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
