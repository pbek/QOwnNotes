# Sprawdzanie pisowni

Skorzystaj z opcji poprawiania błędów w pisowni.

::: tip Info
QOwnNotes has a builtin **Dictionary Manager** (in the _Edit_ menu) which you can use to download dictionaries or delete them (if you are missing your language see below on how to request adding a dictionary). Jeśli chcesz pobrać słowniki ręcznie, możesz skorzystać z poniższych instrukcji.
:::

- W systemie **Linux** możesz zainstalować słowniki Hunspell dla swojego języka, korzystając z menedżera pakietów swojej dystrybucji.
  - If you need to add additional language files you can do so in `~/.local/share/hunspell` or `/usr/share/hunspell`.
  - If you are using the Snap package of QOwnNotes you can also use `~/hunspell` for language files, since the Snap can't access `~/.local`

- W systemie **Windows** musisz skopiować słowniki Hunspell dla swojego języka do folderu `dicts` w folderze aplikacji QOwnNotes lub w ścieżce `C:\Użytkownicy\[username]\dicts`.
  - Słowniki możesz pobrać z sekcji [słowników LibreOffice](https://github.com/LibreOffice/dictionaries).
  - W systemie **macOS** musisz skopiować słowniki Hunspell dla swojego języka do lokalizacji `/Library/Spelling` lub `~/Library/Spelling`. Jeśli ten katalog nie istnieje, musisz go utworzyć.

Słowniki możesz pobrać z sekcji [słowników QOwnNotes](https://github.com/qownnotes/dictionaries). Aby znaleźć swój język i odpowiadający mu kod języka, posłuż się tabelą na końcu tego dokumentu.

Jeśli nie znajdziesz słownika dla swojego języka w **Menedżerze słownika** lub pod powyższym linkiem, poszukaj go w Internecie lub zgłoś błąd. Jeśli znajdziemy odpowiedni słownik, dołączymy go do kolejnej wersji QOwnNotes.

::: tip
Wskazówka Jeśli nie możesz znaleźć swojego języka w tej tabeli, niestety nie istnieje dla niego słownik w plikach LibreOffice. Jeśli chcesz utworzyć słownik, przejdź na stronę [Documentfoundation.org Dictionaries Development](https://wiki.documentfoundation.org/Development/Dictionaries).
:::

| Nr | Kod języka | Język                                    |
| -- | ---------- | ---------------------------------------- |
| 1  | af_ZA      | afrikaans (RPA)                          |
| 2  | an_ES      | aragoński (Hiszpania)                    |
| 3  | ar         | arabski                                  |
| 4  | be_BY      | białoruski                               |
| 5  | bg_BG      | bułgarski                                |
| 6  | bn_BD      | bengalski                                |
| 7  | bo         | tybetański                               |
| 8  | br_FR      | bretoński (Francja)                      |
| 9  | bs_BA      | bośniacki                                |
| 10 | ca         | kataloński                               |
| 11 | cs_CZ      | czeski                                   |
| 12 | da_DK      | duński                                   |
| 13 | de         | niemiecki                                |
| 14 | el_GR      | grecki                                   |
| 15 | en         | angielski                                |
| 16 | es         | hiszpański                               |
| 17 | et_EE      | estoński                                 |
| 18 | gd_GB      | gaelicki                                 |
| 19 | gl         | galicyjski                               |
| 20 | gu_IN      | gudżarati                                |
| 21 | gug        | guarani                                  |
| 22 | he_IL      | hebrajski                                |
| 23 | hi_IN      | hindi                                    |
| 24 | hr_HR      | chorwacki                                |
| 25 | id         | indonezyjski                             |
| 26 | it_IT      | włoski                                   |
| 27 | kmr_Latn   | kurdyjski (Turcja)                       |
| 28 | lo_LA      | laotański                                |
| 29 | lt_LT      | litewski                                 |
| 30 | lv_LV      | łotewski                                 |
| 31 | ne_NP      | nepalski                                 |
| 32 | nl_NL      | niderlandzki                             |
| 33 | no         | norweski                                 |
| 34 | oc_FR      | prowansalski                             |
| 35 | pl_PL      | polski                                   |
| 36 | pt_BR      | portugalski (Brazylia)                   |
| 37 | pt_PT      | portugalski                              |
| 38 | ro         | rumuński                                 |
| 39 | ru_RU      | rosyjski                                 |
| 40 | si_LK      | syngaleski                               |
| 41 | sk_SK      | słowacki                                 |
| 42 | sl_Sl      | słoweński                                |
| 43 | sq_AL      | albański                                 |
| 44 | sr         | serbski                                  |
| 45 | sv_SE      | szwedzki                                 |
| 46 | sw_TZ      | suahili                                  |
| 47 | te_IN      | telugu                                   |
| 48 | th_TH      | tajski                                   |
| 49 | tr_TR      | turecki                                  |
| 51 | uk_UA      | ukraiński                                |
| 52 | vi         | wietnamski                               |
| 53 | zu_ZA      | zulu (brak słownika sprawdzania pisowni) |

::: tip
Uznania Do sprawdzania pisowni używany jest [Sonnet](https://github.com/KDE/sonnet) z [Hunspell](https://hunspell.github.io/)
:::
