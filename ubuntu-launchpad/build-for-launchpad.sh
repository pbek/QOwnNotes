#!/bin/sh

QOWNNOTES_VERSION=0.67.3
BRANCH=develop
#BRANCH=master
DISTRO=wily

DATE=$(LC_ALL=C date +'%a, %d %b %Y %T %z')
QOWNNOTES_SRC_DIR="qownnotes_$QOWNNOTES_VERSION"
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

if [ -d "QOwnNotes" ]; then
    rm -rf QOwnNotes
fi

# checkout the source code
git clone git@github.com:pbek/QOwnNotes.git
cd QOwnNotes
git co $BRANCH

# rename and archive the src directory
mv src $QOWNNOTES_SRC_DIR
tar -czf $QOWNNOTES_SRC_DIR.orig.tar.gz $QOWNNOTES_SRC_DIR

cd $QOWNNOTES_SRC_DIR

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
