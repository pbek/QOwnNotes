#!/bin/env bash
#
# Runs qt6 build script
#

# This is used inside the release docker container
if [ "$1" = "--docker" ]; then
  echo "Importing PGP key..."
  gpg --import ~/private.pgp
  if ! gpg --list-secret-keys patrizio@bekerle.com >/dev/null 2>&1; then
    echo "The imported PGP key does not contain a secret key for patrizio@bekerle.com." >&2
    echo "Ensure the Bitwarden attachment 'private.pgp' was exported with 'gpg --export-secret-keys'." >&2
    exit 1
  fi
  echo "Adding AUR ssh key..."
  eval "$(ssh-agent -s)"
  ssh-add ~/.ssh/aur_rsa
fi

DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

cd "$DIR" || exit 1

../ubuntu-launchpad/build-for-launchpad-qt6.sh
