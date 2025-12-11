#!/bin/bash
# build.sh - Paul Brain Modem Core Build Script (Linux/macOS)
# Usage: ./build.sh [-i patch|minor|major]

set -e

# Parse arguments
INCREMENT=""
while getopts "i:" opt; do
    case $opt in
        i) INCREMENT="$OPTARG" ;;
        *) echo "Usage: $0 [-i patch|minor|major]"; exit 1 ;;
    esac
done

# Read current version from VERSION file
VERSION_FILE="VERSION"
if [ -f "$VERSION_FILE" ]; then
    BASE_VERSION=$(cat "$VERSION_FILE" | tr -d '[:space:]')
else
    BASE_VERSION="1.0.0"
    echo -n "$BASE_VERSION" > "$VERSION_FILE"
fi

# Parse version components
IFS='.' read -r MAJOR MINOR PATCH <<< "$BASE_VERSION"

# Increment version if requested
if [ -n "$INCREMENT" ]; then
    case $INCREMENT in
        major) MAJOR=$((MAJOR + 1)); MINOR=0; PATCH=0 ;;
        minor) MINOR=$((MINOR + 1)); PATCH=0 ;;
        patch) PATCH=$((PATCH + 1)) ;;
        *) echo "Invalid increment: $INCREMENT"; exit 1 ;;
    esac
    BASE_VERSION="$MAJOR.$MINOR.$PATCH"
    echo -n "$BASE_VERSION" > "$VERSION_FILE"
    echo "Version incremented to: $BASE_VERSION"
fi

echo "============================================"
echo "Paul Brain Modem Core - Build"
echo "============================================"

# Generate local version string with git info
COMMIT_SHORT=$(git rev-parse --short HEAD 2>/dev/null || echo "unknown")
LOCAL_VERSION="v$BASE_VERSION-local-$COMMIT_SHORT"
echo "Version: $LOCAL_VERSION"

# Detect platform
UNAME_S=$(uname -s)
UNAME_M=$(uname -m)

case "$UNAME_S" in
    Linux*)
        PLATFORM="linux64"
        LDFLAGS="-Llib/$PLATFORM -lm188110a -lpthread -static-libgcc -static-libstdc++"
        ;;
    Darwin*)
        PLATFORM="macos64"
        # macOS doesn't support static libgcc/libstdc++
        LDFLAGS="-Llib/$PLATFORM -lm188110a -lpthread"
        ;;
    MINGW*|MSYS*|CYGWIN*)
        PLATFORM="win64"
        LDFLAGS="-Llib/$PLATFORM -lm188110a -lws2_32 -static -static-libgcc -static-libstdc++"
        ;;
    *)
        echo "Unknown platform: $UNAME_S"
        exit 1
        ;;
esac

echo "Platform: $PLATFORM ($UNAME_M)"

# Check if library exists
if [ ! -f "lib/$PLATFORM/libm188110a.a" ]; then
    echo ""
    echo "ERROR: Static library not found: lib/$PLATFORM/libm188110a.a"
    echo ""
    echo "To build the library for this platform:"
    echo "  ./build_lib.sh"
    echo ""
    exit 1
fi

CXX="g++"
CXXFLAGS="-std=c++17 -O2 -I. -Iinclude -Iinclude/m188110a -D_USE_MATH_DEFINES"

SOURCES="src/main.cpp"

if [ "$UNAME_S" = "Darwin" ] || [ "$UNAME_S" = "Linux" ]; then
    OUTPUT="brain_modem_server"
else
    OUTPUT="brain_modem_server.exe"
fi

# Create output directory
mkdir -p tx_pcm_out

# Generate version header
cat > src/version.h << EOF
// Auto-generated version header
#ifndef VERSION_H
#define VERSION_H
#define VERSION_STRING "$LOCAL_VERSION"
#endif
EOF

echo ""
echo "Compiling..."
echo "$CXX $CXXFLAGS -o $OUTPUT $SOURCES $LDFLAGS"

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
