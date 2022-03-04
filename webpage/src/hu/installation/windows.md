# Telepítse a Microsoft Windows™ rendszerre

Töltse le a legfrissebb **Windows-archívumot** a [QOwnNotes Releases on GitHub webhelyről](https://github.com/pbek/QOwnNotes/releases), és csomagolja ki bárhová. Nincs szükség telepítésre!

::: warning
Ha az **automatikus frissítőt** szeretné használni, kérjük, győződjön meg arról, hogy kicsomagolja-e egy olyan helyre, ahol a felhasználói fiók rendelkezik írási hozzáféréssel. Alapértelmezés szerint a felhasználói fiókjának valószínűleg nincs **írási hozzáférése** olyan helyekhez, mint a `C:\Program Files (x86)` vagy a `C:\Program Files`.
:::

Ezután közvetlenül futtathatja a `QOwnNotes.exe` fájlt a `QOwnNotes` mappából, nincs szükség telepítésre.

### Hordozható üzemmód

A `QOwnNotesPortable.bat` használatával futtathatja a QOwnNotes **hordozható módban**, ahol mindent (beleértve a jegyzeteket is) csak a `QOwnNotes` mappájában tárolunk.

::: tip
Nincs szüksége a hordozható módra, ha csak nincs adminisztrációs engedélye a számítógépéhez. A QOwnNotes programot nem kell telepíteni!
:::

## Windows XP

A Qt elvetette az 5.8-as verziójú Windows XP támogatását, de a QOwnNotes mostantól a Qt 5.7-tel is fel van építve, hogy a Windows XP felhasználói továbbra is használhassák.

You need to download the ZIP file from [AppVeyor](https://ci.appveyor.com/project/pbek/qownnotes/build/artifacts) yourself and unzip it to a folder of your liking.

You can then directly run `QOwnNotes.exe` from that folder, no installation is required.

::: tip Info
The automatic update mechanism does not work with the AppVeyor build for Windows XP! You will have to download new releases yourself.
:::

## Chocolatey

There is a community maintained package of QOwnNotes at [Chocolatey](https://chocolatey.org/packages/qownnotes/).

You can install it with:

```shell
choco install qownnotes
```

## Scoop

There is a [community maintained package of QOwnNotes](https://github.com/ScoopInstaller/Extras/blob/master/bucket/qownnotes.json) at [Scoop](https://scoop.sh/). If you add the Extras bucket you can use it to install QOwnNotes in portable mode.

```shell
scoop bucket add extras
scoop update
scoop install qownnotes
```
