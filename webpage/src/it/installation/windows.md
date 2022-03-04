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

You need to download the ZIP file from [AppVeyor](https://ci.appveyor.com/project/pbek/qownnotes/build/artifacts) yourself and unzip it to a folder of your liking.

You can then directly run `QOwnNotes.exe` from that folder, no installation is required.

::: tip Info
The automatic update mechanism does not work with the AppVeyor build for Windows XP! You will have to download new releases yourself.
:::

## Chocolatey

There is a community maintained package of QOwnNotes at [Chocolatey](https://chocolatey.org/packages/qownnotes/).

You can install it with:

```shell
choco install qownnotes
```

## Scoop

There is a [community maintained package of QOwnNotes](https://github.com/ScoopInstaller/Extras/blob/master/bucket/qownnotes.json) at [Scoop](https://scoop.sh/). If you add the Extras bucket you can use it to install QOwnNotes in portable mode.

```shell
scoop bucket add extras
scoop update
scoop install qownnotes
```
