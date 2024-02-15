#!/bin/bash

# Get the current directory
CURRENT_DIR="$(pwd)"

# Determine the operating system
if [[ $OSTYPE == "darwin" ]]; then
    # macOS
    export DYLD_LIBRARY_PATH="$DYLD_LIBRARY_PATH:$CURRENT_DIR/vendor/SFML/build/lib"
elif [[ true ]]; then
    # Linux
    export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$CURRENT_DIR/vendor/SFML/build/lib"

    echo "$CURRENT_DIR/vendor/SFML/build/lib"
fi


cd "vendor/SFML/"
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build

cd "../.."

make -C build/
