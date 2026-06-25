# Install on KaOS Linux

There is a community maintained package `qownnotes` at the [KaOS Community Packages](https://github.com/KaOS-Community-Packages/qownnotes) repository.

You can install it by typing:

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
