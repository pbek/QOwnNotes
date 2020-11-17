# Installieren Sie unter Ubuntu Linux, Elementary OS und Linux Mint

## Ubuntu Linux 18.04 oder neuer, elementares Betriebssystem, Linux Mint 18 oder neuer

Installieren Sie **QOwnNotes** unter **Ubuntu Linux** (mindestens 18.04) mithilfe des PPA-Repositorys.

Beachten Sie, dass das Repository auch Pakete für ARM enthält, falls Sie QOwnNotes auf einem **Raspberry Pi** verwenden möchten.

Öffnen Sie ein Terminal und geben Sie die folgenden Zeilen ein, um das Repository hinzuzufügen und QOwnNotes zu installieren.

```bash
sudo add-apt-repository ppa:pbek/qownnotes
sudo apt-get update
sudo apt-get install qownnotes
```

[Direkter Download](https://launchpad.net/~pbek/+archive/ubuntu/qownnotes/+packages)

## Ältere Ubuntu Linux-Distributionen und -Derivate

Versuchen Sie am besten, das AppImage aus [QOwnNotes Releases auf GitHub](https://github.com/pbek/QOwnNotes/releases) zu verwenden.

Dann können Sie die Ausführungsberechtigungen für die Datei ändern:

```bash
chmod a+x QOwnNotes-*.AppImage
```

Anschließend sollten Sie AppImage ausführen können, um dann QOwnNotes auszuführen.

## OBS Repository

Möglicherweise können Sie auch die QOwnNotes unter Ubuntu-Versionen verwenden, die auf Ubuntu Launchpad nicht mehr aktualisiert wurden. Anschließend können Sie die Repositorys des [Open Build Service](https://build.opensuse.org/package/show/home:pbek:QOwnNotes/desktop) verwenden.

Im Folgenden finden Sie Beispielaufrufe für das xUbuntu 16.10-Repository.

Führen Sie den folgenden Shell-Befehl aus, um dem Repository zu vertrauen.

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/xUbuntu_16.10/Release.key -O - | sudo apt-key add -
```

Führen Sie die folgenden Shell-Befehle aus, um das Repository hinzuzufügen und QOwnNotes von dort aus zu installieren.

```bash
sudo su -
sh -c "echo 'deb http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/xUbuntu_16.10/ /' >> /etc/apt/sources.list.d/qownnotes.list"
apt-get update
apt-get install qownnotes
```

[Direkter Download](https://build.opensuse.org/package/binaries/home:pbek:QOwnNotes/desktop/xUbuntu_16.10)
