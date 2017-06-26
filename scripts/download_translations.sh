#!/bin/bash
# Use this script in $ProjectFileDir$ to download the translations from Crowdin

TRANSLATION_PATH="src/languages"

echo "Downloading translation from Crowdin..."
java -jar /opt/crowdin-cli/crowdin-cli.jar download translations
echo

# we have to do this because this currently doesn't work in crowdin-cli
echo "Renaming translation files..."
cd ${TRANSLATION_PATH}

mv QOwnNotes_ar_SA.ts QOwnNotes_ar.ts
mv QOwnNotes_ca_ES.ts QOwnNotes_ca.ts
mv QOwnNotes_cs_CZ.ts QOwnNotes_cs.ts
mv QOwnNotes_de_DE.ts QOwnNotes_de.ts
mv QOwnNotes_es_ES.ts QOwnNotes_es.ts
mv QOwnNotes_fr_FR.ts QOwnNotes_fr.ts
mv QOwnNotes_hr_HR.ts QOwnNotes_hr.ts
mv QOwnNotes_hu_HU.ts QOwnNotes_hu.ts
mv QOwnNotes_it_IT.ts QOwnNotes_it.ts
mv QOwnNotes_ja_JP.ts QOwnNotes_ja.ts
mv QOwnNotes_nl_NL.ts QOwnNotes_nl.ts
mv QOwnNotes_pl_PL.ts QOwnNotes_pl.ts
mv QOwnNotes_ru_RU.ts QOwnNotes_ru.ts
mv QOwnNotes_uk_UA.ts QOwnNotes_uk.ts
mv QOwnNotes_zh_CN.ts QOwnNotes_zh.ts
