# Installieren unter Slackware Linux

## Slackware 15.0

First install [Qt 6 from SlackBuilds.org](https://slackbuilds.org/repository/15.0/libraries/qt6/), including its required dependencies.

Then check out the QOwnNotes SlackBuilds repository and build the application.

```bash
cd /tmp
git clone https://github.com/pbek/qownnotes-slackbuilds.git
cd qownnotes-slackbuilds/15.0/qownnotes
./dobuild.sh
```

Danach können Sie das generierte Paket mit `installpkg` installieren.

[QOwnNotes Slackbuild auf GitHub](https://github.com/pbek/qownnotes-slackbuilds/)

::: tip
If QOwnNotes logs `Could not write secret to keychain`, install a Secret Service implementation and restart your desktop session.

For GNOME and other Secret Service based desktops, install `gnome-keyring`, `libsecret` and `seahorse` from your Slackware repositories or SlackBuilds.

For KDE Plasma, install KWallet support such as `kwalletmanager`.

QOwnNotes will fall back to legacy encryption if the desktop keychain is unavailable.
:::
