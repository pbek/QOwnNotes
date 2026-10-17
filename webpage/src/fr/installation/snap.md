# Installer depuis Snap

Il existe un snap `qownnotes` dans le [Snap store](https://snapcraft.io/qownnotes).

Vous pouvez l'installer en tapant :

```bash
snap install qownnotes
```

Vous devriez ensuite pouvoir exécuter QOwnNotes avec la commande `qownnotes`.

Les [Snaps](http://snapcraft.io) fonctionnent sur de nombreuses distributions Linux comme Ubuntu, Arch Linux, Debian, Fedora, openSUSE, Gentoo Linux, OpenWRT, open embedded et yocto project.
:::

::: tip
Si QOwnNotes affiche `Could not write secret to keychain`, installez les paquets Secret Service manquants et redémarrez votre session de bureau.

Pour GNOME et les autres environnements reposant sur Secret Service, installez `gnome-keyring`, `libsecret` et `seahorse`.

Pour KDE Plasma, installez un support de KWallet tel que `kwalletmanager`.

QOwnNotes repassera sur le chiffrement obsolète si le porte-clés de l’environnement n’est pas disponible.
:::

::: tip
In a Wayland session the snap runs natively on Wayland, otherwise it uses X11. If you run into Wayland related issues, you can force X11 (XWayland) by starting QOwnNotes with `QOWNNOTES_SNAP_FORCE_X11=1 qownnotes`.
:::

::: tip
The Qt6 snap uses the host desktop's file chooser through `xdg-desktop-portal`. Install `xdg-desktop-portal` and your desktop's portal backend (for example, `xdg-desktop-portal-cosmic`, `xdg-desktop-portal-gnome`, or `xdg-desktop-portal-kde`) on the host to use its native file chooser. Without a working portal, Qt falls back to an in-snap file chooser.

The home directory shown in file dialogs is your real home directory. Settings, databases, and caches remain in the snap's private storage; existing note folders are not moved. Snap confinement still limits which folders can be accessed.
:::
