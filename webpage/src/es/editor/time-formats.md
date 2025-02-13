# Formatos de hora

Puede utilizar su propio formato de hora al insertar la hora actual en una nota.

Puede utilizar esta página para crear un formato de hora predeterminado personal. Puedes introducir esa cadena en _Nota_ > _Configuración_ > _Editor _ > _Insertar hora actual_.

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

| Expresión  | Salida                                                                        |
| ---------- | ----------------------------------------------------------------------------- |
| `h`        | la hora sin un cero al principio (0 a 23 o 1 a 12 si se muestra como AM/PM)   |
| `hh`       | la hora con un cero al principio (00 a 23 o 01 a 12 si se muestra como AM/PM) |
| `H`        | la hora sin un cero al principio (0 a 23, incluso si se muestra como AM/PM)   |
| `HH`       | la hora sin un cero al principio (00 a 23, incluso si se muestra como AM/PM)  |
| `m`        | el minuto sin un cero al principio (0 a 59)                                   |
| `mm`       | el minuto con un cero al principio (00 a 59)                                  |
| `s`        | el segundo sin un cero al principio (0 a 59)                                  |
| `ss`       | el segundo con un cero al principio (00 a 59)                                 |
| `z`        | los milisegundos sin ceros al principio (0 a 999)                             |
| `zzz`      | los milisegundos con ceros al principio (000 a 999)                           |
| `AP` o `A` | mostrar AM/PM. A/AP será reemplazado ya sea por `AM` o `PM`.                  |
| `ap` o `a` | mostrar am/pm. a/ap será reemplazado por `am` o `pm`.                         |
| `t`        | la zona horaria (por ejemplo `CEST`)                                          |

## Ejemplo de formato strings

| Formato            | Resultado        |
| ------------------ | ---------------- |
| `dd.MM.yyyy hh:mm` | 21.05.2001 14:13 |
| `dd.MM.yyyy`       | 21.05.2001       |
| `ddd MMMM d yy`    | Mar May 21 01    |
| `hh:mm:ss.zzz`     | 14:13:09.042     |
| `h:m:s ap`         | 2:13:9 pm        |

::: tip
Información Eche un vistazo a la [documentación de Qt](http://doc.qt.io/qt-5/qdatetime.html#toString) para obtener más información sobre los formatos de hora.
:::

::: tip
Si necesita realizar operaciones más complejas para generar la hora actual, por favor considere crear una [acción personalizada](../scripting/methods-and-objects.md#registering-a-custom-action).
:::
