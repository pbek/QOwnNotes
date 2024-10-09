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

[Descarga Directa](https://launchpad.net/~pbek/+archive/ubuntu/qownnotes/+packages)

## Disponibilidad preliminar de QOwnNotes Qt6

Si quiere poner a prueba los paquetes Qt6 desde ahora y utiliza Ubuntu 22.04 o más reciente, utilice este repositorio:

```bash
# Quite el PPA de Qt5 si lo ha instalado
sudo add-apt-repository --remove ppa:pbek/qownnotes

# Añada el PPA de Qt6
sudo add-apt-repository ppa:pbek/qownnotes-qt6

# Instale QOwnNotes
sudo apt-get update
sudo apt-get install qownnotes
```

[Descarga directa](https://launchpad.net/~pbek/+archive/ubuntu/qownnotes-qt6/+packages)

Si encuentra cualquier problema, ¡por favor repórtelos en los [problemas de QOwnNotes en GItHub](https://github.com/pbek/QOwnNotes/issues)!

## Distribuciones y derivados más antiguos de Ubuntu Linux

Lo mejor es usar el AppImage de los [lanzamientos de QOwnNotes en GitHub](https://github.com/pbek/QOwnNotes/releases).

Luego puede cambiar los permisos de ejecución en el archivo:

```bash
chmod a+x QOwnNotes-*.AppImage
```

Tras hacer esto, debería ser capaz de ejecutar el AppImage para ejecutar QOwnNotes.

## Repositorio de OBS

You may also be able to use the QOwnNotes on Ubuntu versions that aren't anymore updated on Ubuntu Launchpad then you can use the repositories from the [Open Build Service](https://build.opensuse.org/package/show/home:pbek:QOwnNotes/desktop).

A continuación se muestran ejemplos de llamadas para el repositorio de xUbuntu 20.04.

Run the following shell command to trust the repository.

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/xUbuntu_20.04/Release.key -O - | sudo apt-key add -
```

Run the following shell commands to add the repository and install QOwnNotes from there.

```bash
sudo su -
sh -c "echo 'deb http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/xUbuntu_20.04/ /' >> /etc/apt/sources.list.d/qownnotes.list"
apt-get update
apt-get install qownnotes
```

[Descarga directa](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/xUbuntu_20.04)
