# Installer sur Funtoo Linux

Funtoo Linux possède un paquet QOwnNotes auto-généré dans [desktop-kit](https://github.com/funtoo/desktop-kit/tree/1.4-release/app-office/qownnotes). Il peut être installé avec :

```bash
sudo emerge -av qownnotes
```

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
