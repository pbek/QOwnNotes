#!/bin/sh
#
# This is the build script for Ubuntu Launchpad
# https://launchpad.net/~pbek/+archive/ubuntu/qownnotes
#
# We will need some packages to execute this locally:
# sudo apt-get install build-essential autoconf automake autotools-dev dh-make debhelper devscripts fakeroot xutils lintian pbuilder cdbs
#
# The GPG public key $GPG_PUBLIC_KEY also has to be in place locally
#

# uncomment this if you want to force a version
#QOWNNOTES_VERSION=0.67.5

BRANCH=develop
#BRANCH=master
DISTRO=wily

DATE=$(LC_ALL=C date +'%a, %d %b %Y %T %z')
PROJECT_PATH="/tmp/QOwnNotes-$$"
CUR_DIR=$(pwd)
UPLOAD="true"
DEBUILD_ARGS=""
GPG_PUBLIC_KEY=D55B7124
CHANGELOG_TEXT="Released $QOWNNOTES_VERSION"
export DEBFULLNAME="Patrizio Bekerle"
export DEBEMAIL="patrizio@bekerle.com"


while test $# -gt 0
do
    case "$1" in
        --no-upload) UPLOAD="false"
            ;;
        --no-orig-tar-upload) DEBUILD_ARGS="-sd"
            ;;
    esac
    shift
done

echo "Started the debian source packaging process, using latest '$BRANCH' git tree"

if [ -d $PROJECT_PATH ]; then
    rm -rf $PROJECT_PATH
fi

# checkout the source code
git clone git@github.com:pbek/QOwnNotes.git $PROJECT_PATH
cd $PROJECT_PATH
git checkout $BRANCH

if [ -z $QOWNNOTES_VERSION ]; then
    # get version from version.h
    QOWNNOTES_VERSION=`cat src/version.h | sed "s/[^0-9,.]//g"`
else
    # set new version if we want to override it
    echo "#define VERSION \"$QOWNNOTES_VERSION\"" > src/version.h
fi

QOWNNOTES_SRC_DIR="qownnotes_$QOWNNOTES_VERSION"

# rename and archive the src directory
mv src $QOWNNOTES_SRC_DIR
tar -czf $QOWNNOTES_SRC_DIR.orig.tar.gz $QOWNNOTES_SRC_DIR

cd $QOWNNOTES_SRC_DIR

echo "Using version $QOWNNOTES_VERSION..."

# update the changelog file
dch -v $QOWNNOTES_VERSION-1ubuntu1 $CHANGELOG_TEXT
dch -r $CHANGELOG_TEXT

#Create the changelog file
#echo "qownnotes ($QOWNNOTES_VERSION-1ubuntu1) $DISTRO; urgency=low" > debian/changelog
#echo "" >> debian/changelog
#echo "  * Released $QOWNNOTES_VERSION" >> debian/changelog
#echo "" >> debian/changelog
#echo " -- $DEBFULLNAME <$DEBEMAIL>  $DATE" >> debian/changelog

# launch debuild
debuild -S -sa -k$GPG_PUBLIC_KEY $DEBUILD_ARGS
cd ..

# send to launchpad
if [ "$UPLOAD" = "true" ]; then
    dput ppa:pbek/qownnotes $QOWNNOTES_SRC_DIR-1ubuntu1_source.changes
fi;

# remove everything after we are done
if [ -d $PROJECT_PATH ]; then
    rm -rf $PROJECT_PATH
fi
