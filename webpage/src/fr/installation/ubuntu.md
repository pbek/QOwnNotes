# Installer sur Ubuntu Linux, elementary OS et Linux Mint

## Ubuntu Linux 24.04 ou ultérieur, elementary OS, Linux Mint 22.1 ou ultérieur

Installez **QOwnNotes** sur **Ubuntu Linux** (minimum 24.04) à l'aide du dépôt PPA. Cette version de QOwnNotes est compilée avec Qt6.

Notez qu'il existe également des paquets pour ARM dans le dépôt, au cas où vous souhaiteriez utiliser QOwnNotes sur un **Raspberry Pi **.

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

If you find any issues, please report them on [QOwnNotes Issues on GitHub](https://github.com/pbek/QOwnNotes/issues)!

## QOwnNotes Qt5 legacy

For older versions of Ubuntu Linux, like 18.04 or newer, elementary OS, Linux Mint 18 or newer, you can use the Qt5 version of QOwnNotes.

Install **QOwnNotes** on **Ubuntu Linux** (minimum 18.04) using the PPA repository.

Note that there are also packages for ARM in the repository, in case you want to use QOwnNotes on a **Raspberry Pi**.

Open a terminal and enter the following lines to add the repository and install QOwnNotes.

```bash
sudo add-apt-repository ppa:pbek/qownnotes
sudo apt-get update
sudo apt-get install qownnotes
```

[Téléchargement direct](https://launchpad.net/~pbek/+archive/ubuntu/qownnotes/+packages)

## Anciennes distributions et dérivés d'Ubuntu Linux

Best try to use the AppImage from [QOwnNotes Releases on GitHub](https://github.com/pbek/QOwnNotes/releases).

See [Install as AppImage](./appimage.md) for more information.

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

[Téléchargement direct](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/xUbuntu_24.04)
