#!/bin/bash
rm -rf build
mkdir build && cd build

if [[ "$OSTYPE" == "msys"* ]]; then
    echo "Building for Windows..."
    cmake -G "MinGW Makefiles" ..
    make
    executable="device_system.exe"
    
    # I am using Git Bash
    if [[ -n "$MSYSTEM" ]]; then
        echo "Running in Git Bash"
        
        if command -v winpty &> /dev/null; then
            winpty ./$executable
        else
            echo "winpty not found - capturing output to log"
            ./$executable > output.log 2>&1
            cat output.log
        fi
    else
        ./$executable
    fi
    
elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
    echo "Building for Linux..."
    cmake ..
    make
    ./device_system
    
else
    echo "Unsupported OS: $OSTYPE"
    exit 1
fi