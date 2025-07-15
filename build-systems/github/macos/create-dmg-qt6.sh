#!/bin/sh

#
# creating the QOwnNotes.dmg with Applications link for GitHub Actions
#

APP=QOwnNotes
# this directory name will also be shown in the title when the DMG is mounted
TEMPDIR=$APP
SIGNATURE="Patrizio Bekerle"
NAME=$(uname)
PLIST=$APP.app/Contents/Info.plist

if [ "$NAME" != "Darwin" ]; then
  echo "This is not a Mac"
  exit 1
fi

#env
#find ${Qt6_DIR}

echo "Changing bundle identifier"
sed -i -e 's/com.yourcompany.QOwnNotes/com.PBE.QOwnNotes/g' $PLIST

# adding version number
/usr/libexec/PlistBuddy -c "Add :CFBundleVersion string $VERSION" $PLIST
/usr/libexec/PlistBuddy -c "Add :CFBundleShortVersionString string $VERSION" $PLIST

# removing CFBundleGetInfoString
/usr/libexec/PlistBuddy -c "Delete :CFBundleGetInfoString" $PLIST

# removing backup plist
rm -f ${PLIST}-e

# copy translation files to app
cp languages/*.qm $APP.app/Contents/Resources
cp ${QT_ROOT_DIR}/translations/qtbase_*.qm $APP.app/Contents/Resources
#cp ${QT_ROOT_DIR}/translations/qt_*.qm $APP.app/Contents/Resources
#rm $APP.app/Contents/Resources/qt_help_*.qm

# copy updater script to app
#chmod a+x ../travis/osx/update.command
#cp ../travis/osx/update.command $APP.app/Contents/MacOS

# use macdeployqt to deploy the application
echo "Calling macdeployqt"
${QT_ROOT_DIR}/bin/macdeployqt -verbose=3 ./$APP.app
# Qt 6.4.2 can't find macdeployqt6
#${Qt6_DIR}/bin/macdeployqt6 ./$APP.app
#macdeployqt ./$APP.app

if [ "$?" -ne "0" ]; then
  echo "Failed to run macdeployqt"
  exit 1
fi

echo "Showing contents of the application bundle"
find ./$APP.app

##
## macdeployqtfix doesn't seem to be needed on Qt 5.15.1
##
# trying to fix the macdeployqt problem with making the binary use the Qt
# library files from /usr/local instead of the bundle
# see: https://github.com/Homebrew/homebrew-core/issues/6161
# example: https://github.com/iltommi/neutrino/blob/master/.travis.yml
#echo "Cloning macdeployqtfix"
#git clone --depth=1 https://github.com/aurelien-rainone/macdeployqtfix.git
#echo "Calling macdeployqtfix"
#python macdeployqtfix/macdeployqtfix.py $APP.app/Contents/MacOS/$APP /usr/local

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
