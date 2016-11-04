#!/usr/bin/env bash
################################################
#
# QOwnNotes macOS updater script
#
################################################

#diskImageUrl="https://github.com/pbek/QOwnNotes/releases/download/macosx-b2381-103543/QOwnNotes-16.10.10.dmg"
#diskImageUrl="https://github.com/pbek/QOwnNotes/releases/download/macosx-b2376-114124/QOwnNotes-16.10.9.dmg"
diskImageUrl="$1"

#applicationsFolder="/Applications"
applicationsFolder="$2"

# https://bash.cyberciti.biz/guide/$IFS
IFS="
"
die () {
    echo >&2 "$@"
    exit 1
}

echo "QOwnNotes macOS updater script"
echo

# check if we have 2 parameters
[ "$#" -eq 2 ] || die "2 arguments required, $# provided"

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
