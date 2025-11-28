#!/usr/bin/env bash
#
# This is the build/deploy script for the Slackware QOwnNotes SlackBuild
# repository
# https://github.com/pbek/qownnotes-slackbuilds
#
# The QOwnNotes repository for Slackware relies on our source archive on
# https://github.com/pbek/QOwnNotes/releases
#

# uncomment this if you want to force a version
#QOWNNOTES_VERSION=16.06.12

BRANCH=main
#BRANCH=master

PROJECT_PATH="/tmp/QOwnNotes-slackware-$$"

# use temporary checksum variable file
_QQwnNotesCheckSumVarFile="/tmp/QOwnNotes.checksum.vars"

if [[ ! -f ${_QQwnNotesCheckSumVarFile} ]]; then
  echo "${_QQwnNotesCheckSumVarFile} doesn't exist. build-github-src.sh must be run ahead of build script!"
  exit 1
fi

# shellcheck source=/dev/null
source "${_QQwnNotesCheckSumVarFile}"

# check checksum variable from build-systems/github/build-github-src.sh
if [ -z "${QOWNNOTES_ARCHIVE_MD5}" ]; then
  echo "QOWNNOTES_ARCHIVE_MD5 was not set!"
  exit 1
fi

echo "Started the slackware packaging process, using latest '$BRANCH' git tree"

if [ -d "$PROJECT_PATH" ]; then
  rm -rf "$PROJECT_PATH"
fi

mkdir "$PROJECT_PATH"
cd "$PROJECT_PATH" || exit 1

echo "Project path: $PROJECT_PATH"

# checkout slackbuilds repository
git clone --depth=1 git@github.com:pbek/qownnotes-slackbuilds.git slackbuilds

# checkout the source code
git clone --depth=1 git@github.com:pbek/QOwnNotes.git QOwnNotes -b "$BRANCH"
cd QOwnNotes || exit 1

if [ -z "$QOWNNOTES_VERSION" ]; then
  # get version from version.h
  QOWNNOTES_VERSION=$(cat src/version.h | sed "s/[^0-9,.]//g")
fi

buildSystemPath="../../../QOwnNotes/build-systems/slackware"

cd ../slackbuilds/14.1/qownnotes/ || exit 1
cp ${buildSystemPath}/qownnotes.SlackBuild .
cp ${buildSystemPath}/qownnotes.info .
cp ${buildSystemPath}/dobuild.sh .

# replace the version in the build file
sed -i "s/VERSION-STRING/$QOWNNOTES_VERSION/g" qownnotes.SlackBuild
sed -i "s/VERSION-STRING/$QOWNNOTES_VERSION/g" qownnotes.info
sed -i "s/VERSION-STRING/$QOWNNOTES_VERSION/g" dobuild.sh

# replace the md5sum
sed -i "s/ARCHIVE-MD5/$QOWNNOTES_ARCHIVE_MD5/g" qownnotes.info

path14_2="../../14.2/qownnotes"
cp qownnotes.SlackBuild ${path14_2}
cp qownnotes.info ${path14_2}
cp dobuild.sh ${path14_2}

path15_0="../../15.0/qownnotes"
cp qownnotes.SlackBuild ${path15_0}
cp qownnotes.info ${path15_0}
sed -i 's/REQUIRES="qt5 libproxy"/REQUIRES=""/' ${path15_0}/qownnotes.info
cp dobuild.sh ${path15_0}

echo "Committing changes..."
# shellcheck disable=SC2035
git commit -m "releasing version $QOWNNOTES_VERSION" -- * ${path14_2}/* ${path15_0}/*
git push

# remove everything after we are done
if [ -d "$PROJECT_PATH" ]; then
  rm -rf "$PROJECT_PATH"
fi
