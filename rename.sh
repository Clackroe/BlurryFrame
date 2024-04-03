#!/bin/bash

if [ $# -eq 0 ]; then
    echo "Usage: $0 <directory>"
    exit 1
fi
# Set the starting index
index=1

dir="$1"

# Loop through each file in the directory
for file in "$dir"/*; do
    # Check if the file is a regular file
    if [ -f "$file" ]; then
        # Rename the file to the incrementing index with .png extension
        mv "$file" "$dir/$index.png"
        # Increment the index for the next file
        ((index++))
    fi
done
