#!/usr/bin/env bash
#
# This is the build/deploy script for the Launchpad Snap image
# https://launchpad.net/qownnotes-snap
#
# We need a ssh key to push to the git repository
# git+ssh://pbek@git.launchpad.net/~pbek/qownnotes-snap
#
# The QOwnNotes repository for Launchpad Snap relies on our source archive on
# https://sourceforge.net/projects/qownnotes/files/src/
#


# uncomment this if you want to force a version
#QOWNNOTES_VERSION=16.07.6

BRANCH=develop
#BRANCH=master

PROJECT_PATH="/tmp/QOwnNotes-launchpad-snap-$$"
CUR_DIR=$(pwd)

# use temporary checksum variable file
_QQwnNotesCheckSumVarFile="/tmp/QOwnNotes.checksum.vars"

if [[ ! -f ${_QQwnNotesCheckSumVarFile} ]]; then
	echo "${_QQwnNotesCheckSumVarFile} doesn't exist. build-tuxfamily-src.sh must be run ahead of build script!"
	exit 1
fi

source ${_QQwnNotesCheckSumVarFile}

# check checksum variable from build-systems/tuxfamily/build-tuxfamily-src.sh
if [ -z ${QOWNNOTES_ARCHIVE_SHA256} ]; then
    echo "QOWNNOTES_ARCHIVE_SHA256 was not set!"
	exit 1
fi

echo "Started the Launchpad Snap packaging process, using latest '$BRANCH' git tree"

if [ -d $PROJECT_PATH ]; then
    rm -rf $PROJECT_PATH
fi

mkdir $PROJECT_PATH
cd $PROJECT_PATH

echo "Project path: $PROJECT_PATH"

# checkout Launchpad Snap repository
git clone --depth=1 git+ssh://pbek@git.launchpad.net/~pbek/qownnotes-snap snap

# checkout the source code
git clone --depth=1 git@github.com:pbek/QOwnNotes.git QOwnNotes -b $BRANCH
cd QOwnNotes

if [ -z $QOWNNOTES_VERSION ]; then
    # get version from version.h
    QOWNNOTES_VERSION=`cat src/version.h | sed "s/[^0-9,.]//g"`
fi

cd ../snap
cp ../QOwnNotes/build-systems/snap/snapcraft/* . -R

# replace the version in the snapcraft.yaml file
sed -i "s/VERSION-STRING/$QOWNNOTES_VERSION/g" snapcraft.yaml

# replace the archive sha256 hash in the snapcraft.yaml file
sed -i "s/ARCHIVE-SHA256/$QOWNNOTES_ARCHIVE_SHA256/g" snapcraft.yaml


echo "Committing changes..."
git commit -m "releasing version $QOWNNOTES_VERSION" -a
git push

# remove everything after we are done
if [ -d $PROJECT_PATH ]; then
    rm -rf $PROJECT_PATH
fi
