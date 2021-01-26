# QOwnNotes szkriptek

A QOwnNotes szkript többnyire **JavaScript** [Qt QML fájlokban](https://doc.qt.io/qt-5/qtqml-index.html).

```js
import QtQml 2.0
import QOwnNotesTypes 1.0

Script {
    /**
        * Futtatásra kerül, amikor a parancsfájlmotor inicializálódik
        */
    function init() {
        script.log("Hello world!");
    }
}
```

Ezeket a QML fájlokat bárhová elhelyezheti, és **hozzáadhatja őket a QOwnNotes fájlokhoz** azáltal, hogy felveszi őket a **Szkriptbeállítások** ba (van egy gomb `Szkript hozzáadása` / `Helyi parancsfájl hozzáadása`).

::: tip A gyors kezdéshez nézze meg a [példa szkripteket](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples) :::

A **szkriptbeállítások** ban parancsfájlokat is telepíthet közvetlenül a [**parancsfájltárból**](https://github.com/qownnotes/scripts).

A **parancsfájltárból** származó szkriptekkel kapcsolatos problémákkal, kérdésekkel vagy szolgáltatáskérésekkel nyissa meg a problémát a [QOwnNotes szkripttárház kiadási oldalán](https://github.com/qownnotes/scripts/issues).

::: tip Ha javaslatot akarsz tenni egy szkriptre a **parancsfájl-tárházhoz**, kérjük, kövesd a [QOwnNotes szkripttárház](https://github.com/qownnotes/scripts) utasításait. :::

Ha hozzáférésre van szüksége a QOwnNotes bizonyos funkcióihoz, vagy kérdései vagy ötletei vannak, kérjük, nyisson meg egy problémát a [QOwnNotes probléma oldalán](https://github.com/pbek/QOwnNotes/issues).

::: tip Naplózáshoz a ` script.log () ` paranccsal jelentkezhet be a napló widgetbe. :::