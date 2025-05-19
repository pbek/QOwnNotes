# Installieren unter Ubuntu Linux, Elementary OS und Linux Mint

## Ubuntu Linux 18.04 oder neuer, elementary OS, Linux Mint 18 oder neuer

Installieren Sie **QOwnNotes** unter **Ubuntu Linux** (mindestens 18.04) mithilfe des PPA-Repositorys.

Beachten Sie, dass das Repository auch Pakete für ARM enthält, falls Sie QOwnNotes auf einem **Raspberry Pi** verwenden möchten.

Öffnen Sie ein Terminal und geben Sie die folgenden Zeilen ein, um das Repository hinzuzufügen und QOwnNotes zu installieren.

```bash
sudo add-apt-repository ppa:pbek/qownnotes
sudo apt-get update
sudo apt-get install qownnotes
```

[Direkter Download](https://launchpad.net/~pbek/+archive/ubuntu/qownnotes/+packages)

## QOwnNotes Qt6 Preview

If you already want to try the Qt6 packages, built with cmake and are on Ubuntu 24.04 or newer, you can use this repository:

```bash
# Entfernt Qt5 PPA, falls Sie das installiert hatten
sudo add-apt-repository --remove ppa:pbek/qownnotes

# Fügt Qt6 PPA hinzu
sudo add-apt-repository ppa:pbek/qownnotes-qt6

# Installiert QOwnNotes
sudo apt-get update
sudo apt-get install qownnotes
```

[Direkter Download](https://launchpad.net/~pbek/+archive/ubuntu/qownnotes-qt6/+packages)

If you find any issues, please report them on [QOwnNotes Issues on GitHub](https://github.com/pbek/QOwnNotes/issues)!

## Ältere Ubuntu Linux-Distributionen und -Derivate

Am Besten probieren Sie das AppImage von [QOwnNotes Versionen auf GitHub](https://github.com/pbek/QOwnNotes/releases).

Dann können Sie die Ausführungsberechtigungen für die Datei ändern:

```bash
chmod a+x QOwnNotes-*.AppImage
```

Danach sollten Sie das AppImage ausführen können, um QOwnNotes zu starten.

## OBS-Repository

You may also be able to use the QOwnNotes on Ubuntu versions that aren't anymore updated on Ubuntu Launchpad then you can use the repositories from the [Open Build Service](https://build.opensuse.org/package/show/home:pbek:QOwnNotes/desktop). The packages for Ubuntu 24.04+ are built with cmake and Qt6.

Below are example calls for the xUbuntu 24.04 repository.

Führen Sie die folgenden Befehle aus, um dem Repository zu vertrauen.

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/xUbuntu_24.04/Release.key -O - | sudo apt-key add -
```

Führen Sie die folgenden Shell-Befehle aus, um das Repository hinzuzufügen und von dort aus QOwnNotes zu installieren.

```bash
sudo su -
sh -c "echo 'deb http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/xUbuntu_24.04/ /' >> /etc/apt/sources.list.d/qownnotes.list"
apt-get update
apt-get install qownnotes
```

[Direkter Download](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/xUbuntu_24.04)
