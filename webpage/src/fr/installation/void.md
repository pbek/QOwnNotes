# Installer sur Void Linux

Il existe un paquet créé par la communauté pour **Void Linux**, disponible dans [qownnotes srcpkg](https://github.com/void-linux/void-packages/tree/master/srcpkgs/qownnotes) et qui peut être installé comme suit :

```bash
# Synchroniser les fichiers d'index du dépôt distant et installer QOwnNotes
xbps-install -S qownnotes
```

::: tip
Si QOwnNotes affiche `Could not write secret to keychain`, installez les paquets Secret Service manquants et redémarrez votre session de bureau.

Pour GNOME et les autres environnements reposant sur Secret Service :

```bash
sudo xbps-install -S gnome-keyring libsecret seahorse
```

Pour KDE Plasma :

```bash
sudo xbps-install -S kwalletmanager
```

QOwnNotes repassera sur le chiffrement obsolète si le porte-clés de l’environnement n’est pas disponible.
:::
