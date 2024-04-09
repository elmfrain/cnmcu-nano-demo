# CodeNode Nano Demo
A proof-of-conecpt application the emulates a 6502 based microcontroller for use in Minecraft. You are able to program the MCU in assembly and run the code in real-time with the help of [vasm](http://sun.hasenbraten.de/vasm/), a 6502 assembler. The MCU is able to interact with four redstone wires and handle interrupts.

![Screencapture](./screenshots/Screenshot%20from%202024-04-09%2002-17-05.png)

## Table of Contents
* [Features](#features)
* [MCU Documentation (How to use)](#mcu-documentation-how-to-use)
* [How to build](#how-to-build)
* [How to run](#how-to-run)
* [Libraries Used](#libraries-used)
* [Misc](#misc)

## Features
* Emulation of a 6502 microcontroller.
* Control of MCU and four redstone wires connected to it.
* CPU and memory status display.
* Built code editor with syntax highlighting (sort-of).
* Ability to load and save code.
* Utilizes vasm to assemble code.
* Upload code to the MCU and run it in real-time.

## MCU Documentation (How to use)
* Official microcontroller documentation can be found here: [CodeNode Microcontroller Documentation](https://elmfrain.github.io/code-node-docs/).
* Instruction set of the 6502: [Ultimate Commodore 64 Reference](https://www.pagetable.com/c64ref/6502/?tab=2#).
* This application allows you to view a table of registers and memory locations:

![Screenshot](./screenshots/Screenshot%20from%202024-04-09%2002-17-52.png)

Goto `CodeNode Nano Demo` > `Help` > `Documentation`.

## How to build
This works for most operating systems, but you need to have CMake installed.

First, clone the repository recursively:
```bash
git clone https://github.com/elmfrain/cnmcu-nano-demo.git --recursive
```

Create a build directory
```bash
mkdir build && cd build
```

Run CMake
```bash
cmake ..
```

Build the project
```bash
make cnmcu-nano-demo
```

On Windows, you can use Visual Studio to build the project. Open the project in the `build` directory and build the `cnmcu-nano-demo` target.

## How to run
Before running the application, you must download and/or build the vasm assembler. Some prebuilt binaries are avaiable [here](http://www.compilers.de/vasm.html), but you may need to build them yourself on some platforms.

You must put `vasm6502_oldstyle` and `vobjdump` (and `cywin.dll` for Windows) binaries in the `toolchain` folder.

```
/ cnmcu-nano-demo
  / toolchain
    / vasm6502_oldstyle
    / vobjdump
    / cywin.dll
```

 If you've built the project yourself, chances are that you need to be in the `build` directory, the structure should look like this:

```
/ cnmcu-nano-demo
  / build
    / toolchain
      / vasm6502_oldstyle
      / vobjdump
      / cywin.dll
```

The application will use these binaries to assemble the code.

Then run the application:
```bash
./cnmcu-nano-demo
```

### Use different toolchain (assembler)
If you wish to use a different assembler, you can change the `compile_command` in the textbox besides the `Upload` button.

![Screenshot](./screenshots/Screenshot%20from%202024-04-09%2017-58-44.png)

`res/program.s` should be set as the input file for your assembler.
Then the output flag should be set at the end of the command, for example:
```bash
vasm6502_oldstyle -Fbin -dotdir -wdc02 res/program.s -o
```
because the application expects `rom.bin` to be the output file.


## Libraries Used
* [GLFW](https://www.glfw.org/) - Windowing and input handling.
* [GLAD](https://glad.dav1d.de/) - OpenGL loader.
* [GLM](https://glm.g-truc.net/0.9.9/index.html) - Math library for OpenGL.
* [Dear ImGui](https://github.com/ocornut/imgui) - GUI library.
* [stb_image](https://github.com/nothings/stb/blob/master/stb_image.h) - Image loading library.
* [Assimp](https://github.com/assimp/assimp) - Model loading library.
* [Lua C API](https://www.lua.org/manual/5.4/manual.html) - For loading the scene.
* [mos6502](https://github.com/gianlucag/mos6502) - 6502 emulator.

## Misc
This project uses code from this tiny game-engine used in [SARE Logo Visualizer](https://github.com/rgv-sare/sare-init-visualizer) that uses Lua to load assets and setup scene composition. Created assets and planned scene composition using [Blender](https://www.blender.org/).