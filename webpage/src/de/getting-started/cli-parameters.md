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
| `--action <name>`      | Triggers a menu action after the application was started (see below)                                                                                |

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

## Trigger menu actions after startup

With the parameter `--action <name>` you can trigger menu actions after the application was started.

For example to open the todo list dialog after startup use:

```bash
QOwnNotes --action actionShow_Todo_List
```

::: tip
You can get the object names of the menu action from [mainwindow.ui](https://github.com/pbek/QOwnNotes/blob/develop/src/mainwindow.ui). Just search for the English menu title. Note that these texts can change over time.
:::

To trigger scripting actions use `customAction_` followed by the identifier of the custom action. For example to run the custom action `myAction` start QOwnNotes like this:

```bash
QOwnNotes --action customAction_myAction
```

The identifier of the custom action is the first parameter of the `script.registerCustomAction` call in the script.

::: tip
If you run QOwnNotes a second time with the action parameter and only have allowed one application instance, then the menu action will be triggered in the first instance.
:::
