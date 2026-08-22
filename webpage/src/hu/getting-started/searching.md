# Jegyzetek keresése

To search for notes or for text inside notes you can use the shortcut <kbd>Ctrl</kbd> + <kbd>⇧ Shift</kbd> + <kbd>F</kbd> (or <kbd>⌘ Cmd</kbd> + <kbd>⇧ Shift</kbd> + <kbd>F</kbd> on macOS) to jump to the _Note search panel_.

A jegyzetek keresésére többféle lehetőség van:

- `note book` will search for notes containing both `note` and `book` in the file name or note text
- A `"jegyzetfüzet"` olyan jegyzeteket keres, amelyek pontosan a `jegyzetfüzet` szöveget tartalmazzák a fájlnévben vagy a jegyzetszövegben
- Az `n:könyv` vagy a `név:könyv` feljegyzéseket keres a `könyv` felirattal, csak a jegyzet vagy fájl nevében.
- `n:"note book"` or `name:"note book"` will search for notes with exactly `note book` just in the note name or file name
- `w:note` or `word:note` will search for `note` as a whole word, excluding partial matches like `notebook`
- Search operators can be combined, for example `n:w:note` searches for the whole word `note` only in the note name or file name

![Keresés](/img/searching.png)

::: tip
Searching starts after typing at least two characters.
:::
