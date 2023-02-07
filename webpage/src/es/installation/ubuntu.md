# Instalar en Ubuntu Linux, sistema operativo elemental y Linux Mint

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

## Distribuciones y derivados más antiguos de Ubuntu Linux

Es recomendable probar el AppImage de las [versiones de QOwnNotes en GitHub](https://github.com/pbek/QOwnNotes/releases).

Luego puede cambiar los permisos de ejecución del archivo:

```bash
chmod a+x QOwnNotes-*.AppImage
```

Tras hacer esto, podrá ejecutar el AppImage para usar QOwnNotes.

## Repositorio de OBS

También puede usar QOwnNotes en versiones de Ubuntu que ya no se actualizan en Launchpad de Ubuntu por medio de los repositorios del [Open Build Service](https://build.opensuse.org/package/show/home:pbek:QOwnNotes/desktop).

A continuación se muestran ejemplos de llamadas para el repositorio de xUbuntu 16.10.

Ejecute la orden de consola siguiente para indicar que el repositorio es de fiar.

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/xUbuntu_16.10/Release.key -O - | sudo apt-key add -
```

Ejecute las órdenes de consola siguientes para añadir el repositorio e instalar QOwnNotes desde allí.

```bash
sudo su -
sh -c "echo 'deb http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/xUbuntu_16.10/ /' >> /etc/apt/sources.list.d/qownnotes.list"
apt-get update
apt-get install qownnotes
```

[Descarga directa](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/xUbuntu_16.10)
