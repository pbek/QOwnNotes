# Markdown Cheatsheet

This cheatsheet is intended as a quick reference and showcase of the markdown syntax in [QOwnNotes](https://www.qownnotes.org).

## Headers

```markdown
# H1

## H2

### H3

#### H4

##### H5

###### H6

Alternatively, for H1 and H2, an underline-ish style:

Alt-H1
======

Alt-H2
------
```

# H1

## H2

### H3

#### H4

##### H5

###### H6

Alternatively, for H1 and H2, an underline-ish style:

Alt-H1
======

Alt-H2
------

## Emphasis

```markdown
Emphasis, aka italics, with *asterisks* or _underscores_.

Strong emphasis, aka bold, with **asterisks** or __underscores__.

Combined emphasis with **asterisks and _underscores_**.

Strikethrough uses two tildes. ~~Scratch this.~~
```

Emphasis, aka italics, with *asterisks* or _underscores_.

Strong emphasis, aka bold, with **asterisks** or __underscores__.

Combined emphasis with **asterisks and _underscores_**.

Strikethrough uses two tildes. ~~Scratch this.~~

## Lists

(In this example, leading and trailing spaces are shown with dots: ⋅)

```markdown
1. First ordered list item
2. Another item
   ⋅⋅* Unordered sub-list
3. Actual numbers don't matter, just that it's a number
   ⋅⋅1. Ordered sub-list
4. And another item.

⋅⋅⋅You can have properly indented paragraphs within list items. Notice the blank line above, and the leading spaces.

⋅⋅⋅To have a line break without a paragraph, you will need to use two trailing spaces.⋅⋅
⋅⋅⋅Note that this line is separate, but within the same paragraph.⋅⋅

* Unordered lists can use asterisks
- Or hyphens (minuses)
+ Or pluses
```

1. First ordered list item
2. Another item
   * Unordered sub-list
3. Actual numbers don't matter, just that it's a number
   1. Ordered sub-list
4. And another item.

   You can have properly indented paragraphs within list items. Notice the blank line above, and the leading spaces.

   To have a line break without a paragraph, you will need to use two trailing spaces.  
   Note that this line is separate, but within the same paragraph.

* Unordered lists can use asterisks
- Or hyphens (minuses)
+ Or pluses

## Links

There are two ways to create links.

```markdown
[I'm an inline-style link](https://www.google.com)

[I'm an inline-style link with title](https://www.google.com "Google's Homepage")

[You can use numbers for reference-style link definitions][1]

URLs and URLs in angle brackets will automatically get turned into links in the preview.
http://www.example.com or <http://www.example.com>

[1]: https://www.qownnotes.org
```

[I'm an inline-style link](https://www.google.com)

[I'm an inline-style link with title](https://www.google.com "Google's Homepage")

[You can use numbers for reference-style link definitions][1]

URLs and URLs in angle brackets will automatically get turned into links in the preview.
http://www.example.com or <http://www.example.com>

[1]: https://www.qownnotes.org

## Inline code and code blocks

```markdown
Inline `code` has `backticks around` it.
```

Inline `code` has `backticks around` it.

Blocks of code are either fenced by lines with three backticks, or are indented with four spaces.

### 4-Spaces fence

```markdown
    s = "Code with space indent"
    print s
```

    s = "Code with space indent"
    print s

### Backtick fence

    ```
    Code goes here
    Code goes here
    ```

```
Code goes here
Code goes here
```

### Backtick fence with code highlighting

    ```bash
    # I am a comment
    cd Notes
    ```

```bash
# I am a comment
cd Notes
```

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
The outer pipes (|) are optional, and you don't need to make the
raw Markdown line up prettily. You can also use inline Markdown.

| Markdown | Less      | Pretty     |
| -------- | --------- | ---------- |
| _Still_  | `renders` | **nicely** |
| 1        | 2         | 3          |
```

Colons can be used to align columns.

| Tables        |      Are      |  Cool |
| ------------- | :-----------: | ----: |
| col 3 is      | right-aligned | $1600 |
| col 2 is      |   centered    |   $12 |
| zebra stripes |   are neat    |    $1 |

There must be at least 3 dashes separating each header cell.  
The outer pipes (|) are optional, and you don't need to make the raw Markdown line up prettily. You can also use inline Markdown.

| Markdown | Less      | Pretty     |
| -------- | --------- | ---------- |
| _Still_  | `renders` | **nicely** |
| 1        | 2         | 3          |

## Blockquotes

```markdown
> Blockquotes are very handy in email to emulate reply text.
> This line is part of the same quote.

Quote break.

> This is a very long line that will still be quoted properly when it wraps. Oh boy let's keep writing to make sure this is long enough to actually wrap for everyone. Oh, you can _put_ **Markdown** into a blockquote.
```

> Blockquotes are very handy in email to emulate reply text.
> This line is part of the same quote.

Quote break.

> This is a very long line that will still be quoted properly when it wraps. Oh boy let's keep writing to make sure this is long enough to actually wrap for everyone. Oh, you can _put_ **Markdown** into a blockquote.

## Horizontal Rule

```markdown
Three or more...

---

Hyphens

***

Asterisks

___

Underscores
```

Three or more...

---

Hyphens

***

Asterisks

___

Underscores

## Line Breaks

```markdown
Here's a line for us to start with.

This line is separated from the one above by two newlines, so it will be a _separate paragraph_.

This line also begins a separate paragraph, but...  
This line is only separated by two trailing spaces and a single newline, so it's a separate line in the _same paragraph_.
```

Here's a line for us to start with.

This line is separated from the one above by two newlines, so it will be a _separate paragraph_.

This line also begins a separate paragraph, but...  
This line is only separated by two trailing spaces and a single newline, so it's a separate line in the _same paragraph_.

## Comments

```markdown
[comment]: # "This comment will not appear in the preview"
```

[comment]: # "This comment will not appear in the preview"

## Checkbox lists

```markdown
- [x] done
- [ ] todo
```

- [x] done
- [ ] todo

---

Credit: [markdown-here](https://github.com/adam-p/markdown-here/wiki/Markdown-Cheatsheet)  
License: [CC-BY](https://creativecommons.org/licenses/by/3.0/)
