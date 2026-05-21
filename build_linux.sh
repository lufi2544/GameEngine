#!/usr/bin/env bash
set -e

# -----------------------------
# Set variables
# -----------------------------
THIS_DIR="$(cd "$(dirname "$0")" && pwd)"
SRC_DIR="$THIS_DIR/source"
TESTS_DIR="$THIS_DIR/tests"
BIN_DIR="$THIS_DIR/bin"

MAIN_FILE="$SRC_DIR/linux_main.cpp"
MAYORANA_DIR="$SRC_DIR/MayoranaFramework/source"
MAYORANA_REF="$SRC_DIR/mayorana-reflection/source"

BINARY_NAME="engine"
FLAGS="MAYORANA"

CXX=g++
CXXFLAGS="-std=c++17 -g"
INCLUDES=(
    "-I$SRC_DIR"
    "-I$TESTS_DIR"
    "-I$MAYORANA_DIR"
    "-I$MAYORANA_REF"
)
LIBS=(
    -lvulkan
    -lX11
    -ldl
    -lpthread
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
    $CXXFLAGS \
    "${INCLUDES[@]}" \
    -D$FLAGS \
    "${LIBS[@]}" \
    "$MAIN_FILE" \
    -o "$BINARY_NAME"

echo "Build successful: $BIN_DIR/$BINARY_NAME"
