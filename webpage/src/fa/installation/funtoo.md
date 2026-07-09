# نصب در فانتو لینوکس

فانتو لینوکس حاوی یک بسته QOwnNotes در [desktop-kit](https://github.com/funtoo/desktop-kit/tree/1.4-release/app-office/qownnotes)است که به صورت خودکار ایجاد شده و قابل نصب در مسیر زیر می باشد:

```bash
sudo emerge -av qownnotes
```

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
