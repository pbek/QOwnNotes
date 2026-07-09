# التثبيت كحزمة Snap

توجد حزمة snap اسمها `QOwnNotes` في [Snap store](https://snapcraft.io/qownnotes).

يمكنك تثبيتها بكتابة:

```bash
snap install qownnotes
```

بعدئذٍ يمكنك تشغيل QOwnNotes بالأمر `qownnotes`.

::: tip
[Snaps](http://snapcraft.io) are working on many Linux distributions like Ubuntu, Arch Linux, Debian, Fedora, openSUSE, Gentoo Linux, OpenWRT, open embedded and yocto project.
:::

::: tip
If QOwnNotes logs `Could not write secret to keychain`, install the missing Secret Service packages on the host system and restart your desktop session.

For GNOME and other Secret Service based desktops, install `gnome-keyring`, `libsecret` and `seahorse`.

For KDE Plasma, install KWallet support such as `kwalletmanager`.

QOwnNotes will fall back to legacy encryption if the desktop keychain is unavailable.
:::
