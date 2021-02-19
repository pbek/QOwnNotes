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
