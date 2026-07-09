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
