#!/bin/bash

# Check for required arguments
if [ $# -ne 2 ]; then
  echo "Error: Please provide two arguments: directory path and search string."
  exit 1
fi

# Check if directory exists
if [ ! -d "$1" ]; then
  echo "Error: '$1' is not a directory."
  exit 1
fi

# Define variables
filesdir="$1"
searchstr="$2"

# Initialize counters
file_count=0
match_count=0

# Loop through files recursively using find command
find "$filesdir" -type f -exec grep -Hn "$searchstr" {} \; | while read -r line; do
  # Increment file count
  file_count=$((file_count + 1))

  # Check if line contains search string (grep returns non-zero on match)
  if [ $? -eq 0 ]; then
    match_count=$((match_count + 1))
  fi
done

# Print final message
echo "The number of files are $file_count and the number of matching lines are $match_count"

exit 0
