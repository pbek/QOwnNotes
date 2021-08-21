# Telepítés a Debian Linux rendszerre

## Debian 11.0

Futtassa a következő shell parancsokat, hogy megbízhasson az adattárban.

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_11/Release.key -O - | sudo apt-key add -
```

Futtassa a következő shell parancsokat az adattár hozzáadásához és a QOwnNotes telepítéséhez onnan.

```bash
sudo bash -c "echo 'deb http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_11/ /' >> /etc/apt/sources.list.d/qownnotes.list"
sudo apt-get update
sudo apt-get install qownnotes
```

::: tip
Ha ezt az adattárat más Debian Linux verziókhoz használja, ellenőrizze, hogy **Qt** telepítve van-e legalább az **5.11 verzió**-ja.
:::

[Közvetlen letöltés](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_11)

## Debian 10.0

Futtassa a következő shell parancsokat, hogy megbízhasson az adattárban.

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_10/Release.key -O - | sudo apt-key add -
```

Futtassa a következő shell parancsokat az adattár hozzáadásához és a QOwnNotes telepítéséhez onnan.

```bash
sudo bash -c "echo 'deb http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_10/ /' >> /etc/apt/sources.list.d/qownnotes.list"
sudo apt-get update
sudo apt-get install qownnotes
```

::: tip
Ha ezt az adattárat más Debian Linux verziókhoz használja, ellenőrizze, hogy **Qt** telepítve van-e legalább az **5.8 verzió**-ja.
:::

[Közvetlen letöltés](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_10)

## Debian 9.0

Futtassa a következő shell parancsokat, hogy megbízhasson az adattárban.

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_9.0/Release.key -O - | sudo apt-key add -
```

Futtassa a következő shell parancsokat az adattár hozzáadásához és a QOwnNotes telepítéséhez onnan.

```bash
sudo bash -c "echo 'deb http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_9.0/ /' >> /etc/apt/sources.list.d/qownnotes.list"
sudo apt-get update
sudo apt-get install qownnotes
```

::: tip
Ha ezt az adattárat más Debian Linux verziókhoz használja, győződjön meg arról, hogy **Qt** telepítve van-e legalább az **5.5 verzióra**.
:::

[Közvetlen letöltés](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_9.0)

## Debian 8.0

Futtassa a következő shell parancsokat az adattár megbízhatóságához.

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_8.0/Release.key -O - | sudo apt-key add -
```

Futtassa a következő shell parancsokat az adattár hozzáadásához és a QOwnNotes onnan való telepítéséhez.

```bash
sudo bash -c "echo 'deb http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_8.0/ /' >> /etc/apt/sources.list.d/qownnotes.list"
sudo apt-get update
sudo apt-get install qownnotes
```

::: tip
Ha ezt az adattárat más Debian Linux verziókhoz használja, ellenőrizze, hogy **Qt** telepítve van-e legalább az **5.3 verzióra**.
:::

[Közvetlen letöltés](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_8.0)
