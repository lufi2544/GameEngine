#!/usr/bin/env bash
set -e

# -----------------------------
# Set variables
# -----------------------------
THIS_DIR="$(cd "$(dirname "$0")" && pwd)"
SRC_DIR="$THIS_DIR/source"
TESTS_DIR="$THIS_DIR/tests"
BIN_DIR="$THIS_DIR/bin"
BUILD_DIR="$THIS_DIR/build"

MAIN_FILE="$SRC_DIR/macos_main.mm"   # rename later if you want
MAYORANA_DIR="$SRC_DIR/MayoranaFramework/source"
MAYORANA_REF="$SRC_DIR/mayorana-reflection/source"
FRAMEWORKS=(
	-framework Cocoa
)

BINARY_NAME="engine"
FLAGS="MAYORANA"

CXX=clang++
CXXFLAGS="-std=c++17"
INCLUDES=(
    "-I$SRC_DIR"
    "-I$TESTS_DIR"
    "-I$MAYORANA_DIR"
    "-I$MAYORANA_REF/reflection_includes.h"
)

# -----------------------------
# Create bin directory
# -----------------------------
mkdir -p "$BIN_DIR"
cd "$BIN_DIR"

# -----------------------------
# Compile
# -----------------------------
$CXX \
    "${CXXFLAGS}" \
    "${INCLUDES[@]}" \
    -D$FLAGS \
	"${FRAMEWORKS[@]}"\
    "$MAIN_FILE" \
    -o "$BINARY_NAME"

echo "Build successful: $BIN_DIR/$BINARY_NAME"