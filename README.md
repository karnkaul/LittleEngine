# Little Engine

Built on SFML v2.5 and written in C++, Little Engine is a hobby project with little defined ambition or scope; in other words, let's see where we can take this! 

In the very least, it is desired for the engine to be capable of the following:
1. Have a custom 2D coordinate system [Complete]
2. Have both fixed and variable time step update calls [Complete]
3. Load an arbitrary level at runtime [Complete]
4. Specify a game object's position and orientation in said coordinate space [Complete]
5. Render a game object on screen based on various renderables [Complete]
6. Create dynamic game objects (on the heap) [Complete]
7. Add user created components to game objects at run time [Complete]
8. Have a collision detection and resolution system [Pending]
9. Play sounds [Pending]
10. Play seamless music [Pending]

## Requirements

### To Run [Release pending]
1. Microsoft VC++ Runtime

### To Build Source
1. Microsoft Visual Studio 15 (2017)
2. Windows 10 SDK

Unzip the SFML libraries (in /lib_x86 and /lib_x64) before linking.

If you would like to use another version of SFML and/or another IDE/compiler(/OS), you'll need to get the corresponding libraries from SFML's website, and set up your project to include the SFML headers and downloaded libraries, before you will be able to build.
