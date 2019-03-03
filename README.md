# Little Engine

Written in C++14, utilising SFML v2.5.1 and PhysicsFS 3.0.1, Little Engine is a hobby project with little defined ambition or scope; in other words, let's see where we can take this! 

## Intended Usage
### Build Configurations
1. Debug: Uses MSVCRTd (and SFML debug libraries)
1. Development: Uses MSVCRT, has DEBUGGING enabled, SHIPPING disabled, is not optimised
1. Release: Uses MSVCRT, has DEBUGGING and SHIPPING enabled, is fully optimised
1. Shipping: Uses MSVCRT, has SHIPPING enabled, is fully optimised

### Asset Management
Before loading the World 0, the engine first loads assets into memory.

When !SHIPPING, the engine can load assets into memory at any time, and directly through the filesystem. It is recommended to set a root directory such as GameAssets and store all assets hierarchically there. In SHIPPING builds, the engine will only load assets through a compressed archive named "GameAssets.cooked", which is simply a zip archive of the root assets directory. Expect warning logs for loading assets not present in cooked archives.

The engine needs an AssetManifest to locate relevant bytes within the archive, and expects this to be in the root directory of the cooked archive, named "Manifest.minified". The file must not contain any spaces, tabs, quotes, newlines, or any other special characters (it is read directly as bytes). For convience you could maintain a pretty version in the assets directory for editing during development, and copy its contents to Manifest.minified and minify it, before creating the cooked archive. Expect warning logs for loading assets not present in the manifest.

### Using the Engine
The engine uses Worlds to organise flow and entities, and uses a state machine to manage the active World. Naturally, there must be at least one world for the engine to run.

1. Create an instance of the engine using EngineLoop::Create()
1. Add derived World classes in order using Services::Engine()->Worlds()->CreateWorld<T>();
1. Run the engine by calling EngineLoop::Run() on the created instance

### Gameplay Code
A World is capable of spawning new Entities and its subclasses, as well as new Components and subclasses, attached to existing Entities. The Engine will call World::Tick(Time dt) at a fixed time slice, set in EngineConfig. World will then call Entity::Tick() and Component::Tick() on all active objects.

A World also contains a UIManager, which manages a stack of UIContext objects. Each UIContext can contain numerous UIElements (base UI object) UIWidgets (navigatable and interactable UIElements). There are several existing UIWidgets and UIContexts ready for use in GameplayFramework.

## What is this?
In the very least, it is desired for the engine to be capable of the following:
1. Have a custom 2D coordinate system [Complete]
1. Load an arbitrary world at runtime [Complete]
1. Specify a game object's position and orientation in said coordinate space [Complete]
1. Render a game object on screen based on various renderables [Complete]
1. Create dynamic game objects (on the heap) [Complete]
1. Add user created components to game objects at run time [Complete]
1. Have a collision detection system [Complete]
1. Play sounds [Complete]
1. Play seamless music [Complete]
1. Use a featured Engine Repository for asset management [Complete]
1. Have in game Console when debugging [Complete]
1. Use player-modifiable game-settings file [Complete]
1. Use Debug-only engine-settings file [Complete]
1. Load Assets in memory through a cooked/compressed archive on filesystem [Complete]
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
