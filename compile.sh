#!/bin/bash

cd build/
cmake ..
cmake --build .
mv compile_commands.json ../
cd ../
./build/Blur
