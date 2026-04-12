# AHRetroEmuMacOS

A retro console emulator for iOS built with C++ and SDL3.

## Description

AHRetroEmuiOS is a iOS application that provides retro gaming emulation functionality. It serves as the macOS platform target for the AHRetroEmu project.

## Requirements

- iOS 15.6 or later
- Xcode 15.0 or later
- SDL 3.4.4 source code

## Creating Xcode Project from SDL3

Follow these steps to create a new iOS Xcode project with SDL3.
[SDL Install.md](https://github.com/libsdl-org/SDL/blob/release-3.4.4/INSTALL.md)

## Using Existing Xcode Project

The pre-configured project is located in `Build/AHRetroEmuiOS/AHRetroEmuiOS.xcodeproj`

### Using Xcode

Open the project in Xcode and build:

```bash
open AHRetroEmuiOS.xcodeproj
```

Then press Cmd+R to build and run.

## Project Structure

```
AHRetroEmu/
├── Source/              # C++ source files
├── Build/
│   └── AHRetroEmuiOS/ # iOS Xcode project
│       └── AHRetroEmuiOS.xcodeproj/
```

## License

See project root for license information.