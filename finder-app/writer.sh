#!/bin/bash

# Check for required arguments
if [ $# -ne 2 ]; then
  echo "Error: Please provide two arguments: file path and string to write."
  exit 1
fi

# Define variables
writefile="$1"
writestr="$2"

# Create the file and write content
touch "$writefile" && echo "$writestr" > "$writefile" && echo "File created successfully!" || echo "Error: Could not create file."

exit 0
