#! /usr/bin/env nix-shell
#! nix-shell --pure -i bash -p xmlstarlet kdePackages.breeze-icons
# shellcheck shell=bash
#
# Copies the given icon from the nix breeze-icons package and saves it to the
# breeze-qownnotes and breeze-dark-qownnotes folders and adds file entries to
# the resource files
#

# Copy icon from the nix package
copyIcon() {
  iconName=$1

  # The breeze-icons package is available via XDG_DATA_DIRS from the nix-shell
  # Find it by looking for the share/icons directory
  breezeIconsPath=""
  for dataDir in ${XDG_DATA_DIRS//:/ }; do
    if [ -d "$dataDir/icons/breeze" ]; then
      breezeIconsPath="$dataDir"
      break
    fi
  done

  if [ -z "$breezeIconsPath" ]; then
    echo "Error: Could not find breeze-icons package in XDG_DATA_DIRS"
    exit 1
  fi

  echo "Using breeze-icons from: $breezeIconsPath"

  # Copy light icon
  lightIconPath="$breezeIconsPath/icons/breeze/actions/22/${iconName}.svg"
  if [ -f "$lightIconPath" ]; then
    echo "Copying light icon from $lightIconPath..."
    cp "$lightIconPath" "breeze-qownnotes/16x16/${iconName}.svg"
    chmod u+w "breeze-qownnotes/16x16/${iconName}.svg"
  else
    echo "Error: Light icon not found at $lightIconPath"
    exit 1
  fi

  # Copy dark icon
  darkIconPath="$breezeIconsPath/icons/breeze-dark/actions/22/${iconName}.svg"
  if [ -f "$darkIconPath" ]; then
    echo "Copying dark icon from $darkIconPath..."
    cp "$darkIconPath" "breeze-dark-qownnotes/16x16/${iconName}.svg"
    chmod u+w "breeze-dark-qownnotes/16x16/${iconName}.svg"
  else
    echo "Dark icon not found at $darkIconPath, copying from light icon..."
    cp "breeze-qownnotes/16x16/${iconName}.svg" "breeze-dark-qownnotes/16x16/${iconName}.svg"
    chmod u+w "breeze-dark-qownnotes/16x16/${iconName}.svg"
  fi
}

# Check if the first parameter is empty
if [ -z "$1" ]; then
  echo "Please specify the icon name as the first parameter!"
  echo "Example: $0 edit-delete"
  exit 1
fi

# Change to the current directory
cd "$(dirname "$0")" || exit 2

copyIcon "$1"

# Add file entries to the resource files
echo "Adding file entries to the resource files..."
xmlstarlet ed -L -s "/RCC/qresource[@prefix='/icons/breeze-qownnotes']" -t elem -n "file" -v "icons/breeze-qownnotes/16x16/$1.svg" -i "/RCC/qresource[@prefix='/icons/breeze-qownnotes']/file[not(@alias)]" -t attr -n "alias" -v "16x16/$1.svg" ../breeze-qownnotes.qrc
xmlstarlet ed -L -s "/RCC/qresource[@prefix='/icons/breeze-dark-qownnotes']" -t elem -n "file" -v "icons/breeze-dark-qownnotes/16x16/$1.svg" -i "/RCC/qresource[@prefix='/icons/breeze-dark-qownnotes']/file[not(@alias)]" -t attr -n "alias" -v "16x16/$1.svg" ../breeze-dark-qownnotes.qrc
