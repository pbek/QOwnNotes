Spellchecking
~~~~~~~~~~~~~
QOwnNotes has a builtin **Dictionary Manager** (in the *Edit* menu) which you can use to download dictionaries or delete them (if you are missing your language see below on how to request adding a dictionary). If you want to download the dictionaries manually, you can follow the instructions below:

-  Under **Linux** you can install the Hunspell dictionaries for your language using your distro's pkg manager
    - If you need to add additional language files you can do so in ``~/.local/share/hunspell`` or ``/usr/share/hunspell``.
    - If you are using the Snap package of QOwnNotes you can also use ``~/hunspell`` for language files, since the Snap can't access ``~/.local``
-  Under **Windows** you need to copy Hunspell dictionaries for your language to
   a folder ``dicts`` in your QOwnNotes application folder or in ``C:\Users\[username]\dicts``
  -  You can download dictionaries from `LibreOffice
     dictionaries <https://github.com/LibreOffice/dictionaries>`__
-  Under **macOS** you need to copy Hunspell dictionaries for your language to ``/Library/Spelling`` or ``~/Library/Spelling``. If the directory doesn't exist you need to create it.

Dictionaries can be downloaded from `QOwnNotes dictionaries <https://github.com/qownnotes/dictionaries>`_. Use the table at the end of this document to find your language and its respective language code.

If the dictionary for your language is not in the **Dictionary Manager** or in the above mentioned link, please look for it on the internet or create an issue. We'll be sure to include it in the next release of QOwnNotes if the dictionary is found.

Note:
- If you can't find your language in this table then unfortunately a dictionary for it doesn't exist in the LibreOffice files
- If you want to create a dictionary, go to the `Documentfoundation.org Dictionaries Development <https://wiki.documentfoundation.org/Development/Dictionaries>`_


+------+-----------------+----------------------------+
| Sr   | Language Code   | Language                   |
+======+=================+============================+
| 1    | af\_ZA          | Afrikaans (South Africa)   |
+------+-----------------+----------------------------+
| 2    | an\_ES          | Aragonese - Spain (ES)     |
+------+-----------------+----------------------------+
| 3    | ar              | Arabic                     |
+------+-----------------+----------------------------+
| 4    | be\_BY          | Belarusian                 |
+------+-----------------+----------------------------+
| 5    | bg\_BG          | Bulgarian                  |
+------+-----------------+----------------------------+
| 6    | bn\_BD          | Bengali                    |
+------+-----------------+----------------------------+
| 7    | bo              | Tibetan                    |
+------+-----------------+----------------------------+
| 8    | br\_FR          | Breton - France            |
+------+-----------------+----------------------------+
| 9    | bs\_BA          | Bosnian                    |
+------+-----------------+----------------------------+
| 10   | ca              | Catalan                    |
+------+-----------------+----------------------------+
| 11   | cs\_CZ          | Czech                      |
+------+-----------------+----------------------------+
| 12   | da\_DK          | Danish                     |
+------+-----------------+----------------------------+
| 13   | de              | German                     |
+------+-----------------+----------------------------+
| 14   | el\_GR          | Greek                      |
+------+-----------------+----------------------------+
| 15   | en              | English                    |
+------+-----------------+----------------------------+
| 16   | es              | Spanish                    |
+------+-----------------+----------------------------+
| 17   | et\_EE          | Estonian                   |
+------+-----------------+----------------------------+
| 18   | gd\_GB          | Gaelic                     |
+------+-----------------+----------------------------+
| 19   | gl              | Galician                   |
+------+-----------------+----------------------------+
| 20   | gu\_IN          | Gujarati                   |
+------+-----------------+----------------------------+
| 21   | gug             | Guarani                    |
+------+-----------------+----------------------------+
| 22   | he\_IL          | Hebrew                     |
+------+-----------------+----------------------------+
| 23   | hi\_IN          | Hindi                      |
+------+-----------------+----------------------------+
| 24   | hr\_HR          | Croatian                   |
+------+-----------------+----------------------------+
| 25   | id              | Indonesian                 |
+------+-----------------+----------------------------+
| 26   | it\_IT          | Italian                    |
+------+-----------------+----------------------------+
| 27   | kmr\_Latn       | Kurdish (Turkey)           |
+------+-----------------+----------------------------+
| 28   | lo\_LA          | Lao                        |
+------+-----------------+----------------------------+
| 29   | lt\_LT          | Lithuanian                 |
+------+-----------------+----------------------------+
| 30   | lv\_LV          | Latvian                    |
+------+-----------------+----------------------------+
| 31   | ne\_NP          | Nepali                     |
+------+-----------------+----------------------------+
| 32   | nl\_NL          | Dutch                      |
+------+-----------------+----------------------------+
| 33   | no              | Norwegian                  |
+------+-----------------+----------------------------+
| 34   | oc\_FR          | Occitan                    |
+------+-----------------+----------------------------+
| 35   | pl\_PL          | Polish                     |
+------+-----------------+----------------------------+
| 36   | pt\_BR          | Brazilian Portuguese       |
+------+-----------------+----------------------------+
| 37   | pt\_PT          | Portuguese                 |
+------+-----------------+----------------------------+
| 38   | ro              | Romanian                   |
+------+-----------------+----------------------------+
| 39   | ru\_RU          | Russian                    |
+------+-----------------+----------------------------+
| 40   | si\_LK          | Sinhalese                  |
+------+-----------------+----------------------------+
| 41   | sk\_SK          | Slovak                     |
+------+-----------------+----------------------------+
| 42   | sl\_Sl          | Slovenian                  |
+------+-----------------+----------------------------+
| 43   | sq\_AL          | Albanian                   |
+------+-----------------+----------------------------+
| 44   | sr              | Serbian                    |
+------+-----------------+----------------------------+
| 45   | sv\_SE          | Swedish                    |
+------+-----------------+----------------------------+
| 46   | sw\_TZ          | Swahili                    |
+------+-----------------+----------------------------+
| 47   | te\_IN          | Telugu                     |
+------+-----------------+----------------------------+
| 48   | th\_TH          | Thai                       |
+------+-----------------+----------------------------+
| 49   | tr\_TR          | Turkish                    |
+------+-----------------+----------------------------+
| 51   | uk\_UA          | Ukrainian                  |
+------+-----------------+----------------------------+
| 52   | vi              | Vietnamese                 |
+------+-----------------+----------------------------+
| 53   | zu\_ZA          | Zulu(spell dict not avail) |
+------+-----------------+----------------------------+

Credits
`Sonnet <https://github.com/KDE/sonnet>`__ with `Hunspell <https://hunspell.github.io/>`__ is used for spellchecking
