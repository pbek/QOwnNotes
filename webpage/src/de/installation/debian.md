# Installieren Sie unter Debian Linux

## Debian 11.0

Führen Sie die folgenden Shell-Befehle aus, um dem Repository zu vertrauen.

```bash
SIGNED_BY='/etc/apt/keyrings/qownnotes.gpg'
sudo mkdir -p "$(dirname "${SIGNED_BY}")"
curl --silent --show-error --location http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_11/Release.key | gpg --dearmor | sudo tee "${SIGNED_BY}" > /dev/null
sudo chmod u=rw,go=r "${SIGNED_BY}"
```

Führen Sie die folgenden Shell-Befehle aus, um das Repository hinzuzufügen und QOwnNotes von dort aus zu installieren.

```bash
SIGNED_BY='/etc/apt/keyrings/qownnotes.gpg'
ARCHITECTURE="$(dpkg --print-architecture)"
echo "deb [arch=${ARCHITECTURE} signed-by=${SIGNED_BY}] http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_11/ /" | sudo tee /etc/apt/sources.list.d/qownnotes.list > /dev/null
sudo apt update
sudo apt install qownnotes
```

::: tip
Wenn Sie dieses Repository für andere Debian Linux-Versionen verwenden, stellen Sie bitte sicher, dass Sie **Qt** mindestens in **Version 5.11** installiert haben.
:::

[Direkter Download](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_11)

## Debian 10.0

Führen Sie die folgenden Shell-Befehle aus, um dem Repository zu vertrauen.

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_10/Release.key -O - | sudo apt-key add -
```

Führen Sie die folgenden Shell-Befehle aus, um das Repository hinzuzufügen und QOwnNotes von dort aus zu installieren.

```bash
sudo bash -c "echo 'deb http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_10/ /' >> /etc/apt/sources.list.d/qownnotes.list"
sudo apt-get update
sudo apt-get install qownnotes
```

::: tip
Wenn Sie dieses Repository für andere Debian Linux-Versionen verwenden, stellen Sie bitte sicher, dass Sie **Qt** mindestens in **Version 5.8** installiert haben.
:::

[Direkter Download](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_10)

## Debian 9.0

Führen Sie die folgenden Shell-Befehle aus, um dem Repository zu vertrauen.

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_9.0/Release.key -O - | sudo apt-key add -
```

Führen Sie die folgenden Shell-Befehle aus, um das Repository hinzuzufügen und QOwnNotes von dort aus zu installieren.

```bash
sudo bash -c "echo 'deb http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_9.0/ /' >> /etc/apt/sources.list.d/qownnotes.list"
sudo apt-get update
sudo apt-get install qownnotes
```

::: tip
Wenn Sie dieses Repository für andere Debian Linux-Versionen verwenden, stellen Sie bitte sicher, dass **Qt** mindestens in **Version 5.5** installiert ist.
:::

[Direkter Download](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_9.0)
