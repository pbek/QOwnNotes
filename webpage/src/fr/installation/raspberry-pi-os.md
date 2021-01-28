# Install on Raspberry Pi OS

## Raspberry Pi OS 10

Exécutez les commandes shell suivantes pour approuver le référentiel.

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_10/Release.key -O - | sudo apt-key add -
```

Exécutez les commandes shell suivantes pour ajouter le référentiel et installer QOwnNotes à partir de là.

```bash
sudo bash -c "echo 'deb http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_10/ /' >> /etc/apt/sources.list.d/qownnotes.list"
sudo apt-get update
sudo apt-get install qownnotes
```

[Téléchargement direct](https://build.opensuse.org/package/binaries/home:pbek:QOwnNotes/desktop/Raspbian_10)

## Raspbian 9.0

Exécutez les commandes shell suivantes pour approuver le référentiel.

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_9.0/Release.key
-O - | sudo apt-key add -
```

Exécutez les commandes shell suivantes pour ajouter le référentiel et installer QOwnNotes à partir de là.

```bash
sudo bash -c "echo 'deb http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_9.0/ /' >> /etc/apt/sources.list.d/qownnotes.list"
sudo apt-get update
sudo apt-get install qownnotes
```

[Téléchargement direct](https://build.opensuse.org/package/binaries/home:pbek:QOwnNotes/desktop/Raspbian_9.0)
