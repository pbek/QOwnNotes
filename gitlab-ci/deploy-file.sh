#!/usr/bin/env bash

#
# deploy QOwnNotes.dmg to GitHub Releases
#
curl -H "Authorization: token $GITHUB_ACCESS_TOKEN" -H "Accept: application/vnd.github.manifold-preview" -H "Content-Type: application/x-apple-diskimage" --data-binary @QOwnNotes.dmg "https://uploads.github.com/repos/pbek/testrepo/releases/$RELEASE_ID/assets?name=QOwnNotes.dmg"
