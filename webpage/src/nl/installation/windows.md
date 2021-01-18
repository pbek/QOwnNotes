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

Hoewel je zelf de laatste master-build van [AppVeyor](https://ci.appveyor.com/project/pbek/qownnotes/history) moet zoeken, kies je de *Qt 5.7-omgeving* en download je het *artefact*.

::: tip Info
Het automatische updatemechanisme werkt niet met de AppVeyor-build voor Windows XP! U zult zelf nieuwe releases moeten downloaden.
:::

## Chocolatey

Er is een door de gemeenschap onderhouden pakket van QOwnNotes op [Chocolatey](https://chocolatey.org/packages/qownnotes/).

U kunt het installeren met:

```shell
choco install qownnotes
```
