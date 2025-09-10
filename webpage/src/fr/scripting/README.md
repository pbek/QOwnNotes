# QOwnNotes Scriptage

Un script QOwnNotes est principalement **JavaScript** dans des [fichiers Qt QML](https://doc.qt.io/qt-5/qtqml-index.html).

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

Vous pouvez placer ces fichiers QML où vous le souhaitez et **les ajouter dans QOwnNotes** en les ajoutant dans les **Paramètres de script** (il y a un bouton `Ajouter un script` / `Ajouter un script local`).

::: tip
Jetez un œil aux [exemples de scripts](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples) pour démarrer plus rapidement.
:::

Dans les **paramètres de script**, vous pouvez aussi installer des scripts directement depuis leur [**dépôt**](https://github.com/qownnotes/scripts). Il y a un site hébergé par la communauté à : [https://beurt.github.io/QOwnNoteScriptsList](https://beurt.github.io/QOwnNoteScriptsList/) pour chercher des scripts.

Pour les problèmes, questions ou demandes de fonctionnalités pour les scripts du **référentiel de scripts**, veuillez ouvrir un problème sur la [page des problèmes du référentiel de scripts QOwnNotes](https://github.com/qownnotes/scripts/issues).

::: tip
Si vous souhaitez proposer un script pour le **référentiel de scripts**, veuillez suivre les instructions du [référentiel de scripts QOwnNotes](https://github.com/qownnotes/scripts).
:::

Si vous avez besoin d'accéder à une certaine fonctionnalité dans QOwnNotes ou si vous avez des questions ou des idées, veuillez ouvrir un problème sur la [page de problèmes QOwnNotes](https://github.com/pbek/QOwnNotes/issues).

::: tip
For logging, you can use the `script.log()` command to log to the _Log panel_, which you can turn on in the _Window / Panels_ menu.
:::
