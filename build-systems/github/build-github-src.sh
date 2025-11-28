#!/usr/bin/env bash
#
# This is the deploy script of the source archives for GitHub
#
# Files will be uploaded to the release on the GitHub release page:
# https://github.com/pbek/QOwnNotes/releases
#
# Files will be accessible like that:
# https://github.com/pbek/QOwnNotes/releases/download/v23.7.0/qownnotes-23.7.0.tar.xz
#

# uncomment this if you want to force a version
#QOWNNOTES_VERSION=23.7.0

#BRANCH=main
BRANCH=release

PROJECT_PATH="/tmp/QOwnNotes-github-$$"

# exit on error
set -e

echo "Check if we have a GitHub token (login if not)..."
# Insecure storage is needed for the GitHub token to be in a file we can mount into the docker container
# "gh auth status" always seems to return 0, we might need to use the login command manually
# Use this locally in QOwnNotes directory:
# GH_CONFIG_DIR="./secrets/gh" gh auth login -h github.com --insecure-storage
gh auth status || gh auth login -h github.com --insecure-storage

echo "Started the GitHub packaging process, using latest '$BRANCH' git tree"

if [ -d $PROJECT_PATH ]; then
  rm -rf $PROJECT_PATH
fi

mkdir $PROJECT_PATH
cd $PROJECT_PATH || exit 1

echo "Project path: $PROJECT_PATH"

# checkout the source code with checkout submodules
git clone --depth=1 https://github.com/pbek/QOwnNotes.git QOwnNotes -b $BRANCH --recursive
cd QOwnNotes || exit 1

# build binary translation files
lrelease src/QOwnNotes.pro

if [ -z "$QOWNNOTES_VERSION" ]; then
  # get version from version.h
  QOWNNOTES_VERSION=$(cat src/version.h | sed "s/[^0-9,.]//g")
else
  # set new version if we want to override it
  echo "#define VERSION \"$QOWNNOTES_VERSION\"" >src/version.h
fi

# set the release string
echo '#define RELEASE "GitHub"' >src/release.h

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

QOWNNOTES_ARCHIVE_MD5=$(md5sum "${archiveFile}" | awk '{ print $1 }' | tee "${archiveFile}.md5")
QOWNNOTES_ARCHIVE_SHA256=$(sha256sum "${archiveFile}" | awk '{ print $1 }' | tee "${archiveFile}.sha256")
QOWNNOTES_ARCHIVE_SHA512=$(sha512sum "${archiveFile}" | awk '{ print $1 }' | tee "${archiveFile}.sha512")
QOWNNOTES_ARCHIVE_SIZE=$(stat -c "%s" "${archiveFile}")

# also create checksum files that are compatible with sha256sum and sha512sum
sha256sum "${archiveFile}" >"${archiveFile}.sha256sum"
sha512sum "${archiveFile}" >"${archiveFile}.sha512sum"

echo ""
echo "Sums:"
echo "$QOWNNOTES_ARCHIVE_MD5"
echo "$QOWNNOTES_ARCHIVE_SHA256"
echo "$QOWNNOTES_ARCHIVE_SHA512"
echo ""
echo "Size:"
echo "$QOWNNOTES_ARCHIVE_SIZE"

# write temporary checksum variable file for the deployment scripts
_QQwnNotesCheckSumVarFile="/tmp/QOwnNotes.checksum.vars"
{
  echo "QOWNNOTES_ARCHIVE_MD5=$QOWNNOTES_ARCHIVE_MD5"
  echo "QOWNNOTES_ARCHIVE_SHA256=$QOWNNOTES_ARCHIVE_SHA256"
  echo "QOWNNOTES_ARCHIVE_SHA512=$QOWNNOTES_ARCHIVE_SHA512"
  echo "QOWNNOTES_ARCHIVE_SIZE=$QOWNNOTES_ARCHIVE_SIZE"
} >"${_QQwnNotesCheckSumVarFile}"

echo "Uploading files to GitHub..."

# Upload archive and checksum files for 5minutes, retrying every 5 seconds
# We need to be in a git repo for that
timeout=$((5 * 60))
interval=5
start_time=$(date +%s)
while ! gh release upload --clobber "v$QOWNNOTES_VERSION" "${archiveFile}"*; do
  now=$(date +%s)
  elapsed=$((now - start_time))
  if [ $elapsed -ge $timeout ]; then
    echo "Upload failed after 5 minutes."
    exit 1
  fi
  echo "Upload failed, retrying in $interval seconds..."
  sleep $interval
done

# remove everything after we are done
if [ -d $PROJECT_PATH ]; then
  rm -rf $PROJECT_PATH
fi

url="https://github.com/pbek/QOwnNotes/releases/download/v$QOWNNOTES_VERSION/${archiveFile}"
echo "Checking if $url exists..."

# try to download file from github
until curl --output /dev/null --silent --head --fail "$url"; do
  echo "URL does not exist: $url..."
  sleep 2
done

echo "URL exists: $url"
exit 0
