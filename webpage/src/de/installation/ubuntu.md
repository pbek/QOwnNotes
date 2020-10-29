# Install on Ubuntu Linux, elementary OS and Linux Mint

## Ubuntu Linux 18.04 or newer, elementary OS, Linux Mint 18 or newer

Install **QOwnNotes** on **Ubuntu Linux** (minimum 18.04) using the PPA repository.

Note that there are also packages for ARM in the repository, in case you want to use QOwnNotes on a **Raspberry Pi**.

Open a terminal and enter the following lines to add the repository and install QOwnNotes.

```bash
sudo add-apt-repository ppa:pbek/qownnotes
sudo apt-get update
sudo apt-get install qownnotes
```

[Direct Download](https://launchpad.net/~pbek/+archive/ubuntu/qownnotes/+packages)

## Older Ubuntu Linux distributions and derivatives

Best try to use the AppImage from [QOwnNotes Releases on GitHub](https://github.com/pbek/QOwnNotes/releases).

Then you can change the execute-permissions on the file:

```bash
chmod a+x QOwnNotes-*.AppImage
```

Afterwards you should be able to execute the AppImage to run QOwnNotes.

## OBS Repository

You may also be able to use the QOwnNotes on Ubuntu versions that aren't any more updated on Ubuntu Launchpad then you can
use the repositories from the [Open Build Service](https://build.opensuse.org/package/show/home:pbek:QOwnNotes/desktop).

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

[Direct Download](https://build.opensuse.org/package/binaries/home:pbek:QOwnNotes/desktop/xUbuntu_16.10)
