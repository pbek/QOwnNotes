# التثبيت على أوبنتو وإلمنتري ولينكس منت

## Ubuntu Linux 24.04 or newer, elementary OS, Linux Mint 22.1 or newer

Install **QOwnNotes** on **Ubuntu Linux** (minimum 24.04) using the PPA repository. This version of QOwnNotes is built with Qt6.

لاحظ أن المستودع به أيضا حزم لمعمارية ARM، إذا أردت استخدام QOwnNotes على **Raspberry Pi**.

افتح طرفية، وأدخل السطور التالية فيها لإضافة المستودع وتثبيت QOwnNotes.

```bash
# Remove Qt5 PPA in case you had it installed
sudo add-apt-repository --remove ppa:pbek/qownnotes

# Add Qt6 PPA
sudo add-apt-repository ppa:pbek/qownnotes-qt6

# Install QOwnNotes
sudo apt-get update
sudo apt-get install qownnotes
```

[تنزيل مباشر](https://launchpad.net/~pbek/+archive/ubuntu/qownnotes-qt6/+packages)

If you find any issues, please report them on [QOwnNotes Issues on GitHub](https://github.com/pbek/QOwnNotes/issues)!

## QOwnNotes Qt5 legacy

For older versions of Ubuntu Linux, like 18.04 or newer, elementary OS, Linux Mint 18 or newer, you can use the Qt5 version of QOwnNotes.

Install **QOwnNotes** on **Ubuntu Linux** (minimum 18.04) using the PPA repository.

Note that there are also packages for ARM in the repository, in case you want to use QOwnNotes on a **Raspberry Pi**.

Open a terminal and enter the following lines to add the repository and install QOwnNotes.

```bash
sudo add-apt-repository ppa:pbek/qownnotes
sudo apt-get update
sudo apt-get install qownnotes
```

[Direct Download](https://launchpad.net/~pbek/+archive/ubuntu/qownnotes/+packages)

## النسخ الأقدم من أوبنتو ومشتقاتها

Best try to use the AppImage from [QOwnNotes Releases on GitHub](https://github.com/pbek/QOwnNotes/releases).

See [Install as AppImage](./appimage.md) for more information.

## مستودع OBS

You may also be able to use the QOwnNotes on Ubuntu versions that aren't anymore updated on Ubuntu Launchpad then you can use the repositories from the [Open Build Service](https://build.opensuse.org/package/show/home:pbek:QOwnNotes/desktop). The packages for Ubuntu 24.04+ are built with cmake and Qt6.

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
