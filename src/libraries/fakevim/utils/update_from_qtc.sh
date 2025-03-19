#!/bin/bash
files_to_update=(
  fakevimactions.cpp
  fakevimactions.h
  fakevimhandler.cpp
  fakevimhandler.h
  fakevimtr.h

  utils/hostosinfo.h
  utils/optional.h
  utils/qtcassert.cpp
  utils/qtcassert.h
  utils/utils_global.h

  3rdparty/optional/optional.hpp
)

qtc_home=$1

script_dir=$(dirname "$(readlink -f "$0")")
base_dir=$script_dir/..

die() {
  echo "$1" 1>&2
  exit 1
}

set -e

[ -n "$qtc_home" ] ||
  die "Usage: $0 PATH_TO_QT_CREATOR"

echo "--- Fetching latest development code for Qt Creator"
cd "$qtc_home"
git fetch origin master
git checkout origin/master
commit=$(git rev-parse --short HEAD)

echo "--- Updating source files"
cd "$base_dir/fakevim"
for file in "${files_to_update[@]}"; do
  echo "-- $file"
  if [[ $file == fakevim* ]]; then
    dir=plugins/fakevim
  else
    dir=libs
  fi
  rsync --mkpath -- "$qtc_home/src/$dir/$file" "$file"
  git add -- "$file"
done

echo "--- Patching source files and creating commit"
git commit -m "Update from Qt Creator (commit $commit)"
git apply -- "$script_dir/patches/add-patches-for-upstream.patch"
git add -- "${files_to_update[@]}"
git commit --amend --no-edit --allow-empty
