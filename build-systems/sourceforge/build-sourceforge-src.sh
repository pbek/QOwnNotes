#!/usr/bin/env bash
#
# This is the deploy script of the source archives for SourceForge
#
# We need to setup a ssh key to copy files to SourceForge
# https://sourceforge.net/auth/shell_services
#
# Files will be uploaded to:
# https://sourceforge.net/projects/qownnotes/files/src/
#
# Files will be accessible like that:
# http://downloads.sourceforge.net/project/qownnotes/src/qownnotes-16.05.2.tar.xz
#

# uncomment this if you want to force a version
#QOWNNOTES_VERSION=1.1.3.2

BRANCH=main
#BRANCH=master

PROJECT_PATH="/tmp/QOwnNotes-sourceforge-$$"

echo "Started the SourceForge packaging process, using latest '$BRANCH' git tree"

if [ -d $PROJECT_PATH ]; then
  rm -rf $PROJECT_PATH
fi

mkdir $PROJECT_PATH
cd $PROJECT_PATH || exit 1

echo "Project path: $PROJECT_PATH"

# checkout the source code
git clone --depth=1 git@github.com:pbek/QOwnNotes.git QOwnNotes -b $BRANCH
cd QOwnNotes || exit 1

# checkout submodules
git submodule update --init

# build binary translation files
lrelease src/QOwnNotes.pro

# remove huge .git folder
rm -Rf .git

if [ -z "$QOWNNOTES_VERSION" ]; then
  # get version from version.h
  QOWNNOTES_VERSION=$(cat src/version.h | sed "s/[^0-9,.]//g")
else
  # set new version if we want to override it
  echo "#define VERSION \"$QOWNNOTES_VERSION\"" >src/version.h
fi

# set the release string
echo '#define RELEASE "SourceForge"' >src/release.h

echo "Using version $QOWNNOTES_VERSION..."

qownnotesSrcDir="qownnotes-${QOWNNOTES_VERSION}"

# copy some needed files file
cp LICENSE src
cp README.md src
cp CHANGELOG.md src
cp webpage/src/getting-started/shortcuts.md src

# rename the src directory
mv src "$qownnotesSrcDir"

archiveFile="$qownnotesSrcDir.tar.xz"

# archive the source code
echo "Creating archive $archiveFile..."
tar -cJf "$archiveFile" "$qownnotesSrcDir"

md5sum "$archiveFile" >"$archiveFile.md5"
sha256sum "$archiveFile" | awk '{ print $1 }' >"$archiveFile.sha256"
sha512sum "$archiveFile" | awk '{ print $1 }' >"$archiveFile.sha512"

remotePath="patbek@frs.sourceforge.net:/home/frs/project/qownnotes/src"
sourceForgeReadme="sourceforge-readme.md"

# generate the readme for sourceforge with a screenshot from GitHub
{
  cat README.md | sed 's/screenshots\/screenshot.png/https:\/\/raw.githubusercontent.com\/pbek\/QOwnNotes\/main\/screenshots\/screenshot.png/g'
  echo
  echo
  cat CHANGELOG.md
} >>"${sourceForgeReadme}"

echo "Uploading files to SourceForge..."

# upload the changelog file as README.md
# it will be viewed on the sourceforge webpage
rsync -ahv --progress "${sourceForgeReadme}" "${remotePath}/README.md"

## upload the screenshots
#rsync -ahv --progress screenshots/* ${remotePath}/screenshots

# upload archive and checksum files
rsync -ahv --progress "${archiveFile}"* "${remotePath}"

# remove everything after we are done
if [ -d "$PROJECT_PATH" ]; then
  rm -rf "$PROJECT_PATH"
fi
