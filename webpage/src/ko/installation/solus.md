# Solus에 설치

[Solus Project](https://getsol.us/)에서 유지 관리하는 `qownnotes` 패키지가 있습니다.

다음을 입력하여 설치할 수 있습니다:

```bash
sudo eopkg install qownnotes
```

::: tip
If QOwnNotes logs `Could not write secret to keychain`, install the missing Secret Service packages and restart your desktop session.

For GNOME and other Secret Service based desktops:

```bash
sudo eopkg install gnome-keyring libsecret seahorse
```

For KDE Plasma, install KWallet support from your configured repositories.

QOwnNotes will fall back to legacy encryption if the desktop keychain is unavailable.
:::
