# Installa su Microsoft Windows™

Download the latest **Windows archive** from [QOwnNotes Releases on GitHub](https://github.com/pbek/QOwnNotes/releases) (look for a file called `QOwnNotes.zip`) and unzip it to anywhere you like. Non è necessaria alcuna installazione!

::: warning
Se vuoi usare l'**aggiornamento automatico** assicurati di decomprimerlo in un punto in cui il tuo account utente ha accesso in scrittura. By default, your user account most likely **doesn't have write access** to places like `C:\Program Files (x86)` or `C:\Program Files`.
:::

Puoi eseguire direttamente `QOwnNotes.exe` dalla cartella `QOwnNotes`, non è richiesta alcuna installazione.

### Modalità portatile

Usa `QOwnNotesPortable.bat` per eseguire QOwnNotes in **modalità portatile** dove tutto (comprese le note) sarà memorizzato solo nella tua cartella `QOwnNotes`.

::: tip
Non hai bisogno della modalità portatile se non disponi delle autorizzazioni di amministrazione per il tuo computer. QOwnNotes non ha bisogno di essere installato!
:::

## Windows XP

Qt ha abbandonato il supporto per Windows XP con la versione 5.8, ma QOwnNotes usa ancora Qt 5.7 per consentire agli utenti di Windows XP di utilizzarlo.

Devi scaricare il file Zip da [AppVeyor](https://ci.appveyor.com/project/pbek/qownnotes/build/artifacts) da solo e estrarlo in una cartella che preferisci.

Puoi eseguire direttamente `QOwnNotes.exe` da quella cartella, non è richiesta alcuna installazione.

::: tip
Info
Il meccanismo di aggiornamento automatico non funziona con la build AppVeyor per Windows XP!
Dovrai scaricare tu le nuove versioni.
:::

## Chocolatey

In [Chocolatey](https://chocolatey.org/packages/qownnotes/) c'è un pacchetto di QOwnNotes gestito dalla comunità.

Puoi installarlo con:

```shell
choco install qownnotes
```

## Scoop

In [Scoop](https://scoop.sh/) c'è un [pacchetto di QOwnNotes gestito dalla comunità](https://github.com/ScoopInstaller/Extras/blob/master/bucket/qownnotes.json). Se aggiungi il bucket Extras puoi usare QOwnNotes in modalità portabile.

```shell
scoop bucket add extras
scoop update
scoop install qownnotes
```
