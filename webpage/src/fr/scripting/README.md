# Scripts QOwnNotes

Un script QOwnNotes est principalement du **JavaScript** dans des [fichiers Qt QML](https://doc.qt.io/qt-5/qtqml-index.html).

```js
import QtQml 2.0
import QOwnNotesTypes 1.0

Script {
    /**
     * Sera exécuté lors de l’initialisation du moteur de scripts
     */
    function init() {
        script.log("Hello world!");
    }
}
```

Vous pouvez placer ces fichiers QML où vous le souhaitez et **les ajouter dans QOwnNotes** en les ajoutant dans les **Préférences de script** (avec le bouton `Ajouter un script` / `Ajouter un script local`).

::: tip
Jetez un œil aux [exemples de scripts](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples) pour démarrer plus rapidement.
:::

Dans les **Préférences de script**, vous pouvez aussi installer des scripts directement depuis leur [**dépôt**](https://github.com/qownnotes/scripts). Il existe un site hébergé par la communauté sur : [https://beurt.github.io/QOwnNoteScriptsList](https://beurt.github.io/QOwnNoteScriptsList/) pour chercher des scripts.

Pour les problèmes, questions ou demandes de fonctionnalités pour les scripts du **dépôt de scripts**, veuillez ouvrir un ticket sur la [page des issues du dépôt de scripts QOwnNotes](https://github.com/qownnotes/scripts/issues).

::: tip
Si vous souhaitez proposer un script pour le **dépôt de scripts**, veuillez suivre les instructions du [dépôt de scripts QOwnNotes](https://github.com/qownnotes/scripts).
:::

Si vous avez besoin d'accéder à une certaine fonctionnalité dans QOwnNotes ou si vous avez des questions ou des idées, veuillez ouvrir un ticket sur la [page des issues QOwnNotes](https://github.com/pbek/QOwnNotes/issues).

::: tip
Pour la journalisation, vous pouvez utiliser la commande `script.log()` pour enregistrer dans le _Panneau Journal_, que vous pouvez activer dans le menu _Fenêtre / Panneaux_.
:::
