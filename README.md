# Little Engine

<img align="right" src="https://github.com/karnkaul/LittleEngine/blob/dev/Source/App/App/Resources/Icon.png" width="64" />

Written in **C++17**, utilising [**SFML**](https://www.sfml-dev.org/) and [**PhysicsFS**](https://icculus.org/physfs/), Little Engine is a 2D game engine hobby project with little defined ambition or scope; in other words, let's see where we can take this!

[![Build Status](https://travis-ci.org/karnkaul/LittleEngine.svg?branch=master)](https://travis-ci.org/karnkaul/LittleEngine) [![Licence](https://img.shields.io/github/license/karnkaul/LittleEngine)](LICENSE) [![Twitter](https://img.shields.io/twitter/url/https/karnkaul?label=Follow&style=social)](https://twitter.com/KarnKaul)

### Features
- **CMake** project - develop/build (almost) anywhere
- Full **keyboard**, **mouse** and single **joystick** support
- Multiple 2D coordinate systems (world space, UI space, viewport) using **3x3 matrices**
- **Quad**, **Quads**, **Rectangle**, **Circle**, and **Text** primitives
- **Asynchronous rendering** on a dedicated thread; light main/game thread
- Dynamic game **Entities/Components** with **Circle** and **AABB** collision detection
- Cache-friendly **Particle Systems** with distributed updation
- Engine **Repository** with async loading and asset manifests
- In-game **Console**, **Tweakable** variables, **Profiler**, and other debugging tools
- Stack and Context-based **UI Framework**
- Various Python3 tools for **asset cooking**, **app packaging**, **bundle archiving**, etc

>*Note: For a full game demo using LittleEngine, check out [LEDemo](https://github.com/karnkaul/LEDemo)!*

### Development
Visit the [Little Engine wiki](https://github.com/karnkaul/LittleEngine/wiki/Development) for more detailed and per-platform instructions.

#### Requirements
1. x64 (ARMv8 is supported but not packaged) multi-threaded target
1. [CMake](https://cmake.org/download/) for the target, preferably using [Ninja](https://ninja-build.org/) and [LLVM](https://llvm.org/)
1. [SFML 2.5.1](https://www.sfml-dev.org/download/sfml/2.5.1/) source / internet access (CMake will download it)
1. (Linux) libstdc++-7; all SFML dependencies (X11, Xrandr, OpenGL, UDev, Freetype, Vorbis, Flac, etc)
1. (Win 10) [Windows SDK](https://developer.microsoft.com/en-us/windows/downloads/windows-10-sdk) for ninja/MSBuild / Clang/VC++ or [MinGW](http://www.mingw.org/) for MinGW / g++
1. (MacOSX) Xcode
1. (Optional) Bash (native/MinGW/WSL), [Python 3](https://www.python.org/downloads/) (all tools scripts are in Bash/Python)

#### Setting up the project(s)
Quick Start:
1. Build `LittleEngine`
    1. CMake: generate a project using `CMakeLists.txt`; set `CMAKE_BUILD_TYPE` to desired build configuration
    1. Build
1. Build a game using `LittleEngine`:
    1. Add `LittleEngine` as a git submodule to the main repository
    1. In the top-level `CMakeLists.txt`, set up the following:
        1. Add the subdirectory (`add_subdirectory(LittleEngine)`)
        1. Add `Engine` as a build dependency and link library for the executable target
        1. Use `add_le_executable(EXE_NAME SOURCES)` (defined in `Common.cmake`) to set up include paths, compile flags, etc
    1. Debug/run the built/installed executable

### Contact

* [Twitter](https://twitter.com/KarnKaul)

### Licence
LittleEngine uses the [GNU GPLv3 licence](LICENSE).

>*Note: Code of Conduct can be found [here](CODE_OF_CONDUCT).*
