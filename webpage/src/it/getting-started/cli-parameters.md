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
