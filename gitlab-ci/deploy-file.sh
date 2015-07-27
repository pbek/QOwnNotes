#!/usr/bin/env bash

#
# deploy QOwnNotes.dmg to GitHub Releases
# execute: ./deploy-file.sh $RELEASE_ID $GITHUB_ACCESS_TOKEN
#

curl -H "Authorization: token $2" -H "Accept: application/vnd.github.manifold-preview" -H "Content-Type: application/x-apple-diskimage" --data-binary @QOwnNotes.dmg "https://uploads.github.com/repos/pbek/QOwnNotes/releases/$1/assets?name=QOwnNotes.dmg"
