#!/usr/bin/env bash
################################################
#
# QOwnNotes macOS updater script
#
################################################

#diskImageUrl="https://github.com/pbek/QOwnNotes/releases/download/macosx-b2381-103543/QOwnNotes-16.10.10.dmg"
#diskImageUrl="https://github.com/pbek/QOwnNotes/releases/download/macosx-b2376-114124/QOwnNotes-16.10.9.dmg"
#diskImageUrl="$1"
diskImageUrl="$QOWNNOTES_RELEASE_URL"

#applicationsFolder="/Applications"
#applicationsFolder="$2"
applicationsFolder="$QOWNNOTES_APPLICATIONS_PATH"

# https://bash.cyberciti.biz/guide/$IFS
IFS="
"

die () {
    echo >&2 "$@"
    read -p "Press [Enter] key to end..."
    exit 1
}

echo "QOwnNotes macOS updater script"
echo

# check if we have 2 parameters
#[ "$#" -eq 2 ] || die "2 arguments required, $# provided"

if [ -z "$diskImageUrl" ]; then
    die "diskImageUrl is empty"
fi

if [ -z "$applicationsFolder" ]; then
    die "applicationsFolder is empty"
fi

tempDir=`mktemp -d` && cd $tempDir
mountPoint=`mktemp -d`
diskImage="QOwnNotes.dmg"

echo "Created temporary directory $tempDir"
echo "Downloading $diskImageUrl..."
echo

# downloading dmg
curl -L -o $diskImage $diskImageUrl

# mounting dmg
hdiutil attach -mountpoint $mountPoint $diskImage

# copy app to application folder
for app in `find $mountPoint -type d -maxdepth 2 -name \*.app `; do
  echo Syncing "$app" to applications folder
  rsync -av --progress "$app" "$applicationsFolder"
done

# unmounting dmg
hdiutil detach $mountPoint

echo "Removing temporary files..."
rm $tempDir/$diskImage
rmdir $tempDir
rmdir $mountPoint

echo "Starting QOwnNotes..."
open "$applicationsFolder/QOwnNotes.app"
