# Réduction

Cette feuille de triche Markdown est conçue comme une référence rapide et une vitrine de la syntaxe de Markdown dans QOwnNotes.

## Headings

Utilisez des titres pour structurer vos textes.

```markdown
# H1

## H2

### H3

#### H4

##### H5

###### H6
```

::: tip
Le **panneau de navigation** montre la structure de vos en-têtes.
:::

Alternativement, pour H1 et H2, un style souligné:

```markdown
# Alt-H1

## Alt-H2
```

::: tip
Par défaut, QOwnNotes crée le nom de fichier **d'une note** à partir de l'en-tête **1** (h1).
:::

## Accentuation

```markdown
Emphasis, aka italics, with _asterisks_.

Emphase forte, aka gras, avec **astérisques**.
```

::: tip
Vous pouvez utiliser le [raccourci](./shortcuts.md) <kbd>Ctrl + B</kbd> pour mettre le texte en gras et <kbd>Ctrl + I</kbd> pour le mettre en italique.
:::

## Souligné

There also is an optional setting to enable underline rendering in the _Preview settings_.

```markdown
_souligner_
```

::: tip
Vous pouvez utiliser le [raccourci](./shortcuts.md) <kbd>Ctrl + U</kbd> pour souligner un texte.
:::

## Rayer

```markdown
~~biffer~~
```

::: tip
Vous pouvez utiliser le [raccourci](./shortcuts.md) <kbd>Alt + Maj + S</kbd> pour biffer un texte.
:::

## Listes

Il existe de nombreuses façons de créer des listes.

```markdown
1. Premier élément de liste commandé
2. Another item

- Unordered sub-list.

1. Actual numbers don't matter, just that it's a number
1. Ordered sub-list (only works in the editor, not the preview)
1. Et un autre article.

- Unordered list can use asterisks

* Or minuses

- Or pluses
```

::: tip
Si vous appuyez sur <kbd>Entrée</kbd> à la fin d'une liste, un nouvel élément de liste sera créé.
:::

## Liens

Two of the simpler uses for links are pointing to webpages or other notes. There are multiple ways each of these links can look.

### External links

```markdown
[I'm an inline-style link](https://www.google.com)

[I'm an inline-style link with title](https://www.google.com "Google's Homepage")

[You can use numbers for reference-style link definitions][1]

Plain URLs and URLs in angle brackets will automatically get turned into links in the preview.
http://www.example.com ou <http://www.example.com>

[1]: https://www.qownnotes.org
```

### Internal links

You can link to other notes or attachments like this:

```markdown
[I link to the Journal.md note](Journal.md)

<Journal.md> works similarly.
```

::: tip
Vous pouvez utiliser le [raccourci](./shortcuts.md) <kbd> Ctrl + L</kbd> pour **créer des liens vers des pages Web ou d’autres notes**.

L'utilisation de <kbd>Ctrl + Maj + X</kbd> fera apparaître une boîte de dialogue qui vous aidera à **insérer des pièces jointes** dans votre note.

Vous pouvez appuyer sur <kbd>Ctrl + Espace</kbd> pendant que le curseur se trouve sur un lien dans l'édition de note pour suivre le lien.
:::

::: warning
Info Internal links with a title are not supported:

```markdown
[I link to the Journal.md note](Journal.md "Title not supported")
```

:::

### Bookmarks

Les signets utilisés par [l'extension de navigateur QOwnNotes Web Companion](./browser-extension.md) utilisent des liens dans des listes.

```markdown
- [Webpage name](https://www.example.com) #tag1 #tag2 some description and tags
```

## Images

Les images peuvent être intégrées dans QOwnNotes. Ils seront affichés dans l'aperçu.

```markdown
![alt text](media/my-image.jpg)
```

::: tip
Vous pouvez utiliser le [raccourci](./shortcuts.md) <kbd>Ctrl + Maj + I</kbd> pour insérer une image dans une note. L'image peut également être dans le presse-papiers, la boîte de dialogue la détectera et affichera un aperçu.

Vous pouvez également coller directement une image du presse-papiers dans votre note avec <kbd>Ctrl + Maj + V</kbd>.
:::

::: warning
Info Image links with a title are not supported:

```markdown
![alt text](media/my-image.jpg "Title not supported")
```

:::

## Code en ligne et blocs de code

```markdown
Inline `code` has `backticks around` it.
```

::: tip
Vous pouvez utiliser le [raccourci](./shortcuts.md) <kbd>Ctrl + Maj + C</kbd> sur texte en ligne sélectionné ou juste à l'intérieur du texte pour créer un bloc de code en ligne.
:::

Blocks of code are either fenced by lines with three backticks, or are indented with four spaces.

### 4-Spaces fence

Ajoutez quatre espaces devant votre code pour le marquer comme bloc de code.

```markdown
    s = "Code with space indent"
    print s
```

### Backtick fence

Vous pouvez également utiliser trois backticks pour créer un bloc de code.
````markdown
```
Code goes here
Code goes here
```
````

::: tip
You can use the [shortcut](./shortcuts.md) <kbd>Ctrl + Shift + C</kbd> on
multiple selected lines of text or in an empty line to create a code block.
:::

### Backtick fence with code highlighting

There also is some syntax highlighting with code blocks in QOwnNotes.

````markdown
```bash
# I am a comment
cd Notes
````
````

Currently, supported languages (and code block identifiers) are:

- BASh scripting, `bash`
- C, `c`
- C++, `cpp`
- C++, `cxx`
- C++, `c++`
- C#, `c#`
- CMake, `cmake`
- C#, `csharp`
- CSS, `css`
- Go, `go`
- HTML, `html`
- INI, `ini`
- Java, `java`
- JavaScript, `javascript`
- JavaScript, `js`
- JSON, `json`
- Makefile, `make`
- PHP, `php`
- Python, `py`
- Python, `python`
- QML, `qml`
- Rust, `rust`
- Shell scripting, `sh`
- SQL, `sql`
- TypeScript, `ts`
- TypeScript, `typescript`
- V, `v`
- Vex, `vex`
- XML, `xml`
- YAML, `yml`
- YAML, `yaml`

## Tables

Tables aren't part of the core Markdown spec, but the QOwnNotes preview supports them.

```markdown
Colons can be used to align columns.

| Tables        |      Are      |  Cool |
| ------------- | :-----------: | ----: |
| col 3 is      | right-aligned | $1600 |
| col 2 is      |   centered    |   $12 |
| zebra stripes |   are neat    |    $1 |

There must be at least 3 dashes separating each header cell.

You can also use inline Markdown.

| Markdown | Less      | Pretty     |
| -------- | --------- | ---------- |
| _Still_  | `renders` | **nicely** |
| 1        | 2         | 3          |
````

::: tip
Appuyez sur <kbd>Alt + Maj + T</kbd> pour faire apparaître une boîte de dialogue qui vous aidera à créer des tableaux. Vous pouvez même importer des fichiers CSV dans cette boîte de dialogue.

Utilisez <kbd>Ctrl + Espace</kbd> dans un tableau de démarquage pour le formater automatiquement.
:::

## Blocs de citation

```markdown
> Blockquotes are very handy in email to emulate reply text.
> This line is part of the same quote.

Quote break.

> This is a very long line that will still be quoted properly when it wraps. Oh boy let's keep writing to make sure this is long enough to actually wrap for everyone. Oh, you can _put_ **Markdown** into a blockquote.
```

::: tip
You can tell QOwnNotes to fully highlight blockquotes or just the blockquote character in the _Editor settings_

Vous pouvez utiliser le [raccourci](./shortcuts.md) <kbd>Ctrl + Maj + B</kbd> pour marquer le texte comme blockquote.
:::

## Règle horizontale

Il existe trois façons d'obtenir une règle horizontale: traits d'union, astérisques ou traits de soulignement.

```markdown
Three or more...

Hyphens

---

Asterisks

---

Underscores

---
```

## Sauts de ligne

- Vous pouvez construire un paragraphe comme un enchaînement de plusieurs lignes pour en faciliter l'édition, il sera tout de même affiché comme un seul paragraphe.
- Dans un paragraphe, vous pouvez forcer un retour à la ligne en insérant deux espaces à la fin d'une ligne.
- Vous pouvez définir un paragraphe distinct en le délimitant avec des sauts de ligne.

::: tip
Vous pouvez insérer deux espaces et un retour à la ligne avec <kbd>⇧ Shift</kbd> + <kbd>Entrée</kbd>.
:::

```markdown
Here's a line for us to start with.

This line is separated from the one above by two newlines, so it will be a _separate paragraph_.

This line is also begins a separate paragraph, but...  
This line is only separated by two trailing spaces and a single newline, so it's a separate line in the _same paragraph_.
```

::: tip
Les espaces de fin sont mis en évidence par défaut dans l'éditeur.
:::

## Commentaires

Les commentaires ne sont pas affichés dans l'aperçu.

```markdown
[comment]: # "This comment will not appear in the preview"

<!-- HTML comments are also hidden -->
```

::: tip
Un bloc de commentaire HTML en tête d'une note sera également ignoré dans la génération automatique du nom de fichier de la note.
:::

## Listes de cases à cocher

Vous pouvez créer des listes de tâches simples avec des listes de cases à cocher.

```markdown
- [x] done
- [ ] todo
```

::: tip
Vous pouvez cocher / décocher les cases à cocher dans l'aperçu.
:::

## Frontmatter

Dans QOwnNotes, vous pouvez utiliser un « frontmatter » (par exemple YAML) pour ajouter des méta-informations supplémentaires. Il ne sera **pas affiché dans l'aperçu** et ne perturbera **pas la génération automatique de nom de fichier de note**.

```markdown
---
title: Some name
description: Some description
---

# Note headline starts here

Some text
```

Le nom de fichier de cet exemple de note serait `L'en-tête de la note commence ici.md`.
