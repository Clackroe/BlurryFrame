#!/bin/bash



# Determine the operating system
if [[ $OSTYPE == "darwin" ]]; then
    # macOS
    export DYLD_LIBRARY_PATH="$DYLD_LIBRARY_PATH:$(pwd)/vendor/SFML/build/lib"
else 
    # Linux
    export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$(pwd)/vendor/SFML/build/lib"
fi

./bin/Debug/Blurry
