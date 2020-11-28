# Installeer op Slackware Linux

## Slackware 14.2

Eerst moet u Qt 5, libxkbcommon, libproxy en js185 installeren.

```bash
cd /tmp
wget "http://bear.alienbase.nl/mirrors/people/alien/sbrepos/current/x86_64/qt5/qt5-5.6.1_1-x86_64-1alien.txz"
wget http://bear.alienbase.nl/mirrors/people/alien/sbrepos/current/x86_64/libxkbcommon/libxkbcommon-0.5.0-x86_64-1alien.txz
sudo installpkg libxkbcommon-0.5.0-x86_64-1alien.txz qt5-5.6.1_1-x86_64-1alien.txz
slackpkg update
slackpkg install libproxy js185-1.0.0-x86_64-1
```

Vervolgens kunt u de QOwnNotes slackbuilds repository uitchecken en de applicatie bouwen.

```bash
cd /tmp
git clone https://github.com/pbek/qownnotes-slackbuilds.git
cd qownnotes-slackbuilds/14.2/qownnotes
./dobuild.sh
```

Daarna kunt u het gegenereerde pakket installeren met `installpkg`.

## Slackware 14.1

Eerst moet u Qt 5 installeren.

```bash
cd /tmp wget "http://bear.alienbase.nl/mirrors/people/alien/sbrepos/14.1/x86_64/qt5/qt5-5.5.1-x86_64-4alien.txz"
sudo installpkg qt5-5.5.1-x86_64-4alien.txz
```

Vervolgens kunt u de QOwnNotes slackbuilds repository uitchecken en de applicatie bouwen.

```bash
cd /tmp
git clone https://github.com/pbek/qownnotes-slackbuilds.git
cd qownnotes-slackbuilds/14.1/qownnotes
./dobuild.sh
```

Daarna kunt u het gegenereerde pakket installeren met `installpkg`.

[QOwnNotes Slackbuild op GitHub](https://github.com/pbek/qownnotes-slackbuilds/)
