# 스냅으로 설치

[Snap 저장소](https://snapcraft.io/qownnotes)에 snap `qownnotes`가 있습니다.

다음을 입력하여 설치할 수 있습니다:

```bash
snap install qownnotes
```

그런 다음 `qownnotes` 명령을 사용하여 QOnNotes를 실행할 수 있습니다.

::: tip
[스냅](http://snapcraft.io)은 Ubuntu, Arch Linux, Debian, Fedora, openSUSE, Gentoo Linux, OpenWRT, 오픈 임베디드 및 yocto 프로젝트와 같은 많은 리눅스 배포판에서 작업하고 있습니다.
:::

::: tip
If QOwnNotes logs `Could not write secret to keychain`, install the missing Secret Service packages on the host system and restart your desktop session.

For GNOME and other Secret Service based desktops, install `gnome-keyring`, `libsecret` and `seahorse`.

For KDE Plasma, install KWallet support such as `kwalletmanager`.

QOwnNotes will fall back to legacy encryption if the desktop keychain is unavailable.
:::

::: tip
In a Wayland session the snap runs natively on Wayland, otherwise it uses X11. If you run into Wayland related issues, you can force X11 (XWayland) by starting QOwnNotes with `QOWNNOTES_SNAP_FORCE_X11=1 qownnotes`.
:::

::: tip
The Qt6 snap uses the host desktop's file chooser through `xdg-desktop-portal`. Install `xdg-desktop-portal` and your desktop's portal backend (for example, `xdg-desktop-portal-cosmic`, `xdg-desktop-portal-gnome`, or `xdg-desktop-portal-kde`) on the host to use its native file chooser. Without a working portal, Qt falls back to an in-snap file chooser.

The home directory shown in file dialogs is your real home directory. Settings, databases, and caches remain in the snap's private storage; existing note folders are not moved. Snap confinement still limits which folders can be accessed.
:::
