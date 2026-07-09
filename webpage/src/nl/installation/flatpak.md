# Installatie als Flatpak

Er is een community die wordt onderhouden met Flatpak `org.qownnotes.QOwnNotes` op [Flathub](https://flathub.org/apps/details/org.qownnotes.QOwnNotes).

U kunt het installeren door te typen:

```bash
flatpak remote-add --if-not-exists flathub https://flathub.org/repo/flathub.flatpakrepo
flatpak install flathub org.qownnotes.QOwnNotes
```

Daarna zou je QOwnNotes moeten kunnen draaien met het commando `flatpak run org.qownnotes.QOwnNotes`.

::: tip Info
Meld problemen met het Flatpak-pakket op [ QOwnNotes Flatpak-problemen ](https://github.com/flathub/org.qownnotes.QOwnNotes/issues).
:::

::: tip
If QOwnNotes logs `Could not write secret to keychain`, install the missing Secret Service packages on the host system and restart your desktop session.

For GNOME and other Secret Service based desktops, install `gnome-keyring`, `libsecret` and `seahorse`.

For KDE Plasma, install KWallet support such as `kwalletmanager`.

QOwnNotes will fall back to legacy encryption if the desktop keychain is unavailable.
:::
