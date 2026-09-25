# Development Guide

## Requirements
- CMake 3.20+
- C++20 compiler
- Ninja recommended.

## Local build
Configure: cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
Build: cmake --build build --config Release
Tests: ctest --test-dir build --output-on-failure -C Release
Run: ./build/auto-animation

On Windows with a multi-config generator, the executable may be under build/Release/auto-animation.exe.

## Conventions
- C++20.
- Warnings enabled.
- Public core headers live under core/include/auto_animation.
- Implementations live under matching src directories.
- Domain logic remains independent from UI.
- Add a focused automated test with each new core behavior.
