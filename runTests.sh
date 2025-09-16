#!/bin/bash
set -e

BUILD_DIR="./build"

cd "$BUILD_DIR"
ctest --output-on-failure
cd ..
