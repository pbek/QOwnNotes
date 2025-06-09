# Installa su Microsoft Windows™

Scarica l'ultimo **archivio di Windows** da [QOwnNotes Releases su GitHub](https://github.com/pbek/QOwnNotes/releases) (Cerca un fila nominato `QOwnNotes.zip`) ed estrailo dove preferisci. Non è necessaria alcuna installazione!

::: warning
Se vuoi usare l'**aggiornamento automatico** assicurati di decomprimerlo in un punto in cui il tuo account utente ha accesso in scrittura. Di norma il tuo account utente **non ha il permesso di scrittura** su posizioni come `C:\Programmi (x86)` o `C:\Programmi`.
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

There is a community-maintained package of QOwnNotes at [Chocolatey](https://chocolatey.org/packages/qownnotes/).

Puoi installarlo con:

```shell
choco install qownnotes
```

## Scoop

There is a [community-maintained package of QOwnNotes](https://github.com/ScoopInstaller/Extras/blob/master/bucket/qownnotes.json) at [Scoop](https://scoop.sh/). If you add the Extras bucket, you can use it to install QOwnNotes in portable mode.

```shell
scoop bucket add extras
scoop update
scoop install qownnotes
```

## WinGet

There is a [community-maintained package of QOwnNotes](https://github.com/microsoft/winget-pkgs/tree/master/manifests/p/pbek/QOwnNotes) for [WinGet](https://github.com/microsoft/winget-cli).

You can install it with:

```shell
winget install qownnotes
```

## MSYS2

There is a [community-maintained package of QOwnNotes](https://packages.msys2.org/base/mingw-w64-qownnotes) for [MSYS2](hhttps://www.msys2.org/).

You can install it with:

```shell
pacman -S mingw-w64-qownnotes
```
