#!/usr/bin/env sh

set -e

# Copy missing assets (skip if can't find, because we are using this script in a Dockerfile)
cp ../screenshots src/.vitepress/public -R || true
cp ../CHANGELOG.md src/changelog.md || true

# Build with VitePress
vitepress build src

# Set more RTL styles
sed -E 's/^<html /&dir="rtl" /' -i $(find src/.vitepress/dist/fa -name '*.html')
sed -E 's/^<html /&dir="rtl" /' -i $(find src/.vitepress/dist/ar -name '*.html')
