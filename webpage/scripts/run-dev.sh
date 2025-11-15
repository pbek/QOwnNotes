#!/usr/bin/env sh

set -e

# Copy missing assets
cp ../screenshots src/.vitepress/public -R
cp ../CHANGELOG.md src/changelog.md

# Run VitePress dev server
vitepress dev src
