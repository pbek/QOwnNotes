#!/usr/bin/env bash
#
# This is the build script for Ubuntu Launchpad
# https://launchpad.net/~pbek/+archive/ubuntu/qownnotes-qt6
#
# We will need some packages to execute this locally:
# sudo apt-get install build-essential autoconf automake autotools-dev dh-make debhelper devscripts fakeroot xutils lintian pbuilder cdbs gnupg
#
# Also a ~/.dput.cf has to be in place
#

# Exit immediately if any command fails
set -e

# uncomment this if you want to force a version
#QOWNNOTES_VERSION=25.5.8.10

BRANCH=main
#BRANCH=release

# https://wiki.ubuntu.com/Releases
UBUNTU_RELEASES=("noble" "plucky" "questing" "resolute")

DATE=$(LC_ALL=C date +'%a, %d %b %Y %T %z')
PROJECT_PATH="/tmp/QOwnNotes-$$"
UPLOAD="true"
DEBUILD_ARGS=""
SIGNING_EMAIL=patrizio@bekerle.com
export DEBFULLNAME="Patrizio Bekerle"
export DEBEMAIL="patrizio@bekerle.com"

# This is used inside the release docker container
if [ "$1" = "--docker" ]; then
  echo "Importing PGP key..."
  gpg --import ~/private.pgp
  echo "Adding AUR ssh key..."
  eval $(ssh-agent -s)
  ssh-add ~/.ssh/aur_rsa
fi

#echo "/usr/share/cdbs/1/class/qmake.mk"
#cat /usr/share/cdbs/1/class/qmake.mk

while test $# -gt 0; do
  case "$1" in
  --no-upload)
    UPLOAD="false"
    ;;
  --no-orig-tar-upload)
    DEBUILD_ARGS="-sd"
    ;;
  esac
  shift
done

echo "Started the debian source packaging process, using latest '$BRANCH' git tree"

if [ -d $PROJECT_PATH ]; then
  rm -rf $PROJECT_PATH
fi

# checkout the source code
#rsync -avh --exclude-from=/QOwnNotes/.gitignore --exclude .git --exclude secrets --exclude webpage /QOwnNotes/ "$PROJECT_PATH/"
git clone --recurse-submodules --depth=1 https://github.com/pbek/QOwnNotes.git $PROJECT_PATH -b $BRANCH
cd $PROJECT_PATH || exit 1

# build binary translation files
lrelease src/QOwnNotes.pro

if [ -z $QOWNNOTES_VERSION ]; then
  # get version from version.h
  QOWNNOTES_VERSION=$(cat src/version.h | sed "s/[^0-9,.]//g")
else
  # set new version if we want to override it
  echo "#define VERSION \"$QOWNNOTES_VERSION\"" >src/version.h
fi

# set release string to disable the update check
echo '#define RELEASE "Launchpad PPA Qt6"' >src/release.h

changelogText="Released version $QOWNNOTES_VERSION"

echo "Using version $QOWNNOTES_VERSION..."

qownnotesSrcDir="qownnotes-qt6_${QOWNNOTES_VERSION}"

# copy the src directory
cp -R src $qownnotesSrcDir

# DONE: Remove me after commit
#cp /QOwnNotes/src/dialogs/evernoteimportdialog.h $qownnotesSrcDir/dialogs

# archive the source code
tar -czf $qownnotesSrcDir.orig.tar.gz $qownnotesSrcDir

changelogPath=debian/changelog

# check if the signing key is available
gpg --list-secret-keys

# build for every Ubuntu release
for ubuntuRelease in "${UBUNTU_RELEASES[@]}"; do
  :
  echo "Building for $ubuntuRelease..."
  cd $qownnotesSrcDir || exit 1

  # get the modified qt6 files in place
  #    cp ../ubuntu-launchpad/qt6/* debian
  cp /QOwnNotes/ubuntu-launchpad/qt6/* debian

  # Remove obsolete compat file, we are using "debhelper-compat (= 13)"
  rm -f debian/compat

  # Remove obsolete install file for Qt5 build
  rm -f debian/qownnotes.install

  versionPart="$QOWNNOTES_VERSION-1ubuntu3ppa1~${ubuntuRelease}1"

  # update the changelog file
  #dch -v $versionPart $changelogText
  #dch -r $changelogText

  # create the changelog file
  echo "qownnotes-qt6 ($versionPart) $ubuntuRelease; urgency=low" >$changelogPath
  echo "" >>$changelogPath
  echo "  * $changelogText" >>$changelogPath
  echo "" >>$changelogPath
  echo " -- $DEBFULLNAME <$DEBEMAIL>  $DATE" >>$changelogPath

  # launch debuild
  debuild -S -sa -k$SIGNING_EMAIL $DEBUILD_ARGS
  cd ..

  # send to launchpad
  if [ "$UPLOAD" = "true" ]; then
    dput ppa:pbek/qownnotes-qt6 qownnotes-qt6_${versionPart}_source.changes
  fi
done

# remove everything after we are done
if [ -d $PROJECT_PATH ]; then
  rm -rf $PROJECT_PATH
fi
