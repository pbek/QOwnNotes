# Als Flatpak installieren

Es gibt ein von der Community gepflegtes Flatpak `org.qownnotes.QOwnNotes` auf [Flathub](https://flathub.org/apps/details/org.qownnotes.QOwnNotes).

Sie können es installieren, indem Sie Folgendes eingeben:

```bash
flatpak remote-add --if-not-exists flathub https://flathub.org/repo/flathub.flatpakrepo
flatpak install flathub org.qownnotes.QOwnNotes
```

Danach sollten Sie QOwnNotes mit dem Befehl `flatpak run org.qownnotes.QOwnNotes` ausführen.

::: tip Info
Bitte eröffnen Sie GitHub-issues mit dem Flatpak-Paket unter [QOwnNotes Flatpak Issues](https://github.com/flathub/org.qownnotes.QOwnNotes/issues).
:::

::: tip
If QOwnNotes logs `Could not write secret to keychain`, install the missing Secret Service packages on the host system and restart your desktop session.

For GNOME and other Secret Service based desktops, install `gnome-keyring`, `libsecret` and `seahorse`.

For KDE Plasma, install KWallet support such as `kwalletmanager`.

QOwnNotes will fall back to legacy encryption if the desktop keychain is unavailable.
:::
