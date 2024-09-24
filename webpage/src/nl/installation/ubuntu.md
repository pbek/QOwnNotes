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

Als u de Qt6-pakketten al wilt proberen en u op Ubuntu 22.04 of nieuwer wilt gebruiken u kunt deze repository gebruiken:

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

Als u problemen vindt, meld deze dan op [ QOwnNotes-problemen op GitHub ](https://github.com/pbek/QOwnNotes/issues)!

## Oudere Ubuntu Linux-distributies en afgeleiden

U kunt het beste de AppImage van [QOwnNotes Releases op GitHub](https://github.com/pbek/QOwnNotes/releases) proberen te gebruiken.

Vervolgens kunt u de uitvoeringsrechten voor het bestand wijzigen:

```bash
chmod a+x QOwnNotes-*.AppImage
```

Daarna zou u de AppImage moeten kunnen uitvoeren om QOwnNotes uit te voeren.

## OBS Repository

Je kunt QOwnNotes ook gebruiken op Ubuntu-versies die niet meer worden bijgewerkt op Ubuntu Launchpad, gebruik dan de repositories van de [Open Build Service](https://build.opensuse.org/package/show/home:pbek:QOwnNotes/desktop).

Hieronder staan voorbeeldoproepen voor de xUbuntu 20.04-repository.

Voer de volgende shellopdracht uit om de repository te vertrouwen.

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/xUbuntu_20.04/Release.key -O - | sudo apt-key add -
```

Voer de volgende shell-opdrachten uit om de repository toe te voegen en vanaf daar QOwnNotes te installeren.

```bash
sudo su -
sh -c "echo 'deb http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/xUbuntu_20.04/ /' >> /etc/apt/sources.list.d/qownnotes.list"
apt-get update
apt-get install qownnotes
```

[Directe download](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/xUbuntu_20.04)
