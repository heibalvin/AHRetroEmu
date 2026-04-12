# AHRetroEmu

A retro console emulator project written in C++ / SDL3.

## Platforms

| Platform | Path | Description |
|----------|------|-----------|
| **macOS** | `Build/AHRetroEmuMacOS/` | macOS desktop application |
| **iOS** | `Build/AHRetroEmuiOS/` | iOS mobile application |

## Example Applications

- **hello.c** - Basic SDL3 window
- **hello-callbacks.c** - SDL3 callbacks example

## Getting Started

### macOS

See `Build/AHRetroEmuMacOS/README.md` for macOS build instructions.

### iOS

See `Build/AHRetroEmuiOS/README.md` for iOS build instructions.

## Requirements

- SDL3 library (release-3.4.4)
- C++ compiler
- Xcode 15.0+

## Project Structure

```
AHRetroEmu/
├── Source/              # Source files
│   ├── hello.c         # Basic SDL3 example
│   └── hello-callbacks.c # SDL3 callbacks example
├── Build/
│   ├── AHRetroEmuMacOS/   # macOS Xcode project
│   └── AHRetroEmuiOS/    # iOS Xcode project
```

## License

MIT License