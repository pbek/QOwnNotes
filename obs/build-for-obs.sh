#!/usr/bin/env bash
#
# This is the build script for the openSUSE Build Service (OBS)
# https://build.opensuse.org/package/show/home:pbek:QOwnNotes/desktop
#
# We will need some packages to execute this locally:
# sudo apt-get install osc xz
#
# osc needs m2crypto:
# pip3 install m2crypto
#
# A file ~/.oscrc will be generated upon first start of osc
#

# uncomment this if you want to force a version
#QOWNNOTES_VERSION=19.6.1.4

BRANCH=main
#BRANCH=release

DATE=$(LC_ALL=C date +'%a, %d %b %Y %T %z')
PROJECT_PATH="/tmp/QOwnNotes-$$"

echo "Started the OBS source packaging process, using latest '$BRANCH' git tree"

if [ -d $PROJECT_PATH ]; then
  rm -rf $PROJECT_PATH
fi

mkdir $PROJECT_PATH
cd $PROJECT_PATH || exit 1

echo "Project path: $PROJECT_PATH"

# checkout the source code
git clone --depth=1 git@github.com:pbek/QOwnNotes.git QOwnNotes -b $BRANCH
cd QOwnNotes || exit 1

# checkout submodules
git submodule update --init

# build binary translation files
lrelease src/QOwnNotes.pro

# remove huge .git folder
rm -Rf .git

if [ -z $QOWNNOTES_VERSION ]; then
  # get version from version.h
  QOWNNOTES_VERSION=$(cat src/version.h | sed "s/[^0-9,.]//g")
else
  # set new version if we want to override it
  echo "#define VERSION \"$QOWNNOTES_VERSION\"" >src/version.h
fi

# set the release string
echo '#define RELEASE "OBS"' >src/release.h

# replace the version in the spec file
sed -i "s/VERSION-STRING/$QOWNNOTES_VERSION/g" obs/qownnotes.spec

# replace the version in the PKGBUILD file
sed -i "s/VERSION-STRING/$QOWNNOTES_VERSION/g" obs/PKGBUILD

# replace the version in the dsc file
sed -i "s/VERSION-STRING/$QOWNNOTES_VERSION/g" obs/qownnotes.dsc

changelogText="Released version $QOWNNOTES_VERSION"

echo "Using version $QOWNNOTES_VERSION..."

qownnotesSrcDir="qownnotes-${QOWNNOTES_VERSION}"

# copy some needed files file
cp LICENSE src
cp README.md src
cp CHANGELOG.md src
cp webpage/src/getting-started/shortcuts.md src

# rename the src directory
mv src $qownnotesSrcDir

changelogPath=obs/qownnotes.bin

# create the changelog file
echo "-------------------------------------------------------------------" >$changelogPath
echo "$DATE - patrizio@bekerle.com" >>$changelogPath
echo "" >>$changelogPath
echo "- $changelogText" >>$changelogPath

cat $changelogPath

# create the Debian changelog file
debChangelogPath=obs/debian.changelog
versionPart="$QOWNNOTES_VERSION-1debian"
echo "qownnotes ($versionPart) debian; urgency=low" >$debChangelogPath
echo "" >>$debChangelogPath
echo "  * $changelogText" >>$debChangelogPath
echo "" >>$debChangelogPath
echo " -- Patrizio Bekerle <patrizio@bekerle.com>  $DATE" >>$debChangelogPath

cat $debChangelogPath

archiveFile="$qownnotesSrcDir.tar.xz"

# archive the source code
echo "Creating archive $archiveFile..."
tar -cJf $archiveFile $qownnotesSrcDir

echo "Checking out OBS repository..."

# checkout OBS repository
osc checkout home:pbek:QOwnNotes desktop

obsRepoPath="home:pbek:QOwnNotes/desktop"

# remove other archives
echo "Removing old archives..."
cd $obsRepoPath || exit 1
osc rm *.xz
cd ../..

# copying new files to repository
mv $archiveFile $obsRepoPath
cp obs/qownnotes.bin $obsRepoPath
cp obs/qownnotes.spec $obsRepoPath
cp obs/appimage.yml $obsRepoPath
cp obs/_service $obsRepoPath
cp obs/qownnotes.appdata.xml $obsRepoPath
cp $debChangelogPath $obsRepoPath
cp obs/PKGBUILD $obsRepoPath
cp $qownnotesSrcDir/debian/control $obsRepoPath/debian.control
cp $qownnotesSrcDir/debian/copyright $obsRepoPath/debian.copyright
cp $qownnotesSrcDir/debian/compat $obsRepoPath/debian.compat
cp $qownnotesSrcDir/debian/rules $obsRepoPath/debian.rules
cp $qownnotesSrcDir/debian/qownnotes.install $obsRepoPath/debian.qownnotes.install
cp obs/qownnotes.dsc $obsRepoPath

cd $obsRepoPath || exit 1

# add all new files
osc add $archiveFile
#osc add qownnotes.bin
#osc add qownnotes.spec
#osc add PKGBUILD
#osc add debian.changelog
#osc add debian.control
#osc add debian.rules
#osc add debian.copyright
#osc add debian.compat
#osc add debian.qownnotes.install
#osc add qownnotes.dsc
#osc add appimage.yml
#osc add _service
#osc add qownnotes.appdata.xml

echo "Committing changes..."

# commit changes
osc commit -m "$changelogText"

# remove everything after we are done
if [ -d $PROJECT_PATH ]; then
  rm -rf $PROJECT_PATH
fi
