# QOwnNotes szkriptek

A QOwnNotes szkript többnyire **JavaScript** [Qt QML fájlokban](https://doc.qt.io/qt-5/qtqml-index.html).

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

Ezeket a QML fájlokat bárhová elhelyezheti, és **hozzáadhatja őket a QOwnNotes fájlokhoz** azáltal, hogy felveszi őket a **Szkriptbeállítások** ba (van egy gomb `Szkript hozzáadása` / `Helyi parancsfájl hozzáadása`).

::: tip
Take a look at the [example scripts](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples) to get started fast.
:::

In the **Scripting settings** you can also install scripts directly from the [**Script repository**](https://github.com/qownnotes/scripts). There is a community hosted web-application to search for scripts on [beurt.github.io/QOwnNoteScriptsList](https://beurt.github.io/QOwnNoteScriptsList/).

A **parancsfájltárból** származó szkriptekkel kapcsolatos problémákkal, kérdésekkel vagy szolgáltatáskérésekkel nyissa meg a problémát a [QOwnNotes szkripttár kiadási oldalán](https://github.com/qownnotes/scripts/issues).

::: tip
Ha javaslatot akarsz tenni egy szkriptre a **parancsfájl-adattár**hoz, kérjük, kövesd a [QOwnNotes szkripttár](https://github.com/qownnotes/scripts) utasításait.
:::

Ha hozzáférésre van szüksége a QOwnNotes bizonyos funkcióihoz, vagy kérdései vagy ötletei vannak, kérjük, nyisson meg egy problémát a [QOwnNotes probléma oldalán](https://github.com/pbek/QOwnNotes/issues).

::: tip
For logging, you can use the `script.log()` command to log to the _Log panel_, which you can turn on in the _Window / Panels_ menu.
:::
