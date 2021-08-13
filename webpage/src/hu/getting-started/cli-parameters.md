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
| `--action <name>`      | Menüműveletet indít el az alkalmazás elindítása után (lásd alább)                                                                        |

::: tip
If you run into troubles with your installation of QOwnNotes you may want start the application with fresh settings without losing your current settings by using the `--session` parameter.

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

## Indítsa el a menüműveleteket az indítás után

A ` --action <name> ` paraméterrel kiválaszthatja a menüműveleteket az alkalmazás elindítása után.

Például a Todo lista párbeszédpanel megnyitásához indítás után:

```bash
QOwnNotes --action actionShow_Todo_List
```

::: tip
A menümű objektumneveit innen szerezheti be [mainwindow.ui](https://github.com/pbek/QOwnNotes/blob/develop/src/mainwindow.ui). Csak keresse meg az angol menü címét. Ne feledje, hogy ezek a szövegek idővel változhatnak.
:::

A [parancsfájl-műveletek](../scripting/methods-and-objects.md#registering-a-custom-action) elindításához használja a(z) `customAction_` elemet, amelyet az egyéni művelet azonosítója követ. Az egyéni művelet azonosítója a parancsfájl `script.registerCustomAction` hívásának első paramétere.

Például a `myAction` egyéni művelet futtatásához indítsa el a következőképpen a QOwnNotes programot:

```bash
QOwnNotes --action customAction_myAction
```

::: tip
Ha másodszor futtatja a QOwnNotes műveleti paramétert, és csak egy alkalmazáspéldányt engedélyezett, akkor a menüművelet első lépésben aktiválódik.
:::
