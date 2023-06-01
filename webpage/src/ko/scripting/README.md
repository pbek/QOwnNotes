# QOwnNotes 스크립팅

A QOwnNotes script is mostly **JavaScript** in [Qt QML files](https://doc.qt.io/qt-5/qtqml-index.html).

```js
import QtQml 2.0
import QOwnNotesTypes 1.0

Script {
    /**
        * 스크립팅 엔진이 초기화될 때 실행됩니다
        */
    function init() {
        script.log("Hello world!");
    }
}
```

You can place those QML files anywhere you like and **add them in QOwnNotes** by adding them in the **Scripting settings** (there is a button `Add script`/`Add local script`).

::: 팁 [빠른 시작](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples)을 위한 예제 스크립트를 살펴보십시오. :::

In the **Scripting settings** you can also install scripts directly from the [**Script repository**](https://github.com/qownnotes/scripts). There is a community hosted web-application to search for scripts on [beurt.github.io/QOwnNoteScriptsList](https://beurt.github.io/QOwnNoteScriptsList/).

For issues, questions or feature requests for scripts from the **Script repository** please open an issue on the [QOwnNotes script repository issue page](https://github.com/qownnotes/scripts/issues).

::: tip If you want to propose a script for the **Script repository** please follow the instructions at [QOwnNotes script repository](https://github.com/qownnotes/scripts). :::

If you need access to a certain functionality in QOwnNotes or have questions or ideas please open an issue on the [QOwnNotes issue page](https://github.com/pbek/QOwnNotes/issues).

::: tip For logging you can use the `script.log()` command to log to the log widget. :::
