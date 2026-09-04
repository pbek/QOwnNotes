#!/usr/bin/env bash

set -euo pipefail

ProjectDir=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)
BuildSystemsDir="$ProjectDir/build-systems"
BitwardenItem="QOwnNotes release secrets"
BitwardenItemId="d4e20a19-7e22-49a4-a6aa-fa169ff68d19"

for command in bw docker jq; do
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

Item=$(bw get item "$BitwardenItemId")
SecretsDir=$(mktemp -d "${TMPDIR:-/tmp}/qownnotes-secrets.XXXXXXXX")
trap 'rm -rf "$SecretsDir"' EXIT
mkdir -m 700 "$SecretsDir/gh"

Attachments=(
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

echo "Downloading release files from Bitwarden..."
MissingAttachments=()
for attachment in "${Attachments[@]}"; do
  filename=$(basename "$attachment")
  attachment_count=$(jq --arg filename "$filename" '[.attachments[]? | select(.fileName == $filename)] | length' <<<"$Item")
  if [[ $attachment_count -ne 1 ]]; then
    MissingAttachments+=("$filename (found $attachment_count)")
  fi
done

if [[ ${#MissingAttachments[@]} -ne 0 ]]; then
  printf "Required attachments missing from '%s' (%s):\n" "$BitwardenItem" "$BitwardenItemId" >&2
  printf "  %s\n" "${MissingAttachments[@]}" >&2
  printf "Available attachments:\n" >&2
  jq -r '.attachments[]?.fileName | "  " + .' <<<"$Item" >&2
  exit 1
fi

for attachment in "${Attachments[@]}"; do
  filename=$(basename "$attachment")
  output="$SecretsDir/$attachment"
  mkdir -p "$(dirname "$output")"
  bw get attachment "$filename" --itemid "$BitwardenItemId" --output "$output" >/dev/null
done

chmod 600 "$SecretsDir/github_rsa" "$SecretsDir/aur_rsa" "$SecretsDir/private.pgp"

echo "Running docker-compose build process..."
QON_SECRETS_DIR="$SecretsDir" docker compose --project-directory "$BuildSystemsDir" run --rm releaser /QOwnNotes/build-systems/build-all.sh --docker
