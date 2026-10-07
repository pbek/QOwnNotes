#!/usr/bin/env bash
#
# Copies the tracked files of the src directory (including submodules) of the
# current checkout into the "qownnotes-src" directory, which is used as source
# of the "qownnotes" part in snapcraft.yaml. This way the snap is built from
# the checked out commit instead of a fresh clone of the default branch.
#

set -euo pipefail

SNAP_PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_DIR="$(git -C "${SNAP_PROJECT_DIR}" rev-parse --show-toplevel)"
TARGET_DIR="${SNAP_PROJECT_DIR}/qownnotes-src"

rm -rf "${TARGET_DIR}"
mkdir -p "${TARGET_DIR}"

cd "${REPO_DIR}"
git ls-files -z --recurse-submodules -- src |
  tar --null --files-from=- -cf - |
  tar -xf - -C "${TARGET_DIR}" --strip-components=1

test -f "${TARGET_DIR}/QOwnNotes.pro"
echo "Prepared snap source in ${TARGET_DIR}"
