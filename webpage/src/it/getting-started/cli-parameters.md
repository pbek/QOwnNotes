# Parametri dell'interfaccia della riga di comando

È possibile utilizzare questi parametri sull'interfaccia della riga di comando:

| Parametro                    | Descrizione                                                                                                                       |
| ---------------------------- | --------------------------------------------------------------------------------------------------------------------------------- |
| `--help`                     | Mostra la schermata della guida                                                                                                   |
| `--portable`                 | Esegue l'applicazione in modalità portatile                                                                                       |
| `--clear-settings`           | Azzera le impostazioni e avvia l'applicazione                                                                                     |
| `--dump-settings`            | Esporta in formato Markdown l'elenco delle impostazioni e altre informazioni su applicazione e ambiente ed esce dall'applicazione |
| `--session <name>`     | Esegue l'applicazione in un contesto differente per le impostazioni e i file interni                                              |
| `--allow-multiple-instances` | Consente di avviare più istanze di QOwnNotes anche se non consentito nelle impostazioni                                           |
| `--action <name>`      | Triggers a menu action after the application was started (see below)                                                              |

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

## Trigger menu actions after startup

With the parameter `--action <name>` you can trigger menu actions after the application was started.

For example to open the todo list dialog after startup use:

```bash
QOwnNotes --action actionShow_Todo_List
```

::: tip
You can get the object names of the menu action from [mainwindow.ui](https://github.com/pbek/QOwnNotes/blob/develop/src/mainwindow.ui). Just search for the English menu title. Note that these texts can change over time.
:::

To trigger [scripting actions](../scripting/methods-and-objects.md#registering-a-custom-action) use `customAction_` followed by the identifier of the custom action. The identifier of the custom action is the first parameter of the `script.registerCustomAction` call in the script.

For example to run the custom action `myAction` start QOwnNotes like this:

```bash
QOwnNotes --action customAction_myAction
```

::: tip
If you run QOwnNotes a second time with the action parameter and only have allowed one application instance, then the menu action will be triggered in the first instance.
:::
