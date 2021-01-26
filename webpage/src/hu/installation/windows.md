# Telepítse a Microsoft Windows™ rendszerre

Töltse le a legfrissebb **Windows-archívumot** a [QOwnNotes Releases on GitHub webhelyről](https://github.com/pbek/QOwnNotes/releases), és csomagolja ki bárhová. Nincs szükség telepítésre!

::: warning Ha az **automatikus frissítőt** szeretné használni, kérjük, győződjön meg arról, hogy kicsomagolja-e egy olyan helyre, ahol a felhasználói fiók rendelkezik írási hozzáféréssel. Alapértelmezés szerint a felhasználói fiókjának valószínűleg nincs **írási hozzáférése** olyan helyekhez, mint a `C:\Program Files (x86)` vagy a `C:\Program Files`. :::

Ezután közvetlenül futtathatja a `QOwnNotes.exe` fájlt a `QOwnNotes` mappából, nincs szükség telepítésre.

### Hordozható üzemmód

A `QOwnNotesPortable.bat` használatával futtathatja a QOwnNotes **hordozható módban**, ahol mindent (beleértve a jegyzeteket is) csak a `QOwnNotes` mappájában tárolunk.

::: tip Nincs szüksége a hordozható módra, ha csak nincs adminisztrációs engedélye a számítógépéhez. A QOwnNotes programot nem kell telepíteni! :::

## Windows XP

A Qt elvetette az 5.8-as verziójú Windows XP támogatását, de a QOwnNotes mostantól a Qt 5.7-tel is fel van építve, hogy a Windows XP felhasználói továbbra is használhassák.

Bár magának meg kell keresnie az [AppVeyor](https://ci.appveyor.com/project/pbek/qownnotes/history) legfrissebb master buildjét, válassza ki a *Qt 5.7 környezetet* és töltse le az *artefaktum* t.

::: tip Info Az automatikus frissítési mechanizmus nem működik az AppVeyor Windows XP rendszerhez készült verziójával! Magának le kell töltenie az új kiadásokat. :::

## Chocolatey

Van egy közösség által karbantartott QOwnNotes csomag a [Chocolatey](https://chocolatey.org/packages/qownnotes/) címen.

Telepítheti:

```shell
choco install qownnotes
```
