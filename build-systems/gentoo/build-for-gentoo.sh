#!/usr/bin/env bash
#
# This is the build/deploy script for the Gentoo QOwnNotes overlay
# https://github.com/pbek/qownnotes-overlay
#

# uncomment this if you want to force a version
QOWNNOTES_VERSION=1.1.0.1

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
git clone --depth=5 https://github.com/pbek/qownnotes-overlay.git overlay

# checkout the source code
git clone --depth=5 git@github.com:pbek/QOwnNotes.git QOwnNotes -b $BRANCH
cd QOwnNotes

# checkout submodules
git submodule update --init

gitCommitHash=`git rev-parse HEAD`
gitCommitHash2=`cd src/libraries/qmarkdowntextedit && git rev-parse HEAD`
gitCommitHash3=`cd src/libraries/piwiktracker && git rev-parse HEAD`
echo "Current commit: $gitCommitHash"
echo "Current commit2: $gitCommitHash2"
echo "Current commit3: $gitCommitHash3"

if [ -z $QOWNNOTES_VERSION ]; then
    # get version from version.h
    QOWNNOTES_VERSION=`cat src/version.h | sed "s/[^0-9,.]//g"`
fi

cd ../overlay/app-office/qownnotes/
cp ../QOwnNotes/build-systems/gentoo/qownnotes.ebuild .

# replace the version in the PKGBUILD file
sed -i "s/VERSION-STRING/$QOWNNOTES_VERSION/g" qownnotes.ebuild

# replace the commit hash in the PKGBUILD file
sed -i "s/COMMIT-HASH3/$gitCommitHash3/g" qownnotes.ebuild
sed -i "s/COMMIT-HASH2/$gitCommitHash2/g" qownnotes.ebuild
sed -i "s/COMMIT-HASH/$gitCommitHash/g" qownnotes.ebuild

eBuildFile="qownnotes-$QOWNNOTES_VERSION.ebuild"
mv qownnotes.ebuild $eBuildFile


echo "Committing changes..."
git add $eBuildFile
git commit -m "releasing version $QOWNNOTES_VERSION" $eBuildFile Manifest
git push

# remove everything after we are done
if [ -d $PROJECT_PATH ]; then
    rm -rf $PROJECT_PATH
fi
