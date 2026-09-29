# Searching for notes

To search for notes or for text inside notes you can use the shortcut
<kbd>Ctrl</kbd> + <kbd>⇧ Shift</kbd> + <kbd>F</kbd> (or
<kbd>⌘ Cmd</kbd> + <kbd>⇧ Shift</kbd> + <kbd>F</kbd> on macOS) to jump to the _Note search panel_.

There are different ways to search for notes:

- `note book` will search for notes containing both `note` and `book` in the file
  name or note text
- `"note book"` will search for notes with exactly the text `note book` in the
  file name or note text
- `n:book` or `name:book` will search for notes with `book` just in the note
  name or file name
- `n:"note book"` or `name:"note book"` will search for notes with exactly
  `note book` just in the note name or file name
- `w:note` or `word:note` will search for `note` as a whole word, excluding
  partial matches like `notebook`
- Search operators can be combined, for example `n:w:note` searches for the
  whole word `note` only in the note name or file name

![Searching](/img/searching.png)

::: tip
Searching starts after typing at least two characters.
:::

## Ignoring accents

If you turn on _Ignore accents in search_ in the _Panels settings_, searches
without accented letters will also find text with accented letters. For example,
`avion` will also find `Avión`, and `strasse` will also find `Straße`.

- Searches that contain accented letters still only find exact matches, so
  `schön` will not find `schon`
- Only Latin letters are affected, characters of other scripts are always
  matched exactly
- The setting is used for the note search, the highlighting of the found text
  in the note editor and preview, and for the filters of the tag, note subfolder
  and navigation panels
