# Installa su Slackware Linux

## Slackware 14.2

Per prima cosa devi installare Qt 5, libxkbcommon, libproxy e js185.

```bash
cd /tmp
wget "http://bear.alienbase.nl/mirrors/people/alien/sbrepos/current/x86_64/qt5/qt5-5.6.1_1-x86_64-1alien.txz"
wget http://bear.alienbase.nl/mirrors/people/alien/sbrepos/current/x86_64/libxkbcommon/libxkbcommon-0.5.0-x86_64-1alien.txz
sudo installpkg libxkbcommon-0.5.0-x86_64-1alien.txz qt5-5.6.1_1-x86_64-1alien.txz
slackpkg update
slackpkg install libproxy js185-1.0.0-x86_64-1
```

Quindi puoi recuperare il repository slackbuilds di QOwnNotes e creare l'applicazione.

```bash
cd /tmp
git clone https://github.com/pbek/qownnotes-slackbuilds.git
cd qownnotes-slackbuilds/14.2/qownnotes
./dobuild.sh
```

Dopodiché puoi installare il pacchetto generato con `installpkg`.

## Slackware 14.1

Per prima cosa devi installare Qt 5.

```bash
cd /tmp wget "http://bear.alienbase.nl/mirrors/people/alien/sbrepos/14.1/x86_64/qt5/qt5-5.5.1-x86_64-4alien.txz"
sudo installpkg qt5-5.5.1-x86_64-4alien.txz
```

Quindi puoi recuperare il repository slackbuilds di QOwnNotes e creare l'applicazione.

```bash
cd /tmp
git clone https://github.com/pbek/qownnotes-slackbuilds.git
cd qownnotes-slackbuilds/14.1/qownnotes
./dobuild.sh
```

Dopodiché puoi installare il pacchetto generato con `installpkg`.

[QOwnNotes Slackbuild su GitHub](https://github.com/pbek/qownnotes-slackbuilds/)
