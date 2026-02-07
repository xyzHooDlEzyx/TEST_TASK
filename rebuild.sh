#!/bin/bash

# Clean the build directory
cmake --build --preset Debug --target clean

# Recreate the build directory
cmake --preset Debug

# Build the project
cmake --build --preset Debug
