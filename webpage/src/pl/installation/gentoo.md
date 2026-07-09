# Zainstaluj na Gentoo Linux

Dodaj repozytorium QOwnNotes za pomocą następującego polecenia (zobacz [eselect-repository](https://wiki.gentoo.org/wiki/Eselect/Repository)).

```bash
sudo eselect repository enable qownnotes-overlay
```

Następnie możesz zainstalować QOwnNotes w zwykły sposób.

```bash
sudo emerge --sync qownnotes-overlay && sudo emerge -av qownnotes
```

[Nakładka QOwnNotes na GitHub](https://github.com/qownnotes/gentoo-overlay/)

::: tip
If QOwnNotes logs `Could not write secret to keychain`, install the missing Secret Service packages and restart your desktop session.

For GNOME and other Secret Service based desktops:

```bash
sudo emerge -av gnome-extra/gnome-keyring app-crypt/libsecret app-crypt/seahorse
```

For KDE Plasma:

```bash
sudo emerge -av kde-apps/kwalletmanager kde-frameworks/kwallet
```

QOwnNotes will fall back to legacy encryption if the desktop keychain is unavailable.
:::
