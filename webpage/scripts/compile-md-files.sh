#!/usr/bin/env bash
#
# This script compiles together all webpage markdown files in one file to translate
# Deprecated: Is now done with Crowdin directly!
#

compileContent() {
  echo -e "[START $1]\n\n"
  cat "$1"
  echo -e "\n\n[END $1]\n"
}

# shellcheck disable=SC2164
cd src
export -f compileContent
find -type f \( -iname "*.md" ! -iname "changelog.md" \) ! -path "./de/*" ! -path "./blog/*" ! -path "changelog.md" ! -path "./.vuepress/*" -exec bash -c 'compileContent "$0"' {} \; >english.txt
