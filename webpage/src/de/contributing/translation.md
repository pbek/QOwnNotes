# Übersetzung

Vielen Dank für Ihre Hilfe!

Wie kann ich bei der Übersetzung von QOwnNotes helfen?

## Melden Sie sich bei Crowdin an, um QOwnNotes zu übersetzen

![crowdin](/img/crowdin.png)

**Crowdin** ist eine beliebte Lokalisierungsverwaltungsplattform für mobile Apps, Web, Desktop-Software und zugehörige Assets.

Sie können sich unter [Crowdin](https://crowdin.com/project/qownnotes/invite) und anmelden und **QOwnNotes** mit einfachen Tools in Ihre Sprache **übersetzen**.

**[QOwnNotes auf Crowdin](https://crowdin.com/project/qownnotes/invite)**

[Schreiben Sie dem Projektinhaber einfach eine Nachricht auf Crowdin](https://crowdin.com/profile/pbek) oder als [GitHub-Ausgabe](https://github.com/pbek/QOwnNotes/issues), wenn Sie QOwnNotes in eine neue Sprache übersetzen möchten.

Wenn Sie **vorhandene Sprachen** um neue Übersetzungen erweitern möchten, können Sie dies sofort tun.

## Korrekturlesen

Wenn Sie ein ausgezeichneter oder sogar Muttersprachler einer bereits übersetzten Sprache sind, die noch nicht Korrektur gelesen wurde, sind wir Ihnen dankbar, wenn Sie dazu beitragen, die QOwnNotes-Erfahrung zuverlässiger und komfortabler zu gestalten, indem Sie nicht genehmigte Übersetzungen überprüfen und gegebenenfalls korrigieren. Sie müssen den Eigentümer um die Erlaubnis bitten, Korrektor für Ihre Sprache zu sein.

Klicken Sie hier um zu sehen, ob Ihre Sprache zu den bereits Übersetzten gehört!

[QOwnNotes auf Crowdin](https://translate.qownnotes.org/)

Sobald Sie den [Projektinhaber auf Crowdin kontaktiert haben](https://crowdin.com/profile/pbek) und die Erlaubnis zum Korrekturlesen in Ihrer Sprache erhalten haben, können Sie loslegen!

## Allgemeine Hinweise

Bitte beachten Sie die folgenden Hinweise zu Ihrer Arbeit am Projekt.

### Genauigkeit

Bitte lokalisieren Sie mit größtmöglicher Genauigkeit und achten Sie dabei auf die Beherrschung Ihrer Zielsprache.

### Präzision

Das Ziel muss die Quelle genau und ohne Auslassungen oder Hinzufügungen von Teilen der Zeichenfolge darstellen.

### Context

Wenn Sie mehr Kontext oder Hilfe bei einer bestimmten Zeichenfolge benötigen und zufällig eine andere Sprache kennen, können Sie die Sprache ändern und sehen, was andere Übersetzer aus dem Segment gemacht haben. Aber zögern Sie nicht, rechts auf der Seite eine Nachricht zu hinterlassen und um Klärung zu bitten.

### Häufig verwendete Schlüsselbegriffe

Es ist notwendig, für die häufig verwendeten Schlüsselbegriffe die gleichen Übersetzungen zu prüfen und zu verwenden, z. B. "Notiz" muss immer auf die gleiche Weise übersetzt werden.

Wenn `trashed notes` auf Deutsch mit `gelöschte Notizen` übersetzt wurde, verwenden Sie immer diesen Ausdruck. Eine Ausnahme davon ist, wenn Sie grammatikalisch oder sprachlich gebunden sind, eine Alternative zu verwenden.

![screenshot](/img/crowdin/screenshot-7.png)

Um alle identischen Ausdrücke zu finden, durchsuchen Sie Crowdin, um zu sehen, ob alle Vorkommen eines Begriffs tatsächlich dieselbe Übersetzung haben. Sie können eine bessere Übersetzung vorschlagen und über die anderen abstimmen.

Lokalisieren Sie alle Vorkommen so identisch wie möglich.

`Notiz` sollte auf jeden Fall immer nur eine Übersetzung haben.

### Plural Formen

Für Plural fügen Sie bitte jeweils die entsprechende Übersetzung in den entsprechenden Block ein.

![screenshot](/img/crowdin/screenshot-4.png)

### Variablen

Die `%1`-Variablen müssen ohne Leerzeichen geschrieben werden. In RTL-Sprachen können Sie die Variablen entweder aus dem Original einfügen oder in Ihre Übersetzung schreiben.

![screenshot](/img/crowdin/screenshot-1.png)

![screenshot](/img/crowdin/screenshot-5.png)

![screenshot](/img/crowdin/screenshot-3.png)

### Zugangsschlüssel

Verwenden Sie `&` vor jedem Buchstaben, der in einer Verknüpfung verwendet werden soll, kann derselbe wie im Original sein, muss es aber nicht.

![screenshot](/img/crowdin/screenshot-4.png)

### Benutzerdefinierte Container

Strings wie `::: tip`, `::: warning` und `::: info` in der Dokumentation sind Markierungen für [custom containers](https://vuepress.vuejs.org/guide/markdown.html#custom-containers) in VuePress und sollten nicht übersetzt werden.

### Andere nicht zu übersetzende Zeichenfolgen

Einige Segmente, die wir in der Dokumentation nicht übersetzen:

- Übersetzen Sie niemals Codes oder Links; Tags müssen so belassen werden, wie sie sind und ohne zusätzliche Leerzeichen, (wie das manche maschinellen Übersetzungen tun, dann müssen diese Zeichen händisch entfernt werden)
- Wir übersetzen Datum und Uhrzeit nicht (leer lassen)
- Und wir übersetzen auch keine Referenzen (die QON in zahlreichen Sprachen bekommen hat), Sie können dieses Kapitel einfach überspringen.

## Übersetzen und Korrekturlesen kann spannend sein!

Sie setzen sich mit der Sprache dieser Software auseinander, lernen neue Begriffe und erhalten viele Insider-Informationen rund um das Projekt.

Wenn Sie also bereit sind, beim Korrekturlesen der Übersetzung in Ihrer Sprache zu helfen, wenden Sie sich an [Patrizio](https://crowdin.com/profile/pbek) und fragen Sie nach, wenn Sie Fragen haben und/oder die Rechte für das Korrekturlesen in Ihrer Muttersprache erhalten möchten.

Vielen Dank für Deine Hilfe! 🙂
