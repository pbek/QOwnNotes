# نصب در ووید لینوکس

یک بسته ساخت جمعی برای **ووید لینوکس** در [qownnotes srcpkg](https://github.com/void-linux/void-packages/tree/master/srcpkgs/qownnotes) در دسترس قرار دارد که به روش زیر قابل نصب می باشد:

```bash
# Synchronize remote repository index files and install QOwnNotes
xbps-install -S qownnotes
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
