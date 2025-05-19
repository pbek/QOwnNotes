# Installer sur Ubuntu Linux, OS élémentaire et Linux Mint

## Ubuntu Linux 18.04 ou plus récent, elementary OS, Linux Mint 18 ou plus récent

Installez **QOwnNotes** sur **Ubuntu Linux** (minimum 18.04) à l'aide du dépôt PPA.

Notez qu'il existe également des paquets pour ARM dans le dépôt, au cas où vous souhaiteriez utiliser QOwnNotes sur un **Raspberry Pi **.

Ouvrez un terminal et entrez les commandes suivantes pour ajouter le dépôt et installer QOwnNotes.

```bash
sudo add-apt-repository ppa:pbek/qownnotes
sudo apt-get update
sudo apt-get install qownnotes
```

[Téléchargement direct](https://launchpad.net/~pbek/+archive/ubuntu/qownnotes/+packages)

## Aperçu de QOwnNotes Qt6

If you already want to try the Qt6 packages, built with cmake and are on Ubuntu 24.04 or newer, you can use this repository:

```bash
# Enlève le PPA Qt5 au cas où il serait installé
sudo add-apt-repository --remove ppa:pbek/qownnotes

# Ajoute le PPA Qt6
sudo add-apt-repository ppa:pbek/qownnotes-qt6

# Installe QOwnNotes
sudo apt-get update
sudo apt-get install qownnotes
```

[Téléchargement direct](https://launchpad.net/~pbek/+archive/ubuntu/qownnotes-qt6/+packages)

If you find any issues, please report them on [QOwnNotes Issues on GitHub](https://github.com/pbek/QOwnNotes/issues)!

## Anciennes distributions et dérivés d'Ubuntu Linux

Best try to use the AppImage from [QOwnNotes Releases on GitHub](https://github.com/pbek/QOwnNotes/releases).

Then you can change the execute-permissions on the file:

```bash
chmod a+x QOwnNotes-*.AppImage
```

Afterward you should be able to execute the AppImage to run QOwnNotes.

## Dépôt OBS

You may also be able to use the QOwnNotes on Ubuntu versions that aren't anymore updated on Ubuntu Launchpad then you can use the repositories from the [Open Build Service](https://build.opensuse.org/package/show/home:pbek:QOwnNotes/desktop). The packages for Ubuntu 24.04+ are built with cmake and Qt6.

Below are example calls for the xUbuntu 24.04 repository.

Run the following shell command to trust the repository.

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/xUbuntu_24.04/Release.key -O - | sudo apt-key add -
```

Run the following shell commands to add the repository and install QOwnNotes from there.

```bash
sudo su -
sh -c "echo 'deb http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/xUbuntu_24.04/ /' >> /etc/apt/sources.list.d/qownnotes.list"
apt-get update
apt-get install qownnotes
```

[Direct Download](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/xUbuntu_24.04)
