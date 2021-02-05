# Formatos de hora:

Puede utilizar su propio formato de hora al insertar la hora actual en una nota.

## Expresiones para la fecha

| Expresión | Salida                                                        |
| --------- | ------------------------------------------------------------- |
| `d`       | the day as number without a leading zero (1 to 31)            |
| `dd`      | the day as number with a leading zero (01 to 31)              |
| `ddd`     | the abbreviated localized day name (e.g. `Mon` to `Sun`).     |
| `dddd`    | the long localized day name (e.g. `Monday` to `Sunday`).      |
| `M`       | the month as number without a leading zero (1-12)             |
| `MM`      | the month as number with a leading zero (01-12)               |
| `MMM`     | the abbreviated localized month name (e.g. `Jan` to `Dec`).   |
| `MMMM`    | the long localized month name (e.g. `January` to `December`). |
| `yy`      | the year as two digit number (00-99)                          |
| `yyyy`    | el año como número de cuatro dígitos                          |

## Expresiones por tiempo

| Expresión   | Salida                                                                |
| ----------- | --------------------------------------------------------------------- |
| `h`         | the hour without a leading zero (0 to 23 or 1 to 12 if AM/PM display) |
| `hh`        | the hour with a leading zero (00 to 23 or 01 to 12 if AM/PM display)  |
| `H`         | the hour without a leading zero (0 to 23, even with AM/PM display)    |
| `HH`        | the hour with a leading zero (00 to 23, even with AM/PM display)      |
| `m`         | the minute without a leading zero (0 to 59)                           |
| `mm`        | the minute with a leading zero (00 to 59)                             |
| `s`         | the second without a leading zero (0 to 59)                           |
| `ss`        | the second with a leading zero (00 to 59)                             |
| `z`         | the milliseconds without leading zeroes (0 to 999)                    |
| `zzz`       | the milliseconds with leading zeroes (000 to 999)                     |
| `AP` or `A` | use AM/PM display. A/AP will be replaced by either `AM` or `PM`.      |
| `ap` or `a` | use am/pm display. a/ap will be replaced by either `am` or `pm`.      |
| `t`         | the timezone (for example `CEST`)                                     |

## Exemple de chaînes de format

| Formato            | Resultado        |
| ------------------ | ---------------- |
| `dd.MM.yyyy hh:mm` | 21.05.2001 14:13 |
| `dd.MM.yyyy`       | 21.05.2001       |
| `ddd MMMM d yy`    | Tue May 21 01    |
| `hh:mm:ss.zzz`     | 14:13:09.042     |
| `h:m:s ap`         | 2:13:9 pm        |

::: Información de la sugerencia Eche un vistazo a la [documentación de Qt](http://doc.qt.io/qt-5/qdatetime.html#toString) para obtener más información sobre los formatos de hora.
:::

::: tip
Si necesita realizar operaciones más complejas para generar la hora actual, considere crear una [acción personalizada](../scripting/methods-and-objects.md#registering-a-custom-action).
:::