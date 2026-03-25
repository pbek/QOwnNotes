#!/bin/sh
#
# creating the QOwnNotes.dmg with Applications link for GitHub Actions
#

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
/usr/libexec/PlistBuddy -c "Add :CFBundleVersion string $VERSION" $PLIST
/usr/libexec/PlistBuddy -c "Add :CFBundleShortVersionString string $VERSION" $PLIST

# removing CFBundleGetInfoString
/usr/libexec/PlistBuddy -c "Delete :CFBundleGetInfoString" $PLIST

# removing backup plist
rm -f ${PLIST}-e

# copy translation files to app
cp languages/*.qm $APP.app/Contents/Resources
cp "${QT_ROOT_DIR}"/translations/qtbase_*.qm $APP.app/Contents/Resources
#cp ${QT_ROOT_DIR}/translations/qt_*.qm $APP.app/Contents/Resources
#rm $APP.app/Contents/Resources/qt_help_*.qm

# copy updater script to app
#chmod a+x ../travis/osx/update.command
#cp ../travis/osx/update.command $APP.app/Contents/MacOS

# use macdeployqt to deploy the application
echo "Calling macdeployqt"
if ! "${QT_ROOT_DIR}/bin/macdeployqt" ./$APP.app; then
  echo "Failed to run macdeployqt"
  exit 1
fi

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

echo "Create symbolic link"
#Create a symbolic link to the applications folder
if ! ln -s /Applications ./$TEMPDIR/Applications; then
  echo "Failed to create link to /Applications"
  exit 1
fi

# Prevent Spotlight from indexing the source folder, which can cause
# "Resource busy" errors during hdiutil create
mdutil -i off ./$TEMPDIR 2>/dev/null || true

echo "Create new disk image"
#Create the disk image
rm -f ./$APP.dmg

# Retry hdiutil create up to 5 times to work around intermittent
# "Resource busy" errors on macOS CI runners (caused by Spotlight
# indexing or filesystem flush delays after macdeployqt)
MAX_RETRIES=5
RETRY_COUNT=0
while [ $RETRY_COUNT -lt $MAX_RETRIES ]; do
  if hdiutil create -srcfolder ./$TEMPDIR -ov -format UDBZ -fs HFS+ ./$APP.dmg; then
    break
  fi
  RETRY_COUNT=$((RETRY_COUNT + 1))
  if [ $RETRY_COUNT -ge $MAX_RETRIES ]; then
    echo "Failed to create disk image after $MAX_RETRIES attempts"
    exit 1
  fi
  echo "hdiutil create failed (attempt $RETRY_COUNT/$MAX_RETRIES), retrying in 5 seconds..."
  sleep 5
done

# delete the temporary directory
if ! rm -Rf ./$TEMPDIR/*; then
  echo "Failed to clean temporary folder"
  exit 1
fi

exit 0
