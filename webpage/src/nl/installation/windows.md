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

Qt dropped support for Windows XP with version 5.8, but QOwnNotes is now also built with Qt 5.7 to allow Windows XP users to still use it. Windows 8.1 has issues with the regular QOwnNotes build, because it was built on a newer Windows SDK.

Je moet het ZIP-bestand zelf downloaden van [AppVeyor](https://ci.appveyor.com/project/pbek/qownnotes/build/artifacts) en het uitpakken in een map naar keuze.

U kunt dan direct `QOwnNotes.exe` uitvoeren vanuit die map, er is geen installatie vereist.

::: tip
Info
Het automatische updatemechanisme werkt niet met de AppVeyor-build voor Windows XP!
U zult zelf nieuwe releases moeten downloaden.
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
