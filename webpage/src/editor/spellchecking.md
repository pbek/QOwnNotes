# Spellchecking

Get your spelling errors corrected!

::: tip Info
QOwnNotes has a builtin **Dictionary Manager** (in the _Edit_ menu)
which you can use to download dictionaries or delete them (if you are
missing your language see below on how to request adding a dictionary).
If you want to download the dictionaries manually, you can follow the
instructions below.
:::

- Under **Linux** you can install the Hunspell dictionaries for your language using your distro's pkg manager
  - If you need to add additional language files you can do so
    in `~/.local/share/hunspell` or `/usr/share/hunspell`.
  - If you are using the Snap package of QOwnNotes you can also
    use `~/hunspell` for language files, since the Snap can't
    access `~/.local`

- Under **Windows** you need to copy Hunspell dictionaries for your language to
  a folder `dicts` in your QOwnNotes application folder or in `C:\Users\[username]\dicts`
  - You can download dictionaries from [LibreOffice dictionaries](https://github.com/LibreOffice/dictionaries)
  - Under **macOS** you need to copy Hunspell dictionaries for your
    language to `/Library/Spelling` or `~/Library/Spelling`. If the
    directory doesn't exist you need to create it.

Dictionaries can be downloaded from [QOwnNotes
dictionaries](https://github.com/qownnotes/dictionaries). Use the table
at the end of this document to find your language and its respective
language code.

If the dictionary for your language is not in the **Dictionary Manager**
or in the above mentioned link, please look for it on the internet or
create an issue. We'll be sure to include it in the next release of
QOwnNotes if the dictionary is found.

::: tip Info
If you can't find your language in this table then unfortunately
a dictionary for it doesn't exist in the LibreOffice files - If you want
to create a dictionary, go to the
[Documentfoundation.org Dictionaries Development](https://wiki.documentfoundation.org/Development/Dictionaries)
:::

| Sr  | Language Code | Language                   |
| --- | ------------- | -------------------------- |
| 1   | af_ZA         | Afrikaans (South Africa)   |
| 2   | an_ES         | Aragonese - Spain (ES)     |
| 3   | ar            | Arabic                     |
| 4   | be_BY         | Belarusian                 |
| 5   | bg_BG         | Bulgarian                  |
| 6   | bn_BD         | Bengali                    |
| 7   | bo            | Tibetan                    |
| 8   | br_FR         | Breton - France            |
| 9   | bs_BA         | Bosnian                    |
| 10  | ca            | Catalan                    |
| 11  | cs_CZ         | Czech                      |
| 12  | da_DK         | Danish                     |
| 13  | de            | German                     |
| 14  | el_GR         | Greek                      |
| 15  | en            | English                    |
| 16  | es            | Spanish                    |
| 17  | et_EE         | Estonian                   |
| 18  | gd_GB         | Gaelic                     |
| 19  | gl            | Galician                   |
| 20  | gu_IN         | Gujarati                   |
| 21  | gug           | Guarani                    |
| 22  | he_IL         | Hebrew                     |
| 23  | hi_IN         | Hindi                      |
| 24  | hr_HR         | Croatian                   |
| 25  | id            | Indonesian                 |
| 26  | it_IT         | Italian                    |
| 27  | kmr_Latn      | Kurdish (Turkey)           |
| 28  | lo_LA         | Lao                        |
| 29  | lt_LT         | Lithuanian                 |
| 30  | lv_LV         | Latvian                    |
| 31  | ne_NP         | Nepali                     |
| 32  | nl_NL         | Dutch                      |
| 33  | no            | Norwegian                  |
| 34  | oc_FR         | Occitan                    |
| 35  | pl_PL         | Polish                     |
| 36  | pt_BR         | Brazilian Portuguese       |
| 37  | pt_PT         | Portuguese                 |
| 38  | ro            | Romanian                   |
| 39  | ru_RU         | Russian                    |
| 40  | si_LK         | Sinhalese                  |
| 41  | sk_SK         | Slovak                     |
| 42  | sl_Sl         | Slovenian                  |
| 43  | sq_AL         | Albanian                   |
| 44  | sr            | Serbian                    |
| 45  | sv_SE         | Swedish                    |
| 46  | sw_TZ         | Swahili                    |
| 47  | te_IN         | Telugu                     |
| 48  | th_TH         | Thai                       |
| 49  | tr_TR         | Turkish                    |
| 51  | uk_UA         | Ukrainian                  |
| 52  | vi            | Vietnamese                 |
| 53  | zu_ZA         | Zulu(spell dict not avail) |

::: tip Credits
[Sonnet](https://github.com/KDE/sonnet) with
[Hunspell](https://hunspell.github.io/) is used for spellchecking
:::
