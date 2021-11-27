# Why metrics?

## Why are metrics useful?

QOwnNotes (as in the application itself) is **developed by one and sometimes two persons in their spare time**.

Releases zijn gebouwd voor veel platforms en distributies (vaak zeer oude versies ervan). Het is veel werk om ze bij te houden.

De metrische gegevens zijn de enige echte bron van informatie over welke versies van platforms en distributies en welke functies door de gebruikers worden gebruikt, zodat beslissingen kunnen worden genomen welke versies buiten gebruik worden gesteld en welke functies meer werk vergen en welke nauwelijks worden gebruikt.

Hetzelfde geldt voor de gebruikte talen van de gebruikers, die de vertalingen van de applicatie en de documentatie aansturen.

## Waar worden de statistieken opgeslagen?

De statistieken worden verzonden naar en opgeslagen op een zelf-gehoste [Matomo](https://matomo.org/)-server. Alleen de twee belangrijkste ontwikkelaars hebben toegang tot de gegevens. **Er worden geen gegevens gedeeld met derden.**

## How are metrics collected?

Metrics are **anonymized** (e.g. ip address stripped out) on the Matomo server after they are received.

No personal information, like text you entered or note names are collected or stored on the server!

## What is collected?

Some basic information like screen resolution, operating system and version of QOwnNotes used is transmitted.

Furthermore, for example events are sent to the server when actions are used in the applications.

QOwnNotes is free open source software. You can review the metrics all the time by searching for example for [sendVisitIfEnabled](https://github.com/pbek/QOwnNotes/search?q=sendVisitIfEnabled) and [sendEventIfEnabled](https://github.com/pbek/QOwnNotes/search?q=sendEventIfEnabled) in the source code to see what exactly is sent.

**No personal information, like text you entered or note names are collected!**

## Can I turn metrics off?

You can turn them off upon first start of the application in the welcome dialog or at a later time in the settings dialog.

## Why are metrics not off by default?

Defaults are king. Turning metrics off by default would result in no meaningful metrics at all.

Most of the people don't care about metrics. Those who care about metrics and don't want to vote with their metrics about what is worked on or what is supported can easily turn them off at the first start of the application.

You will even get a second dialog that will tell you about the metrics, if you finished the welcome dialog too early.
