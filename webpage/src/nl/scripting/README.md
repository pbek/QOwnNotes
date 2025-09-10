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

U kunt die QML-bestanden waar u maar wilt plaatsen en **toevoegen in QOwnNotes** door ze toe te voegen in de **Scripting-instellingen** (er is een knop `Script toevoegen`/`Lokaal script toevoegen`).

::: tip
Bekijk de [voorbeeldscripts](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples) snel aan de slag.
:::

In de **Scripting settings** kun je ook scripts rechtstreeks vanuit de [**Script repository**](https://github.com/qownnotes/scripts) installeren. Er is een community-host webapplicatie om naar scripts te zoeken op [ beurt.github.io/QOwnNoteScriptsList ](https://beurt.github.io/QOwnNoteScriptsList/).

Voor problemen, vragen of functieverzoeken voor scripts uit de **Script repository**, open een probleem op de [QOwnNotes script repository issue page](https://github.com/qownnotes/scripts/issues).

::: tip
Als u een script voor de **Script repository** wilt voorstellen, volg dan de instructies op [QOwnNotes script repository](https://github.com/qownnotes/scripts).
:::

Als u toegang nodig heeft tot een bepaalde functionaliteit in QOwnNotes of als u vragen of ideeën heeft, open dan een probleem op de [QOwnNotes issue page](https://github.com/pbek/QOwnNotes/issues).

::: tip
For logging, you can use the `script.log()` command to log to the _Log panel_, which you can turn on in the _Window / Panels_ menu.
:::
