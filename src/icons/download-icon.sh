#! /usr/bin/env nix-shell
#! nix-shell --pure -i bash -p xmlstarlet curl
#
# Downloads and extract the given icon and save it to the breeze-qownnotes and
# breeze-dark-qownnotes folders and adds file entries to the resource files
# https://github.com/KDE/breeze-icons/blob/master/icons/actions/22/
#

#iconUrlPrefix="https://github.com/KDE/breeze-icons/raw/master/icons/actions/22/"
#darkIconUrlPrefix="https://github.com/KDE/breeze-icons/raw/764cb1d4c6b42e60537b7e986657dee1ffa770c7/icons-dark/actions/22/"

kdeFrameworkVersion="5.111"
iconThemeUrl="https://download.kde.org/stable/frameworks/${kdeFrameworkVersion}/breeze-icons-${kdeFrameworkVersion}.0.tar.xz"

# Download icon from the first parameter from GitHub
downloadIcon() {
  iconName=$1
  #    curl -L "${iconUrlPrefix}${iconName}.svg" -o "breeze-qownnotes/16x16/${iconName}.svg"
  #    curl -L "${darkIconUrlPrefix}${iconName}.svg" -o "breeze-dark-qownnotes/16x16/${iconName}.svg"

  # Download iconThemeUrl to a temporary folder (because dark icons aren't hosted on GitHub any more, they are generated)
  tmpDir=$(mktemp -d)
  echo "Downloading breeze-icons to ${tmpDir}..."
  curl -L "${iconThemeUrl}" -o "${tmpDir}/breeze-icons.tar.xz"

  # Extract icon files
  echo "Extracting breeze-icons to breeze-qownnotes/16x16 and breeze-dark-qownnotes/16x16..."
  tar -xf "${tmpDir}/breeze-icons.tar.xz" -C "breeze-qownnotes/16x16" --strip-components=4 "breeze-icons-${kdeFrameworkVersion}.0/icons/actions/22/${iconName}.svg"
  tar -xf "${tmpDir}/breeze-icons.tar.xz" -C "breeze-dark-qownnotes/16x16" --strip-components=4 "breeze-icons-${kdeFrameworkVersion}.0/icons-dark/actions/22/${iconName}.svg"

  # Remove the temporary folder
  echo "Removing ${tmpDir}..."
  rm -rf "${tmpDir}"
}

# Check if the first parameter is empty
if [ -z "$1" ]; then
  echo "Please specify the icon name as the first parameter!"
  echo "Example: $0 edit-delete"
  exit 1
fi

# Change to the current directory
cd "$(dirname "$0")" || exit 2

downloadIcon "$1"

# Add file entries to the resource files
echo "Adding file entries to the resource files..."
xmlstarlet ed -L -s "/RCC/qresource[@prefix='/icons/breeze-qownnotes']" -t elem -n "file" -v "icons/breeze-qownnotes/16x16/$1.svg" -i "/RCC/qresource[@prefix='/icons/breeze-qownnotes']/file[not(@alias)]" -t attr -n "alias" -v "16x16/$1.svg" ../breeze-qownnotes.qrc
xmlstarlet ed -L -s "/RCC/qresource[@prefix='/icons/breeze-dark-qownnotes']" -t elem -n "file" -v "icons/breeze-dark-qownnotes/16x16/$1.svg" -i "/RCC/qresource[@prefix='/icons/breeze-dark-qownnotes']/file[not(@alias)]" -t attr -n "alias" -v "16x16/$1.svg" ../breeze-dark-qownnotes.qrc
