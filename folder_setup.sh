#!/bin/bash

set -e

PROJECT_DIR="freeRTOS_OS"
FREERTOS_REPO="https://github.com/FreeRTOS/FreeRTOS.git"
KERNEL_REPO="https://github.com/FreeRTOS/FreeRTOS-Kernel.git"
BUILD_DIR="${PROJECT_DIR}/simulation"

mkdir -p "${PROJECT_DIR}"
cd "${PROJECT_DIR}"

git clone "${FREERTOS_REPO}"

# Navigate into the FreeRTOS/FreeRTOS/Source directory
cd FreeRTOS/FreeRTOS/Source

# Create a temporary directory for the kernel clone
mkdir -p temp_kernel
cd temp_kernel

# Clone the FreeRTOS-Kernel into the temporary directory
git clone "${KERNEL_REPO}" .

# Move all contents from the cloned repository directly into the parent Source directory
mv * ../

# Navigate back up and remove the temporary directory
cd ..
rm -rf temp_kernel

# Navigate back to the root of the freeRTOS_OS directory
cd ../../../

echo "Checking for CMake installation..."
if command -v cmake &> /dev/null; then
    echo "CMake is already installed."
else
    echo "CMake not found. Attempting to install..."
    if command -v apt-get &> /dev/null; then
        sudo apt-get update
        sudo apt-get install -y cmake
    elif command -v pacman &> /dev/null; then
        sudo pacman -Syu --noconfirm cmake
    elif command -v dnf &> /dev/null; then
        sudo dnf install -y cmake
    else
        echo "Error: Unsupported package manager. Please install CMake manually."
        exit 1
    fi

    if ! command -v cmake &> /dev/null; then
        echo "Error: Failed to install CMake. Exiting."
        exit 1
    fi
    echo "CMake successfully installed."
fi

echo "Cleaning up FreeRTOS source files..."
cd FreeRTOS/FreeRTOS/Source/portable

for item in *; do
    if [[ "${item}" != "GCC" && "${item}" != "MemMang" ]]; then
        echo "Deleting: ${item}"
        rm -rf "${item}"
    else
        echo "Keeping: ${item}"
    fi
done

cd GCC
for item in *; do
    if [[ "${item}" != "ARM_CM4F" ]]; then
        echo "Deleting: ${item}"
        rm -rf "${item}"
    else
        echo "Keeping: ${item}"
    fi
done

cd ../MemMang
for item in *; do
    if [[ "${item}" != "heap_4.c" ]]; then
        echo "Deleting: ${item}"
        rm -rf "${item}"
    else
        echo "Keeping: ${item}"
    fi
done

echo "Returning to simulation directory and building the project..."
cd ../../../../../../simulation

if [ -f "build.sh" ]; then
    chmod +x "build.sh"
    chmod +x "CMakeLists.txt"
    ./build.sh
else
    echo "Error: build.sh not found in the simulation directory. Please ensure it exists."
    exit 1
fi

echo "Script finished successfully."