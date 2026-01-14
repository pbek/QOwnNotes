# Markdown Cheatsheet

This Markdown cheatsheet is intended as a quick reference and showcase of the markdown syntax in QOwnNotes.

## Headings

Use headings to structure your texts.

```md
# H1

## H2

### H3

#### H4

##### H5

###### H6
```

::: tip
The **Navigation panel** shows the structure of your headings.
:::

Alternatively, for H1 and H2, an underline-ish style:

```md
Alt-H1
======

Alt-H2
------
```

::: tip
By default QOwnNotes creates the **filename of a note** from the **heading 1** (h1).
:::

## Emphasis

```md
Emphasis, aka italics, with *asterisks* or _underscores_.

Strong emphasis, aka bold, with **asterisks** or __underscores__.

Combined emphasis with **asterisks and _underscores_**.
```

::: tip
You can use the [shortcut](./shortcuts.md) <kbd>Ctrl + B</kbd> for making text bold
and <kbd>Ctrl + I</kbd> for making it italic.
:::

## Underline

There also is an optional setting to enable underline rendering in the _Preview settings_.

```md
_underline_
```

::: tip
You can use the [shortcut](./shortcuts.md) <kbd>Ctrl + U</kbd> to underline a text.
:::

## Strike out

```md
~~strike out~~
```

::: tip
You can use the [shortcut](./shortcuts.md) <kbd>Alt + Shift + S</kbd> to strike out a text.
:::

## Lists

There are many ways to create lists.
(In this example, leading and trailing spaces are shown with dots: ⋅)

```md
1. First ordered list item
2. Another item
   ⋅⋅* Unordered sub-list.
3. Actual numbers don't matter, just that it's a number
   ⋅⋅1. Ordered sub-list (only works in the editor, not the preview)
4. And another item.
⋅⋅⋅You can have properly indented paragraphs within list items. Notice the blank line above, and the leading spaces.
⋅⋅⋅To have a line break without a paragraph, you will need to use two trailing spaces.⋅⋅
⋅⋅⋅Note that this line is separate, but within the same paragraph.⋅⋅
* Unordered lists can use asterisks
- Or hyphens (minuses)
+ Or pluses
```

1. First ordered list item
e2. Another item
   * Unordered sub-list.
3. Actual numbers don't matter, just that it's a number
   1. Ordered sub-list (only works in the editor, not the preview)
4. And another item.
   You can have properly indented paragraphs within list items. Notice the blank line above, and the leading spaces.
   To have a line break without a paragraph, you will need to use two trailing spaces.
   Note that this line is separate, but within the same paragraph.
* Unordered lists can use asterisks
- Or hyphens (minuses)
+ Or pluses

::: tip
If you press <kbd>Enter</kbd> at the end of a list a new list item will be created.
:::

## Links

Two of the simpler uses for links are pointing to webpages or other notes. There are multiple ways each of these links can look.

### External links

```md
[I'm an inline-style link](https://www.google.com)

[I'm an inline-style link with title](https://www.google.com "Google's Homepage")

[You can use numbers for reference-style link definitions][1]

Plain URLs and URLs in angle brackets will automatically get turned into links in the preview.

http://www.example.com or <http://www.example.com>

[1]: https://www.qownnotes.org
```

### Internal links

You can link to other notes or attachments like this:

```md
[I link to the Journal.md note](Journal.md)

<Journal.md> works similarly.
```

::: tip
You can use the [shortcut](./shortcuts.md) <kbd>Ctrl + L</kbd> to
**create links to webpages or other notes**.
Using <kbd>Ctrl + Shift + X</kbd> will bring up a dialog that helps you to
**insert attachments** into your note.
You can press <kbd>Ctrl + Space</kbd> while the cursor in on a link in the
note edit to follow the link.
:::

::: warning Info
Internal links with a title are not supported:

```md
[I link to the Journal.md note](Journal.md "Title not supported")
```
:::

### Bookmarks

The bookmarks used by the [QOwnNotes Web Companion browser extension](./browser-extension.md) are using links in lists.

```md
- [Webpage name](https://www.example.com) #tag1 #tag2 some description and tags
```

## Images

Images can be embedded into QOwnNotes. They will be shown in the preview.

```md
![alt text](media/my-image.jpg)
```

::: tip
You can use the [shortcut](./shortcuts.md) <kbd>Ctrl + Shift + I</kbd> to insert an image into a note.
The image can also be in the clipboard, the dialog will detect it and show a preview.
You can also directly paste an image from the clipboard into your note with <kbd>Ctrl + Shift + V</kbd>.
:::

::: warning Info
Image links with a title are not supported:

```md
![alt text](media/my-image.jpg "Title not supported")
```
:::

## Inline code and code blocks

```md
Inline `code` has `backticks around` it.
```

::: tip
You can use the [shortcut](./shortcuts.md) <kbd>Ctrl + Shift + C</kbd> on
selected inline text or just inside of text to create an inline code block.
:::

Blocks of code are either fenced by lines with three backticks, or are indented with four spaces.

### 4-Spaces fence

Add four spaces in front of your code to mark it as code block.

```md
    s = "Code with space indent"
    print s
```

    s = "Code with space indent"
    print s

### Backtick fence

You can also use three backticks to create a code block.

````md
```
Code goes here
Code goes here
```
````

```
Code goes here
Code goes here
```

::: tip
You can use the [shortcut](./shortcuts.md) <kbd>Ctrl + Shift + C</kbd> on
multiple selected lines of text or in an empty line to create a code block.
:::

### Backtick fence with code highlighting

There also is some syntax highlighting with code blocks in QOwnNotes.

````md
```bash
# I am a comment
cd Notes
```
````

```bash
# I am a comment
cd Notes
```

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

```md
Colons can be used to align columns.

| Tables        |      Are      |  Cool |
| ------------- | :-----------: | ----: |
| col 3 is      | right-aligned | $1600 |
| col 2 is      |   centered    |   $12 |
| zebra stripes |   are neat    |    $1 |

There must be at least 3 dashes separating each header cell.
The outer pipes (|) are optional, and you don't need to make the
raw Markdown line up prettily. You can also use inline Markdown.

| Markdown | Less | Pretty |
| -------- | --------- | ---------- |
| _Still_ | `renders` | **nicely** |
| 1 | 2 | 3 |

```

Colons can be used to align columns.

| Tables        |      Are      |  Cool |
| ------------- | :-----------: | ----: |
| col 3 is      | right-aligned | $1600 |
| col 2 is      |   centered    |   $12 |
| zebra stripes |   are neat    |    $1 |

There must be at least 3 dashes separating each header cell.
The outer pipes (|) are optional, and you don't need to make the
raw Markdown line up prettily. You can also use inline Markdown.

| Markdown | Less | Pretty |
| -------- | --------- | ---------- |
| _Still_ | `renders` | **nicely** |
| 1 | 2 | 3 |

::: tip
Press <kbd>Alt + Shift + T</kbd> to activate a dialog that can help you to create tables.
You can even import CSV files in that dialog.

Use <kbd>Ctrl + Space</kbd> inside a markdown table to automatically format it.
:::

## Blockquotes

```md
> Blockquotes are very handy in email to emulate reply text.
> This line is part of the same quote.

Quote break.

> This is a very long line that will still be quoted properly when it wraps. Oh boy let's keep writing to make sure this is long enough to actually wrap for everyone. Oh, you can _put_ **Markdown** into a blockquote.
```
> Blockquotes are very handy in email to emulate reply text.
> This line is part of the same quote.

Quote break.

> This is a very long line that will still be quoted properly when it wraps. Oh boy let's keep writing to make sure this is long enough to actually wrap for everyone. Oh, you can _put_ **Markdown** into a blockquote.

::: tip
You can tell QOwnNotes to fully highlight blockquotes or just the blockquote character in the _Editor settings_

You can use the [shortcut](./shortcuts.md) <kbd>Ctrl + Shift + B</kbd> for marking text as blockquote.
:::

## Horizontal rule

There are three ways to get a horizontal rule:
hyphens, asterisks or underscores.
```md
Three or more of any:

Hyphens

---

Asterisks

***

Underscores

___
```

Three or more of any:

Hyphens

---

Asterisks

***

Underscores

___

## Line breaks

- You can break a paragraph into more than a single line for easier editing, they still render as a single paragraph with no breaks.
- You can force a line break inside a paragraph by ending a line with two spaces.
- You can make a separate paragraph by delimiting it by empty lines.

::: tip
You can enter two spaces and a newline with <kbd>⇧ Shift</kbd> + <kbd>Return</kbd>.
:::

```md
Here's a line for us to start with.

This line is separated from the one above by two newlines, so it will be a _separate paragraph_.

This line also begins a separate paragraph, but...
This line is only separated by two trailing spaces and a single newline, so it's a separate line in the _same paragraph_.
```
Here's a line for us to start with.

This line is separated from the one above by two newlines, so it will be a _separate paragraph_.

This line also begins a separate paragraph, but...
This line is only separated by two trailing spaces and a single newline, so it's a separate line in the _same paragraph_.

::: tip
Trailing spaces are highlighted by default in the editor.
:::

## Comments

Comments are not shown in the preview.

```md
[comment]: # "This comment will not appear in the preview"

<!-- HTML comments are also hidden -->
```

::: tip
A leading html comment block in a note will also be ignored in the automatic note filename generation.
:::

## Checkbox lists

You can create simple todo lists with checkbox lists.

```md
- [x] done
- [ ] todo
```

- [x] done
- [ ] todo

::: tip
You can check/un-check checkboxes in the preview.
:::

## Frontmatter

In QOwnNotes you can use a frontmatter (e.g. YAML) to add some extra meta information.
It will **not be shown in the preview** and will
**not disturb the automatic note filename generation**.

```md
---
title: Some name
description: Some description
---

# Note headline starts here

Some text
```

The filename of this example note would be `Note headline starts here.md`.
