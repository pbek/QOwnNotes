# Scripting di QOwnNotes

Uno script QOwnNotes è principalmente **JavaScript** in [file Qt QML](https://doc.qt.io/qt-5/qtqml-index.html).

```js
import QtQml 2.0
import QOwnNotesTypes 1.0

Script {
    /**
     * Will be run when the scripting engine initializes
     */
    function init() {
        script.log("Hello world!");
    }
}
```

Potete mettere questi documenti QML ovunque preferite e **aggiungerli a QOwnNotes** tramite le **impostazioni di scripting** (c'è un bottone `Aggiungi script`/`Aggiungi script locale`).

::: tip
Dai un'occhiata agli [script di esempio](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples) per iniziare velocemente.
:::

Nelle **impostazioni di Scripting** puoi anche installare script dall'[**archivio degli script**](https://github.com/qownnotes/scripts). E' presente un'applicazione web creata dalla community per cercare scripts su [beurt.github.io/QOwnNoteScriptsList](https://beurt.github.io/QOwnNoteScriptsList/).

Per problemi, domande o richieste di funzionalità per gli script dal **repository di script**, aprire un problema nella [pagina dei problemi del repository di script QOwnNotes](https://github.com/qownnotes/scripts/issues).

::: tip
Se vuoi proporre uno script per il **repository di script** segui le istruzioni in [repository di script QOwnNotes](https://github.com/qownnotes/scripts).
:::

Se hai bisogno di accedere a una determinata funzionalità in QOwnNotes o hai domande o idee, apri un problema nella [pagina del problema QOwnNotes](https://github.com/pbek/QOwnNotes/issues).

::: tip
For logging, you can use the `script.log()` command to log to the _Log panel_, which you can turn on in the _Window / Panels_ menu.
:::
