#!/usr/bin/env sh

set -e

# Fix Date RangeError in dependencies after NodeJS 12
./scripts/fix-date-range-error.sh

# Copy missing assets
cp ../screenshots src/.vuepress/public -R
cp ../CHANGELOG.md src/changelog.md

# Fix "Error: error:0308010C:digital envelope routines::unsupported" with "--openssl-legacy-provider"
# Fix https://github.com/vuejs/vuepress/issues/966 with "--max_old_space_size=8192"
NODE_OPTIONS=--openssl-legacy-provider node --max_old_space_size=8192 ./node_modules/vuepress/cli.js dev src
