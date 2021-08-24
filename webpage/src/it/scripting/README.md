# QOwnNotes Scripting

Uno script QOwnNotes è principalmente **JavaScript** in [file Qt QML](https://doc.qt.io/qt-5/qtqml-index.html).

```js
import QtQml 2.0
import QOwnNotesTypes 1.0

Script {
    /**
        * Sarà eseguito quando il motore di scripting parte
        */
    function init() {
        script.log("Ciao mondo!");
    }
}
```

Potete mettere questi documenti QML ovunque preferite e **aggiungerli a QOwnNotes** tramite le **impostazioni di scripting** (c'è un bottone `Aggiungi script`/`Aggiungi script locale`).

::: tip
Dai un'occhiata agli [script di esempio](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples) per iniziare velocemente.
:::

Nelle **Impostazioni di scripting** puoi anche installare gli script direttamente dal [**Repository di script**](https://github.com/qownnotes/scripts).

Per problemi, domande o richieste di funzionalità per gli script dal **repository di script**, aprire un problema nella [pagina dei problemi del repository di script QOwnNotes](https://github.com/qownnotes/scripts/issues).

::: tip
Se vuoi proporre uno script per il **repository di script** segui le istruzioni in [repository di script QOwnNotes](https://github.com/qownnotes/scripts).
:::

Se hai bisogno di accedere a una determinata funzionalità in QOwnNotes o hai domande o idee, apri un problema nella [pagina del problema QOwnNotes](https://github.com/pbek/QOwnNotes/issues).

::: tip
Per il tracciamento puoi usare il comando `script.log()` per accedere al widget di registro.
:::