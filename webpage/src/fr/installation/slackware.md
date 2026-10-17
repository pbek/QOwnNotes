# Installer sur Slackware Linux

## Slackware 15.0

First install [Qt 6 from SlackBuilds.org](https://slackbuilds.org/repository/15.0/libraries/qt6/), including its required dependencies.

Then check out the QOwnNotes SlackBuilds repository and build the application.

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
