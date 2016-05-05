#!/usr/bin/env bash
#
# This is the build/deploy script for the Gentoo QOwnNotes overlay
# https://github.com/pbek/qownnotes-overlay
#

# uncomment this if you want to force a version
#QOWNNOTES_VERSION=1.1.0.4

BRANCH=develop
#BRANCH=master

PROJECT_PATH="/tmp/QOwnNotes-gentoo-$$"
CUR_DIR=$(pwd)


echo "Started the ebuild packaging process, using latest '$BRANCH' git tree"

if [ -d $PROJECT_PATH ]; then
    rm -rf $PROJECT_PATH
fi

mkdir $PROJECT_PATH
cd $PROJECT_PATH

echo "Project path: $PROJECT_PATH"

# checkout AUR repository
git clone --depth=5 git@github.com:pbek/qownnotes-overlay.git overlay

# checkout the source code
git clone --depth=5 git@github.com:pbek/QOwnNotes.git QOwnNotes -b $BRANCH
cd QOwnNotes

if [ -z $QOWNNOTES_VERSION ]; then
    # get version from version.h
    QOWNNOTES_VERSION=`cat src/version.h | sed "s/[^0-9,.]//g"`
fi

cd ..

ARCHIVE_FILE=qownnotes-${QOWNNOTES_VERSION}.tar.xz
wget http://downloads.sourceforge.net/project/qownnotes/src/${ARCHIVE_FILE}

ARCHIVE_SHA512=`sha512sum ${ARCHIVE_FILE} | awk '{ print $1 }'`
ARCHIVE_SIZE=`stat -c "%s" ${ARCHIVE_FILE}`

cd QOwnNotes

echo "Archive sha512: ${ARCHIVE_SHA512}"
echo "Archive size: ${ARCHIVE_SIZE}"

cd ../overlay/app-office/qownnotes/
cp ../../../QOwnNotes/build-systems/gentoo/qownnotes.ebuild .

# replace the version in the ebuild file
sed -i "s/VERSION-STRING/$QOWNNOTES_VERSION/g" qownnotes.ebuild

# update the Manifest file
echo "DIST ${ARCHIVE_FILE} ${ARCHIVE_SIZE} SHA512 ${ARCHIVE_SHA512}" >> Manifest

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
