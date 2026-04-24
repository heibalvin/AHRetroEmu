# NES Emulator

A Nintendo Entertainment System (NES) emulator with CPU, PPU, and APU emulation written in C++ with SDL3.

## Table of Contents

- [Features](#features)
- [Building](#building-&-testing)
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
- **Debug Modes**: Step, Line, VBlank, Frame, and Run modes for debugging

## Building & Testing

From `NES/` directory:

```bash
make clean        # Clean build artifacts
make              # Build NES emulator (one executable: `Build/nesemu`)
make run          # Build and run
```

## Running

```bash
# Run with a ROM file
Build/nesemu rom.nes
```

ROM files should be placed in the `Roms/` directory.

### Default Behavior

After poweron, the emulator starts in **PAUSED** mode.

### Debug Controls (when built with `make debug`)

| Key | Action |
|-----|--------|
| `Space` | Step mode - execute one CPU cycle, then pause |
| `L` | Line mode - run until next PPU scanline, then pause |
| `V` | VBlank mode - run until next VBlank, then pause |
| `F` | Frame mode - run until frame complete, then pause |
| `R` | Run mode - run continuously |
| `ESC` | Quit emulator |

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

- **main.cpp**: Entry point
- **SDLEMUAPP.h/cpp**: SDL3 application wrapper
- **NESEMU.h/cpp**: Main emulator core with debug modes
- **NESCPU.h/cpp**: 6502 CPU emulation
- **NESPPU.h/cpp**: PPU graphics emulation
- **NESBUS.h/cpp**: System bus and memory mapping
- **NESDSK.h/cpp**: Disk System support
- **NESCMP.h**: Component Protocol class

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
