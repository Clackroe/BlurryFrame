#!/bin/bash

cd "vendor/SFML/"
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build

cd "../.."

make -C build/
