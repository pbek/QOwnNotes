#!/usr/bin/env bash
###
# This script updates the symlinks to the qmake binaries in the nix store
# It will be executed by direnv when entering the project directory via the shell.nix
# The symlinks are to be used in Qt Creator and CLion
###

# With direnv, the current directory is the project directory
BIN_DIR="$(pwd)/bin"

mkdir -p "${BIN_DIR}"
ln -sf "$(which qmake)" "${BIN_DIR}/qmake6" && "${BIN_DIR}/qmake6" --version
# Run a pure shell so the shell.nix will not be loaded again by direnv
# shellcheck disable=SC2016
nix-shell --pure -p which libsForQt5.qt5.qtbase --run 'ln -sf $(which qmake | tail -n1) $PWD/bin/qmake5' && "${BIN_DIR}/qmake5" --version
