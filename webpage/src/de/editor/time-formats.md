# Zeitformate

Sie können Ihr eigenes Zeitformat verwenden, wenn Sie die aktuelle Zeit in eine Notiz einfügen.

## Ausdrücke für das Datum

| Ausdruck | Ausgabe                                                                            |
| -------- | ---------------------------------------------------------------------------------- |
| `d`      | der Tag als Zahl ohne führende Null (1 bis 31)                                     |
| `dd`     | der Tag als Zahl mit einer führenden Null (01 bis 31)                              |
| `ddd`    | der abgekürzte lokalisierte Tagesname (z. B. ` Mo ` bis ` So `).                   |
| `dddd`   | der lange lokalisierte Tagesname (z. B. ` Montag < 0> bis <code> Sonntag `). |
| `M`      | der Monat als Zahl ohne führende Null (1-12)                                       |
| `MM`     | der Monat als Zahl mit einer führenden Null (01-12)                                |
| `MMM`    | der abgekürzte lokalisierte Monatsname (z. B. ` Jan ` bis ` Dec `).                |
| `MMMM`   | der lange lokalisierte Monatsname (z. B. ` Januar ` bis ` Dezember `).             |
| `yy`     | das Jahr als zweistellige Zahl (00-99)                                             |
| `yyyy`   | das Jahr als vierstellige Zahl                                                     |

## Ausdrücke für die Zeit

| Ausdruck          | Ausgabe                                                                                   |
| ----------------- | ----------------------------------------------------------------------------------------- |
| `h`               | die Stunde ohne führende Null (0 bis 23 oder 1 bis 12 bei AM / PM-Anzeige)                |
| `hh`              | die Stunde mit einer führenden Null (00 bis 23 oder 01 bis 12 bei AM / PM-Anzeige)        |
| `H`               | die Stunde ohne führende Null (0 bis 23, auch mit AM / PM-Anzeige)                        |
| `HH`              | die Stunde mit einer führenden Null (00 bis 23, auch mit AM / PM-Anzeige)                 |
| `m`               | die Minute ohne führende Null (0 bis 59)                                                  |
| `mm`              | die Minute mit einer führenden Null (00 bis 59)                                           |
| `s`               | die zweite ohne führende Null (0 bis 59)                                                  |
| `ss`              | die zweite mit einer führenden Null (00 bis 59)                                           |
| `z`               | die Millisekunden ohne führende Nullen (0 bis 999)                                        |
| `zzz`             | die Millisekunden mit führenden Nullen (000 bis 999)                                      |
| ` AP ` oder ` A ` | Verwenden Sie die AM / PM-Anzeige. A / AP wird entweder durch ` AM ` oder ` PM ` ersetzt. |
| `ap` or `a`       | use am/pm display. a /ap wird entweder durch ` am ` oder ` pm ` ersetzt.                  |
| `t`               | die Zeitzone (zum Beispiel ` MESZ `)                                                      |

## Beispielformatzeichenfolgen

| Format             | Ergebnis         |
| ------------------ | ---------------- |
| `dd.MM.yyyy hh:mm` | 21.05.2001 14:13 |
| `dd.MM.yyyy`       | 21.05.2001       |
| `ddd MMMM d yy`    | Die Mai 21 01    |
| `hh:mm:ss.zzz`     | 14:13:09.042     |
| `h:m:s ap`         | 2:13:9 pm        |

::: tip Info
Weitere Informationen zu Zeitformaten finden Sie in der [ Qt-Dokumentation ](http://doc.qt.io/qt-5/qdatetime.html#toString).
:::

::: tip
Wenn Sie komplexere Vorgänge ausführen müssen, um die aktuelle Zeit auszugeben, sollten Sie eine benutzerdefinierte [ Aktion ](../scripting/methods-and-objects.md#registering-a-custom-action) erstellen.
:::