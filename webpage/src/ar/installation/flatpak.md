# التثبيت كحزمة Flatpak

يتعهد المجتمع حزمة Flatpak تسمى `org.qownnotes.QOwnNotes` على [Flathub](https://flathub.org/apps/details/org.qownnotes.QOwnNotes).

يمكنك تثبيتها بكتابة:

```bash
flatpak remote-add --if-not-exists flathub https://flathub.org/repo/flathub.flatpakrepo
flatpak install flathub org.qownnotes.QOwnNotes
```

بعدئذٍ سيمكنك تشغيل QOwnNotes بالأمر `flatpak run org.qownnotes.QOwnNotes`.

::: tip
معلومة نرجو الإبلاغ عن علل حزمة Flatpak في صفحة [مسائل QOwnNotes Flatpak](https://github.com/flathub/org.qownnotes.QOwnNotes/issues).
:::

::: tip
If QOwnNotes logs `Could not write secret to keychain`, install the missing Secret Service packages on the host system and restart your desktop session.

For GNOME and other Secret Service based desktops, install `gnome-keyring`, `libsecret` and `seahorse`.

For KDE Plasma, install KWallet support such as `kwalletmanager`.

QOwnNotes will fall back to legacy encryption if the desktop keychain is unavailable.
:::
