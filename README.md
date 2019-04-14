# Little Engine

Written in **C++14**, utilising [**SFML v2.5.1**](https://www.sfml-dev.org/) and [**PhysicsFS 3.0.1**](https://icculus.org/physfs/), Little Engine is a hobby project with little defined ambition or scope; in other words, let's see where we can take this!

## Using LittleEngine
### Requirements

#### To Run
1. Microsoft VC++ Runtime

#### To Build Source
1. Microsoft Visual Studio 15 (2017)
1. [LLVM for Windows](http://releases.llvm.org/download.html)
1. [VS2017 LLVM Compiler Toolchain plugin](https://marketplace.visualstudio.com/items?itemName=LLVMExtensions.llvm-toolchain)
1. Windows 7+ SDK

>*Note: Ensure to unzip all libraries before linking.*

### Introduction
**LittleEngine** uses two core threads: the main thread, that Ticks all entities at a fixed time slice, and a render thread, that runs at the display's refresh rate (via VSYNC) and interpolates between render states to draw entities on screen. Depending on CPU availability, there will be a file logging thread as well, and a number of job workers for gameplay code to delegate tasks to. As an example, asynchronous loading of assets is performed entirely via the job system.

The Engine offers multiple build configurations for debugging / authoring content / public redistribution. It uses an `Entity/Component` architecture for organising gameplay objects, where either can be subclassed. All Entities and Components are spawned and owned at game-time by an active `World`, which can be changed at any time, and any number of `Worlds` can be added to the Engine before running it. Assets are expected to be cooked into a zip archive and will be loaded in-memory at game-time, though non `SHIPPING` builds can directly load assets from the filesystem as well.

### Build Configurations

Config | CRT | DEBUGGING | SHIPPING | Optimisation | Disk Assets
-------|-----|-----------|----------|--------------|--------------------
**Debug**       | MSVCRTd   | True  | False     | None  |   True
**Development** | MSVCRT    | True  | False     | None  |   True
**Release**     | MSVCRT    | True  | True      | O2    |   False
**Shipping**    | MSVCRT    | False | True      | O2    |   False

### Asset Management
Before activating `World 0`, the Engine first loads assets into memory.

When not `SHIPPING`, the Engine can load assets into memory at any time, and directly through the filesystem. It is recommended to set a root directory such as GameAssets and store all assets hierarchically there. In `SHIPPING` builds, the Engine will only load assets through a compressed archive named `GameAssets.cooked`, which should be a zip archive of the root assets directory.
>*Expect warning logs for loading assets not present in `GameAssets.cooked`.*

The Engine needs an `AssetManifest` to locate relevant bytes within the cooked archive, and expects this to be in the root directory of the archive, named `Manifest.minified`. (Text files with a `.minified` / `.min` suffix must not contain any spaces, tabs, quotes, newlines, or any other special characters: they are read directly as bytes.) For convenience a pretty version can be maintained alongside for editing during development, with its contents minified and copied to `Manifest.minified` before creating the cooked archive.
>*Expect warning logs and potential hitches on calling Load<T> for assets not present in the `Manifest.minified`.*

### Running the Engine
The Engine uses a state machine to manage the active `World` and switch to a different `World`. Follow these steps in `main()` to set up and run the engine:

1. Create an instance of the Engine using `EngineLoop::Create()`
1. Add derived World classes in order using `Services::Engine()->Worlds()->CreateWorld<T>()`;
1. Run the Engine by calling `EngineLoop::Run()` on the created instance

A `World` is capable of spawning new `Entities` and its subclasses, as well as new `Components` and subclasses, attached to existing `Entities`. The Engine will call `World::Tick(Time dt)` at a fixed time slice, set in `EngineConfig`. `World` will then call `Entity::Tick()` and `Component::Tick()` on all active objects.

A `World` also contains a `UIManager`, which manages a stack of `UIContext` objects. Each `UIContext` can contain numerous `UIElements` (base UI object) `UIWidgets` (navigatable and interactable `UIElements`). There are several existing `UIWidgets` and `UIContexts` ready for use in `GameplayFramework`.

At the end of each `Tick()` cycle, the current state of all primitives will be copied into `GFXBuffer` for the renderer to interpolate between until the next `Tick()`.
>*Gameplay code should only update `SFPrimitive::SFRenderState`, and never directly call `draw()`*

### Solution Structure

**Project**         | **Description**
--------------------|----------------
**_PCH**            | Pre-compiled header `stdafx.h/cpp` (shared across all projects)
**Core**            | Core structures and utilities used by all projects
**SFMLAPI**         | Wrapper classes for and implementations of SFML libraries
**LittleEngine**    | All Engine code
**GameFramework**   | Gameplay classes and implementations
**LittleGame**      | Test Game

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
- [ ] Options Menu
- [ ] Animation system
- [ ] Collision Resolution

**To use another version of SFML and/or another IDE/compiler(/OS):**
1. Obtain the corresponding libraries from SFML's website 
1. Create a [PhysicsFS](https://icculus.org/physfs/) static library for your platform, or add the source as a project (used for archive decompression)
1. Set up all the projects with include paths, files/filters, libraries, and dependencies
1. Build LittleGame