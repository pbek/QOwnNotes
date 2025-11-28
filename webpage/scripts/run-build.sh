#!/usr/bin/env sh

set -e

# Fix Date RangeError in dependencies after NodeJS 12
./scripts/fix-date-range-error.sh

# Copy missing assets (skip if can't find, because we are using this script in a Dockerfile)
cp ../screenshots src/.vuepress/public -R || true
cp ../CHANGELOG.md src/changelog.md || true

# Fix "Error: error:0308010C:digital envelope routines::unsupported" with "--openssl-legacy-provider"
# Fix https://github.com/vuejs/vuepress/issues/966 with "--max_old_space_size=8192"
NODE_OPTIONS=--openssl-legacy-provider node --max_old_space_size=8192 ./node_modules/vuepress/cli.js build src

# Set more RTL styles
# shellcheck disable=SC2046
sed -E 's/^<html /&dir="rtl" /' -i $(find src/.vuepress/dist/fa -name '*.html')
# shellcheck disable=SC2046
sed -E 's/^<html /&dir="rtl" /' -i $(find src/.vuepress/dist/ar -name '*.html')
