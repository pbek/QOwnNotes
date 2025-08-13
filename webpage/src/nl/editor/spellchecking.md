# Spellingcontrole

Laat uw spelfouten corrigeren!

::: tip Info
QOwnNotes has a builtin **Dictionary Manager** (in the _Edit_ menu) which you can use to download dictionaries or delete them (if you are missing your language see below on how to request adding a dictionary). Als u de woordenboeken handmatig wilt downloaden, kunt u onderstaande instructies volgen.
:::

- Onder **Linux** kunt u de Hunspell-woordenboeken voor uw taal installeren met behulp van de pkg-manager van uw distro
  - If you need to add additional language files you can do so in `~/.local/share/hunspell` or `/usr/share/hunspell`.
  - If you are using the Snap package of QOwnNotes you can also use `~/hunspell` for language files, since the Snap can't access `~/.local`

- Onder **Windows** moet u Hunspell-woordenboeken voor uw taal kopiëren naar een map `dicts` in uw QOwnNotes-toepassingsmap of in ` C:\Users\[gebruikersnaam]\dicts`
  - U kunt woordenboeken downloaden van [LibreOffice-woordenboeken](https://github.com/LibreOffice/dictionaries)
  - Onder **macOS** moet u Hunspell-woordenboeken voor uw taal kopiëren naar `/Library/Spelling` of `~/Library/Spelling`. Als de directory niet bestaat, moet u deze maken.

Woordenboeken kunnen worden gedownload van [QOwnNotes dictionaries](https://github.com/qownnotes/dictionaries). Gebruik de tabel aan het einde van dit document om uw taal en de bijbehorende taalcode te vinden.

Als het woordenboek voor uw taal niet in de **Dictionary Manager** of in de bovengenoemde link staat, zoek het dan op internet of maak een probleem aan. We zullen het zeker opnemen in de volgende uitgave van QOwnNotes als het woordenboek wordt gevonden.

::: tip Info
Als u uw taal niet in deze tabel kunt vinden, bestaat er helaas geen woordenboek voor in de LibreOffice-bestanden - Als u een woordenboek wilt maken, gaat u naar de [Documentfoundation.org Woordenboeken-ontwikkeling](https://wiki.documentfoundation.org/Development/Dictionaries)
:::

| Sr | Taal code | Taal                         |
| -- | --------- | ---------------------------- |
| 1  | af_ZA     | Afrikaans (South Africa)     |
| 2  | an_ES     | Aragonesisch - Spain (ES)    |
| 3  | ar        | Arabic                       |
| 4  | be_BY     | Wit-Russisch                 |
| 5  | bg_BG     | Bulgaars                     |
| 6  | bn_BD     | Bengaals                     |
| 7  | bo        | Tibetanisch                  |
| 8  | br_FR     | Breton - France              |
| 9  | bs_BA     | Bosnisch                     |
| 10 | ca        | Catalaans                    |
| 11 | cs_CZ     | Tsjechisch                   |
| 12 | da_DK     | Deens                        |
| 13 | de        | Duits                        |
| 14 | el_GR     | Grieks                       |
| 15 | en        | Engels                       |
| 16 | es        | Spaans                       |
| 17 | et_EE     | Estlands                     |
| 18 | gd_GB     | Gaelisch                     |
| 19 | gl        | Galizian                     |
| 20 | gu_IN     | Gujarati                     |
| 21 | gug       | Guarani                      |
| 22 | he_IL     | Hebreeuws                    |
| 23 | hi_IN     | Hindi                        |
| 24 | hr_HR     | Kroatisch                    |
| 25 | id        | Indonesisch                  |
| 26 | it_IT     | Italiaans                    |
| 27 | kmr_Latn  | Koerdisch (Turkije)          |
| 28 | lo_LA     | Lao                          |
| 29 | lt_LT     | Litouws                      |
| 30 | lv_LV     | Lets                         |
| 31 | ne_NP     | Nepali                       |
| 32 | nl_NL     | Nederlands                   |
| 33 | no        | Noors                        |
| 34 | oc_FR     | Occitaans                    |
| 35 | pl_PL     | Pools                        |
| 36 | pt_BR     | Braziliaans Portugees        |
| 37 | pt_PT     | Portugees                    |
| 38 | ro        | Roemeens                     |
| 39 | ru_RU     | Russisch                     |
| 40 | si_LK     | Singalees                    |
| 41 | sk_SK     | Slowaaks                     |
| 42 | Slovenian | Sloveens                     |
| 43 | sq_AL     | Albanian                     |
| 44 | sr        | Servisch                     |
| 45 | sv_SE     | Zweeds                       |
| 46 | sw_TZ     | Swahili                      |
| 47 | te_IN     | Telugu                       |
| 48 | th_TH     | Thais                        |
| 49 | tr_TR     | Turks                        |
| 51 | uk_UA     | Oekraïens                    |
| 52 | vi        | Vietnamees                   |
| 53 | zu_ZA     | Zulu (dict niet beschikbaar) |

::: tip
Credits [Sonnet](https://github.com/KDE/sonnet) met [Hunspell](https://hunspell.github.io/) wordt gebruikt voor spellingcontrole
:::
