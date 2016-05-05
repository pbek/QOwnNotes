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

BRANCH=develop
#BRANCH=master

PROJECT_PATH="/tmp/QOwnNotes-sourceforge-$$"
CUR_DIR=$(pwd)


echo "Started the SourceForge packaging process, using latest '$BRANCH' git tree"

if [ -d $PROJECT_PATH ]; then
    rm -rf $PROJECT_PATH
fi

mkdir $PROJECT_PATH
cd $PROJECT_PATH

echo "Project path: $PROJECT_PATH"

# checkout the source code
git clone --depth=50 git@github.com:pbek/QOwnNotes.git QOwnNotes -b $BRANCH
cd QOwnNotes

# checkout submodules
git submodule update --init

# remove huge .git folder
rm -Rf .git

if [ -z $QOWNNOTES_VERSION ]; then
    # get version from version.h
    QOWNNOTES_VERSION=`cat src/version.h | sed "s/[^0-9,.]//g"`
else
    # set new version if we want to override it
    echo "#define VERSION \"$QOWNNOTES_VERSION\"" > src/version.h
fi

# set the release string
echo "#define RELEASE \"SourceForge\"" > src/release.h

echo "Using version $QOWNNOTES_VERSION..."

qownnotesSrcDir="qownnotes-${QOWNNOTES_VERSION}"

# copy some needed files file
cp LICENSE src
cp README.md src
cp CHANGELOG.md src
cp SHORTCUTS.md src

# rename the src directory
mv src $qownnotesSrcDir

archiveFile="$qownnotesSrcDir.tar.xz"

# archive the source code
echo "Creating archive $archiveFile..."
tar -cJf $archiveFile $qownnotesSrcDir

echo "Uploading archive to SourceForge..."

#rsync -ahv --progress $archiveFile patbek,qownnotes@frs.sourceforge.net:/src
rsync -ahv --progress $archiveFile patbek@frs.sourceforge.net:/home/frs/project/qownnotes/src

# remove everything after we are done
if [ -d $PROJECT_PATH ]; then
    rm -rf $PROJECT_PATH
fi
