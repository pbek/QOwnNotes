# نصب در سیستم عامل رزبری پای

## Raspberry Pi OS 12

فرمان های پوسته زیر را برای اطمینان از مخزن اجرا کنید.

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_12/Release.key -O - | sudo apt-key add -
```

فرمان های پوسته زیر را برای افزودن مخزن اجرا کرده و QOwnNotes را از آنجا نصب کنید.

```bash
sudo bash -c "echo 'deb http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_12/ /' >> /etc/apt/sources.list.d/qownnotes.list"
sudo apt-get update
sudo apt-get install qownnotes
```

[بارگیری مستقیم](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_12)

## سیستم عامل رزبری پای 11

فرمان های پوسته زیر را برای اطمینان از مخزن اجرا کنید.

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_10/Release.key -O - | sudo apt-key add -
```

فرمان های پوسته زیر را برای افزودن مخزن اجرا کرده و QOwnNotes را از آنجا نصب کنید.

```bash
sudo bash -c "echo 'deb http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_10/ /' >> /etc/apt/sources.list.d/qownnotes.list"
sudo apt-get update
sudo apt-get install qownnotes
```

[بارگیری مستقیم](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_11)

## سیستم عامل رزبری پای 10

فرمان های پوسته ای زیر را برای اطمینان از مخزن اجرا کنید.

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_10/Release.key -O - | sudo apt-key add -
```

فرمان های پوسته ای زیر را برای افزودن مخزن اجرا کرده و QOwnNotes را از آنجا نصب کنید.

```bash
sudo bash -c "echo 'deb http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_10/ /' >> /etc/apt/sources.list.d/qownnotes.list"
sudo apt-get update
sudo apt-get install qownnotes
```

[دانلود مستقیم](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_11)

## رزبین 9.0

Run the following shell commands to trust the repository.

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_9.0/Release.key
-O - | sudo apt-key add -
```

Run the following shell commands to add the repository and install QOwnNotes from there.

```bash
sudo bash -c "echo 'deb http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_9.0/ /' >> /etc/apt/sources.list.d/qownnotes.list"
sudo apt-get update
sudo apt-get install qownnotes
```

[Direct Download](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Raspbian_11)

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
