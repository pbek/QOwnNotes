#!/usr/bin/env bash
#
# This is the build/deploy script for the Slackware QOwnNotes SlackBuild
# repository
# https://github.com/pbek/qownnotes-slackbuilds
#
# The QOwnNotes repository for Slackware relies on our source archive on
# https://sourceforge.net/projects/qownnotes/files/src/
#


# uncomment this if you want to force a version
#QOWNNOTES_VERSION=16.06.12

BRANCH=develop
#BRANCH=master

PROJECT_PATH="/tmp/QOwnNotes-slackware-$$"
CUR_DIR=$(pwd)


echo "Started the slackware packaging process, using latest '$BRANCH' git tree"

if [ -d $PROJECT_PATH ]; then
    rm -rf $PROJECT_PATH
fi

mkdir $PROJECT_PATH
cd $PROJECT_PATH

echo "Project path: $PROJECT_PATH"

# checkout slackbuilds repository
git clone --depth=1 git@github.com:pbek/qownnotes-slackbuilds.git slackbuilds

# checkout the source code
git clone --depth=1 git@github.com:pbek/QOwnNotes.git QOwnNotes -b $BRANCH
cd QOwnNotes

if [ -z $QOWNNOTES_VERSION ]; then
    # get version from version.h
    QOWNNOTES_VERSION=`cat src/version.h | sed "s/[^0-9,.]//g"`
fi

cd ..

ARCHIVE_FILE=qownnotes-${QOWNNOTES_VERSION}.tar.xz
wget https://download.tuxfamily.org/qownnotes/src/${ARCHIVE_FILE}

ARCHIVE_SHA512=`sha512sum ${ARCHIVE_FILE} | awk '{ print $1 }'`
ARCHIVE_MD5=`md5sum ${ARCHIVE_FILE} | awk '{ print $1 }'`
ARCHIVE_SIZE=`stat -c "%s" ${ARCHIVE_FILE}`

cd QOwnNotes

echo "Archive md5: ${ARCHIVE_MD5}"
echo "Archive size: ${ARCHIVE_SIZE}"

if [ -z ${ARCHIVE_MD5} ]; then
    echo
    echo "Archive md5 is empty!"
    exit 1
fi

buildSystemPath="../../../QOwnNotes/build-systems/slackware"

cd ../slackbuilds/14.1/qownnotes/
cp ${buildSystemPath}/qownnotes.SlackBuild .
cp ${buildSystemPath}/qownnotes.info .
cp ${buildSystemPath}/dobuild.sh .

# replace the version in the build file
sed -i "s/VERSION-STRING/$QOWNNOTES_VERSION/g" qownnotes.SlackBuild
sed -i "s/VERSION-STRING/$QOWNNOTES_VERSION/g" qownnotes.info
sed -i "s/VERSION-STRING/$QOWNNOTES_VERSION/g" dobuild.sh

# replace the md5sum
sed -i "s/ARCHIVE-MD5/$ARCHIVE_MD5/g" qownnotes.info

path14_2="../../14.2/qownnotes"
cp qownnotes.SlackBuild ${path14_2}
cp qownnotes.info ${path14_2}
cp dobuild.sh ${path14_2}

echo "Committing changes..."
git commit -m "releasing version $QOWNNOTES_VERSION" * ${path14_2}/*
git push

# remove everything after we are done
if [ -d $PROJECT_PATH ]; then
    rm -rf $PROJECT_PATH
fi
