# Markdown

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
Hervorhebung durch "Kursivschrift", mit *Sternchen*.

Starke Hervorhebung, auch: "fett", mit **Sternchen**.
```

::: tip
Sie können die [Tastenkombination](./shortcuts.md) <kbd>Strg + B</kbd> verwenden, um Text fett hervorzuheben, und <kbd>Strg + I</kbd>, um ihn kursiv hervorzuheben.
:::

## Unterstreichen

Es gibt auch eine optionale Einstellung in den *Vorschau-Einstellungen*, um das Unterstreichen zu aktivieren.

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
2. Ein weiterer Eintrag
   * Ungeordnete Unterliste.
1. Tatsächliche Zahlen spielen keine Rolle, nur dass es eine Zahl ist!
   1. Geordnete Unterliste (funktioniert nur im Editor, nicht in der Vorschau)
4. Und noch ein Eintrag.

* Ungeordnete Liste kann Sternchen verwenden
- Oder Minusse
+ Oder Plusse
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

```markdown
[Ich verlinke zur Notiz Journal.md](Journal.md)

<Journal.md> funktioniert ähnlich.
```

::: tip
Sie können den [shortcut](./shortcuts.md) <kbd>Ctrl + L</kbd> verwenden: **Erstellen Sie Links zu Webseiten oder anderen Notizen**.

Wenn Sie <kbd>Strg + Umschalt + X</kbd> verwenden, wird ein Dialogfeld angezeigt, durch das Sie in Ihre Notiz **Anhänge einfügen** können.

Sie können <kbd>Strg + Leertaste</kbd> drücken, während sich der Cursor auf einem Link in der Notizbearbeitung befindet, um dem Link zu folgen.
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
~~~markdown
```
Code kommt hierher
Code kommt hierher
```
~~~

::: tip
Du kannst den [shortcut](./shortcuts.md) <kbd>Strg + Umschalt + C</kbd> auf mehreren ausgewählten Textzeilen oder in einer leeren Zeile benutzen, um einen Codeblock zu generieren. 
:::

### Backtick-Zaun mit Code-Hervorhebung

Es gibt auch einige Syntaxhervorhebungen mit Codeblöcken in QOwnNotes.

~~~markdown
```bash
# Ich bin ein Kommentar
cd Notes
```
~~~

Momentan unterstützte Sprachen (und Codeblock Identifier) sind:

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

Tabellen sind nicht Teil vom Kern-Markdown, aber QOwnNotes unterstützt sie. 

~~~markdown
Doppelpunkte können zum Ausrichten von Spalten verwendet werden.

| Tabellen      | Sind          | Cool  |
| ------------- |:-------------:| -----:|
| Spalte 3 ist  | rechtsbündig  | $1600 |
| Spalte 3 ist  | zentriert     |   $12 |
| Zebrastreifen | sind toll     |    $1 |

Die Kopfzeilen-Zellen müssen immer durch mindestens drei Bindestriche getrennt werden.

Sie können auch inline-Markdown verwenden.

| Markdown | Weniger | Hübsch |
| --- | --- | --- |
| *Rendert* | `immernoch` | **schön** |
| 1 | 2 | 3 |
~~~

::: tip
Drücken Sie <kbd>Alt + Umschalt + T</kbd>, um ein Dialogfeld zu aktivieren, mit dem Sie Tabellen erstellen können. In diesem Dialog können Sie sogar CSV-Dateien importieren!

Verwenden Sie <kbd>Strg + Leertaste</kbd> in einer Markdown-Tabelle, um sie automatisch zu formatieren.
:::

## Zitat-Blöcke

```markdown
> Blockzitate sind sehr nützlich, um Antworttext in E-Mails zu emulieren.
> Diese Zeile ist Teil desselben Zitats.

Zitatpause.

> Das ist eine sehr lange Zeile, die nach einem Zeilenumbruch immer noch richtig zitiert wird. Oh Mann, lass uns weiter schreiben, um sicherzustellen, dass es lang genug ist, um tatsächlich für alle umgebrochen zu werden. Oh, Sie können **Markdown** in ein Blockquote *packen*. 
```

::: tip
Sie können QOwnNotes anweisen, Blockzitate oder nur das Blockzitatzeichen in den *Editoreinstellungen* vollständig hervorzuheben

Sie können die [ Verknüpfung ](./shortcuts.md) <kbd> Strg + Umschalt + B </kbd> verwenden, um Text als Blockzitat zu markieren.
:::

## Horizontale Linie

Es gibt drei Möglichkeiten, um eine horizontale Linie zu erhalten: Bindestriche, Sternchen oder Unterstriche.

```markdown
Drei oder mehr...

Bindestriche

---

Sternchen

***

Unterstriche

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

Diese Zeile ist von der vorherigen getrennt, weshalb sie ein *separater Absatz* ist.

Diese Zeile fängt ebenfalls einen separaten Absatz an, aber...  
Diese Zeile wird nur durch zwei anhängende Leerzeichen und einen einzigen Zeilenumbruch getrennt, weshalb sie eine separate Zeile im *selben Absatz* ist.
```

::: tip
Nachgestellte Leerzeichen werden im Editor standardmäßig hervorgehoben.
:::

## Kommentare

Kommentare werden in der Vorschau nicht angezeigt.

```markdown
[comment]: # (Dieser Kommentar erscheint nicht in der Vorschau)

<!-- HTML Kommentare werden ebenfalls verborgen -->
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
