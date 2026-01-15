# Markdown Cheatsheet

Ez a Markdown-cheatsheet gyors leírása és bemutatása a QOwnNotes markdown szintaxisában.

## Headings

Használja a fejléceket a szövegek strukturálásához.

```markdown
# H1

## H2

### H3

#### H4

##### H5

###### H6
```

::: tip
A **Navigációs panel** mutatja a címsorok felépítését.
:::

Alternatív megoldásként H1 és H2 esetében aláhúzás-stílus stílus:

```markdown
Alt-H1
======

Alt-H2
------
```

::: tip
Alapértelmezés szerint a QOwnNotes létrehozza a jegyzet **fájlnevét** az **1 fejlécből** (h1).
:::

## Hangsúly

```markdown
Emphasis, aka italics, with *asterisks* or _underscores_.

Strong emphasis, aka bold, with **asterisks** or __underscores__.

Combined emphasis with **asterisks and _underscores_**.
```

::: tip
Használhatja a [parancsikont](./shortcuts.md) <kbd>Ctrl + B</kbd> a félkövér szöveg készítéséhez, és a <kbd>Ctrl + I</kbd> dőlt betűséhez.
:::

## Aláhúzás

There also is an optional setting to enable underline rendering in the _Preview settings_.

```markdown
_underline_
```

::: tip
A szöveg aláhúzásához használhatja a [parancsikont](./shortcuts.md) <kbd>Ctrl + U</kbd>.
:::

## Kihúz

```markdown
~~strike out~~
```

::: tip
Használhatja az [parancsikon](./shortcuts.md) <kbd>Alt + Shift + S</kbd> a szöveg törléséhez.
:::

## Listák

A listák létrehozásának számos módja van.

```markdown
1. Első rendezett listaelem
2. Another item

- Unordered sub-list.

1. Actual numbers don't matter, just that it's a number
1. And another item.

* Unordered list can use asterisks

- Or minuses

+ Or pluses
```

::: tip
Ha megnyomja az <kbd>Enter</kbd> gombot a lista végén, akkor egy új listaelem jön létre.
:::

## Linkek

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
Használhatja a [parancsikont](./shortcuts.md) <kbd>Ctrl + L</kbd> **hozzon létre linkeket weboldalakra vagy egyéb jegyzetekre**.

A <kbd>Ctrl + Shift + X</kbd> használatával megjelenik egy párbeszédpanel, amely segít **csatolni a mellékleteket** a jegyzetbe.

Nyomja meg a <kbd>Ctrl + szóköz</kbd> billentyűt, miközben a kurzor a jegyzet szerkesztésében egy linken van, hogy kövesse a linket.
:::

::: warning
Info Internal links with a title are not supported:

```markdown
[I link to the Journal.md note](Journal.md "Title not supported")
```

:::

### Bookmarks

A [QOwnNotes Web Companion böngészőbővítmény](./browser-extension.md) által használt könyvjelzők hivatkozásokat használnak a listákban.

```markdown
- [Webpage name](https://www.example.com) #tag1 #tag2 some description and tags
```

## Képek

A képek beágyazhatók a QOwnNotes programba. Megjelennek az előnézetben.

```markdown
![alt text](media/my-image.jpg)
```

::: tip
Használhatja a [parancsikont](./shortcuts.md) <kbd>Ctrl + Shift + I</kbd> a kép beszúrásához egy jegyzetbe. A kép a vágólapon is lehet, a párbeszédpanel észleli és előnézetet mutat.

A vágólapról közvetlenül beilleszthet egy képet a jegyzetébe a <kbd>Ctrl + Shift + V</kbd> gombbal.
:::

::: warning
Info Image links with a title are not supported:

```markdown
![alt text](media/my-image.jpg "Title not supported")
```

:::

## Beépített kód és kódblokkok

```markdown
Inline `code` has `backticks around` it.
```

::: tip
Használhatja a [parancsikont](./shortcuts.md) <kbd>Ctrl + Shift + C</kbd> a kiválasztott szöveges szövegen vagy közvetlenül a szöveg belsejében, hogy létrehozzon egy inline kódblokkot.
:::

Blocks of code are either fenced by lines with three backticks, or are indented with four spaces.

### 4-Spaces fence

Tegyen négy szóközt a kód elé, hogy kódblokkként jelölje meg.

```markdown
    s = "Code with space indent"
    print s
```

### Backtick fence

Három backticket is használhat egy kódblokk létrehozásához.
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
Nyomja meg az <kbd>Alt+Shift+T</kbd> billentyűkombinációt egy párbeszédpanel aktiválásához, amely segíthet a táblázatok létrehozásában. Akár CSV fájlokat is importálhat abban a párbeszédpanelen.

Használja a <kbd>Ctrl + szóköz</kbd>t egy jelölési táblázatban az automatikus formázáshoz.
:::

## Idézetek

```markdown
> Blockquotes are very handy in email to emulate reply text.
> This line is part of the same quote.

Quote break.

> This is a very long line that will still be quoted properly when it wraps. Oh boy let's keep writing to make sure this is long enough to actually wrap for everyone. Oh, you can _put_ **Markdown** into a blockquote.
```

::: tip
You can tell QOwnNotes to fully highlight blockquotes or just the blockquote character in the _Editor settings_

Használhatja a [parancsikont](./shortcuts.md) <kbd>Ctrl + Shift + B</kbd> a szöveg blokk idézetként történő megjelölésére.
:::

## Vízszintes vonal

Három módon lehet vízszintes szabályt kapni: kötőjel, csillag vagy aláhúzás.

```markdown
Three or more...

Hyphens

---

Asterisks

***

Underscores

___
```

## Sortörések

- Egy bekezdést több sorra is bonthat a könnyebb szerkesztés érdekében, de ezek továbbra is egyetlen bekezdésként jelennek meg törés nélkül.
- Kényszeríthet sortörést a bekezdésen belül, ha egy sort két szóközzel fejez be.
- Külön bekezdést készíthet úgy, hogy üres sorokkal határolja el.

::: tip
Két szóközt és egy újsort írhat be a <kbd>⇧ Shift</kbd> + <kbd>Return</kbd> billentyűkombinációval.
:::

```markdown
Here's a line for us to start with.

This line is separated from the one above by two newlines, so it will be a _separate paragraph_.

This line also begins a separate paragraph, but...  
This line is only separated by two trailing spaces and a single newline, so it's a separate line in the _same paragraph_.
```

::: tip
A szóközöket a szerkesztő alapértelmezés szerint kiemeli.
:::

## Hozzászólások

A jegyzet nem jelenik meg az előnézetben.

```markdown
[comment]: # "This comment will not appear in the preview"

<!-- HTML comments are also hidden -->
```

::: tip
A jegyzetek vezető html megjegyzésblokkja az automatikus jegyzetfájl-generálás során sem kerül figyelembe.
:::

## Jelölőnégyzet -listák

Létrehozhat egyszerű todo listákat jelölőnégyzet listákkal.

```markdown
- [x] done
- [ ] todo
```

::: tip
Az előnézetben bejelölheti / eltávolíthatja a jelölőnégyzeteket.
:::

## Frontmatter

A QOwnNotes alkalmazásban frontmatterrel (pl. YAML) adhat további meta-információkat. **nem jelenik meg az előnézetben**, és **nem zavarja az automatikus jegyzetfájlnév előállítását**.

```markdown
---
title: Some name
description: Some description
---

# Note headline starts here

Some text
```

Ennek a példafájlnak a neve `Note headline starts here.md` lenne.
