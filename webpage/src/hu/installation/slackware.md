# Telepítse a Slackware Linux rendszerre

## Slackware 14.2

Először telepítenie kell a Qt 5, libxkbcommon, libproxy és js185 fájlokat.

```bash
cd /tmp
wget "http://bear.alienbase.nl/mirrors/people/alien/sbrepos/current/x86_64/qt5/qt5-5.6.1_1-x86_64-1alien.txz"
wget http://bear.alienbase.nl/mirrors/people/alien/sbrepos/current/x86_64/libxkbcommon/libxkbcommon-0.5.0-x86_64-1alien.txz
sudo installpkg libxkbcommon-0.5.0-x86_64-1alien.txz qt5-5.6.1_1-x86_64-1alien.txz
slackpkg update
slackpkg install libproxy js185-1.0.0-x86_64-1
```

Ezután kiválaszthatja a QOwnNotes slackbuilds adattárat, és elkészítheti az alkalmazást.

```bash
cd /tmp
git clone https://github.com/pbek/qownnotes-slackbuilds.git
cd qownnotes-slackbuilds/14.2/qownnotes
./dobuild.sh
```

Ezt követően telepítheti a létrehozott csomagot a `installpkg` paranccsal.

## Slackware 14.1

Először telepítenie kell a Qt 5-et.

```bash
cd /tmp wget "http://bear.alienbase.nl/mirrors/people/alien/sbrepos/14.1/x86_64/qt5/qt5-5.5.1-x86_64-4alien.txz"
sudo installpkg qt5-5.5.1-x86_64-4alien.txz
```

Ezután kiválaszthatja a QOwnNotes slackbuilds adattárat, és elkészítheti az alkalmazást.

```bash
cd /tmp
git clone https://github.com/pbek/qownnotes-slackbuilds.git
cd qownnotes-slackbuilds/14.1/qownnotes
./dobuild.sh
```

Ezt követően telepítheti a létrehozott csomagot a `installpkg` paranccsal.

[QOwnNotes Slackbuild on GitHub](https://github.com/pbek/qownnotes-slackbuilds/)
