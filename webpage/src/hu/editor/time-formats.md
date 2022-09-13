# Időformátumok:

Használhatja saját időformátumát, amikor az aktuális időt beszúrja egy jegyzetbe.

You can use this page to build a personal default time format. You can enter that string at _Note_ > _Settings_ > _Editor_ > _Insert current time_.

## Kifejezések a dátumra

| Kifejezés | Kibocsátás                                                 |
| --------- | ---------------------------------------------------------- |
| `d`       | a nap számként vezető nulla nélkül (1-től 31-ig)           |
| `dd`      | a nap számként vezető nullával (01-től 31-ig)              |
| `ddd`     | a rövidített lokalizált nap neve (pl. `H` - `V`).          |
| `dddd`    | a hosszú lokalizált nap neve (pl. `hétfő` és `vasárnap`).  |
| `M`       | a hónap számként vezető nulla nélkül (1-12)                |
| `MM`      | a hónap számként vezető nullával (01–12)                   |
| `MMM`     | a rövidített honosított havi név (pl. `Jan` - `Dec`).      |
| `MMMM`    | a hosszú lokalizált hónapnév (pl. `január` és `december`). |
| `yy`      | az év kétjegyű számként (00-99)                            |
| `yyyy`    | az év négyjegyű számként                                   |

## Kifejezések az időre

| Kifejezés     | Kibocsátás                                                        |
| ------------- | ----------------------------------------------------------------- |
| `h`           | az óra vezető nulla nélkül (0 to 23 or 1 to 12 if AM/PM display)  |
| `hh`          | az óra vezető nullával (00 to 23 or 01 to 12 if AM/PM display)    |
| `H`           | az óra vezető nulla nélkül (0 to 23, even with AM/PM display)     |
| `HH`          | az óra vezető nullával (00–23, még AM / PM kijelző esetén is)     |
| `m`           | a perc vezető nulla nélkül (0–59)                                 |
| `mm`          | a perc vezető nullával (00–59)                                    |
| `s`           | a második vezető nulla nélkül (0–59)                              |
| `ss`          | a másodperc vezető nullával (00–59)                               |
| `z`           | ezredmásodpercek vezető nullák nélkül (0 és 999 között)           |
| `zzz`         | ezredmásodpercek vezető nullákkal (000–999)                       |
| `AP` or `A`   | AM / PM kijelző használata. Az A/ AP helyére `AM` vagy `PM` lép.  |
| `ap` vagy `a` | am / pm kijelző használata. Az a / ap helyébe `am` vagy `pm` lép. |
| `t`           | az időzóna (például `CEST`)                                       |

## Példa formátumú karakterláncokra

| Formátum           | Eredmény         |
| ------------------ | ---------------- |
| `dd.MM.yyyy hh:mm` | 21.05.2001 14:13 |
| `dd.MM.yyyy`       | 21.05.2001       |
| `ddd MMMM d yy`    | Tue May 21 01    |
| `hh:mm:ss.zzz`     | 14:13:09.042     |
| `h:m:s ap`         | 2:13:9 pm        |

::: tip Info
Take a look at the [Qt documentation](http://doc.qt.io/qt-5/qdatetime.html#toString) for more information about time formats.
:::

::: tip
If you need to make more complex operations to output the current time please consider creating a [custom action](../scripting/methods-and-objects.md#registering-a-custom-action).
:::
