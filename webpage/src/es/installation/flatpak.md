# Instalar como Flatpak

Hay un Flatpak `org.qownnotes.QOwnNotes` mantenido por la comunidad en [Flathub](https://flathub.org/apps/details/org.qownnotes.QOwnNotes).

Puede instalarlo escribiendo:

```bash
flatpak remote-add --if-not-exists flathub https://flathub.org/repo/flathub.flatpakrepo
flatpak install flathub org.qownnotes.QOwnNotes
```

Posteriormente, debería poder ejecutar QOwnNotes con el comando `flatpak run org.qownnotes.QOwnNotes`.

::: tip Info
Por favor abra incidencias con el paquete de Flatpak en la página de [incidencias del Flatpak de QOwnNotes](https://github.com/flathub/org.qownnotes.QOwnNotes/issues).
:::

::: tip
If QOwnNotes logs `Could not write secret to keychain`, install the missing Secret Service packages on the host system and restart your desktop session.

For GNOME and other Secret Service based desktops, install `gnome-keyring`, `libsecret` and `seahorse`.

For KDE Plasma, install KWallet support such as `kwalletmanager`.

QOwnNotes will fall back to legacy encryption if the desktop keychain is unavailable.
:::
