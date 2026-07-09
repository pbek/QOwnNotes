# Installer en tant que Flatpak

Un Flatpak maintenu par la communauté existe : `org.qownnotes.QOwnNotes` sur [Flathub](https://flathub.org/apps/details/org.qownnotes.QOwnNotes).

Vous pouvez l'installer en tapant :

```bash
flatpak remote-add --if-not-exists flathub https://flathub.org/repo/flathub.flatpakrepo
flatpak install flathub org.qownnotes.QOwnNotes
```

Vous devriez ensuite pouvoir exécuter QOwnNotes avec la commande `flatpak run org.qownnotes.QOwnNotes`.

::: tip Info
Merci d’ouvrir les tickets à propos du paquet Flatpak sur [la page des issues de QOwnNotes sur Flatpak](https://github.com/flathub/org.qownnotes.QOwnNotes/issues).
:::

::: tip
Si QOwnNotes affiche `Could not write secret to keychain`, installez les paquets Secret Service manquants et redémarrez votre session de bureau.

Pour GNOME et les autres environnements reposant sur Secret Service, installez `gnome-keyring`, `libsecret` et `seahorse`.

Pour KDE Plasma, installez un support de KWallet tel que `kwalletmanager`.

QOwnNotes repassera sur le chiffrement obsolète si le porte-clés de l’environnement n’est pas disponible.
:::
