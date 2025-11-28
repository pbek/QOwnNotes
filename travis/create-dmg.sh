#!/bin/sh

#
# creating the QOwnNotes.dmg with Applications link
#

# we will get the $QTDIR from Travis CI
#QTDIR="/usr/local/opt/qt5"
APP=QOwnNotes
# this directory name will also be shown in the title when the DMG is mounted
TEMPDIR=$APP
# SIGNATURE="Patrizio Bekerle"
NAME=$(uname)
PLIST=$APP.app/Contents/Info.plist

if [ "$NAME" != "Darwin" ]; then
  echo "This is not a Mac"
  exit 1
fi

echo "Changing bundle identifier"
sed -i -e 's/com.yourcompany.QOwnNotes/com.PBE.QOwnNotes/g' $PLIST

# adding version number
/usr/libexec/PlistBuddy -c "Add :CFBundleVersion string $VERSION_NUMBER" $PLIST
/usr/libexec/PlistBuddy -c "Add :CFBundleShortVersionString string $VERSION_NUMBER" $PLIST

# removing CFBundleGetInfoString
/usr/libexec/PlistBuddy -c "Delete :CFBundleGetInfoString" $PLIST

# removing backup plist
rm -f ${PLIST}-e

# copy translation files to app
cp languages/*.qm $APP.app/Contents/Resources

# copy updater script to app
#chmod a+x ../travis/osx/update.command
#cp ../travis/osx/update.command $APP.app/Contents/MacOS

echo "Adding keys"
# add the keys for OSX code signing
security create-keychain -p travis osx-build.keychain
security import ../travis/osx/apple.cer -k ~/Library/Keychains/osx-build.keychain -T /usr/bin/codesign
security import ../travis/osx/dist.cer -k ~/Library/Keychains/osx-build.keychain -T /usr/bin/codesign
security import ../travis/osx/dist.p12 -k ~/Library/Keychains/osx-build.keychain -P "$KEY_PASSWORD" -T /usr/bin/codesign
security default-keychain -s osx-build.keychain
security unlock-keychain -p travis osx-build.keychain

# use macdeployqt to deploy the application
#echo "Calling macdeployqt and code signing application"
#$QTDIR/bin/macdeployqt ./$APP.app -codesign="$DEVELOPER_NAME"
echo "Calling macdeployqt"
if ! "$QTDIR/bin/macdeployqt" ./$APP.app; then
  echo "Failed to run macdeployqt"
  # remove keys
  security delete-keychain osx-build.keychain
  exit 1
fi

# trying to fix the macdeployqt problem with making the binary use the Qt
# library files from /usr/local instead of the bundle
# see: https://github.com/Homebrew/homebrew-core/issues/6161
# example: https://github.com/iltommi/neutrino/blob/master/.travis.yml
echo "Cloning macdeployqtfix"
git clone --depth=1 https://github.com/aurelien-rainone/macdeployqtfix.git
echo "Calling macdeployqtfix"
python macdeployqtfix/macdeployqtfix.py $APP.app/Contents/MacOS/$APP /usr/local

echo "Verifying code signed app"
codesign --verify --verbose=4 ./$APP.app
spctl --assess --verbose=4 --raw ./$APP.app

echo "Create $TEMPDIR"
#Create a temporary directory if one doesn't exist
if ! mkdir -p $TEMPDIR; then
  echo "Failed to create temporary folder"
  exit 1
fi

echo "Clean $TEMPDIR"
#Delete the contents of any previous builds
if ! rm -Rf ./$TEMPDIR/*; then
  echo "Failed to clean temporary folder"
  exit 1
fi

echo "Move application bundle"
#Move the application to the temporary directory
if ! mv ./$APP.app ./$TEMPDIR; then
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
if ! ln -s /Applications ./$TEMPDIR/Applications; then
  echo "Failed to create link to /Applications"
  exit 1
fi

echo "Create new disk image"
#Create the disk image
rm -f ./$APP.dmg
if ! hdiutil create -srcfolder ./$TEMPDIR -ov -format UDBZ -fs HFS+ ./$APP.dmg; then
  echo "Failed to create disk image"
  exit 1
fi

#echo "Code signing disk image"
#codesign --force --verify --verbose --sign "$DEVELOPER_NAME" ./$APP.dmg

echo "Verifying code signed disk image"
codesign --verify --verbose=4 ./$APP.dmg
spctl --assess --verbose=4 --raw ./$APP.dmg

echo "moving $APP.dmg to $APP-$VERSION_NUMBER.dmg"
mv $APP.dmg "$APP-$VERSION_NUMBER.dmg"

echo "Removing keys"
# remove keys
security delete-keychain osx-build.keychain

# delete the temporary directory
if ! rm -Rf ./$TEMPDIR/*; then
  echo "Failed to clean temporary folder"
  exit 1
fi

exit 0
