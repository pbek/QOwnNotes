# Paramètres de l'interface de ligne de commande

Les paramètres suivants peuvent être utilisés dans l'interface en ligne de commande (CLI) pour contrôler le comportement de l'application :

| Paramètre                    | La description                                                                                                                                                |
| ---------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `--help`                     | Affiche l'écran d'aide                                                                                                                                        |
| `--version`                  | Affiche le numéro de version de l'application                                                                                                                 |
| `--portable`                 | Exécute l'application en mode portable                                                                                                                        |
| `--clear-settings`           | Efface les paramétrages et exécute l'application                                                                                                              |
| `--dump-settings`            | Affiche l'ensemble des paramètres ainsi que d'autres informations à propos de l'application et de l'environnement en MarkDown Github puis ferme l'application |
| `--session <name>`     | Exécute l'application dans un contexte de paramètres et de fichiers internes différent                                                                        |
| `--allow-multiple-instances` | Autorise l'exécution simultanée de plusieurs instances de QOwnNotes, même si cela est interdit dans les paramètres                                            |
| `--action <name>`      | Déclenche une action de menu après le démarrage de l'application (voir ci-dessous)                                                                            |
| `--completion <shell>` | Generate shell completion code. Supports `fish`, `bash`.                                                                                                      |

::: tip
Si vous rencontrez des problèmes avec votre installation de QOwnNotes, vous pouvez démarrer l'application avec de nouveaux paramètres sans perdre vos paramètres actuels en utilisant le paramètre `--session`.

```bash
QOwnNotes --session test
```

:::

Le lancement de l'application via la ligne de commande se fait différemment selon le système d'exploitation :

| Système d'exploitation | Commande                                                         |
| ---------------------- | ---------------------------------------------------------------- |
| Linux                  | `QOwnNotes` (ou `qownnotes` si installé en tant que paquet snap) |
| macOS                  | `/Applications/QOwnNotes.app/Contents/MacOS/QOwnNotes`           |
| Windows                | `QOwnNotes.exe`                                                  |

::: tip
To directly copy the settings dump to the clipboard to post in a [GitHub issue](https://github.com/pbek/QOwnNotes/issues) you can use `xclip` or `xsel` on Linux, `pbcopy` on macOS and `clip` on Windows:

```bash
# Copy the settings dump to the clipboard with xclip on Linux
QOwnNotes --dump-settings | xclip -selection clipboard

# Copy the settings dump to the clipboard with xsel on Linux
QOwnNotes --dump-settings | xsel --clipboard

# Copy the settings dump to the clipboard with pbcopy on macOS
QOwnNotes --dump-settings | pbcopy

# Copy the settings dump to the clipboard with clip on Windows
QOwnNotes --dump-settings | clip
```

:::

## Déclencher des actions de menu après le démarrage

Avec le paramètre `--action <name>` vous pouvez déclencher des actions de menu après que l'application ait été démarrée.

Par exemple, pour ouvrir la boîte de dialogue de la liste de tâches après le démarrage, utilisez :

```bash
QOwnNotes --action actionShow_Todo_List
```

::: tip
Vous pouvez obtenir les noms d'objet de l'action de menu avec [mainwindow.ui](https://github.com/pbek/QOwnNotes/blob/main/src/mainwindow.ui). Recherchez simplement le titre du menu en anglais. Notez que ces textes peuvent changer avec le temps.
:::

Pour déclencher des [actions de scripting](../scripting/methods-and-objects.md#registering-a-custom-action) utilisez `customAction_` suivi de l'identifiant de l'action personnalisée. L'identifiant de l'action personnalisée est le premier paramètre de l'appel `script.registerCustomAction` dans le script.

Par exemple, pour exécuter l'action personnalisée `myAction`, lancez QOwnNotes comme ceci :

```bash
QOwnNotes --action customAction_myAction
```

::: tip
Si vous exécutez QOwnNotes une deuxième fois avec le paramètre d'action et que vous n'avez autorisé qu'une seule instance d'application, l'action de menu sera déclenchée dans la première instance.
:::
