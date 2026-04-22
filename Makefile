# ============================================================================
# AHRetroEmu - Makefile
# A collection of classic computer emulators built with SDL3
# ============================================================================

# ============================================================================
# Project Configuration
# ============================================================================
CH8_NAME := AHCH8EMU
NES_NAME := AHNESEMU

CH8_SOURCE_DIR := Source/CH8
NES_SOURCE_DIR := Source/NES

CH8_BUILD := Build/CH8
NES_BUILD := Build/NES

CH8_OBJ_DIR := $(CH8_BUILD)/Obj
NES_OBJ_DIR := $(NES_BUILD)/Obj

# ============================================================================
# Compiler and Tools
# ============================================================================
CXX := clang++
CC := clang
SDL3_PREFIX := /opt/homebrew/opt/sdl3

# ============================================================================
# SDL3 Configuration
# ============================================================================
SDL3_INCLUDES := -I$(SDL3_PREFIX)/include
SDL3_LDFLAGS := -L$(SDL3_PREFIX)/lib -lSDL3

# ============================================================================
# Compiler Flags
# ============================================================================
CXXFLAGS := -std=c++17 -O2 -Wall -Wextra
CFLAGS := -O2 -Wall -Wextra
INCLUDES := $(SDL3_INCLUDES)
LDFLAGS := $(SDL3_LDFLAGS)

# ============================================================================
# Source Files - Auto-discover all .cpp files in directories
# ============================================================================
CH8_SOURCES := $(wildcard $(CH8_SOURCE_DIR)/*.cpp)
NES_SOURCES := $(wildcard $(NES_SOURCE_DIR)/*.cpp)

# Generate object file names from source files
CH8_OBJECTS := $(patsubst $(CH8_SOURCE_DIR)/%.cpp,$(CH8_OBJ_DIR)/%.o,$(CH8_SOURCES))
NES_OBJECTS := $(patsubst $(NES_SOURCE_DIR)/%.cpp,$(NES_OBJ_DIR)/%.o,$(NES_SOURCES))

# ============================================================================
# Build Targets
# ============================================================================

.PHONY: all ch8 nes clean clean-ch8 clean-nes help run-ch8 run-nes run-all

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

# Generic rule to compile all Chip8 source files
$(CH8_OBJ_DIR)/%.o: $(CH8_SOURCE_DIR)/%.cpp
	@echo "[CXX CH8] Compiling: $<"
	@mkdir -p $(CH8_OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# ============================================================================
# NES Emulator
# ============================================================================

nes: $(NES_BUILD)/$(NES_NAME)

$(NES_BUILD)/$(NES_NAME): $(NES_OBJECTS)
	@echo "[LINK NES] Creating executable: $@"
	@mkdir -p $(NES_BUILD)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)
	@echo "[SUCCESS] NES build complete: $@"

# Generic rule to compile all NES source files
$(NES_OBJ_DIR)/%.o: $(NES_SOURCE_DIR)/%.cpp
	@echo "[CXX NES] Compiling: $<"
	@mkdir -p $(NES_OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# ============================================================================
# Cleaning
# ============================================================================

clean: clean-ch8 clean-nes
	@echo "[SUCCESS] All clean complete"

clean-ch8:
	@echo "[CLEAN] Removing Chip8 build artifacts..."
	@rm -f $(CH8_OBJ_DIR)/*.o $(CH8_BUILD)/$(CH8_NAME)

clean-nes:
	@echo "[CLEAN] Removing NES build artifacts..."
	@rm -f $(NES_OBJ_DIR)/*.o $(NES_BUILD)/$(NES_NAME)

# ============================================================================
# Help
# ============================================================================

help:
	@echo "AHRetroEmu Build System"
	@echo "======================="
	@echo ""
	@echo "Targets:"
	@echo "  make all      - Build all emulators (default)"
	@echo "  make ch8      - Build Chip-8 emulator"
	@echo "  make nes      - Build NES emulator"
	@echo "  make clean    - Clean all build artifacts"
	@echo "  make clean-ch8    - Clean Chip-8 build artifacts only"
	@echo "  make clean-nes    - Clean NES build artifacts only"
	@echo "  make run-ch8  - Build and run Chip-8 emulator"
	@echo "  make run-nes  - Build and run NES emulator"
	@echo "  make run-all  - Build and run all emulators"
	@echo "  make help     - Show this help message"

# ============================================================================
# Development & Running
# ============================================================================

run-ch8: ch8
	@echo "[RUN] Starting Chip-8 emulator..."
	@$(CH8_BUILD)/$(CH8_NAME)

run-nes: nes
	@echo "[RUN] Starting NES emulator..."
	@cd $(NES_BUILD) && ./$(NES_NAME)

run-all: all
	@echo "[RUN] All emulators built!"
