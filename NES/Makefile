CXX = g++
SDL3_DIR = /opt/homebrew/opt/sdl3
CXXFLAGS = -std=c++17 -Wall -I$(SDL3_DIR)/include -I./Source
LDFLAGS = -L$(SDL3_DIR)/lib -lSDL3

SRC = $(wildcard Source/*.cpp)
OBJ_DIR = Build
OBJ = $(patsubst Source/%.cpp,$(OBJ_DIR)/%.o,$(SRC))

TARGET = $(OBJ_DIR)/nesemu

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $^ $(LDFLAGS) -o $@

$(OBJ_DIR)/%.o: Source/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: all
	./$(TARGET)

clean:
	rm -rf $(OBJ_DIR)

.PHONY: all run clean
