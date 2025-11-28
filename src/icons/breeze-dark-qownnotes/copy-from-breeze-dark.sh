#!/usr/bin/env bash

#breezeDarkIconPath="/usr/share/icons/breeze-dark/actions/16/"
breezeDarkIconPath="/run/current-system/sw/share/icons/breeze-dark/actions/16/"

cd ../breeze-qownnotes/16x16 || exit
find . -name '*.svg' -exec basename {} \; | while IFS= read -r file; do
  cp "${breezeDarkIconPath}${file}" ../../breeze-dark-qownnotes/16x16/
done
