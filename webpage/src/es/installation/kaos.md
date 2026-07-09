# Instalar en KaOS Linux

Hay un paquete mantenido por la comunidad `qownnotes` en el repositorio [Paquetes de la comunidad KaOS](https://github.com/KaOS-Community-Packages/qownnotes).

Puede instalarlo escribiendo:

```bash
kcp -i qownnotes
```

::: tip
If QOwnNotes logs `Could not write secret to keychain`, install the missing Secret Service packages and restart your desktop session.

For KDE Plasma:

```bash
sudo pacman -S kwalletmanager kwallet
```

For GNOME and other Secret Service based desktops, install `gnome-keyring`, `libsecret` and `seahorse` from your configured repositories.

QOwnNotes will fall back to legacy encryption if the desktop keychain is unavailable.
:::
