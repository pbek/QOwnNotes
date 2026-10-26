#!/usr/bin/env bash
#
# Update the English source catalog using CMake. Other languages are managed by Crowdin.
# Additional arguments are passed to CMake (for example, -DQON_QT6_BUILD=OFF).
#

set -euo pipefail

project_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
build_dir="$project_dir/build-QOwnNotes-translations"

echo "Update English source translation file..."
cmake -S "$project_dir/src" -B "$build_dir" -DQON_QT6_BUILD=ON "$@"
cmake --build "$build_dir" --target translations-update
