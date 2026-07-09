# Telepítse Solus-ra

Van egy `qownnotes` csomag, amelyet a [Solus Project](https://getsol.us/) karbantart.

Telepítéséhez írja be:

```bash
sudo eopkg install qownnotes
```

::: tip
If QOwnNotes logs `Could not write secret to keychain`, install the missing Secret Service packages and restart your desktop session.

For GNOME and other Secret Service based desktops:

```bash
sudo eopkg install gnome-keyring libsecret seahorse
```

For KDE Plasma, install KWallet support from your configured repositories.

QOwnNotes will fall back to legacy encryption if the desktop keychain is unavailable.
:::
