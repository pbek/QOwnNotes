# Instalar en Ubuntu Linux, elementary OS y Linux Mint

## Ubuntu Linux 24.04 o más reciente, elementary OS, Linux Mint 22.1 o más reciente

Instale **QOwnNotes** en **Ubuntu Linux** (mínimo 24.04) usando el repositorio PPA. Esta versión de QOwnNotes está construida con Qt6.

Tenga en cuenta que también hay paquetes para ARM en el repositorio, en caso de que desee utilizar QOwnNotes en una **Raspberry Pi**.

Abra una terminal e ingrese las siguientes líneas para agregar el repositorio e instalar QOwnNotes.

```bash
# Eliminar el PPA de Qt5 en caso de tenerlo instalado
sudo add-apt-repository --remove ppa:pbek/qownnotes

# Añadir el PPA de Qt6
sudo add-apt-repository ppa:pbek/qownnotes-qt6

# Instalar QOwnNotes
sudo apt-get update
sudo apt-get install qownnotes
```

[Descarga directa](https://launchpad.net/~pbek/+archive/ubuntu/qownnotes-qt6/+packages)

Si encuentra algún problema, ¡por favor repórtelo en la página de [incidencias de QOwnNotes en GitHub](https://github.com/pbek/QOwnNotes/issues)!

## QOwnNotes Qt5 Legado

Para versiones anteriores de Ubuntu Linux, como 18.04 o más reciente, elementary OS, o Linux Mint 18 o más reciente, puede usar la versión QOwnNotes con Qt5.

Instale **QOwnNotes** en **Ubuntu Linux** (mínimo 18.04) usando el repositorio PPA.

Tenga en cuenta que también hay paquetes para ARM en el repositorio, en caso de que desee utilizar QOwnNotes en una **Raspberry Pi**.

Abra una terminal e ingrese las siguientes líneas para agregar el repositorio e instalar QOwnNotes.

```bash
sudo add-apt-repository ppa:pbek/qownnotes
sudo apt-get update
sudo apt-get install qownnotes
```

[Descarga directa](https://launchpad.net/~pbek/+archive/ubuntu/qownnotes/+packages)

## Distribuciones y derivados más antiguos de Ubuntu Linux

Lo mejor es intentar usar la AppImage de los [lanzamientos de QOwnNotes en GitHub](https://github.com/pbek/QOwnNotes/releases).

Véase [Instalar como AppImage](./appimage.md) para obtener más información.

## Repositorio de OBS

También puede usar QOwnNotes en versiones de Ubuntu que ya no se actualizan más en Ubuntu Launchpad. Puede usar los repositorios de [Open Build Service](https://build.opensuse.org/package/show/home:pbek:QOwnNotes/desktop). Los paquetes para Ubuntu 24.04+ están construidos con cmake y Qt6.

A continuación se muestran llamadas de ejemplo para el repositorio de xUbuntu 24.04.

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
