#!/bin/bash

echo "🚀 1. Building Custom Godot Editor (Linux x86_64, 22 cores)..."
cd engine
# dev_build=yes allows deep debugging
scons platform=linuxbsd arch=x86_64 target=editor dev_build=yes -j22
cd ..

echo "📝 2. Dumping API JSON from the fresh binary..."
# We run the binary we just built with --dump-extension-api
./engine/bin/godot.linuxbsd.editor.dev.x86_64 --headless --dump-extension-api

echo "📦 3. Moving JSON to godot-cpp..."
# We overwrite the default JSON so bindings match YOUR binary exactly
mv extension_api.json src/godot-cpp/gdextension/extension_api.json

echo "🔗 4. Building C++ Bindings..."
cd src/godot-cpp
scons platform=linuxbsd arch=x86_64 target=template_debug -j22

echo "🔗 5. Generating the compile_commands.json..."
cd ..
scons platform=linuxbsd arch=x86_64 target=template_debug compiledb=yes -j22

cd ../..

echo "✅ Setup Complete. You are perfectly synced."