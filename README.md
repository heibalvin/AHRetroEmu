# AHRetroEmu

A retro console emulator suite written in C++ with SDL3. Currently supporting Chip-8 and NES emulators.

## Table of Contents

- [Requirements](#requirements)
- [Project Structure](#project-structure)
- [Building](#building)
- [Emulators](#emulators)
  - [Chip-8](#chip-8)
  - [NES](#nes)

## Requirements

- **macOS / Linux**
- **Compiler**: clang++ (C++17 or later)
- **SDL3**: Installed via homebrew (`brew install sdl3`)
- **Make**: Standard build tool

## Project Structure

```
AHRetroEmu/
├── README.md                    # Project overview
├── .gitignore                   # Git ignore rules
│
├── Source/                      # Source code
│   ├── CH8/                     # Chip-8 emulator
│   │   ├── ch8-emu-main.cpp    # Main entrypoint
│   │   ├── ch8-emu.cpp         # Emulator core
│   │   └── ch8-emu.h           # Emulator header
│   └── NES/                     # NES emulator
│       └── ahnesemu-main.cpp   # Main entrypoint
│
├── Build/                       # Build system & outputs
│   ├── Makefile                 # Build configuration
│   ├── README.md               # Build instructions
│   ├── CH8/                     # Chip-8 build output
│   │   ├── AHCH8EMU            # Executable
│   │   ├── Obj/                # Object files
│   │   └── Resources -> ../../Resources/CH8
│   └── NES/                     # NES build output
│       ├── AHNESEMU            # Executable
│       ├── Obj/                # Object files
│       └── Resources -> ../../Resources/NES
│
└── Resources/                   # ROM and asset files
    ├── CH8/                     # Chip-8 test ROMs
    │   ├── *.ch8               # ROM files
    │   ├── beep.wav            # Beep sound
    │   └── buzz.wav            # Buzz sound
    └── NES/                     # NES ROM files
```

## Building

All build instructions and automation are located in the **[`Build/`](./Build/) folder**.

See [Build/README.md](./Build/README.md) for detailed build instructions and configuration options.

### Quick Start

```bash
cd Build
make           # Build both emulators
make ch8       # Build Chip-8 only
make nes       # Build NES only
make clean     # Clean build artifacts
```

### Running

```bash
# From Build directory
./CH8/AHCH8EMU         # Run Chip-8
./NES/AHNESEMU         # Run NES

# Or from project root
./Build/CH8/AHCH8EMU   # Run Chip-8
./Build/NES/AHNESEMU   # Run NES

# Or use make targets
make run-ch8           # Build and run Chip-8
make run-nes           # Build and run NES
```

## Emulators

### Chip-8

A virtual machine / interpreter for the Chip-8 programming language.

**Features:**
- Full Chip-8 instruction set (16 instructions)
- Display: 64x32 pixels
- Keyboard: 16-key hexadecimal keypad
- Sound: Beep and buzz audio support
- SDL3-based graphics and input
- Synchronized CPU, timers, and display at 60Hz

**Controls:**
- Keypad: 1, 2, 3, 4, Q, W, E, R, A, S, D, F, Z, X, C, V (map to Chip-8 0-F)
- ESC: Quit

**Test ROMs:**
- 1-chip8-logo.ch8 ✓
- 2-ibm-logo.ch8 ✓
- 3-corax+.ch8 ✓
- 4-flags.ch8 ✓
- 5-quirks.ch8 ✓
- 6-keypad.ch8 ✓
- 7-beep.ch8 ✓

**References:**
- [Cowgod's Chip-8 Technical Reference](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM)
- [Guide to making a CHIP-8 emulator](https://tobiasvl.github.io/blog/write-a-chip-8-emulator/#instructions)
- [Chip-8 Test Suite](https://github.com/Timendus/chip8-test-suite)
- [SDL3 Documentation](https://wiki.libsdl.org/SDL3/)

**Test Results:**

All included test ROMs execute successfully:

| ROM | Status | Description |
|-----|--------|-------------|
| 1-chip8-logo.ch8 | ✅ PASS | Chip-8 logo display test |
| 2-ibm-logo.ch8 | ✅ PASS | IBM logo display test |
| 3-corax+.ch8 | ✅ PASS | Corax+ graphics test |
| 4-flags.ch8 | ✅ PASS | Flag display test |
| 5-quirks.ch8 | ✅ PASS | Quirks compatibility test |
| 6-keypad.ch8 | ✅ PASS | Keypad input test |
| 7-beep.ch8 | ✅ PASS | Audio beep test |

### NES

Nintendo Entertainment System emulator (in development).

**Features:**
- 256x240 resolution with aspect ratio preservation
- SDL3-based graphics rendering
- Keyboard and gamepad input support
- VSync enabled

**Status:** Core framework complete, CPU emulation in progress

## License

See LICENSE file for details.