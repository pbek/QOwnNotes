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

Si vous rencontrez des problèmes avec votre installation de QOwnNotes, vous voudrez peut-être commencer l'application avec de nouveaux paramètres sans perdre vos paramètres actuels en utilisant le paramètre `--session`.

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

## Trigger menu actions after startup

With the parameter `--action <name>` you can trigger menu actions after the application was started.

For example to open the todo list dialog after startup use:

```bash
QOwnNotes --action actionShow_Todo_List
```

::: tip
You can get the object names of the menu action from [mainwindow.ui](https://github.com/pbek/QOwnNotes/blob/develop/src/mainwindow.ui). Just search for the English menu title. Note that these texts can change over time.
:::

To trigger scripting actions use `customAction_` followed by the identifier of the custom action. For example to run the custom action `myAction` start QOwnNotes like this:

```bash
QOwnNotes --action customAction_myAction
```

The identifier of the custom action is the first parameter of the `script.registerCustomAction` call in the script.

::: tip
If you run QOwnNotes a second time with the action parameter and only have allowed one application instance, then the menu action will be triggered in the first instance.
:::
