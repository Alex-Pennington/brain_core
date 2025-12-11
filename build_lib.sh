#!/bin/bash
# build_lib.sh - Build static library from source (Linux/macOS)
# Usage: ./build_lib.sh

set -e

echo "============================================"
echo "Building libm188110a.a"
echo "============================================"

# Check source exists
if [ ! -d "m188110a" ]; then
    echo "ERROR: m188110a/ source directory not found!"
    echo "This directory should contain the proprietary modem core source."
    exit 1
fi

SOURCES="
    m188110a/de110a.cpp
    m188110a/eq110a.cpp
    m188110a/g110a.cpp
    m188110a/in110a.cpp
    m188110a/ptx110a.cpp
    m188110a/rxm110a.cpp
    m188110a/t110a.cpp
    m188110a/txm110a.cpp
    m188110a/v110a.cpp
"

# Verify all sources exist
for src in $SOURCES; do
    if [ ! -f "$src" ]; then
        echo "ERROR: Missing source file: $src"
        exit 1
    fi
done

# Detect platform
UNAME_S=$(uname -s)
case "$UNAME_S" in
    Linux*)  PLATFORM="linux64" ;;
    Darwin*) PLATFORM="macos64" ;;
    MINGW*|MSYS*|CYGWIN*) PLATFORM="win64" ;;
    *) echo "Unknown platform: $UNAME_S"; exit 1 ;;
esac

echo "Platform: $PLATFORM"

# Create lib directory
mkdir -p "lib/$PLATFORM"

echo ""
echo "Compiling object files..."

OBJECTS=""
for src in $SOURCES; do
    obj=$(basename "$src" .cpp).o
    OBJECTS="$OBJECTS $obj"
    echo "  $src -> $obj"
    g++ -c -std=c++17 -O2 -Iinclude/m188110a -D_USE_MATH_DEFINES -o "$obj" "$src"
done

echo ""
echo "Creating static library..."
ar rcs "lib/$PLATFORM/libm188110a.a" $OBJECTS

# Clean up object files
echo ""
echo "Cleaning up..."
rm -f $OBJECTS

echo ""
echo "============================================"
echo "Library built: lib/$PLATFORM/libm188110a.a"
ls -lh "lib/$PLATFORM/libm188110a.a"
echo "============================================"
