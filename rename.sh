#!/bin/bash

# Set the starting index
index=1

# Loop through each file in the directory
for file in *; do
    # Check if the file is a regular file
    if [ -f "$file" ]; then
        # Rename the file to the incrementing index with .png extension
        mv "$file" "$index.png"
        # Increment the index for the next file
        ((index++))
    fi
done
