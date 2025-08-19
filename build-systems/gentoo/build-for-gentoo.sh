#!/usr/bin/env bash
#
# This is the build/deploy script for the Gentoo QOwnNotes overlay
# https://github.com/qownnotes/gentoo-overlay
#
# The QOwnNotes repository for Gentoo relies on our source archive on
# https://github.com/pbek/QOwnNotes/releases
#

# uncomment this if you want to force a version
#QOWNNOTES_VERSION=1.1.0.4

BRANCH=main
#BRANCH=master

PROJECT_PATH="/tmp/QOwnNotes-gentoo-$$"

# use temporary checksum variable file
_QQwnNotesCheckSumVarFile="/tmp/QOwnNotes.checksum.vars"

if [[ ! -f ${_QQwnNotesCheckSumVarFile} ]]; then
  echo "${_QQwnNotesCheckSumVarFile} doesn't exist. build-github-src.sh must be run ahead of build script!"
  exit 1
fi

source ${_QQwnNotesCheckSumVarFile}

# check checksum variable from build-systems/github/build-github-src.sh
if [ -z ${QOWNNOTES_ARCHIVE_SHA512} ]; then
  echo "QOWNNOTES_ARCHIVE_SHA512 was not set!"
  exit 1
fi

if [ -z ${QOWNNOTES_ARCHIVE_SIZE} ]; then
  echo "QOWNNOTES_ARCHIVE_SIZE was not set!"
  exit 1
fi

list_old_ebuilds_to_delete() {
  all_files=($(find . -maxdepth 1 -type f -name 'qownnotes-*.ebuild' -printf '%f\n'))
  majors=$(printf '%s\n' "${all_files[@]}" | sed -E 's/^qownnotes-([0-9]+)\..*/\1/' | sort -u)
  latest_major=$(printf '%s\n' $majors | sort -n | tail -n 1)
  to_delete=()

  for major in ${majors}; do
    files=($(printf '%s\n' "${all_files[@]}" | grep "^qownnotes-${major}\." | sort -V))

    if [[ $major -eq $latest_major ]]; then
      # Keep the last 3 for latest major
      if (( ${#files[@]} > 3 )); then
        to_delete+=("${files[@]:0:${#files[@]}-3}")
      fi
    else
      # Keep only the last one for older majors
      if (( ${#files[@]} > 1 )); then
        to_delete+=("${files[@]:0:${#files[@]}-1}")
      fi
    fi
  done

  if [[ ${#to_delete[@]} -gt 0 ]]; then
    printf '%s ' "${to_delete[@]}"
  fi
}

echo "Started the ebuild packaging process, using latest '$BRANCH' git tree"

if [ -d $PROJECT_PATH ]; then
  rm -rf $PROJECT_PATH
fi

mkdir $PROJECT_PATH
cd $PROJECT_PATH || exit 1

echo "Project path: $PROJECT_PATH"

# checkout AUR repository
git clone --depth=1 git@github.com:qownnotes/gentoo-overlay.git overlay

# checkout the source code
git clone --depth=1 git@github.com:pbek/QOwnNotes.git QOwnNotes -b $BRANCH
cd QOwnNotes || exit 1

if [ -z $QOWNNOTES_VERSION ]; then
  # get version from version.h
  QOWNNOTES_VERSION=$(cat src/version.h | sed "s/[^0-9,.]//g")
fi

ARCHIVE_FILE=qownnotes-${QOWNNOTES_VERSION}.tar.xz

cd ../overlay/app-office/qownnotes/ || exit 1
cp ../../../QOwnNotes/build-systems/gentoo/qownnotes.ebuild .

# create metadata file
echo '<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE pkgmetadata SYSTEM "https://www.gentoo.org/dtd/metadata.dtd">
<pkgmetadata>
        <upstream>
                <remote-id type="github">pbek/QOwnNotes</remote-id>
        </upstream>
</pkgmetadata>' >metadata.xml

ebuilds_to_delete="$(list_old_ebuilds_to_delete)"

# Remove to-be-deleted versions from Manifest
read -r -a ebuilds_to_delete_array <<< ${ebuilds_to_delete}
patterns=$(for f in "${ebuilds_to_delete_array[@]}"; do
  base=${f%.ebuild}
  echo "DIST ${base}.tar.xz"
done)
grep -vF -f <(printf '%s\n' "$patterns") Manifest > Manifest.new && mv Manifest.new Manifest

# update the Manifest file
echo "DIST ${ARCHIVE_FILE} ${QOWNNOTES_ARCHIVE_SIZE} SHA512 ${QOWNNOTES_ARCHIVE_SHA512}" >>Manifest

eBuildFile="qownnotes-$QOWNNOTES_VERSION.ebuild"
mv qownnotes.ebuild ${eBuildFile}

echo "Committing changes..."
[ -n "${ebuilds_to_delete}" ] && git rm ${ebuilds_to_delete}
git add ${eBuildFile}
git commit -m "releasing version $QOWNNOTES_VERSION" ${eBuildFile} Manifest metadata.xml ${ebuilds_to_delete}
git push

# remove everything after we are done
if [ -d $PROJECT_PATH ]; then
  rm -rf $PROJECT_PATH
fi
