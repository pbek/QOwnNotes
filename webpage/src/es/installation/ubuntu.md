# Instalar en Ubuntu Linux, elementary OS y Linux Mint

## Ubuntu Linux 18.04 o más reciente, elementary OS, Linux Mint 18 o más reciente

Instale **QOwnNotes** en **Ubuntu Linux** (mínimo 18.04) usando el repositorio PPA.

Tenga en cuenta que también hay paquetes para ARM en el repositorio, en caso de que desee utilizar QOwnNotes en una **Raspberry Pi**.

Abra una terminal e ingrese las siguientes líneas para agregar el repositorio e instalar QOwnNotes.

```bash
sudo add-apt-repository ppa:pbek/qownnotes
sudo apt-get update
sudo apt-get install qownnotes
```

[Descarga directa](https://launchpad.net/~pbek/+archive/ubuntu/qownnotes/+packages)

## Vista previa de QOwnNotes con Qt6

If you already want to try the Qt6 packages, built with cmake and are on Ubuntu 24.04 or newer, you can use this repository:

```bash
# Eliminar el PPA de Qt5 si lo ha instalado
sudo add-apt-repository --remove ppa:pbek/qownnotes

# Añadir el PPA de Qt6
sudo add-apt-repository ppa:pbek/qownnotes-qt6

# Instalar QOwnNotes
sudo apt-get update
sudo apt-get install qownnotes
```

[Descarga directa](https://launchpad.net/~pbek/+archive/ubuntu/qownnotes-qt6/+packages)

If you find any issues, please report them on [QOwnNotes Issues on GitHub](https://github.com/pbek/QOwnNotes/issues)!

## Distribuciones y derivados más antiguos de Ubuntu Linux

Lo mejor es usar la AppImage de los [lanzamientos de QOwnNotes en GitHub](https://github.com/pbek/QOwnNotes/releases).

Tras hacer esto, puede cambiar los permisos de ejecución en el archivo:

```bash
chmod a+x QOwnNotes-*.AppImage
```

Posteriormente, debería ser capaz de ejecutar la AppImage para abrir QOwnNotes.

## Repositorio de OBS

You may also be able to use the QOwnNotes on Ubuntu versions that aren't anymore updated on Ubuntu Launchpad then you can use the repositories from the [Open Build Service](https://build.opensuse.org/package/show/home:pbek:QOwnNotes/desktop). The packages for Ubuntu 24.04+ are built with cmake and Qt6.

Below are example calls for the xUbuntu 24.04 repository.

Ejecute la orden de consola siguiente para marcar el repositorio como de fiar.

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/xUbuntu_24.04/Release.key -O - | sudo apt-key add -
```

Ejecute las órdenes de consola siguientes para añadir el repositorio e instalar QOwnNotes desde allí.

```bash
sudo su -
sh -c "echo 'deb http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/xUbuntu_24.04/ /' >> /etc/apt/sources.list.d/qownnotes.list"
apt-get update
apt-get install qownnotes
```

[Descarga directa](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/xUbuntu_24.04)
