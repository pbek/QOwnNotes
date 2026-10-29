# Install as Snap

There is a snap `qownnotes` in the [Snap store](https://snapcraft.io/qownnotes).
The current AMD64 snap uses Qt 6.

You can install it by typing:

```bash
snap install qownnotes
```

Afterward you should be able to run QOwnNotes with the command `qownnotes`.

::: tip
[Snaps](http://snapcraft.io) are working on many Linux distributions like Ubuntu, Arch Linux, Debian, Fedora,
openSUSE, Gentoo Linux, OpenWRT, open embedded and yocto project.
:::

::: tip
If QOwnNotes logs `Could not write secret to keychain`, install the missing Secret Service packages on the host system and restart your desktop session.

For GNOME and other Secret Service based desktops, install `gnome-keyring`, `libsecret` and `seahorse`.

For KDE Plasma, install KWallet support such as `kwalletmanager`.

QOwnNotes will fall back to legacy encryption if the desktop keychain is unavailable.
:::

::: tip
In a Wayland session the snap runs natively on Wayland, otherwise it uses X11.
If you run into Wayland related issues, you can force X11 (XWayland) by starting QOwnNotes with
`QOWNNOTES_SNAP_FORCE_X11=1 qownnotes`.
:::

::: tip
The Qt6 snap uses the host desktop's file chooser through `xdg-desktop-portal`.
Install `xdg-desktop-portal` and your desktop's portal backend (for example,
`xdg-desktop-portal-cosmic`, `xdg-desktop-portal-gnome`, or `xdg-desktop-portal-kde`)
on the host to use its native file chooser. Without a working portal, Qt falls back
to an in-snap file chooser.

The home directory shown in file dialogs is your real home directory. Settings,
databases, and caches remain in the snap's private storage; existing note folders
are not moved. Snap confinement still limits which folders can be accessed.
:::

## Legacy Qt 5 snap

The legacy Qt 5-named GitHub Snap jobs are retired starting with QOwnNotes 26.10.15.
Use the Qt 6 Store snap for ongoing updates. If your system cannot run the current
snap, the final genuine Qt 5 AMD64 build, `QOwnNotes-Qt5-amd64.snap`, and its
SHA-256 checksum remain available in the
[26.10.6 release](https://github.com/pbek/QOwnNotes/releases/tag/v26.10.6).
It will not receive further updates. Later releases' artifacts with the same
Qt 5 filename already contain Qt 6 and are not a Qt 5 fallback.
