# Time formats

You can use your own time format when inserting the current time into a
note.

You can use this page to build a personal default time format.
You can enter that string at _Note_ > _Settings_ > _Editor_ >
_Insert current time_.

## Expressions for date

| Expression | Output                                                        |
| ---------- | ------------------------------------------------------------- |
| `d`        | the day as number without a leading zero (1 to 31)            |
| `dd`       | the day as number with a leading zero (01 to 31)              |
| `ddd`      | the abbreviated localized day name (e.g. `Mon` to `Sun`).     |
| `dddd`     | the long localized day name (e.g. `Monday` to `Sunday`).      |
| `M`        | the month as number without a leading zero (1-12)             |
| `MM`       | the month as number with a leading zero (01-12)               |
| `MMM`      | the abbreviated localized month name (e.g. `Jan` to `Dec`).   |
| `MMMM`     | the long localized month name (e.g. `January` to `December`). |
| `yy`       | the year as two digit number (00-99)                          |
| `yyyy`     | the year as four digit number                                 |

## Expressions for time

| Expression  | Output                                                                |
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

## Example format strings

| Format             | Result           |
| ------------------ | ---------------- |
| `dd.MM.yyyy hh:mm` | 21.05.2001 14:13 |
| `dd.MM.yyyy`       | 21.05.2001       |
| `ddd MMMM d yy`    | Tue May 21 01    |
| `hh:mm:ss.zzz`     | 14:13:09.042     |
| `h:m:s ap`         | 2:13:9 pm        |

::: tip Info
Take a look at the [Qt documentation](http://doc.qt.io/qt-5/qdatetime.html#toString) for more
information about time formats.
:::

::: tip
If you need to make more complex operations to output the current time
please consider creating a [custom action](../scripting/methods-and-objects.md#registering-a-custom-action).
:::
