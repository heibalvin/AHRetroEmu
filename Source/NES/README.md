# NES Emulator

Nintendo Entertainment System emulator for AHRetroEmu.

## Overview

The NES (Nintendo Entertainment System) emulator is a comprehensive implementation of Nintendo's iconic 8-bit game console from 1985. The project follows a modular component architecture, with each system (CPU, PPU, bus, cartridge) implemented as independent components for clarity and extensibility.

## Features

- **Resolution**: 256x240 native resolution with aspect ratio preservation
- **Graphics**: Component-based PPU (Picture Processing Unit) architecture
- **Input**: Keyboard and gamepad support via SDL3
- **Rendering**: Hardware-accelerated graphics with VSync
- **Modular Design**: Component-based system for maintainability

## Architecture

### Component System

All NES emulator components inherit from the `NESCMP` base class with three core methods:

- **`poweron()`** - Initialize component to default state
- **`reset()`** - Clean reset of component state
- **`update()`** - Execute one frame of emulation

### Components

#### NESEMU - Main Emulator Controller
Coordinates all sub-components and manages the overall emulation lifecycle.

**Responsibilities:**
- Initialize all components (CPU, PPU, Bus, Cartridge)
- Synchronize poweron/reset/update calls
- Manage component lifecycle (startup/shutdown)

#### NESCPU - Central Processing Unit
Implements the 6502 CPU core used in the NES.

**Registers:**
- `PC` (Program Counter) - Instruction pointer
- `SP` (Stack Pointer) - Stack position
- `A` (Accumulator) - Main computation register
- `X, Y` (Index Registers) - Loop and addressing
- `P` (Processor Status) - Flags (N, V, B, D, I, Z, C)

**Status:**
- Core structure complete
- TODO: 6502 instruction execution engine

#### NESPPU - Picture Processing Unit
Generates 256x240 pixel display output.

**Memory:**
- `VRAM` - 16KB Video RAM for nametables and attributes
- `OAM` - 256B Object Attribute Memory (sprite data)
- `Framebuffer` - 256x240 pixel output buffer

**Features:**
- Sprite rendering (64 sprites max)
- Background layer support
- Palette system
- Rendering pipeline

**Status:**
- Memory structures initialized
- TODO: Rendering pipeline implementation

#### NESBUS - Memory Bus
Manages the 64KB address space and connects all components.

**Memory Map:**
```
0x0000-0x1FFF: Internal RAM (8KB, mirrored 4x)
0x2000-0x3FFF: PPU Registers (mirrored)
0x4000-0x4017: APU / I/O Registers
0x4018-0x401F: Disabled area
0x4020-0x5FFF: Cartridge RAM
0x6000-0x7FFF: Cartridge RAM (optional)
0x8000-0xFFFF: Cartridge ROM
```

**Responsibilities:**
- RAM access and mirroring
- Component memory mapping
- Cartridge ROM access

**Status:**
- Internal RAM working
- TODO: PPU/APU/cartridge mapping

#### NESDSK - Cartridge Loader
Handles NES ROM cartridge loading and management.

**Supported Formats:**
- iNES (standard format)
- PRG-ROM (program code)
- CHR-ROM (character/graphics data)

**Features:**
- Cartridge detection
- Mapper support (planned)
- ROM bank switching

**Status:**
- Structure defined
- TODO: iNES parser implementation

## Building

From project root:

```bash
make nes       # Build NES emulator only
make clean     # Clean build artifacts
```

Output: `Build/NES/AHNESEMU`

## Running

```bash
# Run emulator (test pattern)
Build/NES/AHNESEMU

# Build and run
make run-nes
```

## Source Files

- **nes-component.h**: Component class definitions and interfaces
- **nes-component.cpp**: Component implementations
- **ahnesemu-main.cpp**: SDL3 graphics, input, and main event loop

## Display

**Resolution**: 256x240 pixels (native NES resolution)
**Aspect Ratio**: Maintained with letterbox/pillarbox as needed

**Color Palette**: 64 colors (2-bit per channel):
```
[0,  85, 170, 255] for each RGB channel
```

**Display Modes:**
- 60Hz refresh rate (VSync enabled)
- Hardware-accelerated rendering

## Input

### Keyboard Mapping

| NES Button | Keyboard |
|-----------|----------|
| A | Z |
| B | X |
| START | Enter |
| SELECT | Space |
| D-Pad Up | ↑ |
| D-Pad Down | ↓ |
| D-Pad Left | ← |
| D-Pad Right | → |

### Gamepad Support

Full gamepad support via SDL3 including:
- Button mapping
- D-Pad input
- Analog stick support (mapped to D-Pad)

## Development Status

### ✅ Completed
- Component framework and base classes
- NESEMU lifecycle management
- NESBUS memory structure
- NESPPU framebuffer and memory
- NESCPU register definitions
- NESDSK cartridge structure
- SDL3 graphics rendering

### 🔄 In Progress
- CPU instruction set implementation
- PPU rendering pipeline
- Cartridge ROM loading
- Memory mapping

### ❌ Not Started
- Mapper implementations
- APU (audio processing unit)
- Save state/load state functionality
- Debugger support

## Performance Characteristics

- **CPU Frequency**: ~1.79 MHz (NES standard)
- **Display Refresh**: 60Hz (NTSC) or 50Hz (PAL)
- **Execution Model**: Cycle-accurate timing (planned)
- **Executable Size**: ~42KB (with framework)

## References & Resources

- [6502 CPU Documentation](http://6502.org/) - 6502 instruction set
- [NES Dev Wiki](https://www.nesdev.org/wiki/Nesdev_Wiki) - Comprehensive NES documentation
- [iNES Format](https://www.nesdev.org/wiki/INES) - Cartridge format specification
- [NES Hardware Documentation](https://www.nesdev.org/wiki/NES_Hardware) - Hardware architecture
- [SDL3 Documentation](https://wiki.libsdl.org/SDL3/) - Graphics and input library

## Troubleshooting

**Issue: No display**
- Verify SDL3 is installed (`brew install sdl3`)
- Check window creation succeeded in SDL_Log output
- Ensure graphics driver supports OpenGL or Vulkan

**Issue: Input not working**
- Check keyboard layout matches expected mapping
- Try gamepad if keyboard not responding
- Verify SDL3 event loop is active

**Issue: Build fails**
- Ensure all source files are present in `Source/NES/`
- Verify SDL3 installation: `brew list sdl3`
- Check compiler supports C++17: `clang++ --version`

## Future Enhancements

- [ ] Complete 6502 CPU instruction set
- [ ] Full PPU rendering pipeline (sprites, backgrounds)
- [ ] Cartridge mapper support (MMC1, MMC3, etc.)
- [ ] Audio (APU) emulation
- [ ] Save state system
- [ ] Debugger with disassembler
- [ ] Game compatibility list
- [ ] Controller configuration UI
- [ ] Cheat code support
- [ ] Super Famicom (SNES) extension
