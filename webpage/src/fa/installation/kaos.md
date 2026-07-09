# نصب در لینوکس KaOS

یک بسته نگهداری شده جمعی برای `qownnotes` در مخزن [بسته های انجمن KaOS](https://github.com/KaOS-Community-Packages/qownnotes) موجود است.

می توانید آن را با تایپ عبارت زیر نصب کنید:

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
