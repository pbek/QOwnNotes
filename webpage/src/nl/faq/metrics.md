# Waarom statistieken?

## Waarom zijn statistieken nuttig?

QOwnNotes (zoals in de applicatie zelf) is **ontwikkeld door één en soms twee personen in hun vrije tijd**.

Releases zijn gebouwd voor veel platforms en distributies (vaak zeer oude versies ervan). Het is veel werk om ze bij te houden.

De metrische gegevens zijn de enige echte bron van informatie over welke versies van platforms en distributies en welke functies door de gebruikers worden gebruikt, zodat beslissingen kunnen worden genomen welke versies buiten gebruik worden gesteld en welke functies meer werk vergen en welke nauwelijks worden gebruikt.

Hetzelfde geldt voor de gebruikte talen van de gebruikers, die de vertalingen van de applicatie en de documentatie aansturen.

## Waar worden de statistieken opgeslagen?

De statistieken worden verzonden naar en opgeslagen op een zelf-gehoste [Matomo](https://matomo.org/)-server. Alleen de twee belangrijkste ontwikkelaars hebben toegang tot de gegevens. **Er worden geen gegevens gedeeld met derden.**

## Hoe worden statistieken verzameld?

Metrieken worden **geanonimiseerd** (bijv. ip-adres verwijderd) op de Matomo-server nadat ze zijn ontvangen.

Er wordt geen persoonlijke informatie, zoals ingevoerde tekst of notitienamen, verzameld of opgeslagen op de server!

## Wat wordt er verzameld?

Sommige basisinformatie zoals schermresolutie, besturingssysteem en gebruikte versie van QOwnNotes wordt verzonden.

Verder worden er bijvoorbeeld gebeurtenissen naar de server gestuurd wanneer acties in de applicaties worden gebruikt.

QOwnNotes is gratis open source software. You can review the metrics all the time by searching for example for [sendVisitIfEnabled](https://github.com/pbek/QOwnNotes/search?q=sendVisitIfEnabled) and [sendEventIfEnabled](https://github.com/pbek/QOwnNotes/search?q=sendEventIfEnabled) in the source code to see what exactly is sent.

**Er wordt geen persoonlijke informatie verzameld, zoals ingevoerde tekst of notitienamen!**

## Kan ik statistieken uitschakelen?

U kunt ze uitschakelen bij de eerste start van de toepassing in het welkomstvenster of op een later tijdstip in het instellingenvenster.

## Waarom zijn metrische gegevens niet standaard uitgeschakeld?

Standaardwaarden zijn koning. Als u statistieken standaard uitschakelt, krijgt u helemaal geen zinvolle statistieken.

De meeste mensen geven niet om statistieken. Degenen die om metrische gegevens geven en niet met hun metrische gegevens willen stemmen over waar aan wordt gewerkt of wat wordt ondersteund, kunnen ze gemakkelijk uitschakelen bij de eerste start van de applicatie.

U krijgt zelfs een tweede dialoogvenster dat u over de statistieken vertelt, als u het welkomstdialoogvenster te vroeg hebt beëindigd.
