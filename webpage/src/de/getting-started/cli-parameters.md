# Parameter der Befehlszeilenschnittstelle

Sie können diese Parameter über die Befehlszeilenschnittstelle (CLI) verwenden, um das das Verhalten der Anwendung steuern:

| Parameter                    | Beschreibung                                                                                                                                        |
| ---------------------------- | --------------------------------------------------------------------------------------------------------------------------------------------------- |
| `--help`                     | Zeigt den Hilfebildschirm an                                                                                                                        |
| `--version`                  | Druckt die Versionsnummer der Anwendung aus                                                                                                         |
| `--portable`                 | Führt die Anwendung im portablen Modus aus                                                                                                          |
| `--clear-settings`           | Löscht die Einstellungen und führt die Anwendung aus                                                                                                |
| `--dump-settings`            | Druckt einen Speicherauszug der Einstellungen und anderer Informationen zur Anwendung und Umgebung in GitHub Markdown aus und beendet die Anwendung |
| `--session <name>`     | Führt die Anwendung in einem anderen Kontext für Einstellungen und interne Dateien aus                                                              |
| `--allow-multiple-instances` | Ermöglicht das Starten mehrerer Instanzen von QOwnNotes, auch wenn dies in den Einstellungen nicht zulässig ist                                     |
| `--action <name>`      | Löst eine Menüaktion aus, nachdem die Anwendung gestartet wurde (siehe unten)                                                                       |

::: tip
Wenn bei der Installation von QOwnNotes Probleme auftreten, möchten Sie die Anwendung möglicherweise mit neuen Einstellungen starten, ohne Ihre aktuellen Einstellungen zu verlieren, indem Sie den Parameter `--session` verwenden.

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

## Menüaktionen nach dem Start auslösen

Mit dem Parameter `--action <name>` können Sie nach dem Start der Anwendung Menüaktionen auslösen.

Um beispielsweise den Todo-Listen-Dialog nach dem Start zu öffnen:

```bash
QOwnNotes --action actionShow_Todo_List
```

::: tip
Die Objektnamen der Menüaktion erhalten Sie von [mainwindow.ui](https://github.com/pbek/QOwnNotes/blob/develop/src/mainwindow.ui). Suchen Sie einfach nach dem englischen Menütitel. Beachten Sie, dass sich diese Texte im Laufe der Zeit ändern können.
:::

Verwenden Sie zum Auslösen von [Skriptaktionen](../scripting/methods-and-objects.md#registering-a-custom-action) ` customAction_` gefolgt von der Kennung der benutzerdefinierten Aktion. Der Bezeichner der benutzerdefinierten Aktion ist der erste Parameter des Aufrufs `script.registerCustomAction` im Skript.

Um beispielsweise die benutzerdefinierte Aktion `myAction` auszuführen, starten Sie QOwnNotes wie folgt:

```bash
QOwnNotes --action customAction_myAction
```

::: tip
Wenn Sie QOwnNotes ein zweites Mal mit dem Parameter action ausführen und nur eine Anwendungsinstanz zugelassen haben, wird die Menüaktion in der ersten Instanz ausgelöst.
:::
