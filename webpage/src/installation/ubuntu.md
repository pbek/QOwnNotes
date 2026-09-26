# Install on Ubuntu Linux, elementary OS and Linux Mint

## Ubuntu Linux 24.04 or newer, elementary OS, Linux Mint 22.1 or newer

Install **QOwnNotes** on **Ubuntu Linux** (minimum 24.04) using the PPA repository.
This version of QOwnNotes is built with Qt6.

Note that there are also packages for ARM in the repository, in case you want to use QOwnNotes on a **Raspberry Pi**.

Open a terminal and enter the following lines to add the repository and install QOwnNotes.

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

If you find any issues, please report them on [QOwnNotes Issues on GitHub](https://github.com/pbek/QOwnNotes/issues)!

## QOwnNotes Qt5 legacy

The Qt5 PPA is no longer updated. Existing packages remain available for older
Ubuntu Linux, elementary OS and Linux Mint installations that cannot use the Qt6
PPA.

Install the final Qt5 package on **Ubuntu Linux** (minimum 20.04) using the PPA
repository.

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

See [Install as AppImage](./appimage.md) for more information.

## OBS Repository

The [Open Build Service](https://build.opensuse.org/package/show/home:pbek:QOwnNotes/desktop-cmake3)
also provides QOwnNotes packages built with CMake and Qt6 for Ubuntu 24.04 and newer.
The older Qt5 OBS package is no longer updated.

Below are example calls for the xUbuntu 24.04 repository.

Run the following shell command to trust the repository.

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/xUbuntu_24.04/Release.key -O - | sudo apt-key add -
```

Run the following shell commands to add the repository and install QOwnNotes from there.

```bash
sudo su -
sh -c "echo 'deb http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/xUbuntu_24.04/ /' >> /etc/apt/sources.list.d/qownnotes.list"
apt-get update
apt-get install qownnotes
```

[Direct Download](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/xUbuntu_24.04)

::: tip
If QOwnNotes logs `Could not write secret to keychain`, install the missing Secret Service packages and restart your desktop session.

For GNOME and other Secret Service based desktops:

```bash
sudo apt install gnome-keyring libsecret-1-0 seahorse
```

For KDE Plasma:

```bash
sudo apt install kwalletmanager
```

QOwnNotes will fall back to legacy encryption if the desktop keychain is unavailable.
:::
