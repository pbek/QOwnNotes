# Installer sur Raspberry Pi OS

## Raspberry Pi OS 11

Exécutez les commandes shell suivantes pour approuver le dépôt.

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_11/Release.key -O - | sudo apt-key add -
```

Exécutez les commandes shell suivantes pour ajouter le dépôt et installer QOwnNotes à partir de là.

```bash
sudo bash -c "echo 'deb http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_11/ /' >> /etc/apt/sources.list.d/qownnotes.list"
sudo apt-get update
sudo apt-get install qownnotes
```

[Téléchargement direct](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_11)

## Raspberry Pi OS 10

Exécutez les commandes shell suivantes pour approuver le dépôt.

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_11/Release.key -O - | sudo apt-key add -
```

Exécutez les commandes shell suivantes pour ajouter le dépôt et installer QOwnNotes à partir de là.

```bash
sudo bash -c "echo 'deb http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_11/ /' >> /etc/apt/sources.list.d/qownnotes.list"
sudo apt-get update
sudo apt-get install qownnotes
```

[Téléchargement direct](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_10)

## Raspbian 9.0

Exécutez les commandes shell suivantes pour approuver le dépôt.

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_9.0/Release.key
-O - | sudo apt-key add -
```

Exécutez les commandes shell suivantes pour ajouter le dépôt puis installer QOwnNotes.

```bash
sudo bash -c "echo 'deb http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_9.0/ /' >> /etc/apt/sources.list.d/qownnotes.list"
sudo apt-get update
sudo apt-get install qownnotes
```

[Téléchargement direct](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_9.0)
