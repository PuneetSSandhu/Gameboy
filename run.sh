#!/bin/sh

# run the GB emulator in the build directory
cd build
make

./GameboyEmulator roms/Tetris.gb