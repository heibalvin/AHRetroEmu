CC = gcc
SDL3_DIR = /opt/homebrew/opt/sdl3
CFLAGS = -Wall -I$(SDL3_DIR)/include -I./Source
LDFLAGS = -L$(SDL3_DIR)/lib -lSDL3

SRC_MAIN = Source/hello.c
SRC_CALLBACKS = Source/hello-callbacks.c

OBJ_DIR = Build
OBJ_MAIN = $(OBJ_DIR)/hello.o
OBJ_CALLBACKS = $(OBJ_DIR)/hello-callbacks.o

TARGET_MAIN = $(OBJ_DIR)/hello
TARGET_CALLBACKS = $(OBJ_DIR)/hello-callbacks

all: main

main: $(TARGET_MAIN)

callbacks: $(TARGET_CALLBACKS)

$(TARGET_MAIN): $(OBJ_MAIN)
	$(CC) $^ $(LDFLAGS) -o $@

$(TARGET_CALLBACKS): $(OBJ_CALLBACKS)
	$(CC) $^ $(LDFLAGS) -o $@

$(OBJ_MAIN): $(SRC_MAIN)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_CALLBACKS): $(SRC_CALLBACKS)
	$(CC) $(CFLAGS) -c $< -o $@

run: main
	./$(TARGET_MAIN)

run-callbacks: callbacks
	./$(TARGET_CALLBACKS)

clean:
	rm -f $(OBJ_MAIN) $(OBJ_CALLBACKS) $(TARGET_MAIN) $(TARGET_CALLBACKS)

.PHONY: all main callbacks run run-callbacks clean
