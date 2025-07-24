#!/bin/bash

echo "Building Wave Simulation..."

# Create build directory
mkdir -p build
cd build

# Run CMake
cmake ..

# Build the project
make

echo "Build complete! Run with: ./build/wave_simulation"