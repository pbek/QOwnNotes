# Parametri dell'interfaccia della riga di comando

È possibile usare questi parametri sull'interfaccia della riga di comando:

| Parametro                    | Descrizione                                                                                                                       |
| ---------------------------- | --------------------------------------------------------------------------------------------------------------------------------- |
| `--help`                     | Mostra la schermata della guida                                                                                                   |
| `--portable`                 | Esegue l'applicazione in modalità portatile                                                                                       |
| `--clear-settings`           | Azzera le impostazioni e avvia l'applicazione                                                                                     |
| `--dump-settings`            | Esporta in formato Markdown l'elenco delle impostazioni e altre informazioni su applicazione e ambiente ed esce dall'applicazione |
| `--session <name>`     | Esegue l'applicazione in un contesto differente per le impostazioni e i file interni                                              |
| `--allow-multiple-instances` | Consente di avviare più istanze di QOwnNotes anche se non consentito nelle impostazioni                                           |
| `--action <name>`      | Attiva un'azione di menu dopo l'avvio dell'applicazione (vedi sotto)                                                              |

::: tip
If you run into troubles with your installation of QOwnNotes you may want start the application with fresh settings without losing your current settings by using the `--session` parameter.

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

Ad esempio, per aprire la finestra di dialogo dell'elenco delle cose da fare dopo l'utilizzo all'avvio:

```bash
QOwnNotes --action actionShow_Todo_List
```

::: tip
Puoi ottenere i nomi degli oggetti dell'azione del menu da [mainwindow.ui](https://github.com/pbek/QOwnNotes/blob/develop/src/mainwindow.ui). Basta cercare il titolo del menu in inglese. Note that these texts can change over time.
:::

To trigger [scripting actions](../scripting/methods-and-objects.md#registering-a-custom-action) use `customAction_` followed by the identifier of the custom action. The identifier of the custom action is the first parameter of the `script.registerCustomAction` call in the script.

For example to run the custom action `myAction` start QOwnNotes like this:

```bash
QOwnNotes --action customAction_myAction
```

::: tip
If you run QOwnNotes a second time with the action parameter and only have allowed one application instance, then the menu action will be triggered in the first instance.
:::
