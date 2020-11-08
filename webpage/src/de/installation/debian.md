# Installieren Sie unter Debian Linux

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
Wenn Sie dieses Repository für andere Debian Linux-Versionen verwenden, stellen Sie bitte sicher, dass Sie ** Qt ** mindestens in ** Version 5.8 ** installiert haben.
:::

[Direkter Download](https://build.opensuse.org/package/binaries/home:pbek:QOwnNotes/desktop/Debian_10)

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
Wenn Sie dieses Repository für andere Debian Linux-Versionen verwenden, stellen Sie bitte sicher, dass ** Qt ** mindestens in ** Version 5.5 ** installiert ist.
:::

[Direkter Download](https://build.opensuse.org/package/binaries/home:pbek:QOwnNotes/desktop/Debian_9.0)

## Debian 8.0

Führen Sie die folgenden Shell-Befehle aus, um dem Repository zu vertrauen.

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_8.0/Release.key -O - | sudo apt-key add -
```

Führen Sie die folgenden Shell-Befehle aus, um das Repository hinzuzufügen und QOwnNotes von dort aus zu installieren.

```bash
sudo bash -c "echo 'deb http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_8.0/ /' >> /etc/apt/sources.list.d/qownnotes.list"
sudo apt-get update
sudo apt-get install qownnotes
```

::: tip
Wenn Sie dieses Repository für andere Debian Linux-Versionen verwenden, stellen Sie bitte sicher, dass Sie ** Qt ** mindestens in ** Version 5.3 ** installiert haben.
:::

[Direct Download](https://build.opensuse.org/package/binaries/home:pbek:QOwnNotes/desktop/Debian_8.0)
