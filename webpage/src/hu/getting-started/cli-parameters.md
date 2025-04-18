# Parancssori interfész paraméterek

Ezeket a paramétereket használhatja a parancssori felületen (CLI) az alkalmazás viselkedésének szabályozására:

| Paraméter                    | Leírás                                                                                                                                   |
| ---------------------------- | ---------------------------------------------------------------------------------------------------------------------------------------- |
| `--help`                     | Megjeleníti a súgó képernyőt                                                                                                             |
| `--version`                  | Kinyomtatja az alkalmazás verziószámát                                                                                                   |
| `--portable`                 | Hordozható üzemmódban fut az alkalmazás                                                                                                  |
| `--clear-settings`           | Törli a beállításokat és futtatja az alkalmazást                                                                                         |
| `--dump-settings`            | Kiírja a GitHub Markdown beállításainak és az alkalmazással és a környezettel kapcsolatos egyéb információkat, és kilép az alkalmazásból |
| `--session <name>`     | Az alkalmazást a beállításoknál és a belső fájloknál egy különböző környezetben futtat                                                   |
| `--allow-multiple-instances` | Lehetővé teszi, hogy a QOwnNotes több példányát indítsa, még akkor is, ha a beállításokban nincs engedélyezve                            |
| `--action <name>`      | Menüműveletet indít el az alkalmazás elindítása után (lásd alább)                                                                        |
| `--completion <shell>` | Generate shell completion code. Supports `fish`, `bash`.                                                                                 |

::: tip
Ha problémái adódnak a QOwnNotes telepítésével, akkor érdemes új beállításokkal indítani az alkalmazást anélkül, hogy elveszítené az aktuális beállításokat a `--session` paraméter használatával.

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

## Indítsa el a menüműveleteket az indítás után

A ` --action <name> ` paraméterrel kiválaszthatja a menüműveleteket az alkalmazás elindítása után.

Például a Todo lista párbeszédpanel megnyitásához indítás után:

```bash
QOwnNotes --action actionShow_Todo_List
```

::: tip
You can get the object names of the menu action from [mainwindow.ui](https://github.com/pbek/QOwnNotes/blob/main/src/mainwindow.ui). Csak keresse meg az angol menü címét. Ne feledje, hogy ezek a szövegek idővel változhatnak.
:::

A [parancsfájl-műveletek](../scripting/methods-and-objects.md#registering-a-custom-action) elindításához használja a(z) `customAction_` elemet, amelyet az egyéni művelet azonosítója követ. Az egyéni művelet azonosítója a parancsfájl `script.registerCustomAction` hívásának első paramétere.

Például a `myAction` egyéni művelet futtatásához indítsa el a következőképpen a QOwnNotes programot:

```bash
QOwnNotes --action customAction_myAction
```

::: tip
Ha másodszor futtatja a QOwnNotes műveleti paramétert, és csak egy alkalmazáspéldányt engedélyezett, akkor a menüművelet első lépésben aktiválódik.
:::
