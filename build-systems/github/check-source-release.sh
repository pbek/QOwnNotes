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
# Allow CI time to publish the release before packaging or uploading assets.
Timeout=300
Interval=5
StartTime=$(date +%s)
while true; do
  if ! IsDraft=$(gh release view "$Tag" --repo pbek/QOwnNotes --json isDraft --jq '.isDraft'); then
    Reason="cannot verify $Tag"
  elif [[ $IsDraft != false ]]; then
    Reason="$Tag is not a published release"
  else
    break
  fi

  if (($(date +%s) - StartTime >= Timeout)); then
    echo "Release safeguard: $Reason after 5 minutes. Wait for CI to publish the release before deploying." >&2
    exit 1
  fi
  echo "Release safeguard: $Reason. Waiting for CI, retrying in $Interval seconds..." >&2
  sleep "$Interval"
done

git fetch --depth=1 origin "refs/tags/$Tag"
ReleaseCommit=$(git rev-parse 'FETCH_HEAD^{commit}')
SourceCommit=$(git rev-parse HEAD)
if [[ $ReleaseCommit != "$SourceCommit" ]]; then
  echo "Release safeguard: $Tag points to $ReleaseCommit, but the release branch is at $SourceCommit. Refusing to package a different commit." >&2
  exit 1
fi

echo "Release safeguard passed: published $Tag matches source commit $SourceCommit."
