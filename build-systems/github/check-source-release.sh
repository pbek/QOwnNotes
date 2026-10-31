#!/usr/bin/env bash

# Run inside the cloned release checkout before packaging or uploading assets.
set -euo pipefail

ExpectedVersion=${1:?Expected local version is required}
RemoteVersion=$(sed 's/[^0-9,.]//g' src/version.h)
if [[ -z $ExpectedVersion || $RemoteVersion != "$ExpectedVersion" ]]; then
  echo "Release safeguard: local version $ExpectedVersion does not match remote release version $RemoteVersion. Push the intended commit to release first." >&2
  exit 1
fi

Tag="v$ExpectedVersion"
if ! IsDraft=$(gh release view "$Tag" --repo pbek/QOwnNotes --json isDraft --jq '.isDraft'); then
  echo "Release safeguard: cannot verify $Tag. Wait for CI to publish the release before deploying." >&2
  exit 1
fi
if [[ $IsDraft != false ]]; then
  echo "Release safeguard: $Tag is not a published release. Wait for CI before deploying." >&2
  exit 1
fi

git fetch --depth=1 origin "refs/tags/$Tag"
ReleaseCommit=$(git rev-parse 'FETCH_HEAD^{commit}')
SourceCommit=$(git rev-parse HEAD)
if [[ $ReleaseCommit != "$SourceCommit" ]]; then
  echo "Release safeguard: $Tag points to $ReleaseCommit, but the release branch is at $SourceCommit. Refusing to package a different commit." >&2
  exit 1
fi

echo "Release safeguard passed: published $Tag matches source commit $SourceCommit."
