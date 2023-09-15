# Markdown

Deze Markdown-cheatsheet is bedoeld als een snelle referentie en showcase van de markdown-syntaxis in QOwnNotes.

## Headers

Gebruik koppen om uw teksten te structureren.

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

Als alternatief, voor H1 en H2, een onderstreepte stijl:

```markdown
Alt-H1
======

Alt-H2
------
```

::: tip
Standaard creëert QOwnNotes de **bestandsnaam van een notitie** uit de **kop 1** (h1).
:::

## Nadruk

```markdown
Nadruk, ook bekend als cursief, met *asterisks*.

Sterke nadruk, ook bekend als vetgedrukt, met **asterisks**.
```

::: tip
U kunt de [sneltoets](./shortcuts.md) <kbd>Ctrl + B</kbd> gebruiken om tekst vetgedrukt te maken en <kbd>Ctrl + I</kbd> om deze cursief te maken.
:::

## Onderstrepen

Er is ook een optionele instelling om onderstreping we te geven in de *Voorbeeldweergave instellingen*.

```markdown
_underline_
```

::: tip
U kunt de [sneltoets](./shortcuts.md) <kbd>Ctrl + U</kbd> gebruiken om een tekst te onderstrepen.
:::

## Doorhalen

```markdown
~~strike out~~
```

::: tip
U kunt de [sneltoets](./shortcuts.md) <kbd>Alt + Shift + S</kbd> gebruiken om een tekst door te halen.
:::

## Lijsten

Er zijn veel manieren om lijsten te maken.

```markdown
1. Eerste geordende lijstitem
2. Een ander item
   * Ongeordende sublijst.
1. Werkelijke cijfers doen er niet toe, alleen dat het een nummer is
   1. Geordende sublijst (werkt alleen in de editor, niet in de voorbeeldweergave)
4. En nog een item.

* Ongeordende lijst kan asterisks gebruiken
- Of minnen
+ Of plussen
```

::: tip
Als u aan het einde van een lijst op <kbd>Enter</kbd> drukt, wordt een nieuw lijstitem aangemaakt.
:::

## Koppelingen

Twee van de eenvoudigere toepassingen voor koppelingen zijn het verwijzen naar webpagina's of andere notities. Er zijn meerdere manieren waarop elk van deze koppelingen eruit kan zien.

### Externe koppelingen

```markdown
[I'm an inline-style link](https://www.google.com)

[I'm an inline-style link with title](https://www.google.com "Google's Homepage")

[You can use numbers for reference-style link definitions][1]

Gewone URL's en URL's tussen hoekhaken worden automatisch omgezet tot koppelingen in de voorbeeldweergave. 
http://www.example.com or <http://www.example.com>

[1]: https://www.qownnotes.org
```

### Interne koppelingen

```markdown
[I link to the Journal.md note](Journal.md)

<Journal.md> werkt op dezelfde wijze.
```

::: tip
U kunt de [sneltoets](./shortcuts.md) <kbd>Ctrl + L</kbd> gebruiken om **links naar webpagina's of andere notities** te maken.

Als u <kbd>Ctrl + Shift + X</kbd> gebruikt, wordt een dialoogvenster geopend waarmee u **bijlagen** in uw notitie kunt invoegen.

U kunt op <kbd>Ctrl + spatiebalk</kbd> drukken terwijl de cursor op een koppeling in de notitiebewerking staat om deze te volgen.
:::

### Bladwijzers

De bladwijzers die worden gebruikt door de [QOwnNotes Web Companion-browserextensie](./browser-extension.md) gebruiken koppelingen in lijsten.

```markdown
- [Webpage name](https://www.example.com) #tag1 #tag2 some description and tags
```

## Afbeeldingen

Afbeeldingen kunnen worden ingesloten in QOwnNotes. Ze worden getoond in de preview.

```markdown
![alt text](media/my-image.jpg)
```

::: tip
U kunt de [shortcut](./shortcuts.md) <kbd>Ctrl + Shift + I</kbd> gebruiken om een afbeelding in een notitie in te voegen. De afbeelding kan ook op het klembord staan, het dialoogvenster zal het detecteren en een voorbeeld tonen.

U kunt ook rechtstreeks een afbeelding van het klembord in uw notitie plakken met <kbd>Ctrl + Shift + V</kbd>.
:::


## Inline code en codeblokken

```markdown
Inline `code` has `backticks around` it.
```

::: tip
U kunt de [shortcut](./shortcuts.md) <kbd>Ctrl + Shift + C</kbd> op geselecteerde inline-tekst of gewoon binnen tekst gebruiken om een inline-codeblok te maken.
:::

Blocks of code are either fenced by lines with three backticks, or are indented with four spaces.

### 4-Spaces fence

Voeg vier spaties toe vóór uw code om deze als codeblok te markeren.

```markdown
    s = "Code with space indent"
    print s
```

### Backtick fence

U kunt ook drie backticks gebruiken om een ​​codeblok te maken.
~~~markdown
```
Code goes here
Code goes here
```
~~~

::: tip
You can use the [shortcut](./shortcuts.md) <kbd>Ctrl + Shift + C</kbd> on
multiple selected lines of text or in an empty line to create a code block. 
:::

### Backtick fence with code highlighting

There also is some syntax highlighting with code blocks in QOwnNotes.

~~~markdown
```bash
# I am a comment
cd Notes
```
~~~

Currently, supported languages (and code block identifiers) are:

* BASh scripting, `bash`
* C, `c`
* C++, `cpp`
* C++, `cxx`
* C++, `c++`
* C#, `c#`
* CMake, `cmake`
* C#, `csharp`
* CSS, `css`
* Go, `go`
* HTML, `html`
* INI, `ini`
* Java, `java`
* JavaScript, `javascript`
* JavaScript, `js`
* JSON, `json`
* Makefile, `make`
* PHP, `php`
* Python, `py`
* Python, `python`
* QML, `qml`
* Rust, `rust`
* Shell scripting, `sh`
* SQL, `sql`
* TypeScript, `ts`
* TypeScript, `typescript`
* V, `v`
* Vex, `vex`
* XML, `xml`
* YAML, `yml`
* YAML, `yaml`

## Tables

Tables aren't part of the core Markdown spec, but the QOwnNotes preview supports them. 

~~~markdown
Colons can be used to align columns.

| Tables        | Are           | Cool  |
| ------------- |:-------------:| -----:|
| col 3 is      | right-aligned | $1600 |
| col 2 is      | centered      |   $12 |
| zebra stripes | are neat      |    $1 |

There must be at least 3 dashes separating each header cell.

You can also use inline Markdown.

| Markdown | Less | Pretty |
| --- | --- | --- |
| *Still* | `renders` | **nicely** |
| 1 | 2 | 3 |
~~~

::: tip
Druk op <kbd>Alt + Shift + T</kbd> om een dialoogvenster te activeren waarmee u tabellen kunt maken. U kunt zelfs CSV-bestanden in dat dialoogvenster importeren.

Gebruik <kbd>Ctrl + Space</kbd> in een markdown-tabel om deze automatisch op te maken.
:::

## Blok citaten

```markdown
> Blockquotes are very handy in email to emulate reply text.
> This line is part of the same quote.

Quote break.

> This is a very long line that will still be quoted properly when it wraps. Oh boy let's keep writing to make sure this is long enough to actually wrap for everyone. Oh, you can *put* **Markdown** into a blockquote. 
```

::: tip
U kunt QOwnNotes vertellen om blockquotes of alleen het blockquote-teken volledig te markeren in de *Editor settings*

U kunt de [sneltoets](./shortcuts.md) <kbd>Ctrl + Shift + B</kbd> gebruiken om tekst als blokcitaat te markeren.
:::

## Horizontale lijn

Er zijn drie manieren om een ​​horizontale regel te krijgen: koppeltekens, sterretjes of onderstrepingstekens.

```markdown
Three or more...

Hyphens

---

Asterisks

***

Underscores

___
```

## Line breaks

- U kunt een alinea opsplitsen in meer dan een enkele regel voor eenvoudiger bewerken, ze worden nog steeds weergegeven als een enkele alinea zonder pauzes.
- U kunt een regeleinde binnen een alinea forceren door een regel met twee spaties te beëindigen.
- U kunt een aparte alinea maken door deze af te bakenen met lege regels.

::: tip
Je kunt twee spaties en een nieuwe regel invoeren met <kbd>⇧ Shift</kbd> + <kbd>Return</kbd>.
:::

```markdown
Here's a line for us to start with.

This line is separated from the one above by two newlines, so it will be a *separate paragraph*.

This line is also begins a separate paragraph, but...  
This line is only separated by two trailing spaces and a single newline, so it's a separate line in the *same paragraph*.
```

::: tip
Spaties achteraan worden standaard gemarkeerd in de editor.
:::

## Opmerkingen

Opmerkingen worden niet weergegeven in het voorbeeld.

```markdown
[comment]: # (This comment will not appear in the preview)

<!-- HTML comments are also hidden -->
```

::: tip
Een leidend html-commentaarblok in een notitie wordt ook genegeerd bij het automatisch genereren van notitiebestandsnamen.
:::

## Checkbox-lijsten

U kunt eenvoudige takenlijsten maken met lijsten met selectievakjes.

```markdown
- [x] done
- [ ] todo
```

::: tip
U kunt selectievakjes in het voorbeeld in- of uitschakelen.
:::

## Frontmatter

In QOwnNotes kun je een frontmatter (bijvoorbeeld YAML) gebruiken om wat extra meta-informatie toe te voegen. Het wordt **niet getoond in het voorbeeld** en zal **het automatisch genereren van de bestandsnaam van notities** niet verstoren.

```markdown
---
title: Some name
description: Some description
---

# Note headline starts here

Some text
```

De bestandsnaam van deze voorbeeldnotitie zou ` Notitiekop begint hier.md ` zijn.
