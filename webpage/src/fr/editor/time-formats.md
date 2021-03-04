# Formats d'heure

Vous pouvez utiliser votre propre format d'heure lors de l'insertion de l'heure actuelle dans une note.

## Expressions pour la date

| Expression | Production                                                         |
| ---------- | ------------------------------------------------------------------ |
| `d`        | the day as number without a leading zero (1 to 31)                 |
| `dd`       | the day as number with a leading zero (01 to 31)                   |
| `ddd`      | the abbreviated localized day name (e.g. `Mon` to `Sun`).          |
| `dddd`     | the long localized day name (e.g. `Monday` to `Sunday`).           |
| `M`        | the month as number without a leading zero (1-12)                  |
| `MM`       | the month as number with a leading zero (01-12)                    |
| `MMM`      | le nom abrégé du mois localisé (par exemple `Jan` à `Déc`).        |
| `MMMM`     | le long nom du mois localisé (par exemple `janvier` à `décembre`). |
| `yy`       | l'année sous forme de nombre à deux chiffres (00-99)               |
| `yyyy`     | l'année sous forme de nombre à quatre chiffres                     |

## Expressions pour le temps

| Expression  | Production                                                                 |
| ----------- | -------------------------------------------------------------------------- |
| `h`         | l'heure sans zéro non significatif (0 à 23 ou 1 à 12 si affichage AM / PM) |
| `hh`        | the hour with a leading zero (00 to 23 or 01 to 12 if AM/PM display)       |
| `H`         | the hour without a leading zero (0 to 23, even with AM/PM display)         |
| `HH`        | the hour with a leading zero (00 to 23, even with AM/PM display)           |
| `m`         | the minute without a leading zero (0 to 59)                                |
| `mm`        | the minute with a leading zero (00 to 59)                                  |
| `s`         | the second without a leading zero (0 to 59)                                |
| `ss`        | the second with a leading zero (00 to 59)                                  |
| `z`         | the milliseconds without leading zeroes (0 to 999)                         |
| `zzz`       | the milliseconds with leading zeroes (000 to 999)                          |
| `AP` or `A` | use AM/PM display. A/AP will be replaced by either `AM` or `PM`.           |
| `ap` or `a` | use am/pm display. a/ap will be replaced by either `am` or `pm`.           |
| `t`         | the timezone (for example `CEST`)                                          |

## Exemple de chaînes de format

| Format             | Résultat         |
| ------------------ | ---------------- |
| `dd.MM.yyyy hh:mm` | 21.05.2001 14:13 |
| `dd.MM.yyyy`       | 21.05.2001       |
| `ddd MMMM d yy`    | Tue May 21 01    |
| `hh:mm:ss.zzz`     | 14:13:09.042     |
| `h:m:s ap`         | 2:13:9 pm        |

::: tip Info
Jetez un œil à la [documentation Qt](http://doc.qt.io/qt-5/qdatetime.html#toString) pour plus d'informations sur les formats d'heure.
:::

::: tip
Si vous devez effectuer des opérations plus complexes pour afficher l'heure actuelle, veuillez envisager de créer une [action personnalisée](../scripting/methods-and-objects.md#registering-a-custom-action).
:::