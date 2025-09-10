# Warum Metriken?

## Warum sind Metriken nützlich?

QOwnNotes (as in the application itself) is **developed by mostly one person in his spare time**.

Versionen werden für viele (häufig sehr alte Versionen von) Plattformen und Distributionen erstellt. Sie aufrechtzuerhalten ist viel Arbeit.

Die Metriken sind die einzige zuverlässige Informationsquelle darüber, auf welchen Versionen von Plattformen und Distributionen QOwnNotes ausgeführt wird. Sie zeigen auch, welche Features am beliebtesten sind, sodass Entscheidungen darüber getroffen werden können, welche Versionen ausgemustert werden, welche Features mehr Arbeit benötigen und welche kaum genutzt werden.

Gleiches gilt für die Auswahl der bevorzugten Sprachen durch den Benutzer, die die Verbesserung der entsprechenden Übersetzungen fördert.

## Wo werden die Metriken gespeichert?

Die Metriken werden an einen selbst gehosteten [Matomo](https://matomo.org/)-Server übertragen und dort gespeichert. Nur die beiden Hauptentwickler haben Zugriff auf die Daten. **Es werden keine Daten an Dritte weitergegeben.**

## Wie werden Metriken erfasst?

Metriken werden nach Erhalt auf dem Matomo-Server **anonymisiert** (z.B. werden IP-Adressen entfernt).

Es werden keine persönlichen Informationen, wie von Ihnen eingegebene Texte oder Notiznamen, gesammelt oder auf dem Server gespeichert!

## Was wird gesammelt?

Einige grundlegende Informationen wie Bildschirmauflösung, Betriebssystem und QOwnNotes-Version werden übertragen.

Darüber hinaus werden beispielsweise Ereignisse an den Server gesendet, wenn Aktionen in den Anwendungen verwendet werden.

QOwnNotes ist eine kostenlose Open-Source-Software. Sie können die Metriken ständig überprüfen, indem Sie beispielsweise im Quellcode nach [sendVisitIfEnabled](https://github.com/pbek/QOwnNotes/search?q=sendVisitIfEnabled) und [sendEventIfEnabled](https://github.com/pbek/QOwnNotes/search?q=sendEventIfEnabled) suchen, um zu sehen, was genau gesendet wird.

**Es werden keine persönlichen Informationen, wie eingegebene Texte oder Notiznamen, gesammelt!**

## Kann ich Metriken deaktivieren?

Sie können sie beim ersten Start der Anwendung im Willkommensdialog oder zu einem späteren Zeitpunkt im Einstellungsdialog ausschalten.

## Warum sind Metriken standardmäßig nicht deaktiviert?

Standardwerte sind König. Das standardmäßige Deaktivieren von Metriken würde zu keinen aussagekräftigen Metriken führen.

Die meisten Leute interessieren sich nicht für Metriken. Wer sich für Metriken interessiert und nicht mit seinen Metriken darüber abstimmen möchte, woran gearbeitet oder was unterstützt wird, kann diese beim ersten Start der Anwendung einfach abschalten.

Sie erhalten sogar einen zweiten Dialog, der Sie über die Metriken informiert, wenn Sie den Begrüßungsdialog zu früh beendet haben.
