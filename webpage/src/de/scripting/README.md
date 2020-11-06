# QOwnNotes Scripting

Ein QOwnNotes-Skript besteht hauptsächlich aus ** JavaScript ** in [ Qt-QML-Dateien ](https://doc.qt.io/qt-5/qtqml-index.html).

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

You can place those QML files anywhere you like and **add use them in QOwnNotes** by adding them in the **Scripting settings** (there is a button `Add script`/`Add local script`).

::: tip
Schauen Sie sich die [ Beispielskripte ](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples) an, um schnell loszulegen.
:::

In den ** Skripteinstellungen ** können Sie Skripte auch direkt aus dem [** Skript-Repository **](https://github.com/qownnotes/scripts) installieren.

Bei Problemen, Fragen oder Funktionsanforderungen für Skripte aus dem ** Skript-Repository ** öffnen Sie bitte ein Problem auf der [ QOwnNotes-Skript-Repository-Problemseite ](https://github.com/qownnotes/scripts/issues).

::: tip
Wenn Sie ein Skript für das ** Skript-Repository ** vorschlagen möchten, befolgen Sie die Anweisungen unter [ QOwnNotes-Skript-Repository ](https://github.com/qownnotes/scripts).
:::

Wenn Sie Zugriff auf eine bestimmte Funktionalität in QOwnNotes benötigen oder Fragen oder Ideen haben, öffnen Sie bitte ein Problem auf der [ QOwnNotes-Problemseite ](https://github.com/pbek/QOwnNotes/issues).

::: tip
Für die Protokollierung können Sie den Befehl ` script.log () ` verwenden, um sich beim Protokoll-Widget anzumelden.
:::