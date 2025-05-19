# Installatie op Ubuntu Linux, elementary OS en Linux Mint

## Ubuntu Linux 18.04 of nieuwer, elementary OS, Linux Mint 18 of nieuwer

Installeer **QOwnNotes** op **Ubuntu Linux** (minimaal 18.04) met behulp van de PPA-repository.

Merk op dat er ook pakketten voor ARM in de repository zijn, voor het geval je QOwnNotes op een **Raspberry Pi** wilt gebruiken.

Open een terminal en voer de volgende regels in om de repository toe te voegen en QOwnNotes te installeren.

```bash
sudo add-apt-repository ppa:pbek/qownnotes
sudo apt-get update
sudo apt-get install qownnotes
```

[Directe download](https://launchpad.net/~pbek/+archive/ubuntu/qownnotes/+packages)

## QOwnNotes Qt6 Preview

If you already want to try the Qt6 packages, built with cmake and are on Ubuntu 24.04 or newer, you can use this repository:

```bash
# Remove Qt5 PPA in case you had it installed
sudo add-apt-repository --remove ppa:pbek/qownnotes

# Add Qt6 PPA
sudo add-apt-repository ppa:pbek/qownnotes-qt6

# Install QOwnNotes
sudo apt-get update
sudo apt-get install qownnotes
```

[Directe download](https://launchpad.net/~pbek/+archive/ubuntu/qownnotes-qt6/+packages)

If you find any issues, please report them on [QOwnNotes Issues on GitHub](https://github.com/pbek/QOwnNotes/issues)!

## Oudere Ubuntu Linux-distributies en afgeleiden

U kunt het beste de AppImage van [QOwnNotes Releases op GitHub](https://github.com/pbek/QOwnNotes/releases) proberen te gebruiken.

Vervolgens kunt u de uitvoeringsrechten voor het bestand wijzigen:

```bash
chmod a+x QOwnNotes-*.AppImage
```

Daarna zou u de AppImage moeten kunnen uitvoeren om QOwnNotes uit te voeren.

## OBS Repository

You may also be able to use the QOwnNotes on Ubuntu versions that aren't anymore updated on Ubuntu Launchpad then you can use the repositories from the [Open Build Service](https://build.opensuse.org/package/show/home:pbek:QOwnNotes/desktop). The packages for Ubuntu 24.04+ are built with cmake and Qt6.

Below are example calls for the xUbuntu 24.04 repository.

Voer de volgende shellopdracht uit om de repository te vertrouwen.

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/xUbuntu_24.04/Release.key -O - | sudo apt-key add -
```

Voer de volgende shell-opdrachten uit om de repository toe te voegen en vanaf daar QOwnNotes te installeren.

```bash
sudo su -
sh -c "echo 'deb http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/xUbuntu_24.04/ /' >> /etc/apt/sources.list.d/qownnotes.list"
apt-get update
apt-get install qownnotes
```

[Directe download](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/xUbuntu_24.04)
