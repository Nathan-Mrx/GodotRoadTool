# Godot Road Tool 🛣️

**Godot Road Tool** is a procedural road and intersection generation plugin with AAA quality for the Godot 4 engine. Developed in C++ with **GDExtension**, it is designed to deliver maximum performance and seamless integration with Level Designer workflows.

Forget tedious manual assembly: draw your splines, and the plugin handles complex geometry, intersection blending, and terrain shaping.

---

## ✨ Key Features

* 🛣️ **Procedural Roads (`ProceduralRoad`):**
    * Mesh generation based on 3D Bezier curves.
    * Capsule-style road profiles (rounded edges) for realistic light response.
    * Parametric lane markings (solid lines, dashed lines, offsets, colors).
    * Full control over thickness, lane width, shoulder width, and camber (tilt).
* 🔄 **Smart Intersections (`ProceduralIntersection`):**
    * N-way connection graph with perfect snapping.
    * **Dynamic interpolation:** Intersections mathematically blend widths and styles (lines, colors) from connected roads.
    * Connection fillets generated with exact cubic Bezier curves.
    * Planar UV projection (world-space) for seamless asphalt.
* ⛰️ **Terrain3D Integration:**
    * *Bake Terrain* feature acting like a virtual bulldozer.
    * Roads and intersections flatten terrain under the asphalt and automatically smooth embankments using a configurable *Falloff* radius.
* 🛡️ **Seamless Physics & Collisions:**
    * Asynchronous generation of `StaticBody3D` and `ConcavePolygonShape3D`.
    * Optimization: white road markings are excluded from the collision mesh to ensure perfectly smooth driving, without micro-bumps for heavy vehicles.

---

## 📦 Dependencies

To use all terrain-shaping capabilities of this tool, your Godot project requires the following plugin:
* **[Terrain3D](https://github.com/tokisan/Terrain3D)** (required for `BakeTerrain` geometric deformation features).

---

## 🚀 Installation (Users)

1. Download the latest release from the **Releases** tab.
2. Extract the `RoadTool` folder into your Godot project's `addons/` directory.
3. Go to `Project -> Project Settings -> Plugins` and enable **Godot Road Tool**.
4. (Make sure the Terrain3D plugin is also installed and enabled.)

---

## 🛠️ Build & Development (C++)

This project uses a **CMake** wrapper on top of the standard Godot-cpp **SCons** build ecosystem to simplify the workflow.

### Prerequisites
* A modern C++ compiler (GCC, Clang, or MSVC).
* CMake (3.20+)
* Python & SCons

### Quick Commands (CMake Presets)

The repository exposes three main high-level CMake targets:
* `build`: Compiles the engine, refreshes `extension_api.json`, recompiles `godot-cpp`, then compiles the extension.
* `editor`: Launches the Godot editor directly on the `game/` folder.
* `play`: Launches the game project in `game/`.

**Quick start:**
```bash
cmake --preset default
cmake --build --preset build
cmake --build --preset editor
cmake --build --preset play
```

**Equivalent explicit commands:**
```bash
cmake -S . -B build-cmake
cmake --build build-cmake --target build
cmake --build build-cmake --target editor
cmake --build build-cmake --target play
```

### Cache Options (Configuration)

You can override these variables during the CMake configure step:

- `GRT_PLATFORM` (default: `linuxbsd`)
- `GRT_GDEXT_PLATFORM` (default: `linux` when `GRT_PLATFORM=linuxbsd`, otherwise follows `GRT_PLATFORM`)
- `GRT_ARCH` (default: `x86_64`)
- `GRT_JOBS` (default: number of host logical CPU cores)
- `GRT_DEV_BUILD` (default: `ON`)
- `GRT_ENGINE_BIN` (overrides the path to the Godot editor binary)
- `GRT_GAME_DIR` (overrides the project path, default: `game/`)

### 🤝 Contributing (Forks & PRs)

Contributions are welcome! Whether you want to optimize spatial geometry algorithms, add editor gizmos, or integrate new procedural props (streetlights, bridges), your help is appreciated.

1. Fork the repository.
2. Create a branch for your feature (`git checkout -b feature/AmazingFeature`).
3. Code while respecting the project philosophy: Performance (C++) and Level Design ergonomics.
4. Commit your changes (`git commit -m 'Add some AmazingFeature'`).
5. Push to your branch (`git push origin feature/AmazingFeature`).
6. Open a Pull Request.

(Note: Please ensure the code compiles with the provided CMake targets before submitting your PR.)

### 📄 License

Distributed under the MIT License. See the LICENSE file for more information.
