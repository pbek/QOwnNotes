# KaOS Linux에 설치

[KaOS Community Packages](https://github.com/KaOS-Community-Packages/qownnotes) 저장소에는 커뮤니티 유지보수 패키지 `qownnotes`가 있습니다.

다음을 입력하여 설치할 수 있습니다:

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
