# Telepítés a Gentoo Linux rendszerre

Adja hozzá a QOwnNotes adattárat a következő paranccsal (lásd: [eselect-repository](https://wiki.gentoo.org/wiki/Eselect/Repository)).

```bash
sudo eselect repository enable qownnotes-overlay
```

Ezután telepítheti a QOwnNotes programot a szokásos módon.

```bash
sudo emerge --sync qownnotes-overlay && sudo emerge -av qownnotes
```

[QOwnNotes overlay a GitHubon](https://github.com/qownnotes/gentoo-overlay/)

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
