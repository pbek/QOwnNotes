# Zainstaluj jako Flatpak

Istnieje społeczność Flatpak `org.qownnotes.QOwnNotes` na [Flathub](https://flathub.org/apps/details/org.qownnotes.QOwnNotes).

Możesz go zainstalować, wpisując:

```bash
flatpak remote-add --if-not-exists flathub https://flathub.org/repo/flathub.flatpakrepo
flatpak install flathub org.qownnotes.QOwnNotes
```

Afterward you should be able to run QOwnNotes with the command `flatpak run org.qownnotes.QOwnNotes`.

::: tip Info
Please open issues with the Flatpak package on [QOwnNotes Flatpak Issues](https://github.com/flathub/org.qownnotes.QOwnNotes/issues).
:::

::: tip
If QOwnNotes logs `Could not write secret to keychain`, install the missing Secret Service packages on the host system and restart your desktop session.

For GNOME and other Secret Service based desktops, install `gnome-keyring`, `libsecret` and `seahorse`.

For KDE Plasma, install KWallet support such as `kwalletmanager`.

QOwnNotes will fall back to legacy encryption if the desktop keychain is unavailable.
:::
