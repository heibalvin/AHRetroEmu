# AHRetroEmu

A retro console emulator suite written in C++ with SDL3. Currently supporting Chip-8 and NES emulators.

## Table of Contents

- [Requirements](#requirements)
- [Project Structure](#project-structure)
- [Building](#building)
- Emulators
  - [Chip-8](./Source/CH8/)
  - [NES](./Source/NES/)

## Requirements

- **macOS / Linux**
- **Compiler**: clang++ (C++17 or later)
- **SDL3**: v3.3.4 Installed via homebrew (`brew install sdl3`)
- **Make**: Standard build tool

## Project Structure

```
AHRetroEmu/
├── README.md                    # Project overview
├── Makefile                     # Build configuration
│
├── Source/                      # Source code
│   ├── CH8/                     # Chip-8 emulator
│   └── NES/                     # NES emulator
│
├── Build/                       # Build system & outputs
│   ├── README.md                # Build instructions
│   ├── CH8/                     # Chip-8 build output
│   │   ├── AHCH8EMU             # Executable
│   │   ├── Obj/                 # Object files
│   │   └── Resources -> ../../Resources/CH8
│   └── NES/                    # NES build output
│       ├── AHNESEMU            # Executable
│       ├── Obj/                # Object files
│       └── Resources -> ../../Resources/NES
│
└── Resources/                   # ROM and asset files
    ├── CH8/                     # Chip-8 test ROMs files
    └── NES/                     # NES test ROM files
```

## Building

### Quick Start

```bash
make           # Build both emulators
make ch8       # Build Chip-8 only
make nes       # Build NES only
make clean     # Clean build artifacts
make run-ch8   # Build and run Chip-8
make run-nes   # Build and run NES
```

All .cpp files in each emulator directory are automatically discovered and compiled.

## License

See LICENSE file for details.