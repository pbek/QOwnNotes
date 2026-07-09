# Installer sur Gentoo Linux

Ajoutez le dépôt QOwnNotes avec la commande suivante (voir [eselect-repository](https://wiki.gentoo.org/wiki/Eselect/Repository)).

```bash
sudo eselect repository enable qownnotes-overlay
```

Vous pouvez ensuite installer QOwnNotes de la manière habituelle.

```bash
sudo emerge --sync qownnotes-overlay && sudo emerge -av qownnotes
```

[Overlay de QOwnNotes sur GitHub](https://github.com/qownnotes/gentoo-overlay/)

::: tip
Si QOwnNotes affiche `Could not write secret to keychain`, installez les paquets Secret Service manquants et redémarrez votre session de bureau.

Pour GNOME et les autres environnements reposant sur Secret Service :

```bash
sudo emerge -av gnome-extra/gnome-keyring app-crypt/libsecret app-crypt/seahorse
```

Pour KDE Plasma :

```bash
sudo emerge -av kde-apps/kwalletmanager kde-frameworks/kwallet
```

QOwnNotes repassera sur le chiffrement obsolète si le porte-clés de l’environnement n’est pas disponible.
:::
