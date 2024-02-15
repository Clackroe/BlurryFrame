#!/bin/bash

cd "vendor/SFML/"
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=FALSE
cmake --build build

cd "../.."

make -C build/
