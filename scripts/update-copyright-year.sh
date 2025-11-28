#!/usr/bin/env bash
#
# Use this script to update all copyright years in the source and license files
#

function help() {
  echo "Usage: $0 <previous-year> <new-year>"
  echo "Example: $0 2022 2023"
  exit 1
}

if [ -z "$1" ]; then
  help
fi

if [ -z "$2" ]; then
  help
fi

echo "Previous year: $1"
echo "New year: $2"

echo
echo Updating QOwnNotes...
echo
git checkout main || exit 2
git pull

echo
echo Updating piwiktracker...
echo
pushd src/libraries/piwiktracker || exit 1
git checkout master || exit 2
git pull
popd || exit 3

echo
echo Updating qmarkdowntextedit...
echo
pushd src/libraries/qmarkdowntextedit || exit 1
git checkout develop || exit 2
git pull
popd || exit 3

echo
echo Updating year "$1" to "$2"...
echo
find src \( -name '*.c' -o -name '*.cpp' -o -name '*.h' -o -name 'copyright' -o -name 'LICENSE' \) -exec sed -E "s/$1 Patrizio/$2 Patrizio/" -i {} +

echo
echo "Are there any $1 entries left? (should return nothing)"
echo
rg "$1 Patrizio" .

echo
echo "Commit message: \"doc: update year from $1 to $2\""
echo
