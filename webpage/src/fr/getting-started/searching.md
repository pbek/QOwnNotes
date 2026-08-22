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
