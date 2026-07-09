# Installer sur Ubuntu Linux, elementary OS et Linux Mint

## Ubuntu Linux 24.04 ou ultérieur, elementary OS, Linux Mint 22.1 ou ultérieur

Installez **QOwnNotes** sur **Ubuntu Linux** (minimum 24.04) à l'aide du dépôt PPA. Cette version de QOwnNotes est compilée avec Qt6.

Notez qu'il existe également des paquets pour ARM dans le dépôt, au cas où vous souhaiteriez utiliser QOwnNotes sur un **Raspberry Pi**.

Ouvrez un terminal et entrez les commandes suivantes pour ajouter le dépôt et installer QOwnNotes.

```bash
# Suppression du PPA de Qt5 au cas où vous l’auriez déjà installé
sudo add-apt-repository --remove ppa:pbek/qownnotes

# Ajout du PPA de Qt6
sudo add-apt-repository ppa:pbek/qownnotes-qt6

# Installation de QOwnNotes
sudo apt-get update
sudo apt-get install qownnotes
```

[Téléchargement direct](https://launchpad.net/~pbek/+archive/ubuntu/qownnotes-qt6/+packages)

Si vous trouvez des problèmes, merci de les signaler sur [la page des problèmes de QOwnNotes sur GitHub](https://github.com/pbek/QOwnNotes/issues) !

## Ancienne version Qt5 de QOwnNotes

Pour les anciennes versions supportées d’Ubuntu Linux, elementary OS, Linux Mint 18 ou suivantes, vous pouvez utiliser la version Qt5 de QOwnNotes.

Installez **QOwnNotes** sur **Ubuntu Linux** (minimum 20.04) à l'aide du dépôt PPA.

Notez qu'il existe également des paquets pour ARM dans le dépôt, au cas où vous souhaiteriez utiliser QOwnNotes sur un **Raspberry Pi**.

Ouvrez un terminal et entrez les commandes suivantes pour ajouter le dépôt et installer QOwnNotes.

```bash
sudo add-apt-repository ppa:pbek/qownnotes
sudo apt-get update
sudo apt-get install qownnotes
```

[Téléchargement direct](https://launchpad.net/~pbek/+archive/ubuntu/qownnotes/+packages)

## Anciennes distributions et dérivés d'Ubuntu Linux

Mieux vaut essayer d’utiliser l’AppImage sur la [pages des Releases de QOwnNotes sur GitHub](https://github.com/pbek/QOwnNotes/releases).

Voir [Installer en tant qu’AppImage](./appimage.md) pour plus d’informations.

## Dépôt OBS

Vous pouvez également utiliser QOwnNotes sur les versions d'Ubuntu qui ne sont plus mises à jour sur Ubuntu Launchpad en utilisant les dépôts du [Open Build Service](https://build.opensuse.org/package/show/home:pbek:QOwnNotes/desktop). Les paquets pour Ubuntu 24.04+ sont compilés avec cmake et Qt6.

Vous trouverez ci-dessous des exemples d'appels pour le dépôt xUbuntu 24.04.

Exécutez la commande shell suivante pour approuver le dépôt.

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/xUbuntu_24.04/Release.key -O - | sudo apt-key add -
```

Exécutez les commandes shell suivantes pour ajouter le dépôt et installer QOwnNotes à partir de là.

```bash
sudo su -
sh -c "echo 'deb http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/xUbuntu_24.04/ /' >> /etc/apt/sources.list.d/qownnotes.list"
apt-get update
apt-get install qownnotes
```

[Téléchargement direct](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/xUbuntu_24.04)

::: tip
Si QOwnNotes affiche `Could not write secret to keychain`, installez les paquets Secret Service manquants et redémarrez votre session de bureau.

Pour GNOME et les autres environnements reposant sur Secret Service :

```bash
sudo apt install gnome-keyring libsecret-1-0 seahorse
```

Pour KDE Plasma :

```bash
sudo apt install kwalletmanager
```

QOwnNotes repassera sur le chiffrement obsolète si le porte-clés de l’environnement n’est pas disponible.
:::
