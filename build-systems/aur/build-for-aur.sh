#!/usr/bin/env bash
#
# This is the build/deploy script for the Arch Linux User Repository (AUR)
# https://aur.archlinux.org/packages/qownnotes/
#
# We need a ssh key to push to the git repository
# https://wiki.archlinux.org/index.php/Arch_User_Repository#Creating_a_new_package
#

# uncomment this if you want to force a version
#QOWNNOTES_VERSION=1.1.3.2

BRANCH=develop
#BRANCH=master

PROJECT_PATH="/tmp/QOwnNotes-aur-$$"
CUR_DIR=$(pwd)


echo "Started the AUR packaging process, using latest '$BRANCH' git tree"

if [ -d $PROJECT_PATH ]; then
    rm -rf $PROJECT_PATH
fi

mkdir $PROJECT_PATH
cd $PROJECT_PATH

echo "Project path: $PROJECT_PATH"

# checkout AUR repository
git clone --depth=5 ssh://aur@aur.archlinux.org/qownnotes.git aur

# checkout the source code
git clone --depth=5 git@github.com:pbek/QOwnNotes.git QOwnNotes -b $BRANCH
cd QOwnNotes

gitCommitHash=`git rev-parse HEAD`
echo "Current commit: $gitCommitHash"

if [ -z $QOWNNOTES_VERSION ]; then
    # get version from version.h
    QOWNNOTES_VERSION=`cat src/version.h | sed "s/[^0-9,.]//g"`
fi

cd ../aur
cp ../QOwnNotes/build-systems/aur/PKGBUILD .
cp ../QOwnNotes/build-systems/aur/.SRCINFO .

# replace the version in the PKGBUILD file
sed -i "s/VERSION-STRING/$QOWNNOTES_VERSION/g" PKGBUILD

# replace the commit hash in the PKGBUILD file
sed -i "s/COMMIT-HASH/$gitCommitHash/g" PKGBUILD

# replace the version in the .SRCINFO file
sed -i "s/VERSION-STRING/$QOWNNOTES_VERSION/g" .SRCINFO


echo "Committing changes..."
git commit -m "releasing version $QOWNNOTES_VERSION" PKGBUILD .SRCINFO
git push

# remove everything after we are done
if [ -d $PROJECT_PATH ]; then
    rm -rf $PROJECT_PATH
fi
