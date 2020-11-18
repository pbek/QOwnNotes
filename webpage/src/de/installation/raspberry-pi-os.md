# Installieren Sie unter Raspberry Pi OS

## Raspberry Pi OS 10

Führen Sie die folgenden Shell-Befehle aus, um dem Repository zu vertrauen.

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_10/Release.key -O - | sudo apt-key add -
```

Führen Sie die folgenden Shell-Befehle aus, um das Repository hinzuzufügen und QOwnNotes von dort aus zu installieren.

```bash
sudo bash -c "echo 'deb http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_10/ /' >> /etc/apt/sources.list.d/qownnotes.list"
sudo apt-get update
sudo apt-get install qownnotes
```

[Direkter Download](https://build.opensuse.org/package/binaries/home:pbek:QOwnNotes/desktop/Raspbian_10)

## Raspbian 9.0

Führen Sie die folgenden Shell-Befehle aus, um dem Repository zu vertrauen.

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_9.0/Release.key
-O - | sudo apt-key add -
```

Führen Sie die folgenden Shell-Befehle aus, um das Repository hinzuzufügen und QOwnNotes von dort aus zu installieren.

```bash
sudo bash -c "echo 'deb http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_9.0/ /' >> /etc/apt/sources.list.d/qownnotes.list"
sudo apt-get update
sudo apt-get install qownnotes
```

[Direkter Download](https://build.opensuse.org/package/binaries/home:pbek:QOwnNotes/desktop/Raspbian_9.0)
