# Chip-8 Emulator

A virtual machine / interpreter for the Chip-8 programming language.

## Overview

The Chip-8 emulator is a faithful implementation of the Chip-8 virtual machine, which was originally designed to make games portable across different home computers in the 1970s and 1980s. Today, it's commonly used as a learning tool for emulator development.

## Features

- **Full Chip-8 Instruction Set**: All 16 instructions properly implemented
- **Display**: 64x32 pixels monochrome graphics
- **Keyboard**: 16-key hexadecimal keypad input support
- **Sound**: Beep and buzz audio support
- **Graphics & Input**: SDL3-based rendering and event handling
- **Timing**: Synchronized CPU, timers, and display at 60Hz
- **Performance**: Optimized execution with accurate cycle timing

## Building

From project root:

```bash
make ch8       # Build Chip-8 emulator only
make clean     # Clean build artifacts
```

Output: `Build/CH8/AHCH8EMU`

## Running

```bash
# Run without ROM (displays test pattern)
Build/CH8/AHCH8EMU

# Run with specific ROM
Build/CH8/AHCH8EMU 5-quirks.ch8

# Build and run
make run-ch8
```

## Controls

The Chip-8 uses a 16-key hexadecimal keypad mapped as follows:

| Chip-8 Key | Keyboard |
|------------|----------|
| 0          | 0        |
| 1-F        | 1-9, Q-V |
| -          | ESC      |

**Key Layout:**
```
Chip-8:     Keyboard:
1 2 3 C     1 2 3 4
4 5 6 D     Q W E R
7 8 9 E     A S D F
A 0 B F     Z X C V
```

**Special Keys:**
- **ESC**: Quit emulator
- **Any Key**: General input

## Test ROMs

All included test ROMs execute successfully:

| ROM | Status | Description |
|-----|--------|-------------|
| 1-chip8-logo.ch8 | ✅ PASS | Chip-8 logo display test |
| 2-ibm-logo.ch8 | ✅ PASS | IBM logo display test |
| 3-corax+.ch8 | ✅ PASS | Corax+ graphics test |
| 4-flags.ch8 | ✅ PASS | Flag display test |
| 5-quirks.ch8 | SLOW | Quirks compatibility test |
| 6-keypad.ch8 | 2 / 3 PASS | Keypad input test |
| 7-beep.ch8 | ✅ PASS | Audio beep test |

Test ROMs are located in: `../../Resources/CH8/`

## Architecture

### Source Files

- **ch8-emu.h**: Core emulator definitions and interfaces
- **ch8-emu.cpp**: CPU emulation core (64KB RAM, registers, opcodes)
- **ch8-emu-main.cpp**: SDL3 graphics, input handling, and main loop

### Emulator State

**Memory:**
- 4KB program space (0x200-0xFFF)
- 16 general-purpose registers (V0-VF)
- Index register (I)
- Program counter (PC)
- Stack (nested subroutine calls)

**Timers:**
- Delay timer (60Hz decrement)
- Sound timer (60Hz decrement, plays tone when > 0)

**Display:**
- 64x32 pixels
- Monochrome (black/white)
- Direct pixel access for XOR drawing

## Performance Characteristics

- **CPU Frequency**: ~60Hz (17ms per instruction cycle)
- **Display Refresh**: 60Hz VSync
- **Memory Access**: Direct RAM access pattern
- **Executable Size**: ~37KB (optimized)

## Implementation Details

### Instruction Format

All Chip-8 instructions are 2 bytes (16-bit) in big-endian format:

```
NXYN = Instruction opcode
N = 4-bit value
X, Y = 4-bit register identifiers
KK = 8-bit value
NNN = 12-bit address
```

### Execution Model

1. **Fetch**: Read 2-byte instruction from memory at PC
2. **Decode**: Parse instruction into opcode and operands
3. **Execute**: Perform operation, update state
4. **Increment**: Move PC to next instruction (or jump)
5. **Update Timers**: Decrement delay and sound timers

## Resources & References

- [Cowgod's Chip-8 Technical Reference](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM) - Comprehensive instruction documentation
- [Guide to Making a CHIP-8 Emulator](https://tobiasvl.github.io/blog/write-a-chip-8-emulator/#instructions) - Step-by-step implementation guide
- [Chip-8 Test Suite](https://github.com/Timendus/chip8-test-suite) - Official test ROMs and documentation
- [SDL3 Documentation](https://wiki.libsdl.org/SDL3/) - Graphics and input library

## Troubleshooting

**Issue: No ROM loads**
- Ensure ROM file path is correct and relative to executable directory
- Check `Resources/CH8/` folder for available test ROMs

**Issue: No sound**
- Verify SDL3 audio initialization succeeded
- Check system audio is not muted
- Look for SDL_Log messages about audio stream

**Issue: Key input not working**
- Verify keyboard driver is active
- Check keypad mapping in controls section above
- Some keyboards may have different scan codes on non-US layouts

## Future Enhancements

- [ ] Schip-8 (Super Chip-8) extension support
- [ ] XO-CHIP extended instruction set
- [ ] Configurable quirks/compatibility modes
- [ ] Debugger with breakpoints and step execution
- [ ] ROM library browser
- [ ] Recording and playback functionality
