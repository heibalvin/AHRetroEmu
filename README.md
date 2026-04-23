# AHRetroEmu

A retro console emulator suite written in C++ with SDL3. Currently supporting Chip-8 and NES emulators.

## Table of Contents

- [Requirements](#requirements)
- [Project Structure](#project-structure)
- [Emulators](#emulators)

## Requirements

- **macOS / Linux**
- **Compiler**: clang++ (C++17 or later)
- **SDL3**: v3.3.4 Installed via homebrew (`brew install sdl3`)
- **Make**: Standard build tool

## Project Structure

```
AHRetroEmu/
├── README.md                    # Project overview
│
├── NES/                         # NES emulator
│   ├── Source/                  # NES source code
│   ├── Build/                   # Build output
│   ├── Roms/                    # NES ROM files
│   └── Makefile                 # NES build configuration
│
├── Chip8/                       # Chip-8 emulator
│   ├── Source/                  # Chip-8 source code
│   ├── Build/                   # Build output
│   ├── Roms/                    # Chip-8 ROM files
│   ├── Assets/                  # Chip-8 assets
│   └── Makefile                 # Chip-8 build configuration
│
└── SDL3/                        # SDL3 examples
    ├── Source/                  # SDL3 example source code
    ├── Build/                   # Build output
    └── Makefile                 # SDL3 build configuration
```

## Emulators

- [NES](./NES/) - NES emulator with CPU, PPU, and APU emulation
- [Chip-8](./Chip8/) - Chip-8 virtual machine emulator
- [SDL3](./SDL3/) - SDL3 example programs
