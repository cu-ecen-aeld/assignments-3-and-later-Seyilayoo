Bash
#!/bin/bash

# Function to perform the search and print results
search_and_print() {
  local dir="$1"
  local search_str="$2"

  if [ ! -d "$dir" ]; then
    echo "Error: Directory '$dir' does not exist!"
    return 1
  fi

  x=$(find "$dir" -type f | wc -l)
  y=$(grep -r "$search_str" "$dir" | wc -l)

  echo "The number of files are $x and the number of matching lines are $y"
}

# Main script execution
if [ $# -ne 2 ]; then
  echo "Error: Please provide exactly two arguments: directory path and search string."
  exit 1
fi

search_and_print "$1" "$2"

# Exit code reflects success or directory error from search_and_print
exit $?
