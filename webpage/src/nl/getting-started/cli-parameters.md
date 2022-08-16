# Opdrachtregel-interface-parameters

U kunt deze parameters op de opdrachtregelinterface (CLI) gebruiken om het gedrag van de toepassing te regelen:

| Parameter                    | Beschrijving                                                                                                                        |
| ---------------------------- | ----------------------------------------------------------------------------------------------------------------------------------- |
| `--help`                     | Toont het helpscherm                                                                                                                |
| `--version`                  | Drukt het versienummer van de applicatie af                                                                                         |
| `--portable`                 | Start de applicatie in draagbare modus                                                                                              |
| `--clear-settings`           | Wist de instellingen en start de applicatie                                                                                         |
| `--dump-settings`            | Print een dump van de instellingen en andere informatie over de applicatie en omgeving in GitHub Markdown en sluit de applicatie af |
| `--session <name>`     | Voert de applicatie uit in een andere context voor instellingen en interne bestanden                                                |
| `--allow-multiple-instances` | Maakt het mogelijk om meerdere exemplaren van QOwnNotes te starten, zelfs als dit niet is toegestaan in de instellingen             |
| `--action <name>`      | Activeert een menuactie nadat de applicatie is gestart (zie hieronder)                                                              |

::: tip
Als u problemen ondervindt met uw installatie van QOwnNotes, wilt u misschien de toepassing starten met nieuwe instellingen zonder uw huidige instellingen te verliezen door de parameter `--session` te gebruiken.

```bash
QOwnNotes --session test
```
:::

U kunt de toepassing op de opdrachtregelinterface op verschillende besturingssystemen op verschillende manieren uitvoeren:

| Besturingssysteem | Opdracht                                                     |
| ----------------- | ------------------------------------------------------------ |
| Linux             | `QOwnNotes` (of `qownnotes` indien geïnstalleerd als module) |
| macOS             | `/Applications/QOwnNotes.app/Contents/MacOS/QOwnNotes`       |
| Windows           | `QOwnNotes.exe`                                              |

## Menuacties activeren na opstarten

Met de parameter `--action <name>` kunt u menuacties activeren nadat de applicatie is gestart.

Gebruik bijvoorbeeld om het dialoogvenster met de takenlijst te openen na het opstarten:

```bash
QOwnNotes --action actionShow_Todo_List
```

::: tip
U kunt de objectnamen van de menuactie ophalen van [hoofdvenster.ui](https://github.com/pbek/QOwnNotes/blob/develop/src/mainwindow.ui). Zoek gewoon naar de Engelse menutitel. Houd er rekening mee dat deze teksten in de loop van de tijd kunnen veranderen.
:::

Om [scriptacties](../scripting/methods-and-objects.md#registering-a-custom-action) te activeren, gebruikt u `customAction_` gevolgd door de identifier van de aangepaste actie. De id van de aangepaste actie is de eerste parameter van de `script.registerCustomAction`-aanroep in het script.

Om bijvoorbeeld de aangepaste actie `myAction` uit te voeren, start u QOwnNotes als volgt:

```bash
QOwnNotes --action customAction_myAction
```

::: tip
Als u QOwnNotes een tweede keer uitvoert met de actieparameter en slechts één toepassingsinstantie hebt toegestaan, wordt de menuactie in eerste instantie geactiveerd.
:::
