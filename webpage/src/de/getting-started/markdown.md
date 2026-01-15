# Markdown Cheatsheet

Dieses Markdown-Cheatsheet dient als Kurzreferenz und Beispiel für die Markdown-Syntax in QOwnNotes.

## Überschriften

Verwenden Sie Überschriften, um Ihre Texte zu strukturieren.

```markdown
# H1

## H2

### H3

#### H4

##### H5

###### H6
```

::: tip
Das Bedienfeld **Navigation** zeigt die Struktur Ihrer Überschriften.
:::

Alternativ für H1 und H2 eine unterstrichene Schriftweise:

```markdown
Alt-H1
======

Alt-H2
------
```

::: tip
Standardmäßig erstellt QOwnNotes den **Dateinamen einer Notiz** aus der **Überschrift 1** (h1).
:::

## Hervorhebung

```markdown
Emphasis, aka italics, with *asterisks* or _underscores_.

Strong emphasis, aka bold, with **asterisks** or __underscores__.

Combined emphasis with **asterisks and _underscores_**.
```

::: tip
Sie können die [Tastenkombination](./shortcuts.md) <kbd>Strg + B</kbd> verwenden, um Text fett hervorzuheben, und <kbd>Strg + I</kbd>, um ihn kursiv hervorzuheben.
:::

## Unterstreichen

There also is an optional setting to enable underline rendering in the _Preview settings_.

```markdown
_unterstreichen_
```

::: tip
Sie können den [Shortcut](./shortcuts.md) <kbd>Strg + U</kbd> verwenden, um einen Text zu unterstreichen.
:::

## Durchstreichen

```markdown
~~durchstreichen~~
```

::: tip
Sie können den [Shortcut](./shortcuts.md) <kbd>Alt + Umschalt + S</kbd> verwenden, um einen Text durchzustreichen.
:::

## Listen

Es gibt viele Möglichkeiten, Listen zu erstellen.

```markdown
1. Erster geordneter Listeneintrag
2. Another item

- Unordered sub-list.

1. Actual numbers don't matter, just that it's a number
1. And another item.

* Unordered list can use asterisks

- Or minuses

+ Or pluses
```

::: tip
Wenn Sie am Ende einer Liste die <kbd>Enter</kbd> -Taste drücken, wird ein neues Listenelement erstellt.
:::

## Links

Zwei einfachere Verwendungen von Links ist das Verweisen auf Webseiten oder andere Notizen. Es gibt mehrere Arten, wie jeder dieser Links aussehen kann.

### Externe Links

```markdown
[Ich bin ein inline-Link](https://www.google.com)

[Ich bin ein inline-Link mit Titel](https://www.google.com "Google's Homepage")

[Sie können Nummern für Referenzlink-Definitionen benutzen][1]

Klartext URLs und URLs in spitzen Klammern werden in der Vorschau automatisch zu Links.
http://www.example.com oder <http://www.example.com>

[1]: https://www.qownnotes.org
```

### Interne Links

You can link to other notes or attachments like this:

```markdown
[Ich verlinke zur Notiz Journal.md](Journal.md)

<Journal.md> funktioniert ähnlich.
```

::: tip
Sie können den [shortcut](./shortcuts.md) <kbd>Ctrl + L</kbd> verwenden: **Erstellen Sie Links zu Webseiten oder anderen Notizen**.

Wenn Sie <kbd>Strg + Umschalt + X</kbd> verwenden, wird ein Dialogfeld angezeigt, durch das Sie in Ihre Notiz **Anhänge einfügen** können.

Sie können <kbd>Strg + Leertaste</kbd> drücken, während sich der Cursor auf einem Link in der Notizbearbeitung befindet, um dem Link zu folgen.
:::

::: warning
Info Internal links with a title are not supported:

```markdown
[I link to the Journal.md note](Journal.md "Title not supported")
```

:::

### Lesezeichen

Die von der [QOwnNotes Web Companion-Browsererweiterung](./browser-extension.md) verwendeten Lesezeichen verwenden Links in Listen.

```markdown
- [Name der Webseite](https://www.example.com) #tag1 #tag2 eine Beschreibung und Schlagworte
```

## Bilder

Bilder können in QOwnNotes eingebettet werden. Sie werden in der Vorschau angezeigt.

```markdown
![alternativer Text](media/my-image.jpg)
```

::: tip
Sie können die [Tastenkombination](./shortcuts.md) <kbd>Strg + Umschalt + I</kbd> verwenden, um ein Bild in eine Notiz einzufügen. Das Bild kann sich auch in der Zwischenablage befinden. Das Dialogfeld erkennt es und zeigt eine Vorschau an.

Sie können ein Bild auch direkt aus der Zwischenablage mit <kbd>Strg + Umschalt + V</kbd> in Ihre Notiz einfügen.
:::

::: warning
Info Image links with a title are not supported:

```markdown
![alt text](media/my-image.jpg "Title not supported")
```

:::

## Inline-Code und Code-Blöcke

```markdown
Inline `Code` hat `Backticks drumherum`.
```

Sie können die [Verknüpfung](./shortcuts.md) <kbd>Strg + Umschalt + C</kbd> für ausgewählten Inline-Text oder nur innerhalb von Text verwenden, um einen Inline-Codeblock zu erstellen.
:::

Codeblocks sind entweder umschlossen von Zeilen mit drei Backticks, oder sind mit vier Leerzeichen eingerückt.

### Code-Blöcke mit vier führenden Leerzeichen

Fügen Sie vier Leerzeichen vor Ihrem Code hinzu, um ihn als Codeblock zu markieren.

```markdown
    s = "Code mit Leerzeicheneinrückung"
    print s
```

### Code-Blöcke mit Backticks

Sie können auch drei Backticks verwenden, um einen Codeblock zu erstellen.
````markdown
```
Code kommt hierher
Code kommt hierher
```
````

::: tip
Du kannst den [shortcut](./shortcuts.md) <kbd>Strg + Umschalt + C</kbd> auf mehreren ausgewählten Textzeilen oder in einer leeren Zeile benutzen, um einen Codeblock zu generieren.
:::

### Backtick-Zaun mit Code-Hervorhebung

Es gibt auch einige Syntaxhervorhebungen mit Codeblöcken in QOwnNotes.

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

Sie können auch inline-Markdown verwenden.

| Markdown | Less      | Pretty     |
| -------- | --------- | ---------- |
| _Still_  | `renders` | **nicely** |
| 1        | 2         | 3          |
````

::: tip
Drücken Sie <kbd>Alt + Umschalt + T</kbd>, um ein Dialogfeld zu aktivieren, mit dem Sie Tabellen erstellen können. In diesem Dialog können Sie sogar CSV-Dateien importieren!

Verwenden Sie <kbd>Strg + Leertaste</kbd> in einer Markdown-Tabelle, um sie automatisch zu formatieren.
:::

## Zitat-Blöcke

```markdown
> Blockzitate sind sehr nützlich, um Antworttext in E-Mails zu emulieren.
> Diese Zeile ist Teil desselben Zitats.

Zitatpause.

> Das ist eine sehr lange Zeile, die nach einem Zeilenumbruch immer noch richtig zitiert wird. Oh Mann, lass uns weiter schreiben, um sicherzustellen, dass es lang genug ist, um tatsächlich für alle umgebrochen zu werden. Oh, you can _put_ **Markdown** into a blockquote.
```

::: tip
You can tell QOwnNotes to fully highlight blockquotes or just the blockquote character in the _Editor settings_

Sie können die [ Verknüpfung ](./shortcuts.md) <kbd> Strg + Umschalt + B </kbd> verwenden, um Text als Blockzitat zu markieren.
:::

## Horizontale Linie

Es gibt drei Möglichkeiten, um eine horizontale Linie zu erhalten: Bindestriche, Sternchen oder Unterstriche.

```markdown
Drei oder mehr...

Hyphens

---

Asterisks

***

Underscores

___
```

## Zeilenumbrüche

- Sie können einen Absatz zur einfacheren Bearbeitung in mehr als eine einzelne Zeile aufteilen, sie werden immer noch als ein einzelner Absatz ohne Unterbrechungen dargestellt.
- Sie können einen Zeilenumbruch innerhalb eines Absatzes erzwingen, indem Sie eine Zeile mit zwei Leerzeichen beenden.
- Sie können einen separaten Absatz erstellen, indem Sie ihn durch Leerzeilen abgrenzen.

::: tip
Mit <kbd>⇧ Shift</kbd> + <kbd>Return</kbd> können Sie zwei Leerzeichen und einen Zeilenumbruch eingeben.
:::

```markdown
Hier ist eine Zeile für den Anfang.

This line is separated from the one above by two newlines, so it will be a _separate paragraph_.

This line also begins a separate paragraph, but...  
This line is only separated by two trailing spaces and a single newline, so it's a separate line in the _same paragraph_.
```

::: tip
Nachgestellte Leerzeichen werden im Editor standardmäßig hervorgehoben.
:::

## Kommentare

Kommentare werden in der Vorschau nicht angezeigt.

```markdown
[comment]: # "This comment will not appear in the preview"

<!-- HTML comments are also hidden -->
```

::: tip
Ein führender HTML-Kommentarblock in einer Notiz wird bei der automatischen Generierung von Notizdateinamen ebenfalls ignoriert.
:::

## Checkboxlisten

Sie können einfache To-do-Listen mit Checkboxlisten erstellen.

```markdown
- [x] erledigt
- [ ] to-do
```

::: tip
Sie können Checkboxen in der Vorschau abhaken / aufheben.
:::

## Präambel

In QOwnNotes können Sie eine Präambel (z.B. YAML) verwenden, um zusätzliche Metainformationen hinzuzufügen. Es wird **nicht in der Vorschau angezeigt** und **stört die automatische Generierung von Notizendateinamen** nicht.

```markdown
---
title: Irgendein Name
description: Etwas Beschreibung
---

# Notizkopfzeile beginnt hier

Etwas Text
```

Der Dateiname dieser Beispielnotiz würde lauten: `Notizkopfzeile beginnt hier.md `.
