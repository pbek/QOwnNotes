# Warum Metriken?

## Warum sind Metriken nützlich?

QOwnNotes (wie auch die Anwendung selbst) wird **von einer und manchmal auch von zwei Personen in ihrer Freizeit entwickelt**.

Releases are built for a lot of platforms and distributions (often very old versions of them). Keeping them up is a lot of work.

The metrics are the only real source of information about which versions of platforms and distributions and what features are used by the users, so decisions can be made which versions to retire and which features need more work and which are hardly used.

The same goes for the used languages of the users, which drives the translations of the application and the documentation.

## Where are the metrics stored?

The metrics are transmitted to and stored on a self-hosted [Matomo](https://matomo.org/) server. Only the two main developers have access to the data. **No data is shared with 3rd parties.**

## How are metrics collected?

Metrics are **anonymized** (e.g. ip address stripped out) on the Matomo server after they are received.

No personal information, like text you entered or note names are collected or stored on the server!

## What is collected?

Some basic information like screen resolution, operating system and version of QOwnNotes used is transmitted.

Furthermore, for example events are sent to the server when actions are used in the applications.

QOwnNotes is free open source software. You can review the metrics all the time by searching for example for [sendVisitIfEnabled](https://github.com/pbek/QOwnNotes/search?q=sendVisitIfEnabled) and [sendEventIfEnabled](https://github.com/pbek/QOwnNotes/search?q=sendEventIfEnabled) in the source code to see what exactly is sent.

**No personal information, like text you entered or note names are collected!**

## Can I turn metrics off?

Sie können sie beim ersten Start der Anwendung im Willkommensdialog oder zu einem späteren Zeitpunkt im Einstellungsdialog ausschalten.

## Warum sind Metriken standardmäßig nicht deaktiviert?

Standardwerte sind König. Das standardmäßige Deaktivieren von Metriken würde zu keinen aussagekräftigen Metriken führen.

Die meisten Leute interessieren sich nicht für Metriken. Wer sich für Metriken interessiert und nicht mit seinen Metriken darüber abstimmen möchte, woran gearbeitet oder was unterstützt wird, kann diese beim ersten Start der Anwendung einfach abschalten.

Sie erhalten sogar einen zweiten Dialog, der Sie über die Metriken informiert, wenn Sie den Begrüßungsdialog zu früh beendet haben.
