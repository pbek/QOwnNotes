# Install on Raspberry Pi OS

## Raspberry Pi OS 10

Futtassa a következő shell parancsokat, hogy megbízhasson az adattárban.

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_10/Release.key -O - | sudo apt-key add -
```

Futtassa a következő shell parancsokat az adattár hozzáadásához és a QOwnNotes onnan való telepítéséhez.

```bash
sudo bash -c "echo 'deb http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_10/ /' >> /etc/apt/sources.list.d/qownnotes.list"
sudo apt-get update
sudo apt-get install qownnotes
```

[Közvetlen letöltés](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_10)

## Raspbian 9.0

Futtassa a következő shell parancsokat, hogy megbízhasson az adattárban.

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_9.0/Release.key
-O - | sudo apt-key add -
```

Futtassa a következő shell parancsokat a lerakat hozzáadásához és a QOwnNotes onnan való telepítéséhez.

```bash
sudo bash -c "echo 'deb http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_9.0/ /' >> /etc/apt/sources.list.d/qownnotes.list"
sudo apt-get update
sudo apt-get install qownnotes
```

[Közvetlen letöltés](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_9.0)
