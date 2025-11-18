# Übersetzung

Vielen Dank für deine Hilfe!

Wie kann ich bei der Übersetzung von QOwnNotes helfen?

::: warning
Please don't attempt to edit the translation files in `src/languages` directly. They are all generated automatically via [Crowdin](https://crowdin.com/project/qownnotes).
:::

## Melde dich bei Crowdin an, um QOwnNotes zu übersetzen

![crowdin](/img/crowdin.png)

**Crowdin** ist eine beliebte Lokalisierungsverwaltungsplattform für mobile Apps, Web, Desktop-Software und zugehörige Assets.

Du kannst dich bei [Crowdin](https://crowdin.com/project/qownnotes) anmelden und **QOwnNotes** ganz einfach in deine Sprache **übersetzen**.

**[QOwnNotes auf Crowdin](https://crowdin.com/project/qownnotes)**

[Schreibe dem Projektinhaber einfach eine Nachricht auf Crowdin](https://crowdin.com/profile/pbek) oder als [GitHub-Issue](https://github.com/pbek/QOwnNotes/issues), wenn du QOwnNotes in eine neue Sprache übersetzen möchtest.

Wenn du **vorhandene Sprachen** um neue Übersetzungen erweitern möchtest, kannst du das sofort tun.

## Korrekturlesen

Falls du Muttersprachler einer Sprache bist, oder eine Sprache exzellent beherrschst, die bisher noch nicht Korrektur gelesen wurde, sind wir dir sehr dankbar, wenn du dazu beitragen würdest, die QOwnNotes-Erfahrungen zuverlässiger und angenehmer zu gestalten, indem du die noch nicht genehmigten Übersetzungen überprüfst und gegebenenfalls korrigierst. Um Korrektor für deine Sprache zu werden, musst du den Projektinhaber um Erlaubnis bitten.

Klicke hier um zu sehen, ob deine Sprache zu den bereits Übersetzten gehört!

[QOwnNotes auf Crowdin](https://translate.qownnotes.org/)

Sobald du den [Projektinhaber auf Crowdin kontaktiert hast](https://crowdin.com/profile/pbek) und die Erlaubnis zum Korrekturlesen in deiner Sprache erhalten hast, kannst du loslegen!

## Allgemeine Hinweise

Bitte beachte folgende Hinweise zu deiner Arbeit am Projekt.

### Genauigkeit

Bitte lokalisiere mit größtmöglicher Genauigkeit und achte dabei auf den Lesefluss deiner Zielsprache.

### Präzision

Das Ziel muss die Quelle genau und ohne Auslassungen oder Hinzufügungen von Teilen der Zeichenfolge darstellen.

### Kontext

Wenn du mehr Kontext oder Hilfe bei einer bestimmten Zeichenfolge benötigst und zufällig eine andere Sprache kennst, kannst du die Sprache ändern und sehen, was andere Übersetzer aus dem Segment gemacht haben. Aber zögere nicht, rechts auf der Seite eine Nachricht zu hinterlassen und um Klärung zu bitten.

### Häufig verwendete Schlüsselbegriffe

Es ist notwendig, für die häufig verwendeten Schlüsselbegriffe die gleichen Übersetzungen zu prüfen und zu verwenden, z. B. "Notiz" muss immer auf die gleiche Weise übersetzt werden.

Wenn `trashed notes` auf Deutsch mit `gelöschte Notizen` übersetzt wurde, verwenden Sie immer diesen Ausdruck. Eine Ausnahme davon ist, wenn du grammatikalisch oder sprachlich gebunden bist, eine Alternative zu verwenden.

![screenshot](/img/crowdin/screenshot-7.png)

Um alle identischen Ausdrücke zu finden, durchsuche Crowdin, um zu sehen, ob alle Vorkommen eines Begriffs tatsächlich dieselbe Übersetzung haben. Du kannst eine bessere Übersetzung vorschlagen und über die anderen abstimmen.

Lokalisiere alle Vorkommen so identisch wie möglich.

`Notiz` sollte auf jeden Fall immer nur eine Übersetzung haben.

### Plural Formen

Füge bitte für den Plural jeweils die angemessene Übersetzung in den entsprechenden Block ein.

![screenshot](/img/crowdin/screenshot-4.png)

### Variablen

Die `%1`-Variablen müssen ohne Leerzeichen geschrieben werden. In linksläufigen Sprachen kannst du die Variablen entweder aus dem Original einfügen oder in deine Übersetzung schreiben.

![screenshot](/img/crowdin/screenshot-1.png)

![screenshot](/img/crowdin/screenshot-5.png)

![screenshot](/img/crowdin/screenshot-3.png)

### Zugangsschlüssel

Verwende `&` vor jedem Buchstaben, der in einer Verknüpfung verwendet werden soll, die dieselbe wie im Original sein kann, aber nicht muss.

![screenshot](/img/crowdin/screenshot-4.png)

### Benutzerdefinierte Container

Strings wie `::: tip`, `::: warning` und `::: info` in der Dokumentation sind Markierungen für [custom containers](https://vuepress.vuejs.org/guide/markdown.html#custom-containers) in VuePress und sollten nicht übersetzt werden.

### Andere nicht zu übersetzende Zeichenfolgen

Einige Segmente, die wir in der Dokumentation nicht übersetzen:

- Übersetze niemals Codes oder Links; Tags müssen so belassen werden, wie sie sind (und ohne zusätzliche Leerzeichen, wie das manche maschinellen Übersetzungen tun; dann müssen diese Zeichen händisch entfernt werden)
- Datum und Uhrzeit übersetzen wir nicht (leer lassen)
- Und wir übersetzen auch keine Referenzen (die QOwnNotes in zahlreichen Sprachen bekommen hat). Sie können dieses Kapitel einfach überspringen

## Übersetzen und Korrekturlesen kann spannend sein!

Du wirst dich mit der Sprache dieser Software auseinandersetzen, neue Begriffe lernen und viele Insider-Informationen rund um das Projekt erhalten.

Wenn du also bereit bist, beim Korrekturlesen der Übersetzung in deiner Sprache zu helfen, wende dich an [Patrizio](https://crowdin.com/profile/pbek) und erkundige dich, wenn du Fragen hast und/oder die Rechte für das Korrekturlesen in deiner Muttersprache erhalten möchtest.

Vielen Dank für Deine Hilfe! 🙂
