# Install on Microsoft Windows™

Download the latest **Windows archive** from [QOwnNotes Releases on GitHub](https://github.com/pbek/QOwnNotes/releases)
(look for a file called `QOwnNotes.zip`) and unzip it to anywhere you like.
There is no installation needed!

::: warning
If you want to use the **automatic updater** please make sure you unzip it
to a place where your user account has write access to.
By default, your user account most likely **doesn't have write access** to places
like `C:\Program Files (x86)` or `C:\Program Files`.
:::

You can then directly run `QOwnNotes.exe` from your `QOwnNotes` folder, no installation is required.

### Portable mode

Use `QOwnNotesPortable.bat` to run QOwnNotes in **portable mode** where everything (including your notes) will only be
stored in your `QOwnNotes` folder.

::: tip
You don't need the portable mode if you just don't have Administration permissions
to your computer. QOwnNotes doesn't need to be installed!
:::

## Windows 8.1 and older

The Windows release is built with **Qt6** and needs at least **Windows 10**.

The legacy Qt 5.7 build for Windows XP to Windows 8.1 is no longer updated.
The final version is **QOwnNotes 26.10.0**, which you can download as
[QOwnNotes-Qt5.7-legacy.zip](https://github.com/pbek/QOwnNotes/releases/download/v26.10.0/QOwnNotes-Qt5.7-legacy.zip)
([SHA-256 checksum](https://github.com/pbek/QOwnNotes/releases/download/v26.10.0/QOwnNotes-Qt5.7-legacy.zip.sha256sum))
from the [26.10.0 release on GitHub](https://github.com/pbek/QOwnNotes/releases/tag/v26.10.0).

Unzip it to a folder of your liking and run `QOwnNotes.exe` from that folder,
no installation is required.

::: tip Info
The automatic update mechanism does not work with the legacy build!
:::

## Chocolatey

There is a community-maintained package of QOwnNotes at [Chocolatey](https://chocolatey.org/packages/qownnotes/).

You can install it with:

```shell
choco install qownnotes
```

## Scoop

There is a [community-maintained package of QOwnNotes](https://github.com/ScoopInstaller/Extras/blob/master/bucket/qownnotes.json) at
[Scoop](https://scoop.sh/). If you add the Extras bucket, you can use it to install QOwnNotes in portable mode.

```shell
scoop bucket add extras
scoop update
scoop install qownnotes
```

## WinGet

There is a [community-maintained package of QOwnNotes](https://github.com/microsoft/winget-pkgs/tree/master/manifests/p/pbek/QOwnNotes) for
[WinGet](https://github.com/microsoft/winget-cli).

You can install it with:

```shell
winget install qownnotes
```

## MSYS2

There is a [community-maintained package of QOwnNotes](https://packages.msys2.org/base/mingw-w64-qownnotes) for
[MSYS2](hhttps://www.msys2.org/).

You can install it with:

```shell
pacman -S mingw-w64-qownnotes
```
