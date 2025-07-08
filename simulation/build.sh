#!/bin/bash
rm -rf build
mkdir build && cd build

if [[ "$OSTYPE" == "msys"* ]]; then
    echo "Building for Windows..."
    cmake -G "MinGW Makefiles" ..
    make
    executable="device1_sim.exe"
    # I am using Git Bash(MSYS) 
    if [[ -n "$MSYSTEM" ]]; then
        echo "Running in Git Bash"
        winpty ./$executable
    else
        ./$executable
    fi
    
elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
    echo "Building for Linux..."
    cmake ..
    make
    ./device1_sim
    
else
    echo "Unsupported OS: $OSTYPE"
    exit 1
fi