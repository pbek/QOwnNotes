# Parametri dell'interfaccia della riga di comando

Puoi usare questi parametri sull'interfaccia della riga di comando (CLI) per controllare il comportamento dell'applicazione:

| Parametro                                  | Descrizione                                                                                                                                 |
| ------------------------------------------ | ------------------------------------------------------------------------------------------------------------------------------------------- |
| `--help`                                   | Mostra la schermata della guida                                                                                                             |
| `--version`                                | Mostra il numero di versione dell'applicazione                                                                                              |
| `--portable`                               | Esegue l'applicazione in modalità portatile                                                                                                 |
| `--clear-settings`                         | Azzera le impostazioni e avvia l'applicazione                                                                                               |
| `--dump-settings`                          | Mostra una lista di impostazioni e altre informazioni sull'applicazione e l'ambiente, in formato GitHub Markdown, ed esce dall'applicazione |
| `--session <name>`                   | Esegue l'applicazione in un contesto differente per le impostazioni e i file interni                                                        |
| `--allow-multiple-instances`               | Consente di avviare più istanze di QOwnNotes anche se non consentito nelle impostazioni                                                     |
| `--action <name>`                    | Attiva un'azione di menu dopo l'avvio dell'applicazione (vedi sotto)                                                                        |
| `--completion <shell>`               | Genera un codice per l'autocompilazione nella shell. Supporta `fish` e `bash`.                                                              |
| `--decrypt-note <file>`              | Prints the decrypted text of an encrypted note file to the command line and exits                                                           |
| `--decrypt-note-password <password>` | Provides the password for `--decrypt-note` instead of prompting for it                                                                      |

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

::: tip
Per copiare direttaemnte le impostazioni di dump nel gestore degli appunti per inserirlo su un post in [GitHub issue](https://github.com/pbek/QOwnNotes/issues) puoi usare `xclip` o `xsel` su Linux, `pbcopy` su macOS e `clip` su Windows:

```bash
# su Linux copia le impostazioni di dump nel gestore degli appunti con xclip 
QOwnNotes --dump-settings | xclip -selection clipboard

# su Linux copia le impostazioni di dump nel gestore degli appunti con xsel 
QOwnNotes --dump-settings | xsel --clipboard

# su MacOS copia le impostazioni di dump nel gestore degli appunti con pbcopy 
QOwnNotes --dump-settings | pbcopy

# su Windows copia le impostazioni di dump nel gestore degli appunti con clip
QOwnNotes --dump-settings | clip
```

:::

## Print decrypted encrypted note text

With the parameter `--decrypt-note <file>` you can print the decrypted text of an encrypted note file to the command line. If no password is provided, QOwnNotes will ask for it on the command line.

```bash
QOwnNotes --decrypt-note secret.md
```

You can also provide the password directly with `--decrypt-note-password`:

```bash
QOwnNotes --decrypt-note secret.md --decrypt-note-password "my password"
```

::: warning
Providing passwords as command line parameters can expose them in your shell
history or process list. Prefer the password prompt if this is a concern.
:::

## Attiva le azioni del menu dopo l'avvio

Con il parametro `--action <name>` è possibile attivare azioni di menu dopo l'avvio dell'applicazione.

Ad esempio, per aprire la finestra di dialogo dell'elenco delle cose da fare dopo l'avvio si può usare:

```bash
QOwnNotes --action actionShow_Todo_List
```

::: tip
Puoi ottenere i nomi degli oggetti dell'azione del menu da [mainwindow.ui](https://github.com/pbek/QOwnNotes/blob/main/src/mainwindow.ui). Basta cercare il titolo del menu in inglese. Nota che questi testi possono cambiare nel tempo.
:::

Per attivare le [azioni scripting](../scripting/methods-and-objects.md#registering-a-custom-action) usa `customAction_` seguito dall'identificativo dell'azione personalizzata. L'identificativo dell'azione personalizzata è il primo parametro della chiamata a `script.registerCustomAction` nello script.

Per esempio, per eseguire l'azione personalizzata `myAction` esegui QOwnNotes così:

```bash
QOwnNotes --action customAction_miaAzione
```

::: tip
Se esegui QOwnNotes una seconda volta con un parametro action e hai permesso una sola istanza dell'applicazione, allora l'azione di menu sarà attivata solo nella prima istanza.
:::
