# Formats d'heure

Vous pouvez utiliser votre propre format d'heure lors de l'insertion de l'heure actuelle dans une note.

Vous pouvez utiliser cette page pour personnaliser le format d'horodatage par défaut. Vous pouvez entrer cette chaîne dans _Note_ > _Préférences_ > _Éditeur_ > _Insérer la date et l'heure actuelles_.

## Expressions pour la date

| Expression | Production                                                                            |
| ---------- | ------------------------------------------------------------------------------------- |
| `d`        | le jour sous forme de chiffre, sans zéro non-significatif (1 à 31)                    |
| `dd`       | le jour sous forme de chiffre, avec un zéro non-significatif (01 à 31)                |
| `ddd`      | l'abréviation du jour, dans la langue configurée (par exemple `Lun` à `Dim`).         |
| `dddd`     | le nom complet du jour, dans la langue configurée (par exemple `Lundi` à `Dimanche`). |
| `M`        | le mois sous forme de chiffre, sans zéro non-significatif (1-12)                      |
| `MM`       | le mois sous forme de chiffre, avec un zéro non-significatif (01-12)                  |
| `MMM`      | le nom abrégé du mois localisé (par exemple `Jan` à `Déc`).                           |
| `MMMM`     | le long nom du mois localisé (par exemple `janvier` à `décembre`).                    |
| `yy`       | l'année sous forme de nombre à deux chiffres (00-99)                                  |
| `yyyy`     | l'année sous forme de nombre à quatre chiffres                                        |

## Expressions pour le temps

| Expression  | Production                                                                      |
| ----------- | ------------------------------------------------------------------------------- |
| `h`         | l'heure sans zéro non significatif (0 à 23 ou 1 à 12 si affichage AM / PM)      |
| `hh`        | l'heure avec un zéro non-significatif (00 à 23 ou 01 à 12 si affichage AM / PM) |
| `H`         | l'heure sans zéro non-significatif (0 à 23, même avec l'affichage AM / PM)      |
| `HH`        | l'heure avec un zéro non-significatif (00 à 23, même avec l'affichage AM / PM)  |
| `m`         | les minutes sans zéro non-significatif (0 à 59)                                 |
| `mm`        | les minutes avec un zéro non-significatif (00 à 59)                             |
| `s`         | les secondes sans zéro non-significatif (0 à 59)                                |
| `ss`        | les secondes avec un zéro non-significatif (00 à 59)                            |
| `z`         | les millisecondes sans zéro non-significatif (0 à 999)                          |
| `zzz`       | les millisecondes avec des zéros non-significatif (000 à 999)                   |
| `AP` or `A` | utiliser l'affichage AM/PM. A/AP sera remplacé par `AM` ou `PM`.                |
| `ap` or `a` | utiliser l'affichage am/pm. a/ap sera remplacé par `am` ou `pm`.                |
| `t`         | le fuseau horaire (par exemple `CEST`)                                          |

## Exemple de chaînes de formats

| Format             | Résultat         |
| ------------------ | ---------------- |
| `dd.MM.yyyy hh:mm` | 21.05.2001 14:13 |
| `dd.MM.yyyy`       | 21.05.2001       |
| `ddd MMMM d yy`    | Mar Mai 21 01    |
| `hh:mm:ss.zzz`     | 14:13:09.042     |
| `h:m:s ap`         | 2:13:9 pm        |

::: tip Info
Jetez un œil à la [documentation Qt](http://doc.qt.io/qt-5/qdatetime.html#toString) pour plus d'informations à propos des formats d'horodatage.
:::

::: tip
Si vous devez effectuer des opérations plus complexes pour afficher la date ou l'heure courantes, envisagez de créer une [action personnalisée](../scripting/methods-and-objects.md#registering-a-custom-action).
:::
