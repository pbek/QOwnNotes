# Markdown

Deze Markdown-cheatsheet is bedoeld als een snelle referentie en showcase van de markdown-syntaxis in QOwnNotes.

## Headings

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
Het **Navigatiepaneel** toont de structuur van uw rubrieken.
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
- [Webpage name](https://www.example.com) #tag1 #tag2 een beschrijving en labels
```

## Afbeeldingen

Afbeeldingen kunnen worden ingesloten in QOwnNotes. Ze worden getoond in de preview.

```markdown
![alt text](media/my-image.jpg)
```

::: tip
U kunt de [sneltoets](./shortcuts.md) <kbd>Ctrl + Shift + I</kbd> gebruiken om een afbeelding in een notitie in te voegen. De afbeelding kan ook op het klembord staan, het dialoogvenster zal het detecteren en een voorbeeld tonen.

U kunt ook rechtstreeks een afbeelding van het klembord in uw notitie plakken met <kbd>Ctrl + Shift + V</kbd>.
:::


## Inline code en codeblokken

```markdown
Inline `code` is `omsloten door accents grave`.
```

::: tip
U kunt de [sneltoets](./shortcuts.md) <kbd>Ctrl + Shift + C</kbd> op geselecteerde inline-tekst of gewoon binnen tekst gebruiken om een inline-codeblok te maken.
:::

Codeblokken worden omsloten door regels met drie accents grave of zijn ingesprongen met vier spaties.

### 4-spatiebegrenzing

Voeg vier spaties toe vóór uw code om deze als codeblok te markeren.

```markdown
    s = "Code met inspringen door spaties"
    print s
```

### Backtick-begrenzing

U kunt ook drie tildetekens gebruiken om een ​​codeblok aan te maken.
~~~markdown
```
Hier komt code
Hier komt code
```
~~~

::: tip
U kunt de [shortcut] (./ shortcuts.md) <kbd> Ctrl + Shift + C </kbd> toepassen op meerdere geselecteerde tekstregels of in een lege regel om een codeblok aan te maken. 
:::

### Backtick-begrenzing met code-markering

Er is ook enige syntaxisaccentuering met codeblokken in QOwnNotes.

~~~markdown
```bash
# Dit is een opmerking
cd Notes
```
~~~

Ondersteunde talen (en codeblok-ID's):

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

## Tabellen

Tabellen maken geen deel uit van de basis Markdown-specificatie, maar de QOwnNotes-voorbeeldweergave ondersteunt deze. 

~~~markdown
Sluistekens (verticale strepen) kunnen worden gebruikt om kolommen uit te lijnen.

| Tabellen      | Zijn             | Vet   |
| ------------- |:----------------:| -----:|
| kol 3 is      | rechts-uitgevuld | $1600 |
| kol 2 is      | gecentreerd      |   $12 |
| zebra strepen | zijn mooi        |    $1 |

Elke koptekstcel moet met minimaal 3 streepjes gescheiden worden.

U kunt ook inline Markdown gebruiken.

| Minder | Fraaie | Markdown |
| --- | --- | --- |
| *Toch* | `correct` | **weergegeven** |
| 1 | 2 | 3 |
~~~

::: tip
Druk op <kbd>Alt + Shift + T</kbd> om een dialoogvenster te activeren waarmee u tabellen kunt maken. U kunt zelfs CSV-bestanden in dat dialoogvenster importeren.

Gebruik <kbd>Ctrl + Space</kbd> in een markdown-tabel om deze automatisch op te maken.
:::

## Blokcitaten

```markdown
> Blokcitaten zijn erg handig in e-mail om antwoordtekst te emuleren.
> Deze regel maakt deel uit van hetzelfde citaat.

Citaatafbreking.

> Dit is een zeer lange zin die correct wordt geciteerd wanneer deze over meerdere regels wordt verdeeld. Oh jongen, laten we blijven schrijven om er zeker van te zijn dat dit lang genoeg is om voor iedereen omgeslagen te worden. Oh, je kunt **Markdown** in een blokcitaat *plaatsen*. 
```

::: tip
U kunt QOwnNotes vertellen om blockquotes of alleen het blokcitaat-teken volledig te markeren in de *Editor instellingen*

U kunt de [sneltoets](./shortcuts.md) <kbd>Ctrl + Shift + B</kbd> gebruiken om tekst als blokcitaat te markeren.
:::

## Horizontale lijn

Er zijn drie manieren om een ​​horizontale regel te krijgen: koppeltekens, sterretjes of onderstrepingstekens.

```markdown
Drie of meer...

Afbreektekens

---

Asterisks

***

Lage streepjes

___
```

## Nieuwe regel

- U kunt een alinea opsplitsen in meer dan een enkele regel voor eenvoudiger bewerken, ze worden nog steeds weergegeven als een enkele alinea zonder pauzes.
- U kunt een regeleinde binnen een alinea forceren door een regel met twee spaties te beëindigen.
- U kunt een aparte alinea maken door deze af te bakenen met lege regels.

::: tip
Je kunt twee spaties en een nieuwe regel invoeren met <kbd>⇧ Shift</kbd> + <kbd>Return</kbd>.
:::

```markdown
Hier is een regel om mee te beginnen.

Deze regel is door twee nieuwe regels van de vorige gescheiden en daarmee wordt het een *afzonderlijke alinea*.

Deze regel begint ook een aparte alinea, maar...  
Deze regel wordt allen gescheiden door twee spaties en een enkele nieuwe lijn en daarmee wordt het een aparte regel in de *dezelfde alinea*.
```

::: tip
Spaties aan het eind van de regel worden standaard gemarkeerd in de editor.
:::

## Opmerkingen

Opmerkingen worden niet weergegeven in het voorbeeld.

```markdown
[comment]: # (Deze opmerking verschijnt niet in het voorbeeld)

<!-- HTML opmerkingen blijven ook verborgen -->
```

::: tip
Een leidend html-commentaarblok in een notitie wordt ook genegeerd bij het automatisch genereren van notitiebestandsnamen.
:::

## Checkbox-lijsten

U kunt eenvoudige takenlijsten maken met lijsten met selectievakjes.

```markdown
- [x] klaar
- [ ] te doen
```

::: tip
U kunt selectievakjes in het voorbeeld in- of uitschakelen.
:::

## Frontmatter

In QOwnNotes kun je een frontmatter (bijvoorbeeld YAML) gebruiken om wat extra meta-informatie toe te voegen. Het wordt **niet getoond in het voorbeeld** en zal **het automatisch genereren van de bestandsnaam van notities** niet verstoren.

```markdown
---
title: Een titel
description: Een beschrijving
---

# Notitie-koptekst begint hier

Wat tekst
```

De bestandsnaam van deze voorbeeldnotitie zou ` Notitiekop begint hier.md ` zijn.
