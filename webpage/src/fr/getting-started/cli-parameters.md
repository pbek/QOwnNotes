# Paramètres de l'interface de ligne de commande

Les paramètres suivants peuvent être utilisés dans l'interface en ligne de commande (CLI) pour contrôler le comportement de l'application :

| Paramètre                                  | Description                                                                                                                                                    |
| ------------------------------------------ | -------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `--help`                                   | Affiche l'écran d'aide                                                                                                                                         |
| `--version`                                | Affiche le numéro de version de l'application                                                                                                                  |
| `--portable`                               | Exécute l'application en mode portable                                                                                                                         |
| `--clear-settings`                         | Efface les préférences et exécute l'application                                                                                                                |
| `--dump-settings`                          | Affiche l'ensemble des préférences ainsi que d'autres informations à propos de l'application et de l'environnement en Markdown GitHub puis ferme l'application |
| `--session <name>`                   | Exécute l’application dans un contexte différent pour les préférences et fichiers internes                                                                     |
| `--allow-multiple-instances`               | Autorise le démarrage de plusieurs instances de QOwnNotes même si l'option est désactivée dans les préférences                                                 |
| `--action <name>`                    | Déclenche une action de menu après le démarrage de l'application (voir ci-dessous)                                                                             |
| `--completion <shell>`               | Génère la complétion automatique dans la console. Supporte `fish`, `bash`.                                                                                     |
| `--decrypt-note <file>`              | Affiche le texte déchiffré d’un fichier de note chiffré dans la ligne de commande puis quitte l’application                                                    |
| `--decrypt-note-password <password>` | Permet de donner le mot de passe pour `--decrypt-note` au lieu qu’il soit demandé                                                                              |

::: tip
Si vous rencontrez des problèmes avec votre installation de QOwnNotes, vous pouvez démarrer l'application avec de nouvelles préférences sans perdre vos préférences actuelles en utilisant le paramètre `--session`.

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
Pour copier directement les préférences dans le presse-papiers pour les poster dans un [ticket GitHub](https://github.com/pbek/QOwnNotes/issues), vous pouvez utiliser `xclip` ou `xsel` sous Linux, `pbcopy` sous macOS et `clip` sous Windows :

```bash
# Copier toutes les préférences dans le presse-papiers avec xclip sous Linux
QOwnNotes --dump-settings | xclip -selection clipboard

# Copier toutes les préférences dans le presse-papiers avec xsel sous Linux
QOwnNotes --dump-settings | xsel --clipboard

# Copier toutes les préférences dans le presse-papiers avec pbcopy sous macOS
QOwnNotes --dump-settings | pbcopy

# Copier toutes les préférences dans le presse-papiers avec clip sous Windows
QOwnNotes --dump-settings | clip
```

:::

## Afficher le texte déchiffré d’une note chiffrée

Avec le paramètre `--decrypt-note<file>`, il est possible d’afficher le texte déchiffré d’un fichier de note chiffré dans la ligne de commande. Si aucun mot de passe n’est fourni, QOwnNotes le demandera dans la ligne de commande.

```bash
QOwnNotes --decrypt-note secret.md
```

Il est également possible de donner directement le mot de passe avec `--decrypt-note-password` :

```bash
QOwnNotes --decrypt-note secret.md --decrypt-note-password "mon mot de passe"
```

::: warning
Donner des mots de passe dans des paramètres de ligne de commande peut les exposer dans
votre historique de shell ou dans votre liste de processus. Veuillez utiliser l’invite de mot de passe si c’est un problème.
:::

## Déclencher des actions de menu après le démarrage

Avec le paramètre `--action <name>` vous pouvez déclencher des actions de menu après que l'application ait été démarrée.

Par exemple, pour ouvrir la boîte de dialogue de la liste de tâches après le démarrage, utilisez :

```bash
QOwnNotes --action actionShow_Todo_List
```

::: tip
Vous pouvez obtenir les noms d'objet des actions de menu de [mainwindow.ui](https://github.com/pbek/QOwnNotes/blob/main/src/mainwindow.ui). Recherchez simplement le titre du menu en anglais. Notez que ces chaînes de caractères peuvent changer avec le temps.
:::

Pour déclencher des [actions de scripting](../scripting/methods-and-objects.md#registering-a-custom-action) utilisez `customAction_` suivi de l'identifiant de l'action personnalisée. L'identifiant de l'action personnalisée est le premier paramètre de l'appel `script.registerCustomAction` dans le script.

Par exemple, pour exécuter l'action personnalisée `myAction`, lancez QOwnNotes comme ceci :

```bash
QOwnNotes --action customAction_myAction
```

::: tip
Si vous exécutez QOwnNotes une deuxième fois avec le paramètre d'action et que vous n'avez autorisé qu'une seule instance d'application, l'action de menu sera déclenchée dans la première instance.
:::
