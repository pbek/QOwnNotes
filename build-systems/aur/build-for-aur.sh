#!/usr/bin/env bash
#
# This is the build/deploy script for the Arch Linux User Repository (AUR)
# https://aur.archlinux.org/packages/qownnotes/
#
# We need a ssh key to push to the git repository
# https://wiki.archlinux.org/index.php/Arch_User_Repository#Creating_a_new_package
#
# The QOwnNotes repository for AUR relies on our source archive on
# https://github.com/pbek/QOwnNotes/releases
#

# Uncomment as workaround to run locally
#QOWNNOTES_ARCHIVE_SHA256=$(curl https://github.com/pbek/QOwnNotes/releases/download/v23.7.0/qownnotes-23.7.0.tar.xz.sha256)
#echo QOWNNOTES_ARCHIVE_SHA256="${QOWNNOTES_ARCHIVE_SHA256}" > /tmp/QOwnNotes.checksum.vars

# uncomment this if you want to force a version
#QOWNNOTES_VERSION=16.05.2

BRANCH=main
#BRANCH=release

PROJECT_PATH="/tmp/QOwnNotes-aur-$$"

# use temporary checksum variable file
_QQwnNotesCheckSumVarFile="/tmp/QOwnNotes.checksum.vars"

if [[ ! -f ${_QQwnNotesCheckSumVarFile} ]]; then
  echo "${_QQwnNotesCheckSumVarFile} doesn't exist. build-github-src.sh must be run ahead of build script!"
  exit 1
fi

# shellcheck source=/dev/null
source "${_QQwnNotesCheckSumVarFile}"

# check checksum variable from build-systems/github/build-github-src.sh
if [ -z "${QOWNNOTES_ARCHIVE_SHA256}" ]; then
  echo "QOWNNOTES_ARCHIVE_SHA256 was not set!"
  exit 1
fi

echo "Started the AUR packaging process, using latest '$BRANCH' git tree"

if [ -d "$PROJECT_PATH" ]; then
  rm -rf "$PROJECT_PATH"
fi

mkdir "$PROJECT_PATH"
cd "$PROJECT_PATH" || exit 1

echo "Project path: $PROJECT_PATH"

# checkout AUR repository
git clone --depth=1 ssh://aur@aur.archlinux.org/qownnotes.git aur -b master

# checkout the source code
git clone --depth=1 git@github.com:pbek/QOwnNotes.git QOwnNotes -b "$BRANCH"
cd QOwnNotes || exit 1

gitCommitHash=$(git rev-parse HEAD)
echo "Current commit: $gitCommitHash"

if [ -z "$QOWNNOTES_VERSION" ]; then
  # get version from version.h
  QOWNNOTES_VERSION=$(cat src/version.h | sed "s/[^0-9,.]//g")
fi

cd ../aur || exit 1
cp ../QOwnNotes/build-systems/aur/PKGBUILD .
cp ../QOwnNotes/build-systems/aur/.SRCINFO .

# replace the version in the PKGBUILD file
sed -i "s/VERSION-STRING/$QOWNNOTES_VERSION/g" PKGBUILD

# replace the commit hash in the PKGBUILD file
sed -i "s/COMMIT-HASH/$gitCommitHash/g" PKGBUILD

# replace the archive sha256 hash in the PKGBUILD file
sed -i "s/ARCHIVE-SHA256/$QOWNNOTES_ARCHIVE_SHA256/g" PKGBUILD
echo "Archive sha256: ${QOWNNOTES_ARCHIVE_SHA256}"

# replace the version in the .SRCINFO file
sed -i "s/VERSION-STRING/$QOWNNOTES_VERSION/g" .SRCINFO

echo "Committing changes..."
git commit -m "releasing version $QOWNNOTES_VERSION" PKGBUILD .SRCINFO
git push

# remove everything after we are done
if [ -d $PROJECT_PATH ]; then
  rm -rf $PROJECT_PATH
fi
