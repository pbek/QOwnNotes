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
Het **Navigatiepaneel** toont de structuur van uw koppen.
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
Nadruk, ook bekend als cursief, met *asterisken*.

Sterke nadruk, ook bekend als vetgedrukt, met **sterretjes**.
```

::: tip
U kunt de [shortcut](./shortcuts.md) <kbd>Ctrl + B</kbd> gebruiken om tekst vetgedrukt te maken en <kbd>Ctrl + I</kbd> om deze cursief te maken.
:::

## Onderstrepen

Er is ook een optionele instelling om onderstrepen te renderen in de *Preview settings*.

```markdown
_underline_
```

::: tip
U kunt de [ shortcut](./shortcuts.md) <kbd>Ctrl + U</kbd> gebruiken om een tekst te onderstrepen.
:::

## Doorhalen

```markdown
~~strike out~~
```

::: tip
U kunt de [shortcut](./shortcuts.md) <kbd>Alt + Shift + S</kbd> gebruiken om een tekst door te halen.
:::

## Lijsten

Er zijn veel manieren om lijsten te maken.

```markdown
1. Eerste bestelde lijstitem
2. Een ander item
   * Ongeordende sublijst.
1. Werkelijke cijfers doen er niet toe, alleen dat het een nummer is
   1. Geordende sublijst
4. En nog een item.

* Ongeordende lijst kan asterisken gebruiken
- Of minnen
+ Of plussen
```

::: tip
Als u aan het einde van een lijst op <kbd>Enter</kbd> drukt, wordt een nieuw lijstitem aangemaakt.
:::

## Links

Er zijn meerdere manieren om links te maken.

```markdown
[Ik ben een inline-stijl link] (https://www.google.com)

[Ik ben een inline-stijl link met titel] (https://www.google.com "Google's Homepage")

[U kunt getallen gebruiken voor linkdefinities in referentiestijl] [1]

URL's en URL's tussen punthaken worden automatisch omgezet in links in het voorbeeld. 
http://www.example.com or <http://www.example.com>

[1]: https://www.qownnotes.org
```

::: tip
U kunt de [sneltoets](./shortcuts.md) <kbd>Ctrl + L</kbd> gebruiken om **links naar webpagina's of andere notities** te maken.

Als u <kbd>Ctrl + Shift + X</kbd> gebruikt, wordt een dialoogvenster geopend waarmee u **bijlagen** in uw notitie kunt invoegen.

U kunt op <kbd>Ctrl + spatiebalk</kbd> drukken terwijl de cursor op een link in de notitiebewerking staat om de link te volgen.
:::

### Bladwijzers

De bladwijzers die worden gebruikt door de [QOwnNotes Web Companion-browserextensie](./browser-extension.md) gebruiken links in lijsten.

```markdown
- [Webpaginanaam] (https://www.example.com) #tag1 #tag2 een beschrijving en tags
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
Inline `code` heeft` back-ticks eromheen`.
```

::: tip
U kunt de [shortcut](./shortcuts.md) <kbd>Ctrl + Shift + C</kbd> op geselecteerde inline-tekst of gewoon binnen tekst gebruiken om een inline-codeblok te maken.
:::

Codeblokken zijn ofwel omheind door regels met drie achterste vinkjes, of zijn ingesprongen met vier spaties.

### 4-vaks hekwerk

Voeg vier spaties toe vóór uw code om deze als codeblok te markeren.

```markdown
    s = "Code met spatie-inspringing"
     afdrukken s
```

### Backtick hek

U kunt ook drie backticks gebruiken om een ​​codeblok te maken.
~~~markdown
```
Code komt hier
Code komt hier
```
~~~

::: tip
U kunt de [sneltoets](./ sneltoetsen.md) <kbd>Ctrl + Shift + C</kbd> gebruiken op meerdere geselecteerde regels tekst of op een lege regel om een codeslot te creëren. 
:::

### Backtick-omheining met code-markering

Er is ook enige syntaxisaccentuering met codeblokken in QOwnNotes.

~~~markdown
```bash
# I am a comment
cd Notes
~~~
~~~

## Tabellen

Tabellen maken geen deel uit van de kernspecificatie van Markdown, maar het QOwnNotes-voorbeeld ondersteunt ze. 

```markdown
Colons can be used to align columns.

| Tables        | Are           | Cool  |
| ------------- |:-------------:| -----:|
| col 3 is      | right-aligned | $1600 |
| col 2 is      | centered      |   $12 |
| zebra stripes | are neat      |    $1 |

There must be at least 3 dashes separating each header cell.

U kunt ook inline markdown gebruiken.

| Markdown | Less | Pretty |
| --- | --- | --- |
| *Still* | `renders` | **nicely** |
| 1 | 2 | 3 |
~~~

::: tip
Druk op <kbd>Alt + Shift + T</kbd> om een dialoogvenster te openen waarmee u tabellen kunt maken. U kunt zelfs CSV-bestanden in dat dialoogvenster importeren.

Gebruik <kbd>Ctrl + Space</kbd> in een markdown-tabel om deze automatisch op te maken.
:::

## Blok citaten

```markdown
> Blockquotes zijn erg handig in e-mail om antwoordtekst te emuleren.
> Deze regel maakt deel uit van hetzelfde citaat.

Offerte pauze.

> Dit is een zeer lange regel die nog steeds correct zal worden geciteerd wanneer deze wordt omgeslagen. Oh jongen, laten we blijven schrijven om er zeker van te zijn dat dit lang genoeg is om voor iedereen omgeslagen te worden. Oh, je kunt **Markdown** in een blockquote *plaatsen*. 
```

::: tip
U kunt QOwnNotes vertellen om blockquotes of alleen het blockquote-teken volledig te markeren in de *Editor settings*

U kunt de [sneltoets](./shortcuts.md) <kbd>Ctrl + Shift + B</kbd> gebruiken om tekst als blokcitaat te markeren.
:::

## Horizontale lijn

Er zijn drie manieren om een ​​horizontale regel te krijgen: koppeltekens, sterretjes of onderstrepingstekens.

```markdown
Drie of meer...

Koppeltekens

---

Asterisken

***

Onderstreept

___
```

## Line breaks

- **Twee nieuwe regels** leveren u een **nieuwe alinea** op.
- **Een nieuwe regel** levert een **nieuwe regel in dezelfde alinea** op.
- Om een **regeleinde zonder alinea** te hebben, moet u **twee spaties achteraan** gebruiken.

```markdown
Hier is een regel om mee te beginnen.

Deze regel is gescheiden van de regel hierboven door twee nieuwe regels, dus het wordt een *aparte alinea*.

Deze regel begint ook een aparte paragraaf, maar ...
Deze regel wordt alleen gescheiden door twee volgspaties en een enkele nieuwe regel, dus het is een aparte regel in *dezelfde alinea*.
```

::: tip
Spaties achteraan worden standaard gemarkeerd in de editor.
:::

## Opmerkingen

Opmerkingen worden niet weergegeven in het voorbeeld.

```markdown
[comment]: # (Deze opmerking verschijnt niet in het voorbeeld)

<! - HTML-opmerkingen zijn ook verborgen - &t;
```

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
title: een naam
description: Enige beschrijving
---

# De kop van de opmerking begint hier

Wat tekst
```

De bestandsnaam van deze voorbeeldnotitie zou ` Notitiekop begint hier.md ` zijn.
