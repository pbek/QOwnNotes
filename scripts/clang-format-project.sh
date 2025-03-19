#!/bin/env bash
#
# A tool to run clang-format on the entire project
#
# Some inspirations were taken from https://github.com/eklitzke/clang-format-all

# Variable that will hold the name of the clang-format command
FMT=""

# Some distros just call it clang-format. Others (e.g. Ubuntu) are insistent
# that the version number be part of the command. We prefer clang-format if
# that's present, otherwise we work backwards from highest version to lowest
# version.
for clangfmt in clang-format{,-{4,3}.{9,8,7,6,5,4,3,2,1,0}}; do
  if which "$clangfmt" &>/dev/null; then
    FMT="$clangfmt"
    break
  fi
done

# Check if we found a working clang-format
if [ -z "$FMT" ]; then
  echo "failed to find clang-format"
  exit 1
fi

# Function to run clang-format on all *.cpp and *.h files in a directory (optionally recursively)
format_directory() {
  directory="$1"
  recursive="$2"

  if [ "$recursive" = "true" ]; then
    files=$(find "$directory" -type f \( -name '*.cpp' -or -name '*.h' \))
  else
    files=$(ls "$directory"/*.cpp "$directory"/*.h 2>/dev/null)
  fi
  for file in $files; do
    echo "Formatting $file"
    $FMT -i "$file"
  done
}

# Use format_directory on the src and tests directories
format_directory src false
format_directory src/entities true
format_directory src/utils true
format_directory src/helpers true
format_directory src/services true
format_directory src/widgets true
format_directory src/dialogs true
format_directory src/api true
format_directory src/dialogs true
format_directory src/dialogs true
format_directory src/dialogs true
format_directory src/libraries/piwiktracker true
format_directory src/libraries/qmarkdowntextedit true

format_directory tests/unit_tests true
