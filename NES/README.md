# NES Emulator

A Nintendo Entertainment System (NES) emulator with CPU, PPU, and APU emulation written in C++ with SDL3.

## Table of Contents

- [Features](#features)
- [Building](#building)
- [Running](#running)
- [Controls](#controls)
- [Architecture](#architecture)
- [Resources](#resources)

## Features

- **6502 CPU Emulation**: Full MOS 6502 processor emulation with all addressing modes
- **PPU (Picture Processing Unit)**: NES graphics rendering with background and sprite layers
- **APU (Audio Processing Unit)**: NES audio emulation
- **Mapper Support**: Support for various NES memory mappers
- **Display**: SDL3-based rendering at 256x240 resolution
- **Input**: Standard NES controller support via SDL3
- **Timing**: Accurate CPU/PPU timing synchronization

## Building

From `NES/` directory:

```bash
make           # Build NES emulator
make clean     # Clean build artifacts
```

Output: `Build/nesemu`

## Running

```bash
# Run with a ROM file
Build/nesemu Roms/rom.nes

# Build and run
make run
```

ROM files should be placed in the `Roms/` directory.

## Controls

| NES Controller | Keyboard |
|----------------|----------|
| D-Pad          | Arrow Keys |
| A Button       | Z |
| B Button       | X |
| Start          | Enter |
| Select         | Right Shift |
| -              | ESC (Quit) |

## Architecture

### Source Files

- **NESEMU.h/cpp**: Main emulator core and SDL3 application wrapper
- **NESCPU.h/cpp**: 6502 CPU emulation
- **NESPPU.h/cpp**: PPU graphics emulation
- **NESBUS.h/cpp**: System bus and memory mapping
- **NESDSK.h/cpp**: Disk System support
- **NESCMP.h**: Compression utilities
- **SDLEMUAPP.h/cpp**: SDL3 application framework
- **ahnesemu-main.cpp**: Entry point

### Build Structure

- **Source/**: C++ source files
- **Build/**: Compiled object files and executable
- **Roms/**: NES ROM files (.nes)

## Resources & References

- [6502 Instruction Set](http://www.6502.org/tutorials/6502opcodes.html) - CPU opcode reference
- [NES Dev Wiki](https://www.nesdev.org/wiki/) - Comprehensive NES documentation
- [SDL3 Documentation](https://wiki.libsdl.org/SDL3/) - Graphics and input library

## Troubleshooting

**Issue: ROM won't load**
- Ensure ROM is a valid .nes file (iNES format)
- Check that the ROM file path is correct

**Issue: Graphics not displaying**
- Verify SDL3 initialization succeeded
- Check system graphics drivers

**Issue: Audio not working**
- Verify SDL3 audio initialization
- Check system audio settings
