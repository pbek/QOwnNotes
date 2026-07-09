# Instalar como Snap

Hay un complemento `qownnotes` en la [tienda Snap](https://snapcraft.io/qownnotes).

Puede instalarlo escribiendo:

```bash
snap install qownnotes
```

Posteriormente, será capaz de ejecutar QOwnNotes mediante el comando `qownnotes`.

::: tip
Los [snaps](http://snapcraft.io) funcionan en muchas distribuciones de Linux, como Ubuntu, Arch Linux, Debian, Fedora, openSUSE, Gentoo Linux, OpenWRT, OpenEmbedded y Yocto Project.
:::

::: tip
If QOwnNotes logs `Could not write secret to keychain`, install the missing Secret Service packages on the host system and restart your desktop session.

For GNOME and other Secret Service based desktops, install `gnome-keyring`, `libsecret` and `seahorse`.

For KDE Plasma, install KWallet support such as `kwalletmanager`.

QOwnNotes will fall back to legacy encryption if the desktop keychain is unavailable.
:::
