# Argumenty interfejsu wiersza poleceń

Możesz użyć tych parametrów w interfejsie wiersza poleceń (CLI), aby kontrolować zachowanie aplikacji:

| Parametr                     | Opis                                                                                                                          |
| ---------------------------- | ----------------------------------------------------------------------------------------------------------------------------- |
| `--help`                     | Wyświetla ekran pomocy                                                                                                        |
| `--version`                  | Wyświetla numer wersji aplikacji                                                                                              |
| `--portable`                 | Uruchamia aplikację w trybie przenośnym                                                                                       |
| `--clear-settings`           | Czyści ustawienia i uruchamia aplikację                                                                                       |
| `--dump-settings`            | Wyświetla zrzut ustawień i innych informacji o programie i środowisku w formacie GitHub Markdown, a następnie zamyka program. |
| `--session <name>`     | Uruchamia aplikację w innym kontekście – dotyczy ustawień i plików wewnętrznych.                                              |
| `--allow-multiple-instances` | Pozwala na uruchomienie kilku instancji QOwnNotes, nawet jeśli jest to niedozwolone w ustawieniach.                           |
| `--action <name>`      | Uruchamia akcję menu po uruchomieniu aplikacji (patrz poniżej)                                                                |

::: tip
Jeśli napotkasz problemy z instalacją QOwnNotes, możesz uruchomić aplikację ze świeżymi ustawieniami bez utraty bieżących ustawień, używając parametru `--session`.

```bash
QOwnNotes --session test
```
:::

Możesz uruchomić aplikację poprzez wiersz poleceń na różnych systemach operacyjnych:

| System operacyjny | Komenda                                                     |
| ----------------- | ----------------------------------------------------------- |
| Linux             | `QOwnNotes` (lub `qownnotes` jeśli zainstalowany jako snap) |
| macOS             | `/Applications/QOwnNotes.app/Contents/MacOS/QOwnNotes`      |
| Windows           | `QOwnNotes.exe`                                             |

## Wyzwalaj akcje menu po uruchomieniu

Za pomocą parametru `--action <name>` możesz wywołać akcje menu po uruchomieniu aplikacji.

Na przykład, aby otworzyć okno dialogowe listy rzeczy do zrobienia po uruchomieniu, użyj:

```bash
QOwnNotes --action actionShow_Todo_List
```

::: tip
Możesz uzyskać nazwy obiektów akcji menu z [główne okno.ui](https://github.com/pbek/QOwnNotes/blob/develop/src/mainwindow.ui). Po prostu wyszukaj tytuł menu w języku angielskim. Pamiętaj, że te teksty mogą się zmieniać w czasie.
:::

Aby uruchomić [działania skryptowe](../scripting/methods-and-objects.md#registering-a-custom-action) użyj `customAction_`, po którym następuje identyfikator akcji niestandardowej. Identyfikator akcji niestandardowej jest pierwszym parametrem wywołania `script.registerCustomAction` w skrypcie.

Na przykład, aby uruchomić niestandardową akcję `myAction`, uruchom QOwnNotes w ten sposób:

```bash
QOwnNotes --action customAction_myAction
```

::: tip
Jeśli uruchomisz QOwnNotes po raz drugi z parametrem akcji i zezwolisz tylko na jedną instancję aplikacji, akcja menu zostanie wyzwolona w pierwszej instancji.
:::
