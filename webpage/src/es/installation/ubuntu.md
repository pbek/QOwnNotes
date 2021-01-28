# Install on Ubuntu Linux, elementary OS and Linux Mint

## Ubuntu Linux 18.04 o más reciente, sistema operativo elemental, Linux Mint 18 o más reciente

Instale **QOwnNotes** en **Ubuntu Linux** (mínimo 18.04) usando el repositorio PPA.

Tenga en cuenta que también hay paquetes para ARM en el repositorio, en caso de que desee utilizar QOwnNotes en una **Raspberry Pi**.

Abra una terminal e ingrese las siguientes líneas para agregar el repositorio e instalar QOwnNotes.

```bash
sudo add-apt-repository ppa:pbek/qownnotes
sudo apt-get update
sudo apt-get install qownnotes
```

[Descarga Directa](https://launchpad.net/~pbek/+archive/ubuntu/qownnotes/+packages)

## Derivados y distribuciones de Ubuntu Linux más antiguas

Lo mejor es intentar usar AppImage de [QOwnNotes Releases en GitHub](https://github.com/pbek/QOwnNotes/releases).

Luego puede cambiar los permisos de ejecución en el archivo:

```bash
chmod a+x QOwnNotes-*.AppImage
```

Después, debería poder ejecutar AppImage para ejecutar QOwnNotes.

## OBS Repository

También puede usar QOwnNotes en versiones de Ubuntu que no están más actualizadas en Ubuntu Launchpad, luego puede usar los repositorios del [Open Build Service](https://build.opensuse.org/package/show/home:pbek:QOwnNotes/desktop).

A continuación se muestran ejemplos de llamadas para el repositorio xUbuntu 16.10.

Ejecute el siguiente comando de shell para confiar en el repositorio.

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/xUbuntu_16.10/Release.key -O - | sudo apt-key add -
```

Ejecute los siguientes comandos de shell para agregar el repositorio e instalar QOwnNotes desde allí.

```bash
sudo su -
sh -c "echo 'deb http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/xUbuntu_16.10/ /' >> /etc/apt/sources.list.d/qownnotes.list"
apt-get update
apt-get install qownnotes
```

[Descarga Directa](https://build.opensuse.org/package/binaries/home:pbek:QOwnNotes/desktop/xUbuntu_16.10)
