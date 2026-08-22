# Instalar en Raspberry Pi OS

## Raspberry Pi OS 12

Ejecute los comandos de consola siguientes para marcar el repositorio como de fiar.

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_12/Release.key -O - | sudo apt-key add -
```

Ejecute los comandos de consola siguientes para añadir el repositorio e instalar QOwnNotes desde allí.

```bash
sudo bash -c "echo 'deb http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_12/ /' >> /etc/apt/sources.list.d/qownnotes.list"
sudo apt-get update
sudo apt-get install qownnotes
```

[Descarga directa](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_12)

## Raspberry Pi OS 11

Ejecute los comandos de consola siguientes para marcar el repositorio como de fiar.

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_11/Release.key -O - | sudo apt-key add -
```

Ejecute los comandos de consola siguientes para añadir el repositorio e instalar QOwnNotes desde allí.

```bash
sudo bash -c "echo 'deb http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_11/ /' >> /etc/apt/sources.list.d/qownnotes.list"
sudo apt-get update
sudo apt-get install qownnotes
```

[Descarga directa](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_11)

## Raspberry Pi OS 10

Ejecute los comandos de consola siguientes para marcar el repositorio como de fiar.

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_10/Release.key -O - | sudo apt-key add -
```

Ejecute los comandos de consola siguientes para añadir el repositorio e instalar QOwnNotes desde allí.

```bash
sudo bash -c "echo 'deb http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_10/ /' >> /etc/apt/sources.list.d/qownnotes.list"
sudo apt-get update
sudo apt-get install qownnotes
```

[Descarga directa](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_10)

## Raspbian 9.0

Ejecute los comandos de consola siguientes para marcar el repositorio como de fiar.

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_9.0/Release.key -O - | sudo apt-key add -
```

Ejecute los comandos de consola siguientes para añadir el repositorio e instalar QOwnNotes desde allí.

```bash
sudo bash -c "echo 'deb http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_9.0/ /' >> /etc/apt/sources.list.d/qownnotes.list"
sudo apt-get update
sudo apt-get install qownnotes
```

[Descarga directa](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_9.0)

::: tip
Si QOwnNotes registra el mensaje `Could not write secret to keychain`, instale los paquetes de Secret Service que falten y reinicie su sesión de escritorio.

Para GNOME y otros ordenadores de escritorio basados en Servicio Secreto:

```bash
sudo apt install gnome-keyring libsecret-1-0 seahorse
```

Para KDE Plasma:

```bash
sudo apt install kwalletmanager
```

QOwnNotes recurrirá al cifrado heredado si el llavero del escritorio no está disponible.
:::
