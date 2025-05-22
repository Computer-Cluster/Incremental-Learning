
CC = gcc
CFLAGS = -Wall -Wextra -lm
TARGET = build/gan
SRC = src/gan.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(SRC) -o $(TARGET) $(CFLAGS)

clean:
	rm -rf build/*

run: $(TARGET)
	./$(TARGET)