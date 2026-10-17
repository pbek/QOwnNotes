# Recherche de notes

Pour rechercher des notes ou du texte à l'intérieur de notes, vous pouvez utiliser le raccourci <kbd>Ctrl</kbd> + <kbd>⇧ Maj</kbd> + <kbd>F</kbd> (ou <kbd>⌘ Cmd</kbd> + <kbd>⇧ Maj</kbd> + <kbd>F</kbd> sur macOS) pour accéder au _Panneau de recherche de notes_.

Il existe différentes manières de rechercher des notes :

- `note book` recherchera les notes avec à la fois le mot `note` et le mot `book` dans le nom du fichier ou le texte de la note
- `"note book"` recherchera des notes contenant exactement le texte `note book` dans le nom du fichier ou le texte de la note
- `n:book` ou `name:book` recherchera les notes avec exactement `book` dans le nom de la note ou le nom du fichier
- `n:"note book"` ou `name:"note book"` recherchera les notes avec exactement `note book` dans le nom de la note ou le nom du fichier
- `w:note` ou `word:note` recherchera le mot entier `note`, excluant les correspondances partielles comme `notebook`
- Les opérateurs de recherche peuvent être combinés, par exemple `n:w:note` cherche le mot entier `note` uniquement dans le nom de la note ou le nom de fichier

![Recherche](/img/searching.png)

::: tip
La recherche commence après avoir tapé au moins deux caractères.
:::

## Ignoring accents

If you turn on _Ignore accents in search_ in the _Panels settings_, searches without accented letters will also find text with accented letters. For example, `avion` will also find `Avión`, and `strasse` will also find `Straße`.

- Searches that contain accented letters still only find exact matches, so `schön` will not find `schon`
- Only Latin letters are affected, characters of other scripts are always matched exactly
- The setting is used for the note search, the highlighting of the found text in the note editor and preview, and for the filters of the tag, note subfolder and navigation panels
