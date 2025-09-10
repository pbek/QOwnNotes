# Obniżka cen

Ta ściągawka Markdown ma służyć jako szybkie odniesienie i prezentacja składni Markdown w QOwnNotes.

## Headings

Użyj nagłówków, aby uporządkować swoje teksty.

```markdown
# H1

## H2

### H3

#### H4

##### H5

###### H6
```

::: tip
**Panel nawigacyjny** przedstawia strukturę nagłówków.
:::

Alternatywnie, dla H1 i H2, styl podkreślony:

```markdown
# Alt-H1

## Alt-H2
```

::: tip
Domyślnie QOwnNotes tworzy **nazwę pliku notatki** z **nagłówka 1** (h1).
:::

## Nacisk

```markdown
Emphasis, aka italics, with _asterisks_.

Silne wyróżnienie, czyli pogrubienie, z **gwiazdkami**.
```

::: tip
Aby pogrubić tekst, możesz użyć [skrótu](./shortcuts.md) <kbd>Ctrl + B</kbd> i <kbd>Ctrl + I</kbd>, aby uczynić go kursywą.
:::

## Podkreślenie

There also is an optional setting to enable underline rendering in the _Preview settings_.

```markdown
_podkreślać_
```

::: tip
Możesz użyć [skrótu](./shortcuts.md) <kbd>Ctrl + U</kbd>, aby podkreślić tekst.
:::

## Skreślony

```markdown
~~strike out~~
```

::: tip
Możesz użyć [skrótu](./shortcuts.md) <kbd>Alt + Shift + S</kbd>, aby przekreślić tekst.
:::

## Listy

Istnieje wiele sposobów tworzenia list.

```markdown
1. Pierwsza zamówiona pozycja na liście
2. Another item

- Unordered sub-list.

1. Actual numbers don't matter, just that it's a number
1. Ordered sub-list (only works in the editor, not the preview)
1. I kolejny przedmiot.

- Unordered list can use asterisks

* Or minuses

- Or pluses
```

::: tip
Jeśli naciśniesz <kbd>Enter</kbd> na końcu listy, zostanie utworzony nowy element listy.
:::

## Łączy

Two of the simpler uses for links are pointing to webpages or other notes. There are multiple ways each of these links can look.

### External links

```markdown
[I'm an inline-style link](https://www.google.com)

[I'm an inline-style link with title](https://www.google.com "Google's Homepage")

[You can use numbers for reference-style link definitions][1]

Plain URLs and URLs in angle brackets will automatically get turned into links in the preview.
http://www.example.com or <http://www.example.com>

[1]: https://www.qownnotes.org
```

### Internal links

You can link to other notes or attachments like this:

```markdown
[I link to the Journal.md note](Journal.md)

<Journal.md> works similarly.
```

::: tip
Możesz użyć [skrótu](./shortcuts.md) <kbd>Ctrl + L</kbd>, aby **utworzyć łącza do stron internetowych lub innych notatek**.

Użycie klawiszy <kbd>Ctrl + Shift + X</kbd> spowoduje wyświetlenie okna dialogowego ułatwiającego **wstawianie załączników** do notatki.

Możesz nacisnąć klawisze <kbd>Ctrl + spacja</kbd>, gdy kursor znajduje się na łączu w edycji notatki, aby podążać za linkiem.
:::

::: warning
Info Internal links with a title are not supported:

```markdown
[I link to the Journal.md note](Journal.md "Title not supported")
```

:::

### Bookmarks

Zakładki używane przez rozszerzenie przeglądarki [QOwnNotes Web Companion](./browser-extension.md) używają łączy na listach.

```markdown
- [Webpage name](https://www.example.com) #tag1 #tag2 some description and tags
```

## Obrazy

Obrazy można osadzać w QOwnNotes. Zostaną wyświetlone w podglądzie.

```markdown
![alt text](media/my-image.jpg)
```

::: tip
Możesz użyć [skrótu](./shortcuts.md) <kbd>Ctrl + Shift + I</kbd>, aby wstawić obraz do notatki. Obraz może również znajdować się w schowku, okno dialogowe wykryje go i pokaże podgląd.

Możesz także wkleić obraz ze schowka bezpośrednio do notatki za pomocą <kbd>Ctrl + Shift + V</kbd>.
:::

::: warning
Info Image links with a title are not supported:

```markdown
![alt text](media/my-image.jpg "Title not supported")
```

:::

## Kod wbudowany i bloki kodu

```markdown
Inline `code` has `backticks around` it.
```

::: tip
Możesz użyć [skrótu](./shortcuts.md) <kbd>Ctrl + Shift + C</kbd> na zaznaczonym tekście wbudowanym lub bezpośrednio w tekście, aby utworzyć wbudowany blok kodu.
:::

Blocks of code are either fenced by lines with three backticks, or are indented with four spaces.

### 4-Spaces fence

Dodaj cztery spacje przed kodem, aby oznaczyć go jako blok kodu.

```markdown
    s = "Code with space indent"
    print s
```

### Backtick fence

Możesz także użyć trzech lewych apostrofów, aby utworzyć blok kodu.
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
Naciśnij klawisze <kbd>Alt + Shift + T</kbd>, aby aktywować okno dialogowe, które może pomóc w tworzeniu tabel. Możesz nawet importować pliki CSV w tym oknie dialogowym.

Użyj <kbd>Ctrl + spacja</kbd> wewnątrz tabeli przecen, aby automatycznie ją sformatować.
:::

## Cytaty blokowe

```markdown
> Blockquotes are very handy in email to emulate reply text.
> This line is part of the same quote.

Quote break.

> This is a very long line that will still be quoted properly when it wraps. Oh boy let's keep writing to make sure this is long enough to actually wrap for everyone. Oh, you can _put_ **Markdown** into a blockquote.
```

::: tip
You can tell QOwnNotes to fully highlight blockquotes or just the blockquote character in the _Editor settings_

Możesz użyć [skrótu](./shortcuts.md) <kbd>Ctrl + Shift + B</kbd>, aby zaznaczyć tekst jako cytat blokowy.
:::

## Linia pozioma

Istnieją trzy sposoby uzyskania reguły horyzontalnej: łączniki, gwiazdki lub podkreślenia.

```markdown
Three or more...

Hyphens

---

Asterisks

---

Underscores

---
```

## Podziały wierszy

- Możesz podzielić akapit na więcej niż jeden wiersz, aby ułatwić edycję, nadal renderują się jako pojedynczy akapit bez przerw.
- Podział wiersza wewnątrz akapitu można wymusić, kończąc wiersz dwiema spacjami.
- Możesz utworzyć oddzielny akapit, oddzielając go pustymi wierszami.

::: tip
Możesz wprowadzić dwie spacje i nowy wiersz za pomocą <kbd>⇧ Shift</kbd> + <kbd>Return</kbd>.
:::

```markdown
Here's a line for us to start with.

This line is separated from the one above by two newlines, so it will be a _separate paragraph_.

This line is also begins a separate paragraph, but...  
This line is only separated by two trailing spaces and a single newline, so it's a separate line in the _same paragraph_.
```

::: tip
Spacje końcowe są domyślnie podświetlone w edytorze.
:::

## Komentarze

Komentarze nie są wyświetlane w podglądzie.

```markdown
[comment]: # "This comment will not appear in the preview"

<!-- HTML comments are also hidden -->
```

::: tip
Wiodący blok komentarza html w notatce zostanie również zignorowany podczas automatycznego generowania nazwy pliku notatki.
:::

## Listy wyboru

Możesz tworzyć proste listy rzeczy do zrobienia z listami wyboru.

```markdown
- [x] done
- [ ] todo
```

::: tip
Możesz zaznaczyć / odznaczyć pola wyboru w podglądzie.
:::

## Frontmatter

W QOwnNotes możesz użyć frontmatera (np.YAML), aby dodać dodatkowe meta informacje. Nie będzie on **wyświetlany w podglądzie** i nie będzie **przeszkadzać w automatycznym generowaniu nazw plików z notatkami**.

```markdown
---
title: Some name
description: Some description
---

# Note headline starts here

Some text
```

Nazwa pliku tej przykładowej notatki byłaby następująca: `Nagłówek notatki zaczyna się tutaj.md`.
