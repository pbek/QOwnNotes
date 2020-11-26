# Parameter der Befehlszeilenschnittstelle

Sie können diese Parameter auf der Befehlszeilenschnittstelle verwenden:

| Parameter                    | Beschreibung                                                                                                                                        |
| ---------------------------- | --------------------------------------------------------------------------------------------------------------------------------------------------- |
| `--help`                     | Zeigt den Hilfebildschirm an                                                                                                                        |
| `--portable`                 | Führt die Anwendung im portablen Modus aus                                                                                                          |
| `--clear-settings`           | Löscht die Einstellungen und führt die Anwendung aus                                                                                                |
| `--dump-settings`            | Druckt einen Speicherauszug der Einstellungen und anderer Informationen zur Anwendung und Umgebung in GitHub Markdown aus und beendet die Anwendung |
| `--session <name>`     | Führt die Anwendung in einem anderen Kontext für Einstellungen und interne Dateien aus                                                              |
| `--allow-multiple-instances` | Ermöglicht das Starten mehrerer Instanzen von QOwnNotes, auch wenn dies in den Einstellungen nicht zulässig ist                                     |

::: tip
Wenn Sie bei der Installation von QOwnNotes auf Probleme stoßen, können Sie die Anwendung mit neuen Einstellungen starten, ohne Ihre aktuellen Einstellungen mithilfe des Parameters der `Sitzung` zu verlieren.

```bash
QOwnNotes --session test
```
:::

Sie können die Anwendung auf der Befehlszeilenschnittstelle unter verschiedenen Betriebssystemen unterschiedlich ausführen:

| Betriebssystem | Befehl                                                    |
| -------------- | --------------------------------------------------------- |
| Linux          | `QOwnNotes` (oder `qownnotes`, wenn als Snap installiert) |
| macOS          | `/Applications/QOwnNotes.app/Contents/MacOS/QOwnNotes`    |
| Windows        | `QOwnNotes.exe`                                           |
