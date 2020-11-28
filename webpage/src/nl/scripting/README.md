# QOwnNotes-scripts

Een QOwnNotes-script is meestal **JavaScript** in [Qt QML-bestanden](https://doc.qt.io/qt-5/qtqml-index.html).

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

You can place those QML files anywhere you like and **add them in QOwnNotes** by adding them in the **Scripting settings** (there is a button `Add script`/`Add local script`).

::: tip
Bekijk de [example scripts](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples) om snel aan de slag te gaan.
:::

In de **Scripting settings** kun je ook scripts rechtstreeks vanuit de [**Script repository**](https://github.com/qownnotes/scripts) installeren.

Voor problemen, vragen of functieverzoeken voor scripts uit de **Script repository**, open een probleem op de [QOwnNotes script repository issue page](https://github.com/qownnotes/scripts/issues).

::: tip
Als u een script voor de **Script repository** wilt voorstellen, volg dan de instructies op [QOwnNotes script repository](https://github.com/qownnotes/scripts).
:::

Als u toegang nodig heeft tot een bepaalde functionaliteit in QOwnNotes of als u vragen of ideeën heeft, open dan een probleem op de [QOwnNotes issue page](https://github.com/pbek/QOwnNotes/issues).

::: tip
Voor logboekregistratie kunt u het `script.log()` commando gebruiken om in te loggen op de log widget.
:::