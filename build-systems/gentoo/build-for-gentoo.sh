#!/usr/bin/env bash
#
# This is the build/deploy script for the Gentoo QOwnNotes overlay
# https://github.com/pbek/qownnotes-overlay
#
# The QOwnNotes repository for Gentoo relies on our source archive on
# https://sourceforge.net/projects/qownnotes/files/src/
#


# uncomment this if you want to force a version
#QOWNNOTES_VERSION=1.1.0.4

BRANCH=develop
#BRANCH=master

PROJECT_PATH="/tmp/QOwnNotes-gentoo-$$"
CUR_DIR=$(pwd)

# use temporary checksum variable file
_QQwnNotesCheckSumVarFile="/tmp/QOwnNotes.checksum.vars"

if [[ ! -f ${_QQwnNotesCheckSumVarFile} ]]; then
	echo "${_QQwnNotesCheckSumVarFile} doesn't exist. build-tuxfamily-src.sh must be run ahead of build script!"
	exit 1
fi

source ${_QQwnNotesCheckSumVarFile}

# check checksum variable from build-systems/tuxfamily/build-tuxfamily-src.sh
if [ -z ${QOWNNOTES_ARCHIVE_SHA512} ]; then
    echo "QOWNNOTES_ARCHIVE_SHA512 was not set!"
	exit 1
fi

if [ -z ${QOWNNOTES_ARCHIVE_SIZE} ]; then
    echo "QOWNNOTES_ARCHIVE_SIZE was not set!"
	exit 1
fi

echo "Started the ebuild packaging process, using latest '$BRANCH' git tree"

if [ -d $PROJECT_PATH ]; then
    rm -rf $PROJECT_PATH
fi

mkdir $PROJECT_PATH
cd $PROJECT_PATH

echo "Project path: $PROJECT_PATH"

# checkout AUR repository
git clone --depth=1 git@github.com:pbek/qownnotes-overlay.git overlay

# checkout the source code
git clone --depth=1 git@github.com:pbek/QOwnNotes.git QOwnNotes -b $BRANCH
cd QOwnNotes

if [ -z $QOWNNOTES_VERSION ]; then
    # get version from version.h
    QOWNNOTES_VERSION=`cat src/version.h | sed "s/[^0-9,.]//g"`
fi

ARCHIVE_FILE=qownnotes-${QOWNNOTES_VERSION}.tar.xz

cd ../overlay/app-office/qownnotes/
cp ../../../QOwnNotes/build-systems/gentoo/qownnotes.ebuild .

# replace the version in the ebuild file
sed -i "s/VERSION-STRING/$QOWNNOTES_VERSION/g" qownnotes.ebuild

# update the Manifest file
echo "DIST ${ARCHIVE_FILE} ${QOWNNOTES_ARCHIVE_SIZE} SHA512 ${QOWNNOTES_ARCHIVE_SHA512}" >> Manifest

eBuildFile="qownnotes-$QOWNNOTES_VERSION.ebuild"
mv qownnotes.ebuild ${eBuildFile}

echo "Committing changes..."
git add ${eBuildFile}
git commit -m "releasing version $QOWNNOTES_VERSION" ${eBuildFile} Manifest
git push

# remove everything after we are done
if [ -d $PROJECT_PATH ]; then
    rm -rf $PROJECT_PATH
fi
