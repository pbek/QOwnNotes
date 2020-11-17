# Installieren Sie unter Slackware Linux

## Slackware 14.2

Zuerst müssen Sie Qt 5, libxkbcommon, libproxy und js185 installieren.

```bash
cd /tmp
wget "http://bear.alienbase.nl/mirrors/people/alien/sbrepos/current/x86_64/qt5/qt5-5.6.1_1-x86_64-1alien.txz"
wget http://bear.alienbase.nl/mirrors/people/alien/sbrepos/current/x86_64/libxkbcommon/libxkbcommon-0.5.0-x86_64-1alien.txz
sudo installpkg libxkbcommon-0.5.0-x86_64-1alien.txz qt5-5.6.1_1-x86_64-1alien.txz
slackpkg update
slackpkg install libproxy js185-1.0.0-x86_64-1
```

Anschließend können Sie das QOwnNotes-Slackbuilds-Repository auschecken und die Anwendung erstellen.

```bash
cd /tmp
git clone https://github.com/pbek/qownnotes-slackbuilds.git
cd qownnotes-slackbuilds/14.2/qownnotes
./dobuild.sh
```

Danach können Sie das generierte Paket mit `installpkg` installieren.

## Slackware 14.1

Zuerst müssen Sie Qt 5 installieren.

```bash
cd /tmp wget "http://bear.alienbase.nl/mirrors/people/alien/sbrepos/14.1/x86_64/qt5/qt5-5.5.1-x86_64-4alien.txz"
sudo installpkg qt5-5.5.1-x86_64-4alien.txz
```

Anschließend können Sie das QOwnNotes-Slackbuilds-Repository auschecken und die Anwendung erstellen.

```bash
cd /tmp
git clone https://github.com/pbek/qownnotes-slackbuilds.git
cd qownnotes-slackbuilds/14.1/qownnotes
./dobuild.sh
```

Danach können Sie das generierte Paket mit `installpkg` installieren.

[QOwnNotes Slackbuild auf GitHub](https://github.com/pbek/qownnotes-slackbuilds/)
