# Telepítse a Microsoft Windows™ rendszerre

Download the latest **Windows archive** from [QOwnNotes Releases on GitHub](https://github.com/pbek/QOwnNotes/releases) (look for a file called `QOwnNotes.zip`) and unzip it to anywhere you like. Nincs szükség telepítésre!

::: warning
Ha az **automatikus frissítőt** szeretné használni, kérjük, győződjön meg arról, hogy kicsomagolja-e egy olyan helyre, ahol a felhasználói fiók rendelkezik írási hozzáféréssel. By default, your user account most likely **doesn't have write access** to places like `C:\Program Files (x86)` or `C:\Program Files`.
:::

Ezután közvetlenül futtathatja a `QOwnNotes.exe` fájlt a `QOwnNotes` mappából, nincs szükség telepítésre.

### Hordozható üzemmód

A `QOwnNotesPortable.bat` használatával futtathatja a QOwnNotes **hordozható módban**, ahol mindent (beleértve a jegyzeteket is) csak a `QOwnNotes` mappájában tárolunk.

::: tip
Nincs szüksége a hordozható módra, ha csak nincs adminisztrációs engedélye a számítógépéhez. A QOwnNotes programot nem kell telepíteni!
:::

## Windows 8.1 and older

Qt dropped support for Windows XP with version 5.8, but QOwnNotes is now also built with Qt 5.7 to allow Windows XP users to still use it. Windows 8.1 has issues with the regular QOwnNotes build, because it was built on a newer Windows SDK.

Saját magának kell letöltenie a ZIP-fájlt az [AppVeyor](https://ci.appveyor.com/project/pbek/qownnotes/build/artifacts) webhelyről, és ki kell csomagolnia egy tetszőleges mappába.

Ezután közvetlenül futtathatja a `QOwnNotes.exe` fájlt a mappából, nincs szükség telepítésre.

::: tip
Info
Az automatikus frissítési mechanizmus nem működik a Windows XP AppVeyor buildjével!
Magának le kell töltenie az új kiadásokat.
:::

## Chocolatey

There is a community-maintained package of QOwnNotes at [Chocolatey](https://chocolatey.org/packages/qownnotes/).

Telepítheti:

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
