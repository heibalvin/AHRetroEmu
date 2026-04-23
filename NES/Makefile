CXX = g++
CXXFLAGS = -std=c++17 -Wall -I./Source
LDFLAGS = -lSDL2

SRC = $(wildcard Source/*.cpp)
OBJ = $(SRC:.cpp=.o)
TARGET = Build/nesemu

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(OBJ) $(LDFLAGS) -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean
