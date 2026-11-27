# Installer sur Slackware Linux

## Slackware 15.0

Installez d’abord [Qt 6 depuis SlackBuilds.org](https://slackbuilds.org/repository/15.0/libraries/qt6/), y compris ses dépendances requises.

Récupérez ensuite le dépôt SlackBuilds de QOwnNotes et compilez l’application.

```bash
cd /tmp
git clone https://github.com/pbek/qownnotes-slackbuilds.git
cd qownnotes-slackbuilds/15.0/qownnotes
./dobuild.sh
```

Vous pouvez ensuite installer le paquet généré avec `installpkg`.

[Slackbuild QOwnNotes sur GitHub](https://github.com/pbek/qownnotes-slackbuilds/)

::: tip
Si QOwnNotes affiche `Could not write secret to keychain`, installez une implémentation Secret Service et redémarrez votre session de bureau.

Pour GNOME et les autres environnements reposant sur Secret Service, installez `gnome-keyring`, `libsecret` et `seahorse` depuis vos dépôts S|ackware ou depuis SlackBuilds.

Pour KDE Plasma, installez un support de KWallet tel que `kwalletmanager`.

QOwnNotes repassera sur le chiffrement obsolète si le porte-clés de l’environnement n’est pas disponible.
:::
