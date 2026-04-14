# Makefile for AH Retro Emulator Suite
# Builds both Chip8 and NES emulators
# Uses SDL3 installed via brew

# ============================================================================
# Project Configuration
# ============================================================================

SOURCE_DIR := Source
BUILD_DIR := Build

# Chip8 Configuration
CH8_NAME := AHCH8EMU
CH8_BUILD := $(BUILD_DIR)/CH8
CH8_OBJ_DIR := $(CH8_BUILD)/Obj
CH8_SOURCE_DIR := $(SOURCE_DIR)/CH8

# NES Configuration
NES_NAME := AHNESEMU
NES_BUILD := $(BUILD_DIR)/NES
NES_OBJ_DIR := $(NES_BUILD)/Obj
NES_SOURCE_DIR := $(SOURCE_DIR)/NES

# ============================================================================
# Compiler and Tools
# ============================================================================

CXX := clang++
CC := clang

# ============================================================================
# SDL3 Configuration
# ============================================================================

SDL3_PREFIX := /opt/homebrew/opt/sdl3
SDL3_INCLUDES := -I$(SDL3_PREFIX)/include
SDL3_LIBS := -L$(SDL3_PREFIX)/lib -lSDL3

# ============================================================================
# Compiler Flags
# ============================================================================

CXXFLAGS := -std=c++17 -Wall -Wextra -O2 -fPIC
CFLAGS := -std=c11 -Wall -Wextra -O2 -fPIC

# Debug flags (uncomment for debug builds)
# CXXFLAGS += -g -O0 -DDEBUG

# Include paths
INCLUDES := $(SDL3_INCLUDES) -I$(SOURCE_DIR)

# ============================================================================
# Linker Flags
# ============================================================================

LDFLAGS := $(SDL3_LIBS)

# ============================================================================
# Chip8 Source Files
# ============================================================================

CH8_SOURCES := \
	$(CH8_SOURCE_DIR)/ch8-emu-main.cpp \
	$(CH8_SOURCE_DIR)/ch8-emu.cpp

CH8_OBJECTS := $(addprefix $(CH8_OBJ_DIR)/,$(notdir $(CH8_SOURCES:%.cpp=%.o)))

# ============================================================================
# NES Source Files
# ============================================================================

NES_SOURCES := \
	$(NES_SOURCE_DIR)/ahnesemu-main.cpp

NES_OBJECTS := $(addprefix $(NES_OBJ_DIR)/,$(notdir $(NES_SOURCES:%.cpp=%.o)))

# ============================================================================
# Build Rules
# ============================================================================

.PHONY: all ch8 nes clean clean-ch8 clean-nes depend help

# Search paths for source files
vpath %.cpp $(CH8_SOURCE_DIR)
vpath %.cpp $(NES_SOURCE_DIR)

all: ch8 nes

# ============================================================================
# Chip8 Emulator
# ============================================================================

ch8: $(CH8_BUILD)/$(CH8_NAME)

$(CH8_BUILD)/$(CH8_NAME): $(CH8_OBJECTS)
	@echo "[LINK CH8] Creating executable: $@"
	@mkdir -p $(CH8_BUILD)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)
	@echo "[SUCCESS] Chip8 build complete: $@"

$(CH8_OBJ_DIR)/%.o: %.cpp
	@echo "[CXX CH8] Compiling: $<"
	@mkdir -p $(CH8_OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(CH8_OBJ_DIR)/%.o: %.c
	@echo "[CC CH8] Compiling: $<"
	@mkdir -p $(CH8_OBJ_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# ============================================================================
# NES Emulator
# ============================================================================

nes: $(NES_BUILD)/$(NES_NAME)

$(NES_BUILD)/$(NES_NAME): $(NES_OBJECTS)
	@echo "[LINK NES] Creating executable: $@"
	@mkdir -p $(NES_BUILD)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)
	@echo "[SUCCESS] NES build complete: $@"

$(NES_OBJ_DIR)/%.o: %.cpp
	@echo "[CXX NES] Compiling: $<"
	@mkdir -p $(NES_OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(NES_OBJ_DIR)/%.o: %.c
	@echo "[CC NES] Compiling: $<"
	@mkdir -p $(NES_OBJ_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# ============================================================================
# Cleaning
# ============================================================================

clean: clean-ch8 clean-nes
	@echo "[SUCCESS] All clean complete"

clean-ch8:
	@echo "[CLEAN] Removing Chip8 build artifacts..."
	@mkdir -p $(CH8_OBJ_DIR)
	@rm -f $(CH8_OBJ_DIR)/*.o $(CH8_BUILD)/$(CH8_NAME)

clean-nes:
	@echo "[CLEAN] Removing NES build artifacts..."
	@mkdir -p $(NES_OBJ_DIR)
	@rm -f $(NES_OBJ_DIR)/*.o $(NES_BUILD)/$(NES_NAME)

# ============================================================================
# Help
# ============================================================================

help:
	@echo "AH Retro Emulator Suite - Build System"
	@echo "======================================="
	@echo ""
	@echo "Targets:"
	@echo "  all         - Build both Chip8 and NES (default)"
	@echo "  ch8         - Build Chip8 emulator"
	@echo "  nes         - Build NES emulator"
	@echo "  clean       - Remove all build artifacts"
	@echo "  clean-ch8   - Remove Chip8 build artifacts"
	@echo "  clean-nes   - Remove NES build artifacts"
	@echo "  help        - Show this help message"
	@echo ""
	@echo "Output Executables:"
	@echo "  Chip8: $(CH8_BUILD)/$(CH8_NAME)"
	@echo "  NES:   $(NES_BUILD)/$(NES_NAME)"
	@echo ""
	@echo "Variables:"
	@echo "  CXX      - C++ compiler (default: clang++)"
	@echo "  CXXFLAGS - C++ compiler flags"
	@echo ""
	@echo "Examples:"
	@echo "  make              # Build both emulators"
	@echo "  make ch8          # Build Chip8 only"
	@echo "  make nes          # Build NES only"
	@echo "  make clean        # Clean all artifacts"
	@echo "  make clean-ch8    # Clean Chip8 only"

# ============================================================================
# Development Targets
# ============================================================================

run-ch8: ch8
	@echo "[RUN] Starting $(CH8_NAME)..."
	@./$(CH8_BUILD)/$(CH8_NAME)

run-nes: nes
	@echo "[RUN] Starting $(NES_NAME)..."
	@./$(NES_BUILD)/$(NES_NAME)

run-all: ch8 nes
	@echo "[RUN] Starting both emulators..."
	@echo "Chip8: "./$(CH8_BUILD)/$(CH8_NAME)""
	@echo "NES:   "./$(NES_BUILD)/$(NES_NAME)""
