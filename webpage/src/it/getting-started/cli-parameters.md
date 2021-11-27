# Parametri dell'interfaccia della riga di comando

È possibile usare questi parametri sull'interfaccia della riga di comando:

| Parametro                    | Descrizione                                                                                                                                |
| ---------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------ |
| `--help`                     | Mostra la schermata della guida                                                                                                            |
| `--version`                  | Prints out the version number of the application                                                                                           |
| `--portable`                 | Runs the application in portable mode                                                                                                      |
| `--clear-settings`           | Clears the settings and runs the application                                                                                               |
| `--dump-settings`            | Prints out a dump of the settings and other information about the application and environment in GitHub Markdown and exits the application |
| `--session <name>`     | Runs the application in a different context for settings and internal files                                                                |
| `--allow-multiple-instances` | Allows multiple instances of QOwnNotes to be started even if disallowed in the settings                                                    |
| `--action <name>`      | Triggers a menu action after the application was started (see below)                                                                       |

::: tip
Se si verificano problemi con l'installazione di QOwnNotes, è possibile avviare l'applicazione con nuove impostazioni senza perdere le impostazioni correnti utilizzando il parametro `--session`.

```bash
QOwnNotes --session test
```
:::

È possibile eseguire l'applicazione sull'interfaccia della riga di comando in modo diverso su diversi sistemi operativi:

| Sistema operativo | Comando                                                |
| ----------------- | ------------------------------------------------------ |
| Linux             | `QOwnNotes` (or `qownnotes` se installato come snap)   |
| macOS             | `/Applications/QOwnNotes.app/Contents/MacOS/QOwnNotes` |
| Windows           | `QOwnNotes.exe`                                        |

## Attiva le azioni del menu dopo l'avvio

Con il parametro `--action <name>` è possibile attivare azioni di menu dopo l'avvio dell'applicazione.

Ad esempio, per aprire la finestra di dialogo dell'elenco delle cose da fare dopo l'avvio si può usare:

```bash
QOwnNotes --action actionShow_Todo_List
```

::: tip
Puoi ottenere i nomi degli oggetti dell'azione del menu da [mainwindow.ui](https://github.com/pbek/QOwnNotes/blob/develop/src/mainwindow.ui). Basta cercare il titolo del menu in inglese. Nota che questi testi possono cambiare nel tempo.
:::

Per attivare le [azioni scripting](../scripting/methods-and-objects.md#registering-a-custom-action) usa `customAction_` seguito dall'identificativo dell'azione personalizzata. L'identificativo dell'azione personalizzata è il primo parametro della chiamata a `script.registerCustomAction` nello script.

Per esempio, per eseguire l'azione personalizzata `myAction` esegui QOwnNotes così:

```bash
QOwnNotes --action customAction_myAction
```

::: tip
Se esegui QOwnNotes una seconda volta con un parametro action e hai permesso una sola istanza dell'applicazione, allora l'azione di menu sarà attivata solo nella prima istanza.
:::
