# Tijdnotatie

U kunt uw eigen tijdnotatie gebruiken wanneer u de huidige tijd in een notitie invoegt.

U kunt deze pagina gebruiken om een persoonlijke standaard tijdnotatie aan te maken. U kunt de tekenreeks invoeren bij _Note_ > _Instellingen_ > _Editor_ > _Huidige tijd invoegen_.

## Uitdrukkingen voor de datum

| Uitdrukking | Uitvoer                                                           |
| ----------- | ----------------------------------------------------------------- |
| `d`         | de dag als getal zonder voorloopnul (1 tot 31)                    |
| `dd`        | de dag als nummer met voorloopnul (01 tot 31)                     |
| `ddd`       | de afgekorte gelokaliseerde dagnaam (bijv.`ma`tot`zo`).           |
| `dddd`      | de lange gelokaliseerde dagnaam (bijv. `maandag` tot `zondag`).   |
| `M`         | de maand als getal zonder voorloopnul (1-12)                      |
| `MM`        | de maand als getal met voorloopnul (01-12)                        |
| `MMM`       | de afgekorte gelokaliseerde maandnaam (bijv. `jan` tot `dec`).    |
| `MMMM`      | de lange gelokaliseerde maandnaam (bijv. `januari`tot`december`). |
| `yy`        | het jaar als tweecijferig nummer (00-99)                          |
| `yyyy`      | het jaar als viercijferig getal                                   |

## Uitdrukkingen voor tijd

| Uitdrukking | Uitvoer                                                                   |
| ----------- | ------------------------------------------------------------------------- |
| `h`         | het uur zonder voorloopnul (0 tot 23 of 1 tot 12 bij AM / PM-weergave)    |
| `hh`        | het uur met een voorloopnul (00 tot 23 of 01 tot 12 als AM / PM-weergave) |
| `H`         | het uur zonder voorloopnul (0 tot 23, zelfs met AM / PM-weergave)         |
| `HH`        | het uur met een voorloopnul (00 tot 23, zelfs met AM / PM-weergave)       |
| `m`         | de minuut zonder voorloopnul (0 tot 59)                                   |
| `mm`        | de minuut met een voorloopnul (00 tot 59)                                 |
| `s`         | de tweede zonder voorloopnul (0 tot 59)                                   |
| `ss`        | de tweede met een voorloopnul (00 tot 59)                                 |
| `z`         | de milliseconden zonder voorloopnullen (0 tot 999)                        |
| `zzz`       | de milliseconden met voorloopnullen (000 tot 999)                         |
| `AP` of `A` | gebruik AM / PM-weergave. A/ AP wordt vervangen door `AM` of `PM`.        |
| `ap` of `a` | gebruik de am/ pm-weergave. a/ ap wordt vervangen door `am` of `pm`.      |
| `t`         | de tijdzone (bijvoorbeeld `CEST`)                                         |

## Voorbeeld opmaakstrings

| Indeling           | Resultaat        |
| ------------------ | ---------------- |
| `dd.MM.yyyy hh:mm` | 21.05.2001 14:13 |
| `dd.MM.yyyy`       | 21.05.2001       |
| `ddd MMMM d yy`    | Tue May 21 01    |
| `hh:mm:ss.zzz`     | 14:13:09.042     |
| `h:m:s ap`         | 2:13:9 pm        |

::: tip Info
Bestudeer de [Qt-documentatie](http://doc.qt.io/qt-5/qdatetime.html#toString) voor meer informatie over tijdopmaak.
:::

::: tip
Als u complexere bewerkingen moet uitvoeren om de huidige tijd uit te voeren overweeg dan een [aangepaste actie](../scripting/methods-and-objects.md#registering-a-custom-action) aan te maken.
:::
