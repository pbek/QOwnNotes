# Installa come Flatpak

C'è un Flatpak `org.qownnotes.QOwnNotes` manutenuto dalla comunità su [Flathub](https://flathub.org/apps/details/org.qownnotes.QOwnNotes).

Puoi installarlo digitando:

```bash
flatpak remote-add --if-not-exists flathub https://flathub.org/repo/flathub.flatpakrepo
flatpak install flathub org.qownnotes.QOwnNotes
```

Successivamente dovresti essere in grado di eseguire QOwnNotes con il comando `flatpak run org.qownnotes.QOwnNotes`.

::: tip Info
Si prega di riferire le problematiche con il pacchetto di Flatpak su [QOwnNotes Flatpak Issues](https://github.com/flathub/org.qownnotes.QOwnNotes/issues).
:::

::: tip
If QOwnNotes logs `Could not write secret to keychain`, install the missing Secret Service packages on the host system and restart your desktop session.

For GNOME and other Secret Service based desktops, install `gnome-keyring`, `libsecret` and `seahorse`.

For KDE Plasma, install KWallet support such as `kwalletmanager`.

QOwnNotes will fall back to legacy encryption if the desktop keychain is unavailable.
:::
