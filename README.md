
# Introduction

This is a clone of the Quake engine "Mark V", original website: http://quakeone.com/markv/.

# Features

* __Graphics__
  * Direct X9 on Windows, up to 1000 FPS on 2080Ti.
  * Mirror effects / reflections.
  * Built-in AVI video capture & screen capture.
  * Particle effects options.
  * Original "water warp" effect.
  * Optional dynamic shadows enabled with: `r_shadows 3`.
  * _Still stays true to original Quake aesthetic by default._
* __Multi Player__
  * IPv6 support.
  * Co-op features: walk-through player spawn, per-player scoreboard.
* __Mods__
  * Install mods from _Qaddicted_ site using game console: `install http://mod-file-link.zip`.
  * External Texture support.
  * External `.vis` support.
  * Single file, no external DLLs. Easy to distribute with your mods.
  * In-game/realtime/interactive mod tools: view texture information, replace textures, etc.
* __Interface__
  * Mouse menu support, easy to use & clean interface.
  * XBox Controller support.
  * Menu list of levels, and custom maps enable you to quickly select maps.
  * Copy+Paste in game console.
  * Auto-completion in game console (`tab, ctrl+space`).
* __Misc.__
  * Demo rewind.
  * And much more inherited from other game engines.

# Building / Compiling

## Windows

Currently only Windows build system has been tested. 
While the build system needs updating and modernization, currently the following setup should get you started:
* Download "Visual Studio 2019 Community Edition".
* Open `Mark_V/Mark_V_Visual_Studio_2019.sln`.
* __Build Project for Solution Platform `x64` (use `win64` branch)__
  * Working Solution Configurations:
    * DX9 Release
    * GL Release
* __Build Project for Solution Platform `Win32` (use `master` branch)__
  * Working Solution Configurations:
    * GL Release
    * DX9 Release
  * Deprecated Solution Configurations:
    * DX8 Debug
    * DX8 Release
* You must have `id1` game files inside the `Mark_V` directory for the executable to run.


# Contribute

I'm looking for experienced "Quake" engine developers to support updating and developing features for this engine. I will also try to merge new updates and feature from the original "Mark V" engine, when they are added. Please read the *Goals*, and if you believe this project's direction suits your needs or if you can help in any way... you know where that "fork" button is!

# Goals

## Short term

* Bug fixes.
* Update dependencies.
* Update development and build system to contemporary standards.

## Medium term

* Update rendering engine to DX11 / Vulkan / Etc.
* Provide an easy to use and flexible engine to support Quake mapping, modding, as well as full conversion games.
* Maintain easy to use and highly focused feature set.

### Long term
* Provide a "Quake" based engine that is easy to use and flexible for creating new community driven as well as commercial games.

