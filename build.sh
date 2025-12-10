#!/bin/bash
# build.sh - Paul Brain Modem Core Build Script (Linux)
# Usage: ./build.sh

set -e

echo "============================================"
echo "Paul Brain Modem Core - Build (Linux)"
echo "============================================"

CXX="g++"
CXXFLAGS="-std=c++17 -O2 -I. -Im188110a -D_USE_MATH_DEFINES"
LDFLAGS="-lpthread"

SOURCES="
    src/main.cpp
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

OUTPUT="brain_modem_server"

# Create output directory
mkdir -p tx_pcm_out

echo ""
echo "Compiling..."

$CXX $CXXFLAGS -o $OUTPUT $SOURCES $LDFLAGS

echo ""
echo "Build successful: $OUTPUT"
ls -lh $OUTPUT

echo ""
echo "============================================"
echo "Usage:"
echo "  ./$OUTPUT"
echo ""
echo "Connect to:"
echo "  Control: localhost:3999"
echo "  Data:    localhost:3998"
echo "============================================"
