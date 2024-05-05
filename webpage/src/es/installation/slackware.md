# Instalar en Slackware Linux

## Slackware 15.0 (current 20240505)

Las dependencias indicadas en [qownnotes.info](https://github.com/pbek/QOwnNotes/blob/main/build-systems/slackware/qownnotes.info) ya están en "current". Si al clonar no hay un directorio específico para la 15, se puede emplear el script de la versión 14.2:

```bash
cd /tmp
git clone https://github.com/pbek/qownnotes-slackbuilds.git
cd qownnotes-slackbuilds/14.2/qownnotes
./dobuild.sh
```

## Slackware 14.2

Primero debe instalar Qt 5, libxkbcommon, libproxy y js185.

```bash
cd /tmp
wget "http://bear.alienbase.nl/mirrors/people/alien/sbrepos/current/x86_64/qt5/qt5-5.6.1_1-x86_64-1alien.txz"
wget http://bear.alienbase.nl/mirrors/people/alien/sbrepos/current/x86_64/libxkbcommon/libxkbcommon-0.5.0-x86_64-1alien.txz
sudo installpkg libxkbcommon-0.5.0-x86_64-1alien.txz qt5-5.6.1_1-x86_64-1alien.txz
slackpkg update
slackpkg install libproxy js185-1.0.0-x86_64-1
```

Luego, puede consultar el repositorio de slackbuilds de QOwnNotes y crear la aplicación.

```bash
cd /tmp
git clone https://github.com/pbek/qownnotes-slackbuilds.git
cd qownnotes-slackbuilds/14.2/qownnotes
./dobuild.sh
```

Después de eso, puede instalar el paquete generado con `installpkg`.

## Slackware 14.1

Primero necesitas instalar Qt 5.

```bash
cd /tmp wget "http://bear.alienbase.nl/mirrors/people/alien/sbrepos/14.1/x86_64/qt5/qt5-5.5.1-x86_64-4alien.txz"
sudo installpkg qt5-5.5.1-x86_64-4alien.txz
```

Luego, puede consultar el repositorio de slackbuilds de QOwnNotes y crear la aplicación.

```bash
cd /tmp
git clone https://github.com/pbek/qownnotes-slackbuilds.git
cd qownnotes-slackbuilds/14.1/qownnotes
./dobuild.sh
```

Después de eso, puede instalar el paquete generado con `installpkg`.

[QOwnNotes Slackbuild on GitHub](https://github.com/pbek/qownnotes-slackbuilds/)
