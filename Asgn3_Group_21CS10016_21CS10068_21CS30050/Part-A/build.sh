#!/bin/bash

# Remove existing build directory
rm -rf build

# Create build directory
mkdir -p build

# Change into build directory
cd build

# Run cmake
cmake ..

# Run make
make