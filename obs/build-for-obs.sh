#!/usr/bin/env bash
#
# This is the build script for the openSUSE Build Service (OBS)
# https://build.opensuse.org/package/show/home:pbek:QOwnNotes/qownnotes
#
# We will need some packages to execute this locally:
# sudo apt-get install osc xz
#
# A file ~/.oscrc will be generated upon first start of osc
#

# uncomment this if you want to force a version
QOWNNOTES_VERSION=0.84.1

BRANCH=develop
#BRANCH=master

DATE=$(LC_ALL=C date +'%a, %d %b %Y %T %z')
PROJECT_PATH="/tmp/QOwnNotes-$$"
CUR_DIR=$(pwd)


echo "Started the OBS source packaging process, using latest '$BRANCH' git tree"

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
git submodule init
git submodule update

# remove huge .git folder
rm -Rf .git

if [ -z $QOWNNOTES_VERSION ]; then
    # get version from version.h
    QOWNNOTES_VERSION=`cat src/version.h | sed "s/[^0-9,.]//g"`
else
    # set new version if we want to override it
    echo "#define VERSION \"$QOWNNOTES_VERSION\"" > src/version.h
fi

# set release string to disable the update check
echo "#define RELEASE \"openSUSE Build Service\"" > src/release.h

# replace version in spec file
sed -i "s/VERSION-STRING/$QOWNNOTES_VERSION/g" obs/qownnotes.spec

# replace version in PKGBUILD file
sed -i "s/VERSION-STRING/$QOWNNOTES_VERSION/g" obs/PKGBUILD

changelogText="Released $QOWNNOTES_VERSION"

echo "Using version $QOWNNOTES_VERSION..."

qownnotesSrcDir="qownnotes-${QOWNNOTES_VERSION}"
cd ..

# rename the directory
mv QOwnNotes $qownnotesSrcDir

changelogPath=$qownnotesSrcDir/obs/qownnotes.bin

# create the changelog file
echo "-------------------------------------------------------------------" > $changelogPath
echo "$DATE - patrizio@bekerle.com" >> $changelogPath
echo "" >> $changelogPath
echo "- $changelogText" >> $changelogPath

cat $changelogPath

archiveFile="$qownnotesSrcDir.tar.xz"

echo "Creating archive $archiveFile..."

# archive the source code
tar -cJf $archiveFile $qownnotesSrcDir

echo "Checking out OBS repository..."

# checkout OBS repository
osc checkout home:pbek:QOwnNotes desktop

obsRepoPath="home:pbek:QOwnNotes/desktop"

# remove other archives
echo "Removing old archives..."
cd $obsRepoPath
osc rm *.xz
cd ../..

# copying new files to repository
mv $archiveFile $obsRepoPath
cp $qownnotesSrcDir/obs/qownnotes.bin $obsRepoPath
cp $qownnotesSrcDir/obs/qownnotes.spec $obsRepoPath
cp $qownnotesSrcDir/obs/PKGBUILD $obsRepoPath

cd $obsRepoPath

# add all new files
osc add $archiveFile
#osc add qownnotes.bin
#osc add qownnotes.spec

echo "Commiting changes..."

# commit changes
osc commit -m "$changelogText"

# remove everything after we are done
if [ -d $PROJECT_PATH ]; then
    rm -rf $PROJECT_PATH
fi
