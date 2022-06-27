# Vérification orthographique

Faites corriger vos fautes d'orthographe !

::: Astuce Info QOwnNotes a un **Gestionnaire de dictionnaires** intégré (dans le menu *Édition*) que vous pouvez utiliser pour télécharger des dictionnaires ou les supprimer (si vous êtes votre langue est manquante, voir ci-dessous comment demander l'ajout d'un dictionnaire). Si vous souhaitez télécharger les dictionnaires manuellement, vous pouvez suivre les instructions ci-dessous.
:::

- Sous **Linux**, vous pouvez installer les dictionnaires Hunspell pour votre langue en utilisant le gestionnaire de pkg de votre distribution
    - Si vous devez ajouter des fichiers de langue supplémentaires, vous pouvez le faire dans `~/.local/share/hunspell` ou `/usr/share/hunspell`.
    - Si vous utilisez le package Snap de QOwnNotes, vous pouvez également utiliser `~/hunspell` pour les fichiers de langue, car le Snap ne peut pas accéder à  `~/.local`

- Sous **Windows**, vous devez copier les dictionnaires Hunspell pour votre langue dans un dossier `dicts` dans votre dossier d’application QOwnNotes ou dans `C:\Users\[username]\dicts`
    - Vous pouvez télécharger des dictionnaires à partir de [dictionnaires LibreOffice](https://github.com/LibreOffice/dictionaries)
    - Sous **macOS**, vous devez copier les dictionnaires Hunspell pour votre langue dans `/Library/Spelling` ou `~/Library/Spelling`. Si le répertoire n'existe pas, vous devez le créer.

Les dictionnaires peuvent être téléchargés à partir de [dictionnaires QOwnNotes](https://github.com/qownnotes/dictionaries). Utilisez le tableau à la fin de ce document pour trouver votre langue et son code de langue respectif.

Si le dictionnaire de votre langue ne se trouve pas dans le **Gestionnaire de dictionnaires** ou dans le lien mentionné ci-dessus, veuillez le rechercher sur Internet ou créer un problème. Nous ne manquerons pas de l'inclure dans la prochaine version de QOwnNotes si le dictionnaire est trouvé.

::: tip Info
Si vous ne trouvez pas votre langue dans ce tableau, il n’existe malheureusement pas de dictionnaire car il n’existe pas dans les fichiers LibreOffice - Si vous voulez créer un dictionnaire, allez dans [Documentfoundation.org Dictionaries Development](https://wiki.documentfoundation.org/Development/Dictionaries)
:::

| Sr    | Code de langue | Langue                            |
| ----- | -------------- | --------------------------------- |
| 1     | af_ZA          | Afrikaans (Afrique du Sud)        |
| 2     | an_ES          | Aragonais - Espagne (ES)          |
| 3     | ar             | Arabe                             |
| 4     | be_BY          | Biélorusse                        |
| 5     | bg_BG          | Bulgare                           |
| 6     | bn_BD          | Bengali                           |
| 7     | bo             | Tibétain                          |
| 8     | br_FR          | Breton - France                   |
| 9     | bs_BA          | Bosniaque                         |
| 10    | ca             | Catalan                           |
| 11    | cs_CZ          | Tchèque                           |
| 12    | da_DK          | Danois                            |
| 13    | de             | Allemand                          |
| 14    | el_GR          | Grec                              |
| 15    | en             | Anglais                           |
| 16    | es             | Espagnol                          |
| 17    | et_EE          | Estonien                          |
| 18    | gd_GB          | Gaélique                          |
| 19    | gl             | Galicien                          |
| 20    | gu_IN          | Gujarati                          |
| 21    | gug            | Guarani                           |
| 22    | he_IL          | Hébreu                            |
| 23    | hi_IN          | Hindi                             |
| 24    | hr_HR          | Croate                            |
| 25    | id             | Indonésien                        |
| 26    | it_IT          | Italien                           |
| 27    | kmr_Latn       | Kurde (Turquie)                   |
| 28    | lo_LA          | Lao                               |
| 29    | lt_LT          | Lituanien                         |
| 30    | lv_LV          | Letton                            |
| 31    | ne_NP          | Népalais                          |
| bn_BD | nl_NL          | Néerlandais                       |
| 33    | no             | Norvégien                         |
| 34    | oc_FR          | Occitan                           |
| 35    | pl_PL          | Polonais                          |
| 36    | pt_BR          | Brésilien Portugais               |
| 37    | pt_PT          | Portugais                         |
| 38    | ro             | Roumain                           |
| 39    | ru_RU          | Russe                             |
| 40    | si_LK          | Cingalais                         |
| 41    | sk_SK          | Slovaque                          |
| 42    | sl_Sl          | Slovène                           |
| 43    | sq_AL          | Albanais                          |
| 44    | sr             | Serbe                             |
| 45    | sv_SE          | Suédois                           |
| 46    | sw_TZ          | Swahili                           |
| 47    | te_IN          | Telugu                            |
| 48    | th_TH          | Thaïlandais                       |
| 49    | tr_TR          | Turc                              |
| 51    | uk_UA          | Ukrainien                         |
| 52    | vi             | Vietnamien                        |
| 53    | zu_ZA          | Zulu (orthographe non disponible) |

::: tip
Remerciements [Sonnet](https://github.com/KDE/sonnet) avec [Hunspell](https://hunspell.github.io/) est utilisé pour la vérification orthographique
:::
