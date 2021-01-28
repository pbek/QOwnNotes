# Installer sur Slackware Linux

## Slackware 14.2

Vous devez d'abord installer Qt 5, libxkbcommon, libproxy et js185.

```bash
cd /tmp
wget "http://bear.alienbase.nl/mirrors/people/alien/sbrepos/current/x86_64/qt5/qt5-5.6.1_1-x86_64-1alien.txz"
wget http://bear.alienbase.nl/mirrors/people/alien/sbrepos/current/x86_64/libxkbcommon/libxkbcommon-0.5.0-x86_64-1alien.txz
sudo installpkg libxkbcommon-0.5.0-x86_64-1alien.txz qt5-5.6.1_1-x86_64-1alien.txz
slackpkg update
slackpkg install libproxy js185-1.0.0-x86_64-1
```

Ensuite, vous pouvez extraire le référentiel slackbuilds de QOwnNotes et créer l'application.

```bash
cd /tmp
git clone https://github.com/pbek/qownnotes-slackbuilds.git
cd qownnotes-slackbuilds/14.2/qownnotes
./dobuild.sh
```

Après cela, vous pouvez installer le package généré avec `installpkg`.

## Slackware 14.1

Vous devez d'abord installer Qt 5.

```bash
cd /tmp wget "http://bear.alienbase.nl/mirrors/people/alien/sbrepos/14.1/x86_64/qt5/qt5-5.5.1-x86_64-4alien.txz"
sudo installpkg qt5-5.5.1-x86_64-4alien.txz
```

Ensuite, vous pouvez extraire le référentiel slackbuilds de QOwnNotes et créer l'application.

```bash
cd /tmp
git clone https://github.com/pbek/qownnotes-slackbuilds.git
cd qownnotes-slackbuilds/14.1/qownnotes
./dobuild.sh
```

Après cela, vous pouvez installer le package généré avec `installpkg`.

[QOwnNotes Slackbuild on GitHub](https://github.com/pbek/qownnotes-slackbuilds/)
