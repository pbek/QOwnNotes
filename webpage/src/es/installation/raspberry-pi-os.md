# Instalar en el sistema operativo Raspberry Pi

## Raspberry Pi OS 10

Ejecute los siguientes comandos de shell para confiar en el repositorio.

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_10/Release.key -O - | sudo apt-key add -
```

Ejecute los siguientes comandos de shell para agregar el repositorio e instalar QOwnNotes desde allí.

```bash
sudo bash -c "echo 'deb http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_10/ /' >> /etc/apt/sources.list.d/qownnotes.list"
sudo apt-get update
sudo apt-get install qownnotes
```

[Descarga Directa](https://build.opensuse.org/package/binaries/home:pbek:QOwnNotes/desktop/Raspbian_10)

## Raspbian 9.0

Ejecute los siguientes comandos de shell para confiar en el repositorio.

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_9.0/Release.key
-O - | sudo apt-key add -
```

Ejecute los siguientes comandos de shell para agregar el repositorio e instalar QOwnNotes desde allí.

```bash
sudo bash -c "echo 'deb http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_9.0/ /' >> /etc/apt/sources.list.d/qownnotes.list"
sudo apt-get update
sudo apt-get install qownnotes
```

[Descarga Directa](https://build.opensuse.org/package/binaries/home:pbek:QOwnNotes/desktop/Raspbian_9.0)
