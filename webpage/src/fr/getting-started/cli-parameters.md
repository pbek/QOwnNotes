# Paramètres de l'interface de ligne de commande

Vous pouvez utiliser ces paramètres sur l'interface de ligne de commande:

| Paramètre                    | La description                                                                                                                              |
| ---------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------- |
| `--help`                     | Affiche l'écran d'aide                                                                                                                      |
| `--portable`                 | Exécute l'application en mode portable                                                                                                      |
| `--clear-settings`           | Efface les paramètres et exécute l'application                                                                                              |
| `--dump-settings`            | Imprime un vidage des paramètres et d'autres informations sur l'application et l'environnement dans GitHub Markdown et quitte l'application |
| `--session <name>`     | Exécute l'application dans un contexte différent pour les paramètres et les fichiers internes                                               |
| `--allow-multiple-instances` | Permet de démarrer plusieurs instances de QOwnNotes même si elles ne sont pas autorisées dans les paramètres                                |
| `--action <name>`      | Triggers a menu action after the application was started (see below)                                                                        |

::: tip
If you run into troubles with your installation of QOwnNotes you may want start the application with fresh settings without losing your current settings by using the `--session` parameter.

```bash
QOwnNotes --session test
```
:::

Vous pouvez exécuter l'application sur l'interface de ligne de commande différemment sur différents systèmes d'exploitation:

| Système opérateur | Commander                                                                          |
| ----------------- | ---------------------------------------------------------------------------------- |
| Linux             | `QOwnNotes` (ou `qownnotes` si installé en tant que composant logiciel enfichable) |
| macOS             | `/Applications/QOwnNotes.app/Contents/MacOS/QOwnNotes`                             |
| Windows           | `QOwnNotes.exe`                                                                    |

## Déclencher des actions de menu après le démarrage

Avec le paramètre `--action <name>` vous pouvez déclencher des actions de menu après le démarrage de l'application.

Par exemple, pour ouvrir la boîte de dialogue de la liste de tâches après le démarrage, utilisez :

```bash
QOwnNotes --action actionShow_Todo_List
```

::: tip
Vous pouvez obtenir les noms d'objet de l'action de menu à partir de [mainwindow.ui](https://github.com/pbek/QOwnNotes/blob/develop/src/mainwindow.ui). Recherchez simplement le titre du menu en anglais. Notez que ces textes peuvent changer avec le temps.
:::

To trigger [scripting actions](../scripting/methods-and-objects.md#registering-a-custom-action) use `customAction_` followed by the identifier of the custom action. L'identifiant de l'action personnalisée est le premier paramètre de l'appel `script.registerCustomAction` dans le script.

Par exemple, pour exécuter l'action personnalisée `myAction`, lancez QOwnNotes comme ceci :

```bash
QOwnNotes --action customAction_myAction
```

::: tip
Si vous exécutez QOwnNotes une deuxième fois avec le paramètre d'action et que vous n'avez autorisé qu'une seule instance d'application, l'action de menu sera déclenchée dans la première instance.
:::
