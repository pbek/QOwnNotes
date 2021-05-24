# Parancssori interfész paraméterek

Ezeket a paramétereket a parancssori felületen használhatja:

| Paraméter                    | Leírás                                                                                                                                   |
| ---------------------------- | ---------------------------------------------------------------------------------------------------------------------------------------- |
| `--help`                     | Megjeleníti a súgó képernyőt                                                                                                             |
| `--portable`                 | Hordozható üzemmódban fut az alkalmazás                                                                                                  |
| `--clear-settings`           | Törli a beállításokat és futtatja az alkalmazást                                                                                         |
| `--dump-settings`            | Kiírja a GitHub Markdown beállításainak és az alkalmazással és a környezettel kapcsolatos egyéb információkat, és kilép az alkalmazásból |
| `--session <name>`     | Az alkalmazást a beállításoknál és a belső fájloknál egy különböző környezetben futtat                                                   |
| `--allow-multiple-instances` | Lehetővé teszi, hogy a QOwnNotes több példányát indítsa, még akkor is, ha a beállításokban nincs engedélyezve                            |
| `--action <name>`      | Triggers a menu action after the application was started (see below)                                                                     |

::: tip Ha problémái adódnak a QOwnNotes telepítésével, akkor érdemes új beállításokkal indítani az alkalmazást anélkül, hogy elveszítené az aktuális beállításokat a `--session` paraméter használatával.

```bash
QOwnNotes --session test
```
:::

Különböző operációs rendszereken futtathatja az alkalmazást a parancssori felületen:

| Operációs rendszer | Parancs                                                      |
| ------------------ | ------------------------------------------------------------ |
| Linux              | `QOwnNotes` (vagy `qownnotes` ha kattanásként van telepítve) |
| macOS              | `/Applications/QOwnNotes.app/Contents/MacOS/QOwnNotes`       |
| Windows            | `QOwnNotes.exe`                                              |

## Trigger menu actions after startup

With the parameter `--action <name>` you can trigger menu actions after the application was started.

For example to open the todo list dialog after startup use:

```bash
QOwnNotes --action actionShow_Todo_List
```

::: tip You can get the object names of the menu action from [mainwindow.ui](https://github.com/pbek/QOwnNotes/blob/develop/src/mainwindow.ui). Just search for the English menu title. Note that these texts can change over time. :::

To trigger scripting actions use `customAction_` followed by the identifier of the custom action. For example to run the custom action `myAction` start QOwnNotes like this:

```bash
QOwnNotes --action customAction_myAction
```

The identifier of the custom action is the first parameter of the `script.registerCustomAction` call in the script.

::: tip If you run QOwnNotes a second time with the action parameter and only have allowed one application instance, then the menu action will be triggered in the first instance. :::
