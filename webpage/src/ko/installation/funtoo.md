# Funtoo Linux에 설치

Funtoo Linux에는 [데스크톱 키트](https://github.com/funtoo/desktop-kit/tree/1.4-release/app-office/qownnotes)로 자동 생성된 QOwnNotes 패키지가 있으며 다음과 같은 방법으로 설치할 수 있습니다:

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
