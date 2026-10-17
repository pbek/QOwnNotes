# Buscar notas

Para buscar notas o texto dentro de las notas, puede usar el atajo <kbd>Ctrl</kbd> + <kbd>⇧ Shift</kbd> + <kbd>F</kbd> (o <kbd> ⌘ Cmd </kbd> + <kbd> ⇧ Shift </kbd> + <kbd> F </kbd> en macOS) para saltar al _Panel de búsqueda de notas_.

Hay diferentes formas de buscar notas:

- `cuaderno de notas` buscará notas que contengan tanto `cuaderno` como `notas` en el nombre del archivo o en el texto de la nota
- `"cuaderno de notas"` buscará notas con exactamente el texto `cuaderno de notas` en el nombre del archivo o el texto de la nota
- `n:cuaderno` o `name:cuaderno` buscará notas con `cuaderno` solo en el nombre de la nota o el nombre del archivo
- `n:"note book"` or `name:"note book"` will search for notes with exactly `note book` just in the note name or file name
- `w:note` or `word:note` will search for `note` as a whole word, excluding partial matches like `notebook`
- Search operators can be combined, for example `n:w:note` searches for the whole word `note` only in the note name or file name

![Buscando](/img/searching.png)

::: tip
La búsqueda comienza después de escribir al menos dos caracteres.
:::

## Ignoring accents

If you turn on _Ignore accents in search_ in the _Panels settings_, searches without accented letters will also find text with accented letters. For example, `avion` will also find `Avión`, and `strasse` will also find `Straße`.

- Searches that contain accented letters still only find exact matches, so `schön` will not find `schon`
- Only Latin letters are affected, characters of other scripts are always matched exactly
- The setting is used for the note search, the highlighting of the found text in the note editor and preview, and for the filters of the tag, note subfolder and navigation panels
