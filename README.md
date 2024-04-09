# CodeNode Nano Demo
A proof-of-conecpt application the emulates a 6502 based microcontroller for use in Minecraft. You are able to program the MCU in assembly and run the code in real-time with the help of vasm, a 6502 assembler. The MCU is able to interact with four redstone wires and handle interrupts.

![Screencapture](./screenshots/Screenshot%20from%202024-04-09%2002-17-05.png)

## Table of Contents
* [Features](#features)
* [MCU Documentation (How to use)](#mcu-documentation-how-to-use)
* [References](#references)
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

Just click on `Help` in the `CodeNode Nano Demo` window, and click `Documentation`.

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