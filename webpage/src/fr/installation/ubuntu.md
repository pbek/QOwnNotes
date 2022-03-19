# Installer sur Ubuntu Linux, OS élémentaire et Linux Mint

## Ubuntu Linux 18.04 ou plus récent, elementary OS, Linux Mint 18 ou plus récent

Installez **QOwnNotes** sur **Ubuntu Linux** (minimum 18.04) à l'aide du dépôt PPA.

Notez qu'il existe également des paquets pour ARM dans le dépôt, au cas où vous souhaiteriez utiliser QOwnNotes sur un **Raspberry Pi **.

Ouvrez un terminal et entrez les commandes suivantes pour ajouter le dépôt et installer QOwnNotes.

```bash
sudo add-apt-repository -y ppa:pbek/qownnotes
sudo apt-get update
sudo apt-get install qownnotes
```

[Téléchargement direct](https://launchpad.net/~pbek/+archive/ubuntu/qownnotes/+packages)

## Anciennes distributions et dérivés Ubuntu Linux

Il est préférable d'utiliser l'AppImage des versions [QOwnNotes sur GitHub](https://github.com/pbek/QOwnNotes/releases).

Vous pouvez ensuite modifier les autorisations d'exécution sur le fichier :

```bash
chmod a+x QOwnNotes-*.AppImage
```

Vous devriez ensuite être en mesure d'exécuter l'AppImage pour utiliser QOwnNotes.

## Dépôt OBS

Vous pouvez également utiliser QOwnNotes sur les versions d'Ubuntu qui ne sont plus mises à jour sur Ubuntu Launchpad. Utilisez les dépôts du [Open Build Service](https://build.opensuse.org/package/show/home:pbek:QOwnNotes/desktop).

Vous trouverez ci-dessous des exemples d'appels pour le dépôt xUbuntu 16.10.

Exécutez la commande shell suivante pour approuver le dépôt.

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/xUbuntu_16.10/Release.key -O - | sudo apt-key add -
```

Exécutez les commandes shell suivantes pour ajouter le dépôt et installer QOwnNotes à partir de là.

```bash
sudo su -
sh -c "echo 'deb http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/xUbuntu_16.10/ /' >> /etc/apt/sources.list.d/qownnotes.list"
apt-get update
apt-get install qownnotes
```

[Téléchargement direct](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/xUbuntu_16.10)
