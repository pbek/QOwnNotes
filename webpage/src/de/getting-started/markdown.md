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
Das **Navigation panel** zeigt die Struktur Ihrer Überschriften.
:::

Alternativ für H1 und H2; eine unterstrichene Schriftweise:

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

Es gibt auch eine optionale Einstellung zum Aktivieren des Unterstreichungs-Hervorhebung in den *Preview settings*.

```markdown
_unterstreichen_
```

::: tip
Sie können den [shortcut](./shortcuts.md) <kbd>Ctrl + U</kbd> verwenden, um einen Text zu unterstreichen.
:::

## Durchstreichen

```markdown
~~durchstreichen~~
```

::: tip
Sie können den [shortcut](./shortcuts.md) <kbd>Alt + Shift + S</kbd> verwenden, um einen Text durchzustreichen.
:::

## Listen

Es gibt viele Möglichkeiten, Listen zu erstellen.

```markdown
1. Als Erstes bestimmter Listeneintrag
2. Ein weiterer Eintrag
   * Ungeordnete Unterliste.
1. Tatsächliche Zahlen spielen keine Rolle, nur dass es eine Zahl ist!
   1. Geordnete Unterliste
4. Und noch ein Eintrag.

* Ungeordnete Liste kann Sternchen verwenden
- Oder Minusse
+ Oder Plusse
```

::: tip
Wenn Sie am Ende einer Liste die <kbd>Enter</kbd> -Taste drücken, wird ein neues Listenelement erstellt.
:::

## Links

Es gibt mehrere Möglichkeiten, Links zu erstellen.

```markdown
[Ich bin ein Inline-Link] (https://www.google.com)

[Ich bin ein Inline-Link mit Titel] (https://www.google.com "Googles Homepage")

[Sie können Zahlen für Referenzdefinitionen im Referenzstil verwenden] [1]

URLs und URLs in spitzen Klammern werden in der Vorschau automatisch in Links umgewandelt. 
http://www.example.com oder <http://www.example.com>

[1]: https://www.qownnotes.org
```

::: tip
Sie können den [shortcut](./shortcuts.md) <kbd>Ctrl + L</kbd> verwenden: **Erstellen Sie Links zu Webseiten oder anderen Notizen**.

Wenn Sie <kbd>Strg + Umschalt + X</kbd> verwenden, wird ein Dialogfeld angezeigt, durch den Sie in Ihre Notiz **Anhänge einfügen** können.

Sie können <kbd>Strg + Leertaste</kbd> drücken, während sich der Cursor auf einem Link in der Notizbearbeitung befindet, um dem Link zu folgen.
:::

### Lesezeichen

Die von der [QOwnNotes Web Companion-Browsererweiterung](./browser-extension.md) verwendeten Lesezeichen verwenden Links in Listen.

```markdown
- [Name der Webseite] (https://www.example.com) #tag1 #tag2 einige Beschreibungen und Tags
```

## Bilder

Bilder können in QOwnNotes eingebettet werden. Sie werden in der Vorschau angezeigt.

```markdown
![alt text](media/my-image.jpg)
```

::: tip
Sie können die [Tastenkombination](./shortcuts.md) <kbd>Strg + Umschalt + I</kbd> verwenden, um ein Bild in eine Notiz einzufügen. Das Bild kann sich auch in der Zwischenablage befinden. Das Dialogfeld erkennt es und zeigt eine Vorschau an.

Sie können ein Bild auch direkt aus der Zwischenablage mit <kbd>Strg + Umschalt + V</kbd> in Ihre Notiz einfügen.
:::


## Inline-Code und Code-Blöcke

```markdown
Inline-`Code` hat `Back-Ticks`.
```

Sie können die [Verknüpfung](./shortcuts.md) <kbd>Strg + Umschalt + C</kbd> für ausgewählten Inline-Text oder nur innerhalb von Text verwenden, um einen Inline-Codeblock zu erstellen.
:::

Codeblöcke werden entweder durch Zeilen mit drei Back-Ticks eingezäunt oder mit vier Leerzeichen eingerückt.

### Code-Blöcke mit vier führenden Leerzeichen

Fügen Sie vier Leerzeichen vor Ihrem Code hinzu, um ihn als Codeblock zu markieren.

```markdown
    s = "Code mit Leerzeicheneinzug"
     drucken s
```

### Code-Blöcke mit Backticks

Sie können auch drei Backticks verwenden, um einen Codeblock zu erstellen.
~~~markdown
```
Code wird hierhin geschrieben
Code wird hierhin geschrieben
```
~~~

::: tip
Sie können die Verknüpfung [shortcut](./shortcuts.md) <kbd>Strg + Umschalt + C</kbd> in mehreren ausgewählten Textzeilen oder in einer leeren Zeile verwenden, um einen Codeblock zu erstellen.
 
:::

### Backtick-Zaun mit Code-Hervorhebung

Es gibt auch einige Syntaxhervorhebungen mit Codeblöcken in QOwnNotes.

~~~markdown
```bash
# Ich bin ein Kommentar
cd Notes
~~~
~~~

## Tabellen

Tabellen sind nicht Teil der Kern-Markdown-Spezifikation, werden jedoch von der QOwnNotes-Vorschau unterstützt. 

```markdown
Doppelpunkte können zum Ausrichten von Spalten verwendet werden.

| Tables        | Are           | Cool  |
| ------------- |:-------------:| -----:|
| col 3 is      | right-aligned | $1600 |
| col 2 is      | centered      |   $12 |
| zebra stripes | are neat      |    $1 |

Es müssen mindestens 3 Striche vorhanden sein, die jede Kopfzelle trennen.

Sie können auch Inline-Markdown verwenden.

| Markdown | Less | Pretty |
| --- | --- | --- |
| *Still* | `renders` | **nicely** |
| 1 | 2 | 3 |
~~~

::: tip
Drücken Sie <kbd>Alt + Umschalt + T</kbd>, um ein Dialogfeld zu aktivieren, mit dem Sie Tabellen erstellen können. In diesem Dialog können Sie sogar CSV-Dateien importieren!

Verwenden Sie <kbd>Strg + Leertaste</kbd> in einer Markdown-Tabelle, um sie automatisch zu formatieren.
:::

## Zitat-Blöcke

```markdown
> Blockzitate sind in E-Mails sehr praktisch, um sich in Antworttexten auf einen zitierten Textteil zu beziehen.
> Diese Zeile ist Teil desselben Zitats.

Zitatpause.

> Dies ist eine sehr lange Zeile, die beim Umbruch immer noch richtig zitiert wird. Oh Mann, lass uns weiter schreiben, um sicherzustellen, dass dies lang genug ist, um tatsächlich für alle umgebrochen zu werden. Oh, du kannst **Markdown** in ein Blockquote *setzen*. 
```

::: tip
Sie können QOwnNotes anweisen, Blockzitate oder nur das Blockzitatzeichen in den *Editoreinstellungen* vollständig hervorzuheben

Sie können die [ Verknüpfung ](./shortcuts.md) <kbd> Strg + Umschalt + B </kbd> verwenden, um Text als Blockzitat zu markieren.
:::

## Horizontale Linie

Es gibt drei Möglichkeiten, um eine horizontale Regel zu erhalten: Bindestriche, Sternchen oder Unterstriche.

```markdown
Drei oder mehr ...

Bindestriche

---

Sternchen

***

Unterstriche

___
```

## Zeilenumbrüche

- **Mit zwei Zeilenumbrüchen** erhalten Sie einen **neuen Absatz**.
- **Mit einer neuen Zeile** erhalten Sie eine **neue Zeile im selben Absatz**.
- Um einen Zeilenumbruch **ohne Absatz** zu erzielen, müssen Sie **zwei nachgestellte Leerzeichen** verwenden.

```markdown
Hier ist eine Zeile, mit der wir beginnen sollten.

Diese Zeile ist durch zwei Zeilenumbrüche von der obigen getrennt, sodass es sich um einen *separaten Absatz* handelt.

Diese Zeile beginnt auch einen separaten Absatz, aber ...
Diese Zeile wird nur durch zwei nachgestellte Leerzeichen und eine einzelne neue Zeile getrennt, sodass es sich um eine separate Zeile im *gleichen Absatz* handelt.
```

::: tip
Nachgestellte Leerzeichen werden im Editor standardmäßig hervorgehoben.
:::

## Kommentare

Kommentare werden in der Vorschau nicht angezeigt.

```markdown
[Kommentar]: # (Dieser Kommentar wird nicht in der Vorschau angezeigt.)

<! - HTML-Kommentare sind ebenfalls ausgeblendet - >
```

## Kontrollkästchenlisten

Sie können einfache Aufgabenlisten mit Kontrollkästchenlisten erstellen.

```markdown
- [x] done
- [ ] todo
```

::: tip
Sie können Kontrollkästchen in der Vorschau aktivieren / deaktivieren.
:::

## Inhaltsübersicht

In QOwnNotes können Sie eine Frontmatter (z.B. YAML) verwenden, um zusätzliche Metainformationen hinzuzufügen. Es wird **nicht in der Vorschau angezeigt** und **stört die automatische Generierung von Notizendateinamen** nicht.

```markdown
---
Titel: Ein Name
Beschreibung: Ein wenig Beschreibung
---

# Notiz Überschrift beginnt hier

Etwas Text
```

Der Dateiname dieser Beispielnotiz lautet `. Die Überschrift der Notiz beginnt hier.md `.
