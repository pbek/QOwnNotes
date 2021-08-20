# Telepítse az Ubuntu Linux-ra, az elemi operációs rendszerre és a Linux Mint-re

## Ubuntu Linux 18.04 vagy újabb, elemi operációs rendszer, Linux Mint 18 vagy újabb

Telepítse a **QOwnNotes** alkalmazást az **Ubuntu Linux** operációs rendszerre (minimum 18.04) a PPA adattár használatával.

Vegye figyelembe, hogy az ARM számára is vannak csomagok az adattárban, arra az esetre, ha a QOwnNotes alkalmazást egy **Raspberry Pi** rendszeren szeretné használni.

Nyissa meg a terminált, és írja be a következő sorokat az adattár hozzáadásához és a QOwnNotes telepítéséhez.

```bash
sudo add-apt-repository ppa:pbek/qownnotes
sudo apt-get update
sudo apt-get install qownnotes
```

[Közvetlen letöltés](https://launchpad.net/~pbek/+archive/ubuntu/qownnotes/+packages)

## Régebbi Ubuntu Linux disztribúciók és származékok

A legjobb, ha megpróbálja használni az AppImage alkalmazást a [QOwnNotes Releases on GitHub](https://github.com/pbek/QOwnNotes/releases) webhelyről.

Ezután megváltoztathatja a fájl végrehajtási engedélyeit:

```bash
chmod a+x QOwnNotes-*.AppImage
```

Ezután képesnek kell lennie az AppImage futtatására a QOwnNotes futtatásához.

## OBS adattár

Lehetséges, hogy a QOwnNotes alkalmazást olyan Ubuntu verziókban is használhatja, amelyek már nincsenek frissítve az Ubuntu Launchpad alkalmazásban, majd használhatja a [Open Build Service](https://build.opensuse.org/package/show/home:pbek:QOwnNotes/desktop) adattárait.

Az alábbiakban bemutatjuk az xUbuntu 16.10 adattár példahívásait.

Futtassa a következő shell parancsot a lerakat megbízhatóságához.

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/xUbuntu_16.10/Release.key -O - | sudo apt-key add -
```

Futtassa a következő shell parancsokat az adattár hozzáadásához és a QOwnNotes onnan való telepítéséhez.

```bash
sudo su -
sh -c "echo 'deb http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/xUbuntu_16.10/ /' >> /etc/apt/sources.list.d/qownnotes.list"
apt-get update
apt-get install qownnotes
```

[Közvetlen letöltés](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/xUbuntu_16.10)
