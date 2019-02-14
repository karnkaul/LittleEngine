# Little Engine

Built on SFML v2.5 and written in C++, Little Engine is a hobby project with little defined ambition or scope; in other words, let's see where we can take this! 

In the very least, it is desired for the engine to be capable of the following:
1. Have a custom 2D coordinate system [Complete]
1. Have both fixed and variable time step update calls [Complete]
1. Load an arbitrary level at runtime [Complete]
1. Specify a game object's position and orientation in said coordinate space [Complete]
1. Render a game object on screen based on various renderables [Complete]
1. Create dynamic game objects (on the heap) [Complete]
1. Add user created components to game objects at run time [Complete]
1. Have a collision detection system [Complete]
1. Play sounds [Complete]
1. Play seamless music [Complete]
1. Have animated sprites [Pending]
1. Have a collision resolution system [Pending]

## UPDATE
### 2019-01 - 2019-02 Project Refactor
1. Improved Build Configurations, added standalone PCH project for precompiled header, split up main project into multiple static libraries
1. Transitioned to clang + LLVM backend, and to using clang-format
1. Laid foundation of multi-threaded job system, with configurable system and user workers
1. Implemented simple asynchronous render loop that interpolates between previous two game states
1. Implemented system-level optional asserts when DEBUGGING

## Requirements

### To Run [Release pending]
1. Microsoft VC++ Runtime

### To Build Source
1. Microsoft Visual Studio 15 (2017)
1. [LLVM for Windows](http://releases.llvm.org/download.html)
1. [VS2017 LLVM Compiler Toolchain plugin](https://marketplace.visualstudio.com/items?itemName=LLVMExtensions.llvm-toolchain)
1. Windows 7+ SDK

Unzip the SFML libraries before linking.

If you would like to use another version of SFML and/or another IDE/compiler(/OS), you'll need to get the corresponding libraries from SFML's website, and set up your project to include the SFML headers and downloaded libraries, before you will be able to build.
