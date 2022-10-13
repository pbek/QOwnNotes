# Formaty czasu

Przy dodawaniu aktualnego czasu do notatki możesz użyć własnego formatu czasu.

You can use this page to build a personal default time format. You can enter that string at _Note_ > _Settings_ > _Editor_ > _Insert current time_.

## Wyrażenia dla daty

| Wyrażenie | Produkt wyjściowy                                                                  |
| --------- | ---------------------------------------------------------------------------------- |
| `d`       | dzień w formie numeru bez zera na początku (1–31)                                  |
| `dd`      | dzień w formie numeru z zerem na początku (01–31)                                  |
| `ddd`     | skrócona nazwa dnia tygodnia odpowiednia dla regionu (np. `pon.`–`niedz.`)         |
| `dddd`    | pełna nazwa dnia tygodnia odpowiednia dla regionu (np. `poniedziałek`–`niedziela`) |
| `M`       | miesiąc w formie numeru bez zera na początku (1–12)                                |
| `MM`      | miesiąc w formie numeru z zerem na początku (01–12)                                |
| `MMM`     | skrócona nazwa miesiąca odpowiednia dla regionu (np. `sty.`–`gru.`)                |
| `MMMM`    | pełna nazwa miesiąca odpowiednia dla regionu (np. `styczeń`–`grudzień`)            |
| `yy`      | rok w formie numeru złożonego z dwóch cyfr (00–99)                                 |
| `yyyy`    | rok w formie numeru złożonego z czterech cyfr                                      |

## Wyrażenia dla czasu

| Wyrażenie    | Produkt wyjściowy                                                                        |
| ------------ | ---------------------------------------------------------------------------------------- |
| `h`          | godzina bez zera na początku (0–23 lub 1–12 w zegarze 12-godzinnym)                      |
| `hh`         | godzina z zerem na początku (00–23 lub 01–12 w zegarze 12-godzinnym)                     |
| `H`          | godzina bez zera na początku (0–23, także w zegarze 12-godzinnym)                        |
| `HH`         | godzina z zerem na początku (00–23, także w zegarze 12-godzinnym)                        |
| `m`          | minuty bez zera na początku (0–59)                                                       |
| `mm`         | minuty z zerem na początku (00–59)                                                       |
| `s`          | sekundy bez zera na początku (0–59)                                                      |
| `ss`         | sekundy z zerem na początku (00–59)                                                      |
| `z`          | milisekundy bez zer na początku (0–999)                                                  |
| `zzz`        | milisekundy z zerami na początku (000–999)                                               |
| `AP` lub `A` | wyświetlanie informacji AM/PM w zegarze 12-godzinnym; A/AP zastąpi wartość `AM` lub `PM` |
| `ap` lub `a` | wyświetlanie informacji am/pm w zegarze 12-godzinnym; a/ap zastąpi wartość `am` lub `pm` |
| `t`          | strefa czasowa (np. `CEST`)                                                              |

## Przykładowe ciągi formatu

| Format             | Rezultat         |
| ------------------ | ---------------- |
| `dd.MM.yyyy hh:mm` | 21.05.2001 14:13 |
| `dd.MM.yyyy`       | 21.05.2001       |
| `ddd MMMM d yy`    | wt. maj 21 01    |
| `hh:mm:ss.zzz`     | 14:13:09.042     |
| `h:m:s ap`         | 2:13:9 pm        |

::: tip Info
Take a look at the [Qt documentation](http://doc.qt.io/qt-5/qdatetime.html#toString) for more information about time formats.
:::

::: tip
If you need to make more complex operations to output the current time please consider creating a [custom action](../scripting/methods-and-objects.md#registering-a-custom-action).
:::
