# Wyszukiwanie notatek

To search for notes or for text inside notes you can use the shortcut <kbd>Ctrl</kbd> + <kbd>⇧ Shift</kbd> + <kbd>F</kbd> (or <kbd>⌘ Cmd</kbd> + <kbd>⇧ Shift</kbd> + <kbd>F</kbd> on macOS) to jump to the _Note search panel_.

Istnieją różne sposoby wyszukiwania notatek:

- `note book` will search for notes containing both `note` and `book` in the file name or note text
- `"note book"` wyszuka notatki ze słowem `note book` w nazwie pliku lub tekście notatki
- `n:book` lub `name:book` wyszuka notatki z `book` tylko w nazwie pliku lub notatce
- `n:"note book"` or `name:"note book"` will search for notes with exactly `note book` just in the note name or file name
- `w:note` or `word:note` will search for `note` as a whole word, excluding partial matches like `notebook`
- Search operators can be combined, for example `n:w:note` searches for the whole word `note` only in the note name or file name

![Wyszukiwanie](/img/searching.png)

::: tip
Searching starts after typing at least two characters.
:::
