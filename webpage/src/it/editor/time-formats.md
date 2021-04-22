# Formati dell'ora

È possibile utilizzare il proprio formato dell'ora quando si inserisce l'ora corrente in una nota.

## Espressioni per la data

| Espressione | Produzione                                                              |
| ----------- | ----------------------------------------------------------------------- |
| `d`         | il giorno come numero senza zero iniziale (da 1 a 31)                   |
| `dd`        | il giorno come numero con uno zero iniziale (da 01 a 31)                |
| `ddd`       | il nome del giorno abbreviato localizzato (ad es. da `Lun` a `Sun`).    |
| `dddd`      | il nome del giorno localizzato lungo (ad es. da `lunedì` a `domenica`). |
| `M`         | il mese come numero senza zero iniziale (1-12)                          |
| `MM`        | il mese come numero con uno zero iniziale (01-12)                       |
| `MMM`       | il nome abbreviato del mese localizzato (e.g. `Jan` to `Dec`).          |
| `MMMM`      | the long localized month name (e.g. `January` to `December`).           |
| `yy`        | the year as two digit number (00-99)                                    |
| `yyyy`      | the year as four digit number                                           |

## Espressioni per il tempo

| Espressione | Produzione                                                                      |
| ----------- | ------------------------------------------------------------------------------- |
| `h`         | l'ora senza zero iniziale (da 0 a 23 o da 1 a 12 se è visualizzato AM / PM)     |
| `hh`        | l'ora con uno zero iniziale (da 00 a 23 o da 01 a 12 se è visualizzato AM / PM) |
| `H`         | the hour without a leading zero (0 to 23, even with AM/PM display)              |
| `HH`        | the hour with a leading zero (00 to 23, even with AM/PM display)                |
| `m`         | the minute without a leading zero (0 to 59)                                     |
| `mm`        | the minute with a leading zero (00 to 59)                                       |
| `s`         | the second without a leading zero (0 to 59)                                     |
| `ss`        | the second with a leading zero (00 to 59)                                       |
| `z`         | the milliseconds without leading zeroes (0 to 999)                              |
| `zzz`       | the milliseconds with leading zeroes (000 to 999)                               |
| `AP` or `A` | use AM/PM display. A/AP will be replaced by either `AM` or `PM`.                |
| `ap` or `a` | use am/pm display. a/ap will be replaced by either `am` or `pm`.                |
| `t`         | the timezone (for example `CEST`)                                               |

## Esempio di stringhe di formato

| Formato            | Risultato        |
| ------------------ | ---------------- |
| `dd.MM.yyyy hh:mm` | 21.05.2001 14:13 |
| `dd.MM.yyyy`       | 21.05.2001       |
| `ddd MMMM d yy`    | Mar Mag 21 01    |
| `hh:mm:ss.zzz`     | 14:13:09.042     |
| `h:m:s ap`         | 2:13:9 pm        |

::: tip Info
Dai un'occhiata alla [documentazione Qt](http://doc.qt.io/qt-5/qdatetime.html#toString) per ulteriori informazioni sui formati dell'ora.
:::

::: tip
Se è necessario eseguire operazioni più complesse per visualizzare l'ora corrente, prendere in considerazione la creazione di un [azione personalizzata](../scripting/methods-and-objects.md#registering-a-custom-action).
:::