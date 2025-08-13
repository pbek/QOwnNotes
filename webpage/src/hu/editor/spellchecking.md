# Helyesírás-ellenőrzés

Javítsa ki a helyesírási hibákat!

::: tip Info
QOwnNotes has a builtin **Dictionary Manager** (in the _Edit_ menu) which you can use to download dictionaries or delete them (if you are missing your language see below on how to request adding a dictionary). Ha manuálisan szeretné letölteni a szótárakat, kövesse az alábbi utasításokat.
:::

- A **Linux** alatt telepítheti a nyelvéhez tartozó Hunspell szótárakat a disztribútor pkg kezelőjével.
  - If you need to add additional language files you can do so in `~/.local/share/hunspell` or `/usr/share/hunspell`.
  - If you are using the Snap package of QOwnNotes you can also use `~/hunspell` for language files, since the Snap can't access `~/.local`

- A **Windows** alatt másolnia kell a nyelvéhez tartozó Hunspell szótárakat a QOwnNotes alkalmazás mappájában található `dicts` mappába vagy a  `C:\Users\[username]\dicts`
  - Szótárakat a [LibreOffice dictionaries](https://github.com/LibreOffice/dictionaries)-böl tölthet le
  - A **macOS** alatt le kell másolnia a Hunspell szótárakat nyelv <1 / Library / Spelling </ 1> vagy `~/Library/Spelling`. Ha a könyvtár nem létezik, létre kell hoznia.

A szótárak letölthetők a [QOwnNotes szótárakból](https://github.com/qownnotes/dictionaries). Használja a dokumentum végén található táblázatot, hogy megtalálja nyelvét és annak megfelelő nyelvi kódját.

Ha az Ön nyelvéhez tartozó szótár nincs a **Szótárkezelőben** vagy a fent említett linken, kérjük, keresse meg az interneten, vagy hozzon létre egy problémát. Ha megtaláljuk a szótárt, biztosan felvesszük a QOwnNotes következő kiadásába.

::: tip Info
Ha nem találja a nyelvét ebben a táblázatban, akkor sajnos nem létezik szótár a LibreOffice fájlokban - Ha szótárat szeretne létrehozni, lépjen a [Documentfoundation.org szótárak fejlesztése](https://wiki.documentfoundation.org/Development/Dictionaries) oldalra
:::

| Sr | Nyelvi kód | Nyelv                                  |
| -- | ---------- | -------------------------------------- |
| 1  | af_ZA      | Afrikaans (Dél-Afrika)                 |
| 2  | an_ES      | Aragóniai - Spanyolország (ES)         |
| 3  | ar         | Arab                                   |
| 4  | be_BY      | Belarusz                               |
| 5  | bg_BG      | Bolgár                                 |
| 6  | bn_BD      | Bengáli                                |
| 7  | bo         | Tibetan                                |
| 8  | br_FR      | Breton - France                        |
| 9  | bs_BA      | Bosnyák                                |
| 10 | ca         | Katalán                                |
| 11 | cs_CZ      | Cseh                                   |
| 12 | da_DK      | Dán                                    |
| 13 | de         | Német                                  |
| 14 | el_GR      | Görög                                  |
| 15 | en         | Angol                                  |
| 16 | es         | Spanyol                                |
| 17 | et_EE      | Észt                                   |
| 18 | gd_GB      | Kelta                                  |
| 19 | gl         | Galíciai                               |
| 20 | gu_IN      | Gudzsaráti                             |
| 21 | gug        | Guarani                                |
| 22 | he_IL      | Héber                                  |
| 23 | hi_IN      | Hindi                                  |
| 24 | hr_HR      | Horvát                                 |
| 25 | id         | Indonéz                                |
| 26 | it_IT      | Olasz                                  |
| 27 | kmr_Latn   | Kurd (Törökország)                     |
| 28 | lo_LA      | Lao                                    |
| 29 | lt_LT      | Litván                                 |
| 30 | lv_LV      | Lett                                   |
| 31 | ne_NP      | Nepáli                                 |
| 32 | nl_NL      | Holland                                |
| 33 | no         | Norvég                                 |
| 34 | oc_FR      | Okszitán                               |
| 35 | pl_PL      | Lengyel                                |
| 36 | pt_BR      | Brazíliai Portugál                     |
| 37 | pt_PT      | Portugál                               |
| 38 | ro         | Román                                  |
| 39 | ru_RU      | Orosz                                  |
| 40 | si_LK      | szingaléz                              |
| 41 | sk_SK      | Szlovák                                |
| 42 | sl_Sl      | Szlovén                                |
| 43 | sq_AL      | Albán                                  |
| 44 | sr         | Szerb                                  |
| 45 | sv_SE      | Svéd                                   |
| 46 | sw_TZ      | Szuahéli                               |
| 47 | te_IN      | Telugu                                 |
| 48 | th_TH      | Thai                                   |
| 49 | tr_TR      | Török                                  |
| 51 | uk_UA      | Ukrán                                  |
| 52 | vi         | Vietnámi                               |
| 53 | zu_ZA      | Zulu (helyesírási szótár nem elérhető) |

::: tip
Credits [Szonett](https://github.com/KDE/sonnet) [Hunspell](https://hunspell.github.io/)-lel a helyesírás-ellenőrzéshez
