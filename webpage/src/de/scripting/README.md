# QOwnNotes Scripting

Ein QOwnNotes-Skript besteht hauptsächlich aus **JavaScript** in [Qt-QML-Dateien](https://doc.qt.io/qt-5/qtqml-index.html).

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

Sie können diese QML-Dateien an einer beliebigen Stelle platzieren und **in QOwnNotes** hinzufügen, indem Sie sie in den**Skripteinstellungen** hinzufügen (es gibt eine Schaltfläche `Skript hinzufügen` / `Lokales Skript hinzufügen`).

::: tip
Take a look at the [example scripts](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples) to get started fast.
:::

In the **Scripting settings** you can also install scripts directly from the [**Script repository**](https://github.com/qownnotes/scripts). There is a community hosted web-application to search for scripts on [beurt.github.io/QOwnNoteScriptsList](https://beurt.github.io/QOwnNoteScriptsList/).

Bei Problemen, Fragen oder Funktionsanforderungen für Skripte aus dem **Script repository**  eröffnen Sie bitte ein Problemthema auf der [QOwnNotes script repository issue page](https://github.com/qownnotes/scripts/issues).

::: tip
Wenn Sie ein Skript für das **Script repository** vorschlagen möchten, befolgen Sie die Anweisungen unter [QOwnNotes-script-repository](https://github.com/qownnotes/scripts).
:::

Wenn Sie Zugriff auf eine bestimmte Funktionalität in QOwnNotes benötigen oder Fragen oder Ideen haben, öffnen Sie bitte ein Problem auf der [QOwnNotes-Problemseite](https://github.com/pbek/QOwnNotes/issues).

::: tip
Für die Protokollierung können Sie den Befehl `script.log ()` verwenden, um sich beim Protokoll-Widget anzumelden.
:::
