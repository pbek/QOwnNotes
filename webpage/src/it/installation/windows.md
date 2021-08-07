# Installa su Microsoft Windows™

Scarica l'ultimo **archivio di Windows** da [QOwnNotes Releases su GitHub](https://github.com/pbek/QOwnNotes/releases) e decomprimilo dove preferisci. Non è necessaria alcuna installazione!

::: warning
Se vuoi usare l'**aggiornamento automatico** assicurati di decomprimerlo in un punto in cui il tuo account utente ha accesso in scrittura. Per impostazione predefinita, molto probabilmente il tuo account utente **non ha accesso in scrittura** a posizioni come `C:\Program Files (x86)` o `C:\Program Files`.
:::

Puoi eseguire direttamente `QOwnNotes.exe` dalla cartella `QOwnNotes`, non è richiesta alcuna installazione.

### Modalità portatile

Usa `QOwnNotesPortable.bat` per eseguire QOwnNotes in **modalità portatile** dove tutto (comprese le note) sarà memorizzato solo nella tua cartella `QOwnNotes`.

::: tip
Non hai bisogno della modalità portatile se non disponi delle autorizzazioni di amministrazione per il tuo computer. QOwnNotes non ha bisogno di essere installato!
:::

## Windows XP

Qt ha abbandonato il supporto per Windows XP con la versione 5.8, ma QOwnNotes usa ancora Qt 5.7 per consentire agli utenti di Windows XP di utilizzarlo.

Se stai cercando  l'ultimo master build su [AppVeyor](https://ci.appveyor.com/project/pbek/qownnotes/history), scegli l'*ambiente Qt 5.7* e scaricane l'*artefatto*.

::: tip Info
Il meccanismo di aggiornamento automatico non funziona con la build AppVeyor per Windows XP! Dovrai scaricare tu le nuove versioni.
:::

## Chocolatey

In [Chocolatey](https://chocolatey.org/packages/qownnotes/) c'è un pacchetto di QOwnNotes gestito dalla comunità.

Puoi installarlo con:

```shell
choco install qownnotes
```
