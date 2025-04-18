# Parameter der Kommandozeilenschnittstelle

Sie können diese Parameter über das Kommandozeilenschnittstelle (CLI) verwenden, um das Verhalten der Anwendung zu steuern:

| Parameter                    | Beschreibung                                                                                                                                      |
| ---------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------- |
| `--help`                     | Zeigt den Hilfebildschirm an                                                                                                                      |
| `--version`                  | Gibt die Versionsnummer der Anwendung aus                                                                                                         |
| `--portable`                 | Führt die Anwendung im portablen Modus aus                                                                                                        |
| `--clear-settings`           | Löscht die Einstellungen und führt die Anwendung aus                                                                                              |
| `--dump-settings`            | Gibt einen Speicherauszug der Einstellungen und anderer Informationen zur Anwendung und Umgebung in GitHub Markdown aus und beendet die Anwendung |
| `--session <name>`     | Führt die Anwendung in einem anderen Kontext für Einstellungen und interne Dateien aus                                                            |
| `--allow-multiple-instances` | Ermöglicht das Starten mehrerer Instanzen von QOwnNotes, auch wenn dies in den Einstellungen abgeschaltet ist                                     |
| `--action <name>`      | Löst eine Menüaktion aus, nachdem die Anwendung gestartet wurde (siehe unten)                                                                     |
| `--completion <shell>` | Erzeugt Shell-Vervollständigungscode. Unterstützt `fish`,`bash`.                                                                                  |

::: tip
Falls bei der Installation von QOwnNotes Probleme auftreten, sollten Sie die Anwendung möglicherweise mit neuen Einstellungen starten, ohne Ihre aktuellen Einstellungen zu verlieren, indem Sie den Parameter `--session` verwenden.

```bash
QOwnNotes --session test
```

:::

Sie können die Anwendung auf der Kommandozeilenschnittstelle unter verschiedenen Betriebssystemen unterschiedlich ausführen:

| Betriebssystem | Befehl                                                    |
| -------------- | --------------------------------------------------------- |
| Linux          | `QOwnNotes` (oder `qownnotes`, wenn als Snap installiert) |
| macOS          | `/Applications/QOwnNotes.app/Contents/MacOS/QOwnNotes`    |
| Windows        | `QOwnNotes.exe`                                           |

::: tip
To directly copy the settings dump to the clipboard to post in a [GitHub issue](https://github.com/pbek/QOwnNotes/issues) you can use `xclip` or `xsel` on Linux, `pbcopy` on macOS and `clip` on Windows:

```bash
# Copy the settings dump to the clipboard with xclip on Linux
QOwnNotes --dump-settings | xclip -selection clipboard

# Copy the settings dump to the clipboard with xsel on Linux
QOwnNotes --dump-settings | xsel --clipboard

# Copy the settings dump to the clipboard with pbcopy on macOS
QOwnNotes --dump-settings | pbcopy

# Copy the settings dump to the clipboard with clip on Windows
QOwnNotes --dump-settings | clip
```

:::

## Menüaktionen nach dem Start auslösen

Mit dem Parameter `--action <name>` können Sie nach dem Start der Anwendung Menüaktionen auslösen.

Um beispielsweise den To-do-Listen-Dialog nach dem Start zu öffnen:

```bash
QOwnNotes --action actionShow_Todo_List
```

::: tip
Die Objektnamen der Menüaktionen finden Sie über [mainwindow.ui](https://github.com/pbek/QOwnNotes/blob/main/src/mainwindow.ui). Suchen Sie einfach nach dem englischen Menütitel. Beachten Sie, dass sich diese Texte im Laufe der Zeit ändern können.
:::

Verwenden Sie zum Auslösen von [Skriptaktionen](../scripting/methods-and-objects.md#registering-a-custom-action) ` customAction_` gefolgt von der Kennung der benutzerdefinierten Aktion. Der Bezeichner der benutzerdefinierten Aktion ist der erste Parameter des Aufrufs `script.registerCustomAction` im Skript.

Um beispielsweise die benutzerdefinierte Aktion `myAction` auszuführen, starten Sie QOwnNotes wie folgt:

```bash
QOwnNotes --action customAction_myAction
```

::: tip
Wenn Sie QOwnNotes ein zweites Mal mit dem action-Parameter ausführen und nur eine Anwendungsinstanz zugelassen haben, wird die Menüaktion in der ersten Instanz ausgelöst.
:::
