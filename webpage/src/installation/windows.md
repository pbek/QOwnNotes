# Install on Microsoft Windows™

Download the latest **Windows archive** from [QOwnNotes Releases on GitHub](https://github.com/pbek/QOwnNotes/releases)
and unzip it to anywhere you like. There is no installation needed!

::: warning
If you want to use the **automatic updater** please make sure you unzip it
to a place where your user account has write access to.
By default your user account most likely **doesn't have write access** to places
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

Although you have to seek out the latest master build from [AppVeyor](https://ci.appveyor.com/project/pbek/qownnotes/history)
yourself, choose the *Qt 5.7 environment* and download the *artifact*.

::: tip Info
The automatic update mechanism does not work with the AppVeyor build for Windows XP!
You will have to download new releases yourself.
:::

## Chocolatey

There is a community maintained package of QOwnNotes at [Chocolatey](https://chocolatey.org/packages/qownnotes/).

You can install it with:

```shell
choco install qownnotes
```
