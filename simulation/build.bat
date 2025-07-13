@echo off
setlocal enabledelayedexpansion

set GENERATOR="MinGW Makefiles"

:: Check for Visual Studio 2022
where msbuild >nul 2>&1
if !errorlevel! == 0 (
    set GENERATOR="Visual Studio 17 2022"
    set CMAKE_ARCH=-A x64
)

:: If VS not found, try MinGW
if "!GENERATOR!"=="" (
    where gcc >nul 2>&1
    if !errorlevel! == 0 (
        set GENERATOR="MinGW Makefiles"
    )
)

if "!GENERATOR!"=="" (
    echo ERROR: No compatible compiler found. Install either:
    echo   - Visual Studio 2022 with C++ tools
    echo   - OR MinGW with gcc in PATH
    exit /b 1
)

echo Generating build files with !GENERATOR!...
cmake -S . -B build -G !GENERATOR! !CMAKE_ARCH!
if !errorlevel! neq 0 (
    echo ERROR: CMake generation failed
    exit /b !errorlevel!
)

echo Building project...
cmake --build build --config Debug
if !errorlevel! neq 0 (
    echo ERROR: Build failed
    exit /b !errorlevel!
)

echo Build successful!
endlocal

start .\ device_system.exe