#!/usr/bin/env bash
#
# This is the build script for a local qownnotes snap image and upload to the
# store
# https://myapps.developer.ubuntu.com/dev/click-apps/
#
# We need to be logged in to ubuntu one with:
# snapcraft login
#
# The QOwnNotes repository for Launchpad Snap relies on our source archive on
# https://github.com/pbek/QOwnNotes/releases
#

# uncomment this if you want to force a version
#QOWNNOTES_VERSION=16.07.3

BRANCH=main
#BRANCH=master

PROJECT_PATH="/tmp/QOwnNotes-local-snap-$$"

echo "Started the Snap building process, using latest '$BRANCH' git tree"

if [ -d "$PROJECT_PATH" ]; then
  rm -rf "$PROJECT_PATH"
fi

mkdir "$PROJECT_PATH"
cd "$PROJECT_PATH" || exit

echo "Project path: $PROJECT_PATH"

# checkout the source code
git clone --depth=1 https://github.com/pbek/QOwnNotes.git QOwnNotes -b "$BRANCH"
cd QOwnNotes || exit

if [ -z "$QOWNNOTES_VERSION" ]; then
  # get version from version.h
  QOWNNOTES_VERSION=$(cat src/version.h | sed "s/[^0-9,.]//g")
fi

cd build-systems/snap/snapcraft || exit

# replace the version in the snapcraft.yaml file
sed -i "s/VERSION-STRING/$QOWNNOTES_VERSION/g" snapcraft.yaml

snapcraft update

echo "Building snap..."
snapcraft

echo "Uploading snap..."
snapcraft push "qownnotes_${QOWNNOTES_VERSION}_amd64.snap" --release stable

#echo "Releasing snap..."
# snapcraft release qownnotes --release ${REVISION} stable

# this may work in the future (we need to set a channel when releasing)
#snapcraft push qownnotes_${QOWNNOTES_VERSION}_amd64.snap --release=Stable
#snapcraft release <snap-name> <revision> <channel-name>

# remove everything after we are done
if [ -d $PROJECT_PATH ]; then
  rm -rf $PROJECT_PATH
fi
