#!/bin/env bash
#
# Runs qt6 build script
#

# This is used inside the release docker container
if [ "$1" = "--docker" ]; then
  /QOwnNotes/build-systems/import-pgp-key.sh || exit 1
  echo "Adding AUR ssh key..."
  eval "$(ssh-agent -s)"
  ssh-add ~/.ssh/aur_rsa
fi

DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

cd "$DIR" || exit 1

../ubuntu-launchpad/build-for-launchpad-qt6.sh
