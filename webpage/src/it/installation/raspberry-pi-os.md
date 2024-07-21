# Installa su Raspberry Pi OS

## Raspberry Pi OS 12

Esegui i seguenti comandi della shell per considerare attendibile il repository.

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_12/Release.key -O - | sudo apt-key add -
```

Esegui i seguenti comandi shell per aggiungere il repository e installare QOwnNotes da lì.

```bash
sudo bash -c "echo 'deb http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_12/ /' >> /etc/apt/sources.list.d/qownnotes.list"
sudo apt-get update
sudo apt-get install qownnotes
```

[Download diretto](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_12)

## Raspberry Pi OS 11

Esegui i seguenti comandi della shell per considerare attendibile il repository.

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_11/Release.key -O - | sudo apt-key add -
```

Esegui i seguenti comandi shell per aggiungere il repository e installare QOwnNotes da lì.

```bash
sudo bash -c "echo 'deb http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_11/ /' >> /etc/apt/sources.list.d/qownnotes.list"
sudo apt-get update
sudo apt-get install qownnotes
```

[Download diretto](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_11)

## Raspberry Pi OS 10

Esegui i seguenti comandi della shell per considerare attendibile il repository.

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_10/Release.key -O - | sudo apt-key add -
```

Esegui i seguenti comandi shell per aggiungere il repository e installare QOwnNotes da lì.

```bash
sudo bash -c "echo 'deb http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_10/ /' >> /etc/apt/sources.list.d/qownnotes.list"
sudo apt-get update
sudo apt-get install qownnotes
```

[Download diretto](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_10)

## Raspbian 9.0

Eseguire i seguenti comandi shell per considerare attendibile il repository.

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_9.0/Release.key
-O - | sudo apt-key add -
```

Eseguire i seguenti comandi shell per aggiungere il repository e installare QOwnNotes direttamente da esso.

```bash
sudo bash -c "echo 'deb http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_9.0/ /' >> /etc/apt/sources.list.d/qownnotes.list"
sudo apt-get update
sudo apt-get install qownnotes
```

[Download diretto](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_9.0)
