CXX = g++
SDL3_DIR = /opt/homebrew/opt/sdl3
CXXFLAGS = -std=c++17 -Wall -I$(SDL3_DIR)/include -I./Source
LDFLAGS = -L$(SDL3_DIR)/lib -lSDL3

SRC_MAIN = Source/ch8-emu-main.cpp
SRC_EMU = Source/ch8-emu.cpp

OBJ_DIR = Build
OBJ_MAIN = $(OBJ_DIR)/ch8-emu-main.o
OBJ_EMU = $(OBJ_DIR)/ch8-emu.o

TARGET = $(OBJ_DIR)/chip8emu

all: $(TARGET)

$(TARGET): $(OBJ_MAIN) $(OBJ_EMU)
	$(CXX) $^ $(LDFLAGS) -o $@

$(OBJ_MAIN): $(SRC_MAIN)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_EMU): $(SRC_EMU)
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: all
	./$(TARGET)

clean:
	rm -f $(OBJ_MAIN) $(OBJ_EMU) $(TARGET)

.PHONY: all run clean
