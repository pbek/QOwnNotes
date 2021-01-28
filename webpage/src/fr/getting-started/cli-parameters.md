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
