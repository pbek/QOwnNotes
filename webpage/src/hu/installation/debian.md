# Telepítés a Debian Linux rendszerre

## Debian 11.0

Futtassa a következő shell parancsokat, hogy megbízhasson az adattárban.

```bash
SIGNED_BY='/etc/apt/keyrings/qownnotes.gpg'
sudo mkdir -p "$(dirname "${SIGNED_BY}")"
curl --silent --show-error --location http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_11/Release.key | gpg --dearmor | sudo tee "${SIGNED_BY}" > /dev/null
sudo chmod u=rw,go=r "${SIGNED_BY}"
```

Futtassa a következő shell parancsokat az adattár hozzáadásához és a QOwnNotes telepítéséhez onnan.

```bash
SIGNED_BY='/etc/apt/keyrings/qownnotes.gpg'
ARCHITECTURE="$(dpkg --print-architecture)"
echo "deb [arch=${ARCHITECTURE} signed-by=${SIGNED_BY}] http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_11/ /" | sudo tee /etc/apt/sources.list.d/qownnotes.list > /dev/null
sudo apt update
sudo apt install qownnotes
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
