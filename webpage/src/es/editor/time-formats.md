# Formatos de hora:

Puede utilizar su propio formato de hora al insertar la hora actual en una nota.

## Expresiones para la fecha

| Expresión | Salida                                                               |
| --------- | -------------------------------------------------------------------- |
| `d`       | el día como número sin cero al principio (1 a 31)                    |
| `dd`      | el día como número con cero al principio (01 a 31)                   |
| `ddd`     | el nombre abreviado del día traducido (p. ej. `lun` a `dom`).        |
| `dddd`    | el nombre completo del día traducido (p. ej. `lunes` a `domingo`).   |
| `M`       | el mes como número sin cero al principio (1 a 12)                    |
| `MM`      | el mes como número con cero al principio (01 a 12)                   |
| `MMM`     | el nombre abreviado del mes traducido (p. ej. `ene` a `dic`).        |
| `MMMM`    | el nombre completo del mes traducido (p. ej. `enero` a `diciembre`). |
| `aa`      | el año como número de dos dígitos (00-99)                            |
| `aaaa`    | el año como número de cuatro dígitos                                 |

## Expresiones por tiempo

| Expresión   | Salida                                                                |
| ----------- | --------------------------------------------------------------------- |
| `h`         | la hora sin cero al principio (0 a 23 o 1 a 12 si se muestra AM/PM)   |
| `hh`        | la hora con cero al principio (00 a 23 o 01 a 12 si se muestra AM/PM) |
| `H`         | la hora sin cero al principio (0 a 23, incluso si se muestra AM/PM)   |
| `HH`        | la hora in cero al principio (00 a 23, incluso si se muestra AM/PM)   |
| `m`         | el minuto sin cero al principio (0 a 59)                              |
| `mm`        | el minuto con cero al principio (00 a 59)                             |
| `s`         | el segundo sin cero al principio (0 a 59)                             |
| `ss`        | el segundo con cero al principio (00 a 59)                            |
| `z`         | los milisegundos sin ceros al principio (0 a 999)                     |
| `zzz`       | los milisegundos con ceros al principio (000 a 999)                   |
| `AP` or `A` | use AM/PM display. A/AP será reemplazado por `AM` O `PM`.             |
| `ap` or `a` | mostrar am/pm. a/ap será reemplazado por `am` o `pm`.                 |
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