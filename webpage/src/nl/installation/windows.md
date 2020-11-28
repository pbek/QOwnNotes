# Installeer op Microsoft Windows™

Download de nieuwste versie **Windows archive** van [QOwnNotes Releases on GitHub](https://github.com/pbek/QOwnNotes/releases) en pak deze uit waar u maar wilt. Er is geen installatie nodig!

::: warning
Als u de **automatic updater** wilt gebruiken, zorg er dan voor dat u deze uitpakt op een plaats waar uw gebruikersaccount schrijftoegang tot heeft. Uw gebruikersaccount heeft standaard waarschijnlijk **geen schrijftoegang** tot plaatsen zoals ` C:\ Program Files (x86)` of `C:\Program Files`.
:::

U kunt dan direct `QOwnNotes.exe` starten vanuit uw `QOwnNotes` -map, er is geen installatie vereist.

### Draagbare modus

A `QOwnNotesPortable.bat` használatával futtathatja a QOwnNotes **hordozható módban**, ahol mindent (a jegyzeteket is beleértve) csak a `QOwnNotes` mappájában tárolunk.

::: tip
You don't need the portable node if you just don't have Administration permissions to your computer. A QOwnNotes programot nem kell telepíteni!
:::

## Windows XP

A Qt elvetette az 5.8-as verziójú Windows XP támogatását, de a QOwnNotes mostantól a Qt 5.7-gyel is fel van építve, hogy a Windows XP felhasználói továbbra is használhassák.

Bár magának meg kell keresnie az [ AppVeyor ](https://ci.appveyor.com/project/pbek/qownnotes/history) legfrissebb master buildjét, válassza a *Qt 5.7 environment-et*-et és töltse le az *artefaktum*-ot.

::: tip Info
Az automatikus frissítési mechanizmus nem működik az AppVeyor Windows XP rendszerhez készült verziójával! Magának le kell töltenie az új kiadásokat.
:::

## Chocolatey

Van egy közösség által karbantartott QOwnNotes csomag a [Chocolatey](https://chocolatey.org/packages/qownnotes/) címen.

U kunt het installeren met:

```shell
choco telepítse qownnotes
```
