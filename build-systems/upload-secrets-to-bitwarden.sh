#!/usr/bin/env bash

set -euo pipefail

ProjectDir=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)
SecretsDir="$ProjectDir/secrets"
BitwardenItem="QOwnNotes release secrets"

for command in bw jq; do
  if ! command -v "$command" >/dev/null 2>&1; then
    echo "Required command '$command' was not found." >&2
    exit 1
  fi
done

BitwardenStatus=$(bw status | jq -r '.status')
if [[ $BitwardenStatus == unauthenticated ]]; then
  bw login
  BitwardenStatus=$(bw status | jq -r '.status')
fi

if [[ $BitwardenStatus != unlocked ]]; then
  BW_SESSION=$(bw unlock --raw)
  export BW_SESSION
fi

Files=(
  .oscrc
  .dput.cf
  github_rsa
  github_rsa.pub
  aur_rsa
  aur_rsa.pub
  known_hosts
  known_hosts.old
  private.pgp
  gh/hosts.yml
  gh/config.yml
)

for file in "${Files[@]}"; do
  if [[ ! -f $SecretsDir/$file ]]; then
    echo "Required file '$SecretsDir/$file' was not found." >&2
    exit 1
  fi
done

if ! Items=$(bw list items --search "$BitwardenItem"); then
  echo "Unable to read Bitwarden items. No changes were made by this invocation." >&2
  exit 1
fi

mapfile -t ItemIds < <(jq -r --arg name "$BitwardenItem" '.[] | select(.name == $name) | .id' <<<"$Items")
if [[ ${#ItemIds[@]} -gt 1 ]]; then
  echo "Found multiple Bitwarden items named '$BitwardenItem'. Remove or rename duplicates first." >&2
  exit 1
fi

if [[ ${#ItemIds[@]} -eq 0 ]]; then
  echo "Creating Bitwarden Secure Note '$BitwardenItem'..."
  if ! ItemId=$(
    bw get template item |
      jq --arg name "$BitwardenItem" \
        '.type = 2
                 | .name = $name
                 | .secureNote = {type: 0}
                 | .notes = "Release credentials used by build-systems/build-with-bitwarden.sh"' |
      bw encode |
      bw create item |
      jq -r '.id'
  ); then
    echo "Unable to create the Bitwarden Secure Note." >&2
    exit 1
  fi
else
  ItemId=${ItemIds[0]}
  echo "Using existing Bitwarden Secure Note '$BitwardenItem'."
fi

Item=$(bw get item "$ItemId")
for file in "${Files[@]}"; do
  filename=$(basename "$file")
  attachment_count=$(jq --arg filename "$filename" '[.attachments[]? | select(.fileName == $filename)] | length' <<<"$Item")
  if [[ $attachment_count -ne 0 ]]; then
    echo "Attachment '$filename' already exists; refusing to overwrite it." >&2
    exit 1
  fi
done

echo "Uploading release files..."
for file in "${Files[@]}"; do
  echo "  $(basename "$file")"
  if ! bw create attachment --file "$SecretsDir/$file" --itemid "$ItemId" >/dev/null; then
    echo "Failed to upload '$(basename "$file")'. Previously uploaded attachments were retained." >&2
    exit 1
  fi
done

bw sync >/dev/null
echo "Uploaded ${#Files[@]} attachments to '$BitwardenItem'."
