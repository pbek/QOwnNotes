# Installeer op Microsoft Windows™

Download de nieuwste versie **Windows archive** van [QOwnNotes Releases on GitHub](https://github.com/pbek/QOwnNotes/releases) en pak deze uit waar u maar wilt. Er is geen installatie nodig!

::: warning
Als u de **automatic updater** wilt gebruiken, zorg er dan voor dat u deze uitpakt op een plaats waar uw gebruikersaccount schrijftoegang tot heeft. Uw gebruikersaccount heeft standaard waarschijnlijk **geen schrijftoegang** tot plaatsen zoals ` C:\ Program Files (x86)` of `C:\Program Files`.
:::

U kunt dan direct `QOwnNotes.exe` starten vanuit uw `QOwnNotes` -map, er is geen installatie vereist.

### Draagbare modus

Gebruik `QOwnNotesPortable.bat` om QOwnNotes in **draagbare modus** uit te voeren, waar alles (inclusief uw notities) alleen wordt opgeslagen in uw map `QOwnNotes`.

::: tip
U hebt het draagbare modus niet nodig als u gewoon geen beheerdersrechten voor uw computer hebt. QOwnNotes hoeft niet te worden geïnstalleerd!
:::

## Windows XP

Qt heeft de ondersteuning voor Windows XP met versie 5.8 laten vallen, maar QOwnNotes is nu ook gebouwd met Qt 5.7 zodat Windows XP-gebruikers het nog steeds kunnen gebruiken.

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
