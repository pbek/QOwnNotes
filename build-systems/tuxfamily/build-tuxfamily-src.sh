#!/usr/bin/env bash
#
# This is the deploy script of the source archives for TuxFamily
#
# We need to setup a ssh key to copy files to TuxFamily
# https://faq.tuxfamily.org/User/En#SSH_access
# <~/.ssh/id_rsa.pub ssh pbek@ssh.tuxfamily.org "umask 077; cat >~/ssh_keys"
#
# Files will be uploaded to:
# https://download.tuxfamily.org/qownnotes/src
#
# Files will be accessible like that:
# https://download.tuxfamily.org/qownnotes/src/qownnotes-18.03.2.tar.xz
#
# Be aware that it usually can take a minute until the files are accessible
# at that url
#

# uncomment this if you want to force a version
#QOWNNOTES_VERSION=1.1.3.2

#BRANCH=main
BRANCH=release
#BRANCH=master

PROJECT_PATH="/tmp/QOwnNotes-tuxfamily-$$"

echo "Started the TuxFamily packaging process, using latest '$BRANCH' git tree"

if [ -d $PROJECT_PATH ]; then
  rm -rf $PROJECT_PATH
fi

mkdir $PROJECT_PATH
cd $PROJECT_PATH || exit 1

echo "Project path: $PROJECT_PATH"

# checkout the source code with checkout submodules
git clone --depth=1 https://github.com/pbek/QOwnNotes.git QOwnNotes -b $BRANCH --recursive
cd QOwnNotes || exit 1

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
echo '#define RELEASE "TuxFamily"' >src/release.h

echo "Using version $QOWNNOTES_VERSION..."

qownnotesSrcDir="qownnotes-${QOWNNOTES_VERSION}"

# copy some needed files file
cp LICENSE src
cp README.md src
cp CHANGELOG.md src
cp webpage/src/getting-started/shortcuts.md src

# rename the src directory
mv src $qownnotesSrcDir

archiveFile="$qownnotesSrcDir.tar.xz"

# archive the source code
echo "Creating archive $archiveFile..."
tar -cJf $archiveFile $qownnotesSrcDir

QOWNNOTES_ARCHIVE_MD5=$(md5sum ${archiveFile} | awk '{ print $1 }' | tee ${archiveFile}.md5)
QOWNNOTES_ARCHIVE_SHA256=$(sha256sum ${archiveFile} | awk '{ print $1 }' | tee ${archiveFile}.sha256)
QOWNNOTES_ARCHIVE_SHA512=$(sha512sum ${archiveFile} | awk '{ print $1 }' | tee ${archiveFile}.sha512)
QOWNNOTES_ARCHIVE_SIZE=$(stat -c "%s" ${archiveFile})

echo ""
echo "Sums:"
echo $QOWNNOTES_ARCHIVE_MD5
echo $QOWNNOTES_ARCHIVE_SHA256
echo $QOWNNOTES_ARCHIVE_SHA512
echo ""
echo "Size:"
echo $QOWNNOTES_ARCHIVE_SIZE

# write temporary checksum variable file for the deployment scripts
_QQwnNotesCheckSumVarFile="/tmp/QOwnNotes.checksum.vars"
echo "QOWNNOTES_ARCHIVE_MD5=$QOWNNOTES_ARCHIVE_MD5" >${_QQwnNotesCheckSumVarFile}
echo "QOWNNOTES_ARCHIVE_SHA256=$QOWNNOTES_ARCHIVE_SHA256" >>${_QQwnNotesCheckSumVarFile}
echo "QOWNNOTES_ARCHIVE_SHA512=$QOWNNOTES_ARCHIVE_SHA512" >>${_QQwnNotesCheckSumVarFile}
echo "QOWNNOTES_ARCHIVE_SIZE=$QOWNNOTES_ARCHIVE_SIZE" >>${_QQwnNotesCheckSumVarFile}

remotePath="pbek@ssh.tuxfamily.org:/home/qownnotes/qownnotes-repository/src"
tuxFamilyReadme="tuxfamily-readme.md"

# generate the readme for tuxfamily with a screenshot from GitHub
cat README.md | sed 's/screenshots\/screenshot.png/https:\/\/raw.githubusercontent.com\/pbek\/QOwnNotes\/main\/screenshots\/screenshot.png/g' >>${tuxFamilyReadme}
echo >>${tuxFamilyReadme}
echo >>${tuxFamilyReadme}
cat CHANGELOG.md >>${tuxFamilyReadme}

echo "Uploading files to TuxFamily..."

# upload the changelog file as README.md
# it will be viewed on the tuxfamily webpage
rsync -ahv --progress ${tuxFamilyReadme} ${remotePath}/README.md

## upload the screenshots
#rsync -ahv --progress screenshots/* ${remotePath}/screenshots

# upload archive and checksum files
rsync -ahv --progress ${archiveFile}* ${remotePath}

# remove everything after we are done
if [ -d $PROJECT_PATH ]; then
  rm -rf $PROJECT_PATH
fi

url="https://download.tuxfamily.org/qownnotes/src/${archiveFile}"
echo "Checking if $url exists..."

# try to download file from tuxfamily
until curl --output /dev/null --silent --head --fail "$url"; do
  echo "URL does not exist: $url..."
  sleep 2
done

echo "URL exists: $url"
exit 0
