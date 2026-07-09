# Installer sur Solus

Il existe un package `qownnotes` maintenu par le [Projet Solus](https://getsol.us/).

Vous pouvez l'installer en tapant :

```bash
sudo eopkg install qownnotes
```

::: tip
Si QOwnNotes affiche `Could not write secret to keychain`, installez les paquets Secret Service manquants et redémarrez votre session de bureau.

Pour GNOME et les autres environnements reposant sur Secret Service :

```bash
sudo eopkg install gnome-keyring libsecret seahorse
```

Pour KDE Plasma, installez un support KWallet depuis vos dépôts configurés.

QOwnNotes repassera sur le chiffrement obsolète si le porte-clés de l’environnement n’est pas disponible.
:::
