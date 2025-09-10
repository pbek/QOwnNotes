# Waarom statistieken?

## Waarom zijn statistieken nuttig?

QOwnNotes (zoals in de applicatie zelf) is **grotendeels ontwikkeld door één persoon in zijn vrije tijd**.

Releases zijn gebouwd voor veel platforms en distributies (vaak zeer oude versies ervan). Het is veel werk om ze bij te houden.

Die Metriken sind die einzige zuverlässige Informationsquelle darüber, auf welchen Versionen von Plattformen und Distributionen QOwnNotes ausgeführt wird. Sie zeigen auch, welche Funktionen am beliebtesten sind, sodass Entscheidungen getroffen werden können, welche Versionen eingestellt werden, welche Funktionen mehr Arbeit erfordern und welche kaum genutzt werden.

Hetzelfde geldt voor de selectie van voorkeurstalen door de gebruiker, wat de verbetering van de bijbehorende vertalingen bevordert.

## Waar worden de statistieken opgeslagen?

De statistieken worden verzonden naar en opgeslagen op een zelf-gehoste [Matomo](https://matomo.org/)-server. Alleen de twee belangrijkste ontwikkelaars hebben toegang tot de gegevens. **Er worden geen gegevens gedeeld met derden.**

## Hoe worden statistieken verzameld?

Metrieken worden **geanonimiseerd** (bijv. ip-adres verwijderd) op de Matomo-server nadat ze zijn ontvangen.

Er wordt geen persoonlijke informatie, zoals ingevoerde tekst of notitienamen, verzameld of opgeslagen op de server!

## Wat wordt er verzameld?

Sommige basisinformatie zoals schermresolutie, besturingssysteem en gebruikte versie van QOwnNotes wordt verzonden.

Verder worden er bijvoorbeeld gebeurtenissen naar de server gestuurd wanneer acties in de applicaties worden gebruikt.

QOwnNotes is gratis open source software. Sie können die Metriken ständig überprüfen, indem Sie beispielsweise im Quellcode nach [sendVisitIfEnabled](https://github.com/pbek/QOwnNotes/search?q=sendVisitIfEnabled) und [sendEventIfEnabled](https://github.com/pbek/QOwnNotes/search?q=sendEventIfEnabled) suchen, um zu sehen, was genau gesendet wird.

**Er wordt geen persoonlijke informatie verzameld, zoals ingevoerde tekst of notitienamen!**

## Kan ik statistieken uitschakelen?

U kunt ze uitschakelen bij de eerste start van de toepassing in het welkomstvenster of op een later tijdstip in het instellingenvenster.

## Waarom zijn metrische gegevens niet standaard uitgeschakeld?

Standaardwaarden zijn koning. Als u statistieken standaard uitschakelt, krijgt u helemaal geen zinvolle statistieken.

De meeste mensen geven niet om statistieken. Degenen die om metrische gegevens geven en niet met hun metrische gegevens willen stemmen over waar aan wordt gewerkt of wat wordt ondersteund, kunnen ze gemakkelijk uitschakelen bij de eerste start van de applicatie.

U krijgt zelfs een tweede dialoogvenster dat u over de statistieken vertelt, als u het welkomstdialoogvenster te vroeg hebt beëindigd.
