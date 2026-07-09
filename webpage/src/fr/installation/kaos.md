# Installer sur KaOS Linux

Il existe un package géré par la communauté `qownnotes` dans le dépôt [KaOS Community Packages](https://github.com/KaOS-Community-Packages/qownnotes).

Vous pouvez l'installer en tapant :

```bash
kcp -i qownnotes
```

::: tip
Si QOwnNotes affiche `Could not write secret to keychain`, installez les paquets Secret Service manquants et redémarrez votre session de bureau.

Pour KDE Plasma :

```bash
sudo pacman -S kwalletmanager kwallet
```

Pour GNOME et les autres environnements reposant sur Secret Service, installez `gnome-keyring`, `libsecret` et `seahorse` depuis vos dépôts configurés.

QOwnNotes repassera sur le chiffrement obsolète si le porte-clés de l’environnement n’est pas disponible.
:::
