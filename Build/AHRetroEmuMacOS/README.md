# AHRetroEmuMacOS

A retro console emulator for macOS built with C++ and SDL3.

## Description

AHRetroEmuMacOS is a macOS application that provides retro gaming emulation functionality. It serves as the macOS platform target for the AHRetroEmu project.

## Requirements

- macOS 12.0 or later
- Xcode 15.0 or later
- SDL3 source code

## Creating Xcode Project from SDL3

Follow these steps to create a new macOS Xcode project with SDL3:

1. **Download SDL3 source code**
   ```bash
   git clone https://github.com/libsdl-org/SDL.git
   cd SDL
   git checkout release-3.4.4
   ```

2. **Create new Xcode project**
   - Open Xcode and create a new project using the App template
   - Select "Command Line Tool" or "App" template with C++ as language
   - Choose macOS as the platform

3. **Add SDL as subproject**
   - Right-click the project in Xcode navigator
   - Select "Add Files to [project]"
   - Navigate to `SDL/Xcode/SDL/` and add `SDL.xcodeproj`

4. **Link SDL3 framework**
   - Select the project, then the target
   - Go to "General" tab > "Frameworks, Libraries, and Embedded Content"
   - Add `SDL3.framework`

5. **Configure build settings**
   - Go to "Build Settings" > "Search Paths"
   - Add header search path: `$(SRCROOT)/../SDL/include`
   - Add library search path: `$(SRCROOT)/../SDL/Xcode/SDL/build/Debug`

6. **Build and run**
   - Press Cmd+R to build and run

## Using Existing Xcode Project

The pre-configured project is located in `Build/AHRetroEmuMacOS/AHRetroEmuMacOS.xcodeproj`

## Building

### Using Makefile

```bash
cd Build/AHRetroEmuMacOS
make build    # Build the project
make run     # Build and run the app
make clean   # Clean build artifacts
```

### Using Xcode

Open the project in Xcode and build:

```bash
open AHRetroEmuMacOS.xcodeproj
```

Then press Cmd+R to build and run.

## Project Structure

```
AHRetroEmu/
├── Source/              # C++ source files
├── Build/
│   └── AHRetroEmuMacOS/ # macOS Xcode project
│       ├── Makefile
│       └── AHRetroEmuMacOS.xcodeproj/
```

## License

See project root for license information.