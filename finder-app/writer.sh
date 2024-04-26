#!/bin/bash

write_to_file() {
  local writefile="$1"
  local writestr="$2"

  if ! [ -f "$writefile" ]; then
    # Create directories and file if needed
    mkdir -p "$(dirname "$writefile")" && touch "$writefile"
  fi

  # Write the string to the file
  echo "$writestr" > "$writefile"
}

# Main script execution
if [ $# -ne 2 ]; then
  echo "Error: Please provide exactly two arguments: file path and string to write."
  exit 1
fi

write_to_file "$1" "$2"

exit $?
