# Instalar en el sistema operativo Raspberry Pi

## Raspberry Pi OS 12

Ejecute los siguientes comandos de shell para confiar en el repositorio.

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_12/Release.key -O - | sudo apt-key add -
```

Ejecute los siguientes comandos de shell para agregar el repositorio e instalar QOwnNotes desde allí.

```bash
sudo bash -c "echo 'deb http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_12/ /' >> /etc/apt/sources.list.d/qownnotes.list"
sudo apt-get update
sudo apt-get install qownnotes
```

[Descarga Directa](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_12)

## Raspberry Pi OS 11

Ejecute los siguientes comandos de shell para confiar en el repositorio.

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_11/Release.key -O - | sudo apt-key add -
```

Ejecute los siguientes comandos de shell para agregar el repositorio e instalar QOwnNotes desde allí.

```bash
sudo bash -c "echo 'deb http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_11/ /' >> /etc/apt/sources.list.d/qownnotes.list"
sudo apt-get update
sudo apt-get install qownnotes
```

[Descarga Directa](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_11)

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

[Descarga directa](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_10)

## Raspbian 9.0

Ejecute las órdenes de consola siguientes para marcar el repositorio como de fiar.

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_9.0/Release.key
-O - | sudo apt-key add -
```

Ejecute las órdenes de consola siguientes para añadir el repositorio e instalar QOwnNotes desde allí.

```bash
sudo bash -c "echo 'deb http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_9.0/ /' >> /etc/apt/sources.list.d/qownnotes.list"
sudo apt-get update
sudo apt-get install qownnotes
```

[Descarga directa](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_9.0)
