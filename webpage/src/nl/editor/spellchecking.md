# Spellingcontrole

Laat uw spelfouten corrigeren!

::: tip Info
QOwnNotes heeft een ingebouwde **Dictionary Manager** (in het menu *Bewerken*) die u kunt gebruiken om woordenboeken te downloaden of te verwijderen (als u uw taal mist, kijk dan hieronder hoe u het toevoegen van een woordenboek kunt aanvragen). Als u de woordenboeken handmatig wilt downloaden, kunt u onderstaande instructies volgen.
:::

- Onder **Linux** kunt u de Hunspell-woordenboeken voor uw taal installeren met behulp van de pkg-manager van uw distro
    - Als u extra taalbestanden moet toevoegen, kunt u dit doen in `~/.local/share/hunspell` of `/usr/share/hunspell`.
    - Als je het Snap-pakket van QOwnNotes gebruikt, kun je ook `~/hunspell` gebruiken voor taalbestanden, aangezien de Snap geen toegang heeft tot `~/.local`

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
| 16 | es        | Spanish                      |
| 17 | et_EE     | Estonian                     |
| 18 | gd_GB     | Gaelic                       |
| 19 | gl        | Galician                     |
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
| 33 | no        | Norwegian                    |
| 34 | oc_FR     | Occitan                      |
| 35 | pl_PL     | Polish                       |
| 36 | pt_BR     | Braziliaans Portugees        |
| 37 | pt_PT     | Portuguese                   |
| 38 | ro        | Romanian                     |
| 39 | ru_RU     | Russian                      |
| 40 | si_LK     | Sinhalese                    |
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
