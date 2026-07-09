# Telepítse Void Linuxra

Van egy közösség által létrehozott csomag a **Void Linux** számára a [qownnotes srcpkg](https://github.com/void-linux/void-packages/tree/master/srcpkgs/qownnotes)-ban, amely a következő módon telepíthető:

```bash
Van egy közösség által létrehozott csomag a Void Linux számára a qownnotes srcpkg-ban, amely a következő módon telepíthető:
```

::: tip
If QOwnNotes logs `Could not write secret to keychain`, install the missing Secret Service packages and restart your desktop session.

For GNOME and other Secret Service based desktops:

```bash
sudo xbps-install -S gnome-keyring libsecret seahorse
```

For KDE Plasma:

```bash
sudo xbps-install -S kwalletmanager
```

QOwnNotes will fall back to legacy encryption if the desktop keychain is unavailable.
:::
