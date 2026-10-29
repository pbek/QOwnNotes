#!/usr/bin/env bash
#
# Imports the release PGP key inside the release docker container
# If a passphrase file was mounted by build-with-bitwarden.sh, the passphrase
# is preset in gpg-agent, so signing (e.g. debuild for Launchpad) doesn't prompt
#

set -euo pipefail

SigningEmail=patrizio@bekerle.com
KeyFile="$HOME/private.pgp"
PassphraseFile="$HOME/.pgp_passphrase"
GnupgDir="$HOME/.gnupg"

mkdir -p "$GnupgDir"
chmod 700 "$GnupgDir"

echo "Importing PGP key..."

# An empty file (e.g. /dev/null) means no passphrase was provided
if [[ ! -s $PassphraseFile ]]; then
  gpg --import "$KeyFile"
else
  # Allow presetting the passphrase and keep it cached for the whole release
  {
    echo "allow-preset-passphrase"
    echo "default-cache-ttl 86400"
    echo "max-cache-ttl 86400"
  } >>"$GnupgDir/gpg-agent.conf"
  gpgconf --kill gpg-agent

  # Secret keys can be imported in batch mode without the passphrase
  gpg --batch --import "$KeyFile"
fi

if ! gpg --list-secret-keys "$SigningEmail" >/dev/null 2>&1; then
  echo "The imported PGP key does not contain a secret key for $SigningEmail." >&2
  echo "Ensure the Bitwarden attachment 'private.pgp' was exported with 'gpg --export-secret-keys'." >&2
  exit 1
fi

if [[ ! -s $PassphraseFile ]]; then
  exit 0
fi

echo "Presetting PGP passphrase in gpg-agent..."
gpgconf --launch gpg-agent
PresetCommand="$(gpgconf --list-dirs libexecdir)/gpg-preset-passphrase"

# Preset the passphrase for the primary key and all subkeys
mapfile -t Keygrips < <(gpg --batch --with-colons --with-keygrip --list-secret-keys "$SigningEmail" | awk -F: '$1 == "grp" { print $10 }')
for keygrip in "${Keygrips[@]}"; do
  "$PresetCommand" --preset "$keygrip" <"$PassphraseFile"
done

# Verify signing works without a prompt, otherwise fall back to the pinentry prompt later
if echo "test" | gpg --batch --pinentry-mode error --local-user "$SigningEmail" --clearsign >/dev/null 2>&1; then
  echo "PGP passphrase was preset successfully."
else
  echo "Warning: Signing with the preset PGP passphrase failed, you will be prompted for it." >&2
fi
