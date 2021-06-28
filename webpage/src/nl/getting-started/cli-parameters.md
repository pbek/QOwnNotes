# Opdrachtregelinterfaceparameters

U kunt deze parameters gebruiken op de opdrachtregelinterface:

| Parameter                    | Beschrijving                                                                                                                        |
| ---------------------------- | ----------------------------------------------------------------------------------------------------------------------------------- |
| `--help`                     | Toont het helpscherm                                                                                                                |
| `--portable`                 | Start de applicatie in draagbare modus                                                                                              |
| `--clear-settings`           | Wist de instellingen en start de applicatie                                                                                         |
| `--dump-settings`            | Print een dump van de instellingen en andere informatie over de applicatie en omgeving in GitHub Markdown en sluit de applicatie af |
| `--session <name>`     | Voert de applicatie uit in een andere context voor instellingen en interne bestanden                                                |
| `--allow-multiple-instances` | Maakt het mogelijk om meerdere exemplaren van QOwnNotes te starten, zelfs als dit niet is toegestaan in de instellingen             |
| `--action <name>`      | Activeert een menuactie nadat de applicatie is gestart (zie hieronder)                                                              |

::: tip
Als u problemen ondervindt bij het installeren van QOwnNotes, wilt u misschien de toepassing starten met nieuwe instellingen zonder uw huidige instellingen te verliezen door de parameter `--session` te gebruiken.

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

Om [scriptacties](../scripting/methods-and-objects.md#registering-a-custom-action) te activeren, gebruikt u `customAction_` gevolgd door de identifier van de aangepaste actie. The identifier of the custom action is the first parameter of the `script.registerCustomAction` call in the script.

For example to run the custom action `myAction` start QOwnNotes like this:

```bash
QOwnNotes --action customAction_myAction
```

::: tip
If you run QOwnNotes a second time with the action parameter and only have allowed one application instance, then the menu action will be triggered in the first instance.
:::
