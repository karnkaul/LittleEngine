# Little Engine

Written in **C++14**, utilising [**SFML v2.5.1**](https://www.sfml-dev.org/) and [**PhysicsFS 3.0.1**](https://icculus.org/physfs/), Little Engine is a hobby project with little defined ambition or scope; in other words, let's see where we can take this!

## Using LittleEngine
### Requirements

#### To Run
1. Windows 7/8/10 64-bit
1. [Microsoft VC++ Runtime (x64)](https://support.microsoft.com/en-us/help/2977003/the-latest-supported-visual-c-downloads)

#### To Build Source
1. Microsoft Visual Studio 15 (2017) / 16 (2019)
1. [LLVM for Windows](http://releases.llvm.org/download.html)
1. [VS2017/2019 LLVM Compiler Toolchain plugin](https://marketplace.visualstudio.com/items?itemName=LLVMExtensions.llvm-toolchain)
1. Windows 10 SDK

>*Note: Ensure to unzip all libraries before linking.*

### Introduction
**LittleEngine** uses two core threads: the main thread - integrates at a fixed time slice, and a render thread - runs at the display's refresh rate (via VSYNC) and interpolates between render states to draw entities on screen. Depending on CPU availability, there will be a file logging thread as well, and a number of job workers for gameplay code to delegate long-running tasks without blocking the main thread. As an example, asynchronous loading of assets is performed entirely via the job system.

>*Note: There must be at least one worker for the engine to start (without async rendering); thus the Engine cannot be run on single-threaded machines.*

The Engine offers multiple build configurations for debugging / authoring content / public redistribution. It uses an `Entity/Component` architecture for organising gameplay objects, where either can be subclassed. All Entities and Components are spawned and owned at game-time by an active `World`, which can be changed at any time, and any number of `Worlds` can be added to the Engine before running it. Assets are expected to be cooked into a zip archive and will be loaded in-memory at game-time, though non `SHIPPING` builds can directly load assets from the filesystem as well.

### Build Configurations

Config | CRT | DEBUGGING | SHIPPING | Optimisation | Disk Assets
-------|-----|-----------|----------|--------------|--------------------
**Debug**       | MSVCRTd   | True  | False     | None  |   True
**Development** | MSVCRT    | True  | False     | None  |   True
**Release**     | MSVCRT    | True  | True      | O2    |   False
**Ship**        | MSVCRT    | False | True      | O2    |   False

### Asset Management
Before activating `World 0`, the Engine first loads assets into memory.

When not `SHIPPING`, the Engine can load assets into memory at any time, and directly through the filesystem. It is recommended to set a root directory such as GameAssets and store all assets hierarchically there. In `SHIPPING` builds, the Engine will only load assets through a compressed archive named `GameAssets.cooked`, which should be a zip archive of the root assets directory.
>*Expect warning logs for loading assets not present in `GameAssets.cooked` (only possible if `!SHIPPING`).*

The Engine needs an `AssetManifest` serialised `GData` object in order to locate relevant bytes within the cooked archive, and expects this to be in the root directory of the archive, named `Manifest.amf`. `AssetCooker.py` is a tool that can be used to automate populating and generating `GameAssets.cooked` via all the assets listed in `Manifest.amf`. The expected workflow is to ensure all assets referenced by the game are in the manifest (critical), and then to simply run the tool, which will backup the cooked archive if it already exists before creating a new one.
>*Expect warning logs and potential hitches on calling `Load<T>` for assets not present in `Manifest.amf`.*

### Running the Engine
1. Add gameplay code files to "LEGame/Gameplay/", including at least one `World` derived class ("world 0").
1. Add any new world headers to "LEGame/Gameplay/Worlds.h"
1. Add `CreateWorld<T>` / `CreateWorlds<T>` to `GameInit::CreateWorlds()`. (This callback will be invoked by `GameLoop` before the Engine is started.)

>*Note: Ensure to set the working directory for the application project (`LittleGame`) as `$(ProjectDir)/Runtime`, to debug/run from the IDE.*

An active `World` will always have an instance of `GameManager` available through `g_pGameManager`, which is capable of spawning new `Entity`s and its subclasses, as well as new `AComponent` subclasses, attached to existing `Entity`s. All the base gameplay classes have a `Super` typedef. The Engine will call `World::Tick(Time dt)` at a fixed time slice, set in `EngineConfig`. `World` will then call `Entity::Tick()` and `Component::Tick()` on all active objects. Derived `World`s can execute code on either side of their call to `World::Tick()` / `Super::Tick()` in case of overrides.

`GameManager` owns an instance of `UIManager`, which manages a stack of `UIContext` objects. Each `UIContext` can contain numerous `UIElements` (base UI object) `UIWidgets` (navigatable and interactable `UIElements`). `GameManager` also provides pointers to `Input`, `UI`, `Renderer`, `Context`, `WSM`, `WorldCamera`, and `Physics` services.

At the end of each `Tick()` cycle, the current state of all primitives will be copied into `GFXBuffer` for the renderer to interpolate between until the next `Tick()`.

There are several existing Entities, Components, UI Widgets and UI Contexts ready for use in "LEGame/Framework/UI".

>*Gameplay code should only update `SFPrimitive::SFRenderState`, and never directly call `draw()`*

### Solution Structure

**Project**         | **Description**
--------------------|----------------
**ThirdParty**      | All dependencies that are compiled from source, like PhysicsFS
**_PCH**            | Pre-compiled header `stdafx.h/cpp` (shared across all downstream projects)
**Core**            | Core structures and utilities used by all projects
**SFMLAPI**         | Wrapper classes for and implementations of SFML libraries
**LittleEngine**    | All Engine code: Context, Audio, Input, Physics, Renderer, Repository, etc.
**LEGame**          | All Gameplay code: World, Entity, Component, World, UI, Framework, etc. Add Worlds and scripts here.
**LEApp**           | Game Loop

## Misc

#### Backlog / Changelog

- [x] Custom 2D coordinate system
- [x] Primitive Render States
- [x] Dynamic game Entities
- [x] Dynamic game Components
- [x] Collision Detection
- [x] Sounds & Music
- [x] Engine Repository (for asset management)
- [x] In-game Console (when `DEBUGGING`)
- [x] Player-modifiable game-settings file
- [x] Debug-only engine-settings file
- [x] In-memory Asset Loads (through a cooked/compressed archive)
- [x] UI Framework
- [x] Gameplay UI classes
- [x] Gameplay Camera (root `Transform` for all `Entity`s)
- [x] Runtime configurable/recreatable Render Window
- [x] Options Menu
- [x] Vertex-Array particle systems
- [x] Asset Cooker tool
- [x] Application Packager tool
- [x] Containerised UI layout via files
- [ ] Animation system
- [ ] Collision Resolution

**To use another version of SFML and/or another IDE/compiler(/OS):**
1. Obtain the corresponding libraries from [SFML's website](https://www.sfml-dev.org/download/sfml/2.5.1/)
1. Set up all the projects as outlined below. Each project must (recursively) add the root directories of all dependent projects to their include paths. All projects emit static libraries and use `stdafx.h` as their pre-compiled header, except where mentioned:
    1. Set up `./PhysicsFS` as a `ThirdParty` static library project, with no PCH
    1. Set up `./PCH` where `stdafx.cpp` generates a `.pch` file
    1. Set up `./Core` that references both the above
    1. Set up `./SFMLAPI` that references `Core`
    1. Set up `./LittleEngine` that references `SFMLAPI`
    1. Set up `./LEGame` that references `LittleEngine`
    1. Set up `./LEApp` as a 64-bit application that references `LEGame`
    1. Add all leaf directories in `./Libraries/...` to the linker's library search paths
    1. Set up any/all of `Debug`, `Development`, `Release`, `Ship` build configurations as outlined [above](#solution-structure)
    1. All pre-processor macros and external `.lib`s that any code files reference are defined / `#pragma`d in source code itself, and need not be defined/added at the project level
1. Build LittleGame
