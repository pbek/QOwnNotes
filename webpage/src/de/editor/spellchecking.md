# Rechtschreibprüfung

Lassen Sie Ihre Rechtschreibfehler korrigieren!

::: tip Info
QOwnNotes verfügt über einen integrierten **Wörterbuch-Manager** (im Menü *Bearbeiten*), mit dem Sie Wörterbücher herunterladen oder löschen können (wenn Sie Ihre Sprache vermissen, lesen Sie weiter unten, wie Sie das Hinzufügen eines Wörterbuchs anfordern können). Wenn Sie die Wörterbücher manuell herunterladen möchten, können Sie den folgenden Anweisungen folgen.
:::

- Unter **Linux** können Sie die Hunspell-Wörterbücher für Ihre Sprache mit dem pkg-Manager Ihrer Distribution installieren
    - Wenn Sie zusätzliche Sprachdateien hinzufügen müssen, können Sie dies in `~/.local/share/hunspell` oder `/usr/share/hunspell` tun.
    - Wenn Sie das Snap-Paket von QOwnNotes verwenden, können Sie auch `~/hunspell` für Sprachdateien verwenden, da der Snap nicht auf `~/.local` zugreifen kann

- Unter **Windows** müssen Sie Hunspell-Wörterbücher für Ihre Sprache in einen Ordner kopieren, der sich in Ihrem QOwnNotes-Anwendungsordner oder in `C:\Users\[Benutzername]\dicts` befindet
    - Sie können Wörterbücher aus [LibreOffice-Wörterbüchern](https://github.com/LibreOffice/dictionaries) herunterladen
    - Unter **macOS** müssen Sie Hunspell-Wörterbücher für Ihre Sprache nach `/Library/Spelling` oder `~/Library/Spelling` kopieren. Wenn das Verzeichnis nicht vorhanden ist, müssen Sie es erstellen.

Wörterbücher können aus [QOwnNotes-Wörterbüchern](https://github.com/qownnotes/dictionaries) heruntergeladen werden. Verwenden Sie die Tabelle am Ende dieses Dokuments, um Ihre Sprache und den entsprechenden Sprachcode zu finden.

Wenn sich das Wörterbuch für Ihre Sprache nicht im **Wörterbuch-Manager** oder unter dem oben genannten Link befindet, suchen Sie es im Internet oder erstellen Sie ein Problem-Thema. Wir werden es sicher in die nächste Version von QOwnNotes aufnehmen, wenn das Wörterbuch gefunden wird.

::: tip Info
Wenn Sie Ihre Sprache in dieser Tabelle nicht finden können, ist in den LibreOffice-Dateien leider kein Wörterbuch dafür vorhanden. Wenn Sie ein Wörterbuch erstellen möchten, gehen Sie zu [Documentfoundation.org Dictionaries Development](https://wiki.documentfoundation.org/Development/Dictionaries)
:::

| Sr | Sprachkod | Sprache                                     |
| -- | --------- | ------------------------------------------- |
| 1  | af_ZA     | Afrikanisch (Südafrika)                     |
| 2  | an_ES     | Aragonisch - Spanien (ES)                   |
| 3  | ar        | Arabisch                                    |
| 4  | be_BY     | Weißrussisch                                |
| 5  | bg_BG     | Bulgarisch                                  |
| 6  | bn_BD     | Bengali                                     |
| 7  | bo        | Tibetanisch                                 |
| 8  | br_FR     | Bretonisch - Frankreich                     |
| 9  | bs_BA     | Bosnisch                                    |
| 10 | ca        | Katalanisch                                 |
| 11 | cs_CZ     | Tschechisch                                 |
| 12 | da_DK     | Dänisch                                     |
| 13 | de        | Deutsch                                     |
| 14 | el_GR     | Griechisch                                  |
| 15 | en        | Englisch                                    |
| 16 | es        | Spanisch                                    |
| 17 | et_EE     | Estnisch                                    |
| 18 | gd_GB     | Gälisch                                     |
| 19 | gl        | Galizisch                                   |
| 20 | gu_IN     | Gujarati                                    |
| 21 | gug       | Guarani                                     |
| 22 | he_IL     | Hebräisch                                   |
| 23 | hi_IN     | Hindi                                       |
| 24 | hr_HR     | Kroatisch                                   |
| 25 | id        | Indonesisch                                 |
| 26 | it_IT     | Italienisch                                 |
| 27 | kmr_Latn  | Kurdisch (Türkei)                           |
| 28 | lo_LA     | Laotisch                                    |
| 29 | lt_LT     | Litauisch                                   |
| 30 | lv_LV     | Lettisch                                    |
| 31 | ne_NP     | Nepalesisch                                 |
| 32 | nl_NL     | Niederländisch                              |
| 33 | no        | Norwegisch                                  |
| 34 | oc_FR     | Occitanisch                                 |
| 35 | pl_PL     | Polnisch                                    |
| 36 | pt_BR     | Portugiesisch (Brasilien)                   |
| 37 | pt_PT     | Portugisisch                                |
| 38 | ro        | Rumänisch                                   |
| 39 | ru_RU     | Russisch                                    |
| 40 | si_LK     | Singalesisch                                |
| 41 | sk_SK     | Slowakisch                                  |
| 42 | sl_Sl     | Slowenisch                                  |
| 43 | sq_AL     | Albanisch                                   |
| 44 | sr        | Serbisch                                    |
| 45 | sv_SE     | Schwedisch                                  |
| 46 | sw_TZ     | Kisuaheli                                   |
| 47 | te_IN     | Telugu                                      |
| 48 | th_TH     | Thai                                        |
| 49 | tr_TR     | Türkisch                                    |
| 51 | uk_UA     | Ukrainisch                                  |
| 52 | vi        | Vietnamesisch                               |
| 53 | zu_ZA     | Zulu (Rechtsschreibprüfung nicht vorhanden) |

::: tip
Credits [Sonett](https://github.com/KDE/sonnet) mit [Hunspell](https://hunspell.github.io/) wird zur Rechtschreibprüfung verwendet
:::
