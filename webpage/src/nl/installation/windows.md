# Installatie op Microsoft Windows™

Download het nieuwste **Windows-archief** van [QOwnNotes Releases op GitHub](https://github.com/pbek/QOwnNotes/releases) (zoek naar een bestand met de naam `QOwnNotes.zip`) en pak het uit waar u maar wilt. Er is geen installatie nodig!

::: warning
Als u de **automatic updater** wilt gebruiken, zorg er dan voor dat u deze uitpakt op een plaats waar uw gebruikersaccount schrijftoegang tot heeft. Standaard heeft uw gebruikersaccount waarschijnlijk **geen schrijfrechten** op locaties zoals `C:\Program Files (x86)` of `C:\Program Files`.
:::

U kunt dan direct `QOwnNotes.exe` starten vanuit uw `QOwnNotes` -map, er is geen installatie vereist.

### Portable mode

Gebruik `QOwnNotesPortable.bat` om QOwnNotes in **draagbare modus** uit te voeren, waar alles (inclusief uw notities) alleen wordt opgeslagen in uw map `QOwnNotes`.

::: tip
U hebt het draagbare modus niet nodig als u gewoon geen beheerdersrechten voor uw computer hebt. QOwnNotes hoeft niet te worden geïnstalleerd!
:::

## Windows 8.1 and older

The Windows release is built with **Qt6** and needs at least **Windows 10**.

The legacy Qt 5.7 build for Windows XP to Windows 8.1 is no longer updated. The final version is **QOwnNotes 26.10.0**, which you can download as [QOwnNotes-Qt5.7-legacy.zip](https://github.com/pbek/QOwnNotes/releases/download/v26.10.0/QOwnNotes-Qt5.7-legacy.zip) ([SHA-256 checksum](https://github.com/pbek/QOwnNotes/releases/download/v26.10.0/QOwnNotes-Qt5.7-legacy.zip.sha256sum)) from the [26.10.0 release on GitHub](https://github.com/pbek/QOwnNotes/releases/tag/v26.10.0).

Unzip it to a folder of your liking and run `QOwnNotes.exe` from that folder, no installation is required.

::: tip
Info
The automatic update mechanism does not work with the legacy build!
:::

## Chocolatey

There is a community-maintained package of QOwnNotes at [Chocolatey](https://chocolatey.org/packages/qownnotes/).

U kunt het installeren met:

```shell
choco install qownnotes
```

## Scoop

There is a [community-maintained package of QOwnNotes](https://github.com/ScoopInstaller/Extras/blob/master/bucket/qownnotes.json) at [Scoop](https://scoop.sh/). If you add the Extras bucket, you can use it to install QOwnNotes in portable mode.

```shell
scoop bucket add extras
scoop update
scoop install qownnotes
```

## WinGet

There is a [community-maintained package of QOwnNotes](https://github.com/microsoft/winget-pkgs/tree/master/manifests/p/pbek/QOwnNotes) for [WinGet](https://github.com/microsoft/winget-cli).

You can install it with:

```shell
winget install qownnotes
```

## MSYS2

There is a [community-maintained package of QOwnNotes](https://packages.msys2.org/base/mingw-w64-qownnotes) for [MSYS2](hhttps://www.msys2.org/).

You can install it with:

```shell
pacman -S mingw-w64-qownnotes
```
