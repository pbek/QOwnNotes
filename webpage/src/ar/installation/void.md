# التثبيت على Void Linux

توجد حزمة أنشأها المجتمع لتوزيعة **Void Linux** في [qownnotes srcpkg](https://github.com/void-linux/void-packages/tree/master/srcpkgs/qownnotes)، والتي يمكن تثبيتها كالتالي:

```bash
# زامن ملفات دليل المستودع البعيد وثبّت التطبيق
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
