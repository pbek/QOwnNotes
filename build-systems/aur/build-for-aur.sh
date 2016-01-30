#!/usr/bin/env bash
#
# This is the build/deploy script for the Arch Linux User Repository (AUR)
# https://aur.archlinux.org/packages/qownnotes-pbek/
#
# We need a ssh key to push to the git repository
# https://wiki.archlinux.org/index.php/Arch_User_Repository#Creating_a_new_package
#

# uncomment this if you want to force a version
#QOWNNOTES_VERSION=0.84.5

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
git clone --depth=5 ssh://aur@aur.archlinux.org/qownnotes-pbek.git aur

# checkout the source code
git clone --depth=5 git@github.com:pbek/QOwnNotes.git QOwnNotes -b $BRANCH
cd QOwnNotes

# checkout submodules
git submodule update --init

# remove the huge .git folder
rm -Rf .git

if [ -z $QOWNNOTES_VERSION ]; then
    # get version from version.h
    QOWNNOTES_VERSION=`cat src/version.h | sed "s/[^0-9,.]//g"`
else
    # set new version if we want to override it
    echo "#define VERSION \"$QOWNNOTES_VERSION\"" > src/version.h
fi

# set the release string
echo "#define RELEASE \"AUR\"" > src/release.h

cd ..
echo "Creating archive..."
# archive the source code
tar -cjf aur/qownnotes.tar.bz2 QOwnNotes

cd aur
cp ../QOwnNotes/building/aur/PKGBUILD .
cp ../QOwnNotes/building/aur/.SRCINFO .

# replace the version in the PKGBUILD file
sed -i "s/VERSION-STRING/$QOWNNOTES_VERSION/g" PKGBUILD

# replace the version in the dsc file
sed -i "s/VERSION-STRING/$QOWNNOTES_VERSION/g" SRCINFO

echo "Committing changes..."
git commit -m "releasing version $QOWNNOTES_VERSION"
#git push

# remove everything after we are done
#if [ -d $PROJECT_PATH ]; then
#    rm -rf $PROJECT_PATH
#fi
