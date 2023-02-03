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

## QOwnNotes Qt6 Preview

If you already want to try the Qt6 packages and are on Ubuntu 22.04 or newer you can use this repository:

```bash
# Remove Qt5 PPA in case you had it installed
sudo add-apt-repository --remove ppa:pbek/qownnotes

# Add Qt6 PPA
sudo add-apt-repository ppa:pbek/qownnotes-qt6

# Install QOwnNotes
sudo apt-get update
sudo apt-get install qownnotes
```

[Direct Download](https://launchpad.net/~pbek/+archive/ubuntu/qownnotes-qt6/+packages)

## Older Ubuntu Linux distributions and derivatives

Best try to use the AppImage from [QOwnNotes Releases on GitHub](https://github.com/pbek/QOwnNotes/releases).

Then you can change the execute-permissions on the file:

```bash
chmod a+x QOwnNotes-*.AppImage
```

Afterwards you should be able to execute the AppImage to run QOwnNotes.

## OBS Repository

You may also be able to use the QOwnNotes on Ubuntu versions that aren't any more updated on Ubuntu Launchpad then you can use the repositories from the [Open Build Service](https://build.opensuse.org/package/show/home:pbek:QOwnNotes/desktop).

Below are example calls for the xUbuntu 16.10 repository.

Run the following shell command to trust the repository.

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/xUbuntu_16.10/Release.key -O - | sudo apt-key add -
```

Run the following shell commands to add the repository and install QOwnNotes from there.

```bash
sudo su -
sh -c "echo 'deb http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/xUbuntu_16.10/ /' >> /etc/apt/sources.list.d/qownnotes.list"
apt-get update
apt-get install qownnotes
```

[Direct Download](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/xUbuntu_16.10)
