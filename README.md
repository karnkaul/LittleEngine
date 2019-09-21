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
- Engine **Repository** with async loading, asset manifests, and sounds & music playback
- In-game **Console**, **Tweakable** variables, **Profiler**, and other debugging tools
- Stack and Context-based **UI Framework**
- Various Python3 tools for **asset cooking**, **app packaging**, **bundle archiving**, etc

### Installation
LittleEngine pre-releases containing a demo game with Win64, Linux-x64, and MacOSX binaries can be found [here](https://github.com/karnkaul/LittleEngine/releases).

#### Requirements
1. x64 CPU with at least two threads
1. (Optional) Python 3.5+ (for `installer.py`)
1. Operating System:
    1. Windows 7/8/10: [Microsoft VC++ Runtime (x64)](https://support.microsoft.com/en-us/help/2977003/the-latest-supported-visual-c-downloads)
    1. Linux: OpenAL libraries (`libopenal-dev`)
    1. MacOSX: Python 3 (app bundles are unsigned; `installer.py` eases the UX)

#### Installing LittleEngine
1. Obtain the game zip (present inside a [GitHub Release](https://github.com/karnkaul/LittleEngine/releases) zip)
1. Unzip it to the installation directory and run `installer.py` via Python 3 (or unzip ".game" manually and copy the contents of "[Your OS]" into the installation directory)

> *Note: OSX builds are in alpha and may not be stable.*

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

>*Note: Due to GitHub's limitations with (free) LFS, Runtime Assets have been moved to DropBox since 0.4.4.0.*

#### Setting up the project(s)
Quick Start:
1. Build `ThirdParty`
    1. CMake: generate a project using `Source/ThirdParty/CMakeLists.txt`
    1. Build `Debug` and `Release`/`RelWithDebInfo`
1. Build `LittleEngine`
    1. CMake: generate a project using `Source/CMakeLists.txt`
    1. Build / install (copies executable and libraries to `Runtime`)
1. Run `LittleEngine`
    1. Set working directory to `Runtime`
    1. Extract and merge [Runtime Assets](https://www.dropbox.com/sh/ygwprt87nr4sigm/AABKD-84WKMNXtyM4LNcnH3ca?dl=0) into `Runtime`
    1. Debug/run the built/installed executable

[Short video](https://youtu.be/Ox5c96EflZU) demonstrating full project setup from scratch on Linux (v0.4.7.0).

### Contact

* [Twitter](https://twitter.com/KarnKaul)

### Licence
LittleEngine uses the [GNU GPLv3 licence](LICENSE).

>*Note: Code of Conduct can be found [here](CODE_OF_CONDUCT).*
