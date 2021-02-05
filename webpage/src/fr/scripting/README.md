# QOwnNotes Scripting

Un script QOwnNotes est principalement **JavaScript** dans [fichiers Qt QML](https://doc.qt.io/qt-5/qtqml-index.html).

```js
importer QtQml 2.0
importer QOwnNotesTypes 1.0

Script {
     /**
         * Sera exécuté lors de l'initialisation du moteur de script
         */
     function init () {
         script.log ("Bonjour tout le monde!");
     }
}
```

Vous pouvez placer ces fichiers QML où vous le souhaitez et **les ajouter dans QOwnNotes** en les ajoutant dans les **Paramètres de script** (il y a un bouton `Ajouter un script` / `Ajouter un script local`).

::: tip
Jetez un œil aux [exemples de scripts](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples) pour commencer rapidement.
:::

Dans les **Paramètres de script**, vous pouvez également installer des scripts directement à partir du [**référentiel de scripts**](https://github.com/qownnotes/scripts).

Pour les problèmes, questions ou demandes de fonctionnalités pour les scripts du **référentiel de scripts**, veuillez ouvrir un problème sur la [page des problèmes du référentiel de scripts QOwnNotes](https://github.com/qownnotes/scripts/issues).

::: tip
Si vous souhaitez proposer un script pour le **référentiel de scripts**, veuillez suivre les instructions du [référentiel de scripts QOwnNotes](https://github.com/qownnotes/scripts).
:::

Si vous avez besoin d'accéder à une certaine fonctionnalité dans QOwnNotes ou si vous avez des questions ou des idées, veuillez ouvrir un problème sur la [page de problèmes QOwnNotes](https://github.com/pbek/QOwnNotes/issues).

::: tip
Pour la journalisation, vous pouvez utiliser la commande `script.log()` pour vous connecter au widget de journal.
:::