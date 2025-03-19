#!/bin/sh

#
# creating the QOwnNotes.dmg with Applications link
#

QTDIR="/Applications/Qt/5.4/clang_64"
APP=QOwnNotes
TEMPDIR=$APP-tmp
SIGNATURE="Patrizio Bekerle"
NAME=$(uname)

if [ "$NAME" != "Darwin" ]; then
  echo "This is not a Mac"
  exit 1
fi

# use macdeployqt to deploy the application
$QTDIR/bin/macdeployqt ./$APP.app
if [ "$?" -ne "0" ]; then
  echo "Failed to run macdeployqt"
  exit 1
fi

echo "Create $TEMPDIR"
#Create a temporary directory if one doesn't exist
mkdir -p $TEMPDIR
if [ "$?" -ne "0" ]; then
  echo "Failed to create temporary folder"
  exit 1
fi

echo "Clean $TEMPDIR"
#Delete the contents of any previous builds
rm -Rf ./$TEMPDIR/*
if [ "$?" -ne "0" ]; then
  echo "Failed to clean temporary folder"
  exit 1
fi

echo "Move application bundle"
#Move the application to the temporary directory
mv ./$APP.app ./$TEMPDIR
if [ "$?" -ne "0" ]; then
  echo "Failed to move application bundle"
  exit 1
fi

#echo "Sign the code"
##This signs the code
#echo "Sign Code with $SIGNATURE"
#codesign -s "$SIGNATURE" -f ./$TEMPDIR/$APP.app
#if [ "$?" -ne "0" ]; then
#    echo "Failed to sign app bundle"
#    exit 1
#fi

echo "Create symbolic link"
#Create a symbolic link to the applications folder
ln -s /Applications ./$TEMPDIR/Applications
if [ "$?" -ne "0" ]; then
  echo "Failed to create link to /Applications"
  exit 1
fi

echo "Create new disk image"
#Create the disk image
rm -f ./$APP.dmg
hdiutil create -srcfolder ./$TEMPDIR -ov -format UDBZ -fs HFS+ ./$APP.dmg
if [ "$?" -ne "0" ]; then
  echo "Failed to create disk image"
  exit 1
fi

# delete the temporary directory
rm -Rf ./$TEMPDIR/*
if [ "$?" -ne "0" ]; then
  echo "Failed to clean temporary folder"
  exit 1
fi

exit 0
