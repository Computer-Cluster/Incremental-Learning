# Makefile ejemplo para proyectos en C
CC = clang
CFLAGS = -Wall -Wextra -lm
TARGET = build/cnn
SRC = src/cnn.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(SRC) -o $(TARGET) $(CFLAGS)

clean:
	rm -rf build/*

run: $(TARGET)
	./$(TARGET)