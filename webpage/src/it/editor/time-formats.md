# Formati dell'ora

È possibile utilizzare il proprio formato dell'ora quando si inserisce l'ora corrente in una nota.

## Espressioni per la data

| Espressione | Risultato                                                                 |
| ----------- | ------------------------------------------------------------------------- |
| `d`         | il giorno come numero senza zero iniziale (da 1 a 31)                     |
| `dd`        | il giorno come numero con uno zero iniziale (da 01 a 31)                  |
| `ddd`       | il nome abbreviato del giorno, localizzato (p.es. da `Lun` a `Dom`).      |
| `dddd`      | il nome intero del giorno, localizzato (ad es. da `lunedì` a `domenica`). |
| `M`         | il mese come numero senza zero iniziale (1-12)                            |
| `MM`        | il mese come numero con uno zero iniziale (01-12)                         |
| `MMM`       | il nome abbreviato del mese, localizzato (p.es. da `Gen` a `Dic`).        |
| `MMMM`      | il nome intero del mese, localizzato (p. es. da `Gennaio` a `Dicembre`).  |
| `yy`        | l'anno come numero a due cifre (00-99)                                    |
| `yyyy`      | l'anno come numero a quattro cifre                                        |

## Espressioni per il tempo

| Espressione     | Risultato                                                                     |
| --------------- | ----------------------------------------------------------------------------- |
| `h`             | l'ora senza zero iniziale (da 0 a 23 o da 1 a 12 se è visualizzato AM/PM)     |
| `hh`            | l'ora con uno zero iniziale (da 00 a 23 o da 01 a 12 se è visualizzato AM/PM) |
| `H`             | l'ora senza zero iniziale (da 0 a 23, anche con AM/PM visualizzato)           |
| `HH`            | l'ora con uno zero iniziale (da 00 a 23, anche con AM/PM visualizzato)        |
| `m`             | il minuto senza zero iniziale (da 0 a 59)                                     |
| `mm`            | il minuto con uno zero iniziale (da 00 a 59)                                  |
| `s`             | i secondi senza zero iniziale (da 0 a 59)                                     |
| `ss`            | i secondi con uno zero iniziale (da 00 a 59)                                  |
| `z`             | i millisecondi senza zero iniziale (da 0 a 999)                               |
| `zzz`           | i millisecondi con uno zero iniziale (da 000 a 999)                           |
| `AP` oppure `A` | mostra AM/PM. A/AP verrà sostituito da `AM` o `PM` rispettivamente.           |
| `ap` oppure `a` | mostra AM/PM. a/ap verrà sostituito da  `am` o `pm` rispettivamente.          |
| `t`             | il fuso orario (per esempio `CEST`)                                           |

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
Se è necessario eseguire operazioni più complesse per visualizzare l'ora corrente, prendere in considerazione la creazione di un'[azione personalizzata](../scripting/methods-and-objects.md#registering-a-custom-action).
:::