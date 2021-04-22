# Installa su Microsoft Windows™

Scarica l'ultimo **archivio di Windows** da [QOwnNotes Releases su GitHub](https://github.com/pbek/QOwnNotes/releases) e decomprimilo dove preferisci. Non è necessaria alcuna installazione!

::: warning
Se desideri utilizzare l '** aggiornamento automatico ** assicurati di decomprimerlo in un punto in cui il tuo account utente ha accesso in scrittura. Per impostazione predefinita, molto probabilmente il tuo account utente **non ha accesso in scrittura** a posizioni come `C:\Program Files (x86)` o `C:\Program Files`.
:::

È quindi possibile eseguire direttamente `QOwnNotes.exe` dalla cartella `QOwnNotes`, non è richiesta alcuna installazione.

### Modalità portabile

Usa `QOwnNotesPortable.bat` per eseguire QOwnNotes in **modalità portatile** dove tutto (comprese le note) verrà memorizzato solo nella tua cartella `QOwnNotes`.

::: tip
Non hai bisogno della modalità portatile se non disponi delle autorizzazioni di amministrazione per il tuo computer. QOwnNotes non ha bisogno di essere installato!
:::

## Windows XP

Qt ha abbandonato il supporto per Windows XP con la versione 5.8, ma QOwnNotes ora è anche costruito con Qt 5.7 per consentire agli utenti di Windows XP di utilizzarlo ancora.

Anche se devi cercare tu stesso l'ultima build master da [AppVeyor](https://ci.appveyor.com/project/pbek/qownnotes/history), scegli l '*ambiente Qt 5.7* e scarica l' *artefatto*.

::: tip Info
Il meccanismo di aggiornamento automatico non funziona con la build AppVeyor per Windows XP! Dovrai scaricare tu stesso le nuove versioni.
:::

## Chocolatey

C'è un pacchetto gestito dalla comunità di QOwnNotes in [Chocolatey](https://chocolatey.org/packages/qownnotes/).

Puoi installarlo con:

```shell
choco install qownnotes
```
