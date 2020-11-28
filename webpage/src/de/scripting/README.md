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
Schauen Sie sich die [Skriptbeispiele](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples) an, um schnell an den Start zu gehen.
:::

In den **Scripting settings** können Sie Skripte auch direkt aus dem [**Skript-Repository**](https://github.com/qownnotes/scripts)installieren.

Bei Problemen, Fragen oder Funktionsanforderungen für Skripte aus dem **Script repository**  eröffnen Sie bitte ein Problemthema auf der [QOwnNotes script repository issue page](https://github.com/qownnotes/scripts/issues).

::: tip
Wenn Sie ein Skript für das **Script repository** vorschlagen möchten, befolgen Sie die Anweisungen unter [QOwnNotes-script-repository](https://github.com/qownnotes/scripts).
:::

Wenn Sie Zugriff auf eine bestimmte Funktionalität in QOwnNotes benötigen oder Fragen oder Ideen haben, öffnen Sie bitte ein Problem auf der [QOwnNotes-Problemseite](https://github.com/pbek/QOwnNotes/issues).

::: tip
Für die Protokollierung können Sie den Befehl `script.log ()` verwenden, um sich beim Protokoll-Widget anzumelden.
:::