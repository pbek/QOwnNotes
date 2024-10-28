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

Si desea poner a prueba los paquetes con Qt6 desde ahora y se encuentra en Ubuntu 22.04 o más reciente, puede utilizar este repositorio:

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

Si encuentra cualquier problema, ¡por favor repórtelos en los [problemas de QOwnNotes en GItHub](https://github.com/pbek/QOwnNotes/issues)!

## Distribuciones y derivados más antiguos de Ubuntu Linux

Lo mejor es usar la AppImage de los [lanzamientos de QOwnNotes en GitHub](https://github.com/pbek/QOwnNotes/releases).

Tras hacer esto, puede cambiar los permisos de ejecución en el archivo:

```bash
chmod a+x QOwnNotes-*.AppImage
```

Posteriormente, debería ser capaz de ejecutar la AppImage para abrir QOwnNotes.

## Repositorio de OBS

También puede usar QOwnNotes en versiones de Ubuntu que ya no se actualizan más en Ubuntu Launchpad. Luego puede usar los repositorios de [Open Build Service](https://build.opensuse.org/package/show/home:pbek:QOwnNotes/desktop).

A continuación se muestran llamadas de ejemplo para el repositorio de xUbuntu 20.04.

Ejecute la orden de consola siguiente para marcar el repositorio como de fiar.

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/xUbuntu_20.04/Release.key -O - | sudo apt-key add -
```

Ejecute las órdenes de consola siguientes para añadir el repositorio e instalar QOwnNotes desde allí.

```bash
sudo su -
sh -c "echo 'deb http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/xUbuntu_20.04/ /' >> /etc/apt/sources.list.d/qownnotes.list"
apt-get update
apt-get install qownnotes
```

[Descarga directa](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/xUbuntu_20.04)
