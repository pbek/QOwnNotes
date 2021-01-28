# Installer sur Ubuntu Linux, OS élémentaire et Linux Mint

## Ubuntu Linux 18.04 or newer, elementary OS, Linux Mint 18 or newer

Installez **QOwnNotes** sur **Ubuntu Linux** (minimum 18.04) à l'aide du référentiel PPA.

Notez qu'il existe également des packages pour ARM dans le référentiel, au cas où vous souhaiteriez utiliser QOwnNotes sur un **Raspberry P **.

Ouvrez un terminal et entrez les lignes suivantes pour ajouter le référentiel et installer QOwnNotes.

```bash
sudo add-apt-repository ppa:pbek/qownnotes
sudo apt-get update
sudo apt-get install qownnotes
```

[Téléchargement direct](https://launchpad.net/~pbek/+archive/ubuntu/qownnotes/+packages)

## Anciennes distributions et dérivés Ubuntu Linux

Il est préférable d'utiliser l'AppImage des versions [QOwnNotes sur GitHub](https://github.com/pbek/QOwnNotes/releases).

Ensuite, vous pouvez modifier les autorisations d'exécution sur le fichier:

```bash
chmod a+x QOwnNotes-*.AppImage
```

Ensuite, vous devriez être en mesure d'exécuter AppImage pour exécuter QOwnNotes.

## OBS Repository

Vous pouvez également utiliser les QOwnNotes sur les versions d'Ubuntu qui ne sont plus mises à jour sur Ubuntu Launchpad, vous pouvez alors utiliser les référentiels du [Open Build Service](https://build.opensuse.org/package/show/home:pbek:QOwnNotes/desktop).

Vous trouverez ci-dessous des exemples d'appels pour le référentiel xUbuntu 16.10.

Exécutez la commande shell suivante pour approuver le référentiel.

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/xUbuntu_16.10/Release.key -O - | sudo apt-key add -
```

Exécutez les commandes shell suivantes pour ajouter le référentiel et installer QOwnNotes à partir de là.

```bash
sudo su -
sh -c "echo 'deb http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/xUbuntu_16.10/ /' >> /etc/apt/sources.list.d/qownnotes.list"
apt-get update
apt-get install qownnotes
```

[Téléchargement direct](https://build.opensuse.org/package/binaries/home:pbek:QOwnNotes/desktop/xUbuntu_16.10)
