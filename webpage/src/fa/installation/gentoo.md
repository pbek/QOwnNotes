# نصب در جنتو لینوکس

مخزن QOwnNotes را با فرمان زیر اضافه کنید ( [eselect-repository](https://wiki.gentoo.org/wiki/Eselect/Repository) را ملاحظه کنید).

```bash
sudo eselect repository enable qownnotes-overlay
```

سپس طبق معمول می توانید QOwnNotes را نصب کنید.

```bash
sudo emerge --sync qownnotes-overlay && sudo emerge -av qownnotes
```

[QOwnNotes با گیت هاب همپوشانی می کند](https://github.com/qownnotes/gentoo-overlay/)

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
