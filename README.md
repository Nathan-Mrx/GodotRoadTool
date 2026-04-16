# Godot Networking

A simple project including a Godot from sources, a game project and a GDExtension setup with SCons.

## CMake wrapper targets

This repository now exposes three top-level CMake targets that wrap the existing SCons flow:

- `build`: builds the engine, refreshes `extension_api.json`, rebuilds `godot-cpp`, then builds the extension.
- `editor`: launches the editor on `game/`.
- `play`: runs the game project in `game/`.

Quick start:

```bash
cmake --preset default
cmake --build --preset build
cmake --build --preset editor
cmake --build --preset play
```

Equivalent explicit commands:

```bash
cmake -S . -B build-cmake
cmake --build build-cmake --target build
cmake --build build-cmake --target editor
cmake --build build-cmake --target play
```

Useful cache options:

- `GRT_PLATFORM` (default: `linuxbsd`)
- `GRT_GDEXT_PLATFORM` (default: `linux` when `GRT_PLATFORM=linuxbsd`, otherwise follows `GRT_PLATFORM`)
- `GRT_ARCH` (default: `x86_64`)
- `GRT_JOBS` (default: host logical core count)
- `GRT_DEV_BUILD` (default: `ON`)
- `GRT_ENGINE_BIN` (override the Godot editor binary path)
- `GRT_GAME_DIR` (override project path, default: `game/`)
