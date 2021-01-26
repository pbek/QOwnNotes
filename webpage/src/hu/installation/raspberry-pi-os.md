# Install on Raspberry Pi OS

## Raspberry Pi OS 10

Futtassa a következő shell parancsokat, hogy megbízhasson a tárban.

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_10/Release.key -O - | sudo apt-key add -
```

Futtassa a következő shell parancsokat a lerakat hozzáadásához és a QOwnNotes telepítéséhez onnan.

```bash
sudo bash -c "echo 'deb http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_10/ /' >> /etc/apt/sources.list.d/qownnotes.list"
sudo apt-get update
sudo apt-get install qownnotes
```

[Közvetlen letöltés](https://build.opensuse.org/package/binaries/home:pbek:QOwnNotes/desktop/Raspbian_10)

## Raspbian 9.0

Futtassa a következő shell parancsokat, hogy megbízhasson a tárban.

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_9.0/Release.key
-O - | sudo apt-key add -
```

Futtassa a következő shell parancsokat a lerakat hozzáadásához és a QOwnNotes telepítéséhez onnan.

```bash
sudo bash -c "echo 'deb http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_9.0/ /' >> /etc/apt/sources.list.d/qownnotes.list"
sudo apt-get update
sudo apt-get install qownnotes
```

[Közvetlen letöltés](https://build.opensuse.org/package/binaries/home:pbek:QOwnNotes/desktop/Raspbian_9.0)
