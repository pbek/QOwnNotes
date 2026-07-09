# Void Linux에 설치

다음과 같은 방법으로 설치할 수 있는 [qownnotes srcpkg](https://github.com/void-linux/void-packages/tree/master/srcpkgs/qownnotes)의 **Void Linux**용 커뮤니티 생성 패키지가 있습니다:

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
