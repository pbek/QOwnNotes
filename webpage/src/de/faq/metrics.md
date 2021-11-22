# Warum Metriken?

## Warum sind Metriken nützlich?

QOwnNotes (wie auch die Anwendung selbst) wird **von einer und manchmal auch von zwei Personen in ihrer Freizeit entwickelt**.

Releases werden für viele Plattformen und Distributionen erstellt (oft sehr alte Versionen davon). Sie aufrechtzuerhalten ist viel Arbeit.

Die Metriken sind die einzige wirkliche Informationsquelle darüber, welche Versionen von Plattformen und Distributionen und welche Funktionen von den Benutzern verwendet werden, sodass Entscheidungen getroffen werden können, welche Versionen eingestellt werden und welche Funktionen mehr Arbeit erfordern und welche kaum verwendet werden.

Gleiches gilt für die verwendeten Sprachen der Benutzer, die die Übersetzungen der Anwendung und der Dokumentation vorantreiben.

## Wo werden die Metriken gespeichert?

Die Metriken werden an einen selbst gehosteten [Matomo](https://matomo.org/)-Server übertragen und dort gespeichert. Nur die beiden Hauptentwickler haben Zugriff auf die Daten. **Es werden keine Daten an Dritte weitergegeben.**

## Wie werden Metriken erfasst?

Metriken werden nach Erhalt auf dem Matomo-Server **anonymisiert** (z. B. IP-Adresse entfernt).

Es werden keine persönlichen Informationen, wie von Ihnen eingegebene Texte oder Notiznamen gesammelt oder auf dem Server gespeichert!

## Was wird gesammelt?

Dabei werden einige grundlegende Informationen wie Bildschirmauflösung, Betriebssystem und Version von QOwnNotes übertragen.

Darüber hinaus werden beispielsweise Ereignisse an den Server gesendet, wenn Aktionen in den Anwendungen verwendet werden.

QOwnNotes ist eine kostenlose Open-Source-Software. Sie können die Metriken ständig überprüfen, indem Sie beispielsweise im Quellcode nach [sendVisitIfEnabled](https://github.com/pbek/QOwnNotes/search?q=sendVisitIfEnabled) und [sendEventIfEnabled](https://github.com/pbek/QOwnNotes/search?q=sendEventIfEnabled) suchen, um zu sehen, was genau gesendet wird.

**Es werden keine persönlichen Informationen, wie eingegebene Texte oder Notiznamen, gesammelt!**

## Kann ich Metriken deaktivieren?

Sie können sie beim ersten Start der Anwendung im Willkommensdialog oder zu einem späteren Zeitpunkt im Einstellungsdialog ausschalten.

## Warum sind Metriken standardmäßig nicht deaktiviert?

Standardwerte sind König. Das standardmäßige Deaktivieren von Metriken würde zu keinen aussagekräftigen Metriken führen.

Die meisten Leute interessieren sich nicht für Metriken. Wer sich für Metriken interessiert und nicht mit seinen Metriken darüber abstimmen möchte, woran gearbeitet oder was unterstützt wird, kann diese beim ersten Start der Anwendung einfach abschalten.

Sie erhalten sogar einen zweiten Dialog, der Sie über die Metriken informiert, wenn Sie den Begrüßungsdialog zu früh beendet haben.
