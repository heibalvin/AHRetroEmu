# ============================================================================
# AHRetroEmu - Makefile
# A collection of classic computer emulators built with SDL3
# ============================================================================

# ============================================================================
# Project Configuration
# ============================================================================
CH8_NAME := AHCH8EMU
NES_NAME := AHNESEMU

CH8_SOURCE_DIR := Source/Source
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
# Source Files
# ============================================================================
CH8_SOURCES := $(CH8_SOURCE_DIR)/ch8-emu.cpp $(CH8_SOURCE_DIR)/ch8-emu-main.cpp
NES_SOURCES := $(NES_SOURCE_DIR)/ahnesemu-main.cpp

CH8_OBJECTS := $(addprefix $(CH8_OBJ_DIR)/,$(notdir $(CH8_SOURCES:.cpp=.o)))
NES_OBJECTS := $(addprefix $(NES_OBJ_DIR)/,$(notdir $(NES_SOURCES:.cpp=.o)))

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

$(CH8_OBJ_DIR)/ch8-emu.o: $(CH8_SOURCE_DIR)/ch8-emu.cpp
	@echo "[CXX CH8] Compiling: $<"
	@mkdir -p $(CH8_OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(CH8_OBJ_DIR)/ch8-emu-main.o: $(CH8_SOURCE_DIR)/ch8-emu-main.cpp
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

$(NES_OBJ_DIR)/ahnesemu-main.o: $(NES_SOURCE_DIR)/ahnesemu-main.cpp
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
	@$(NES_BUILD)/$(NES_NAME)

run-all: all
	@echo "[RUN] All emulators built!"
