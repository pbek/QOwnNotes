# Installatie op KaOS Linux

Er is een door de gemeenschap onderhouden pakket `qownnotes` in de opslagplaats van [KaOS Community Packages](https://github.com/KaOS-Community-Packages/qownnotes).

U kunt het installeren door te typen:

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
