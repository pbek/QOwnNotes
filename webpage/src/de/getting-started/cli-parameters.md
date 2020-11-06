# Parameter der Befehlszeilenschnittstelle

Sie können diese Parameter auf der Befehlszeilenschnittstelle verwenden:

| Parameter                    | Beschreibung                                                                                                                                        |
| ---------------------------- | --------------------------------------------------------------------------------------------------------------------------------------------------- |
| `--help`                     | Zeigt den Hilfebildschirm an                                                                                                                        |
| `--tragbar`                  | Führt die Anwendung im portablen Modus aus                                                                                                          |
| `--klare Einstellungen`      | Löscht die Einstellungen und führt die Anwendung aus                                                                                                |
| `--Dump-Einstellungen`       | Druckt einen Speicherauszug der Einstellungen und anderer Informationen zur Anwendung und Umgebung in GitHub Markdown aus und beendet die Anwendung |
| `- Sitzung <name>`     | Führt die Anwendung in einem anderen Kontext für Einstellungen und interne Dateien aus                                                              |
| `--allow-multiple-instances` | Ermöglicht das Starten mehrerer Instanzen von QOwnNotes, auch wenn dies in den Einstellungen nicht zulässig ist                                     |

::: tip
Wenn Sie Probleme mit der Installation von QOwnNotes haben, möchten Sie möglicherweise starten die Anwendung mit neuen Einstellungen, ohne Ihre aktuellen Einstellungen zu verlieren der Parameter ` --session `.

```bash
QOwnNotes - Sitzung-Test
```
:::

Sie können die Anwendung auf der Befehlszeilenschnittstelle unter verschiedenen Betriebssystemen unterschiedlich ausführen:

| Betriebssystem | Befehl                                                        |
| -------------- | ------------------------------------------------------------- |
| Linux          | ` QOwnNotes ` (oder ` qownnotes `, wenn als Snap installiert) |
| macOS          | `/Applications/QOwnNotes.app/Contents/MacOS/QOwnNotes`        |
| Windows        | `QOwnNotes.exe`                                               |
