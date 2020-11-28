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
