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

## Windows XP

Qt dropped support for Windows XP with version 5.8, but QOwnNotes is now also built
with Qt 5.7 to allow Windows XP users to still use it.

You need to download the ZIP file from [AppVeyor](https://ci.appveyor.com/project/pbek/qownnotes/build/artifacts)
yourself and unzip it to a folder of your liking.

You can then directly run `QOwnNotes.exe` from that folder, no installation is required.

::: tip Info
The automatic update mechanism does not work with the AppVeyor build for Windows XP!
You will have to download new releases yourself.
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
