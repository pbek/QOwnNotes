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
mv QOwnNotes_sv_SE.ts QOwnNotes_sv.ts
mv QOwnNotes_id_ID.ts QOwnNotes_id.ts
mv QOwnNotes_bn_BD.ts QOwnNotes_bn.ts
mv QOwnNotes_tr_TR.ts QOwnNotes_tr.ts
mv QOwnNotes_tl_PH.ts QOwnNotes_tl.ts
mv QOwnNotes_fil_PH.ts QOwnNotes_fil.ts
mv QOwnNotes_ceb_PH.ts QOwnNotes_ceb.ts
mv QOwnNotes_hil_PH.ts QOwnNotes_hil.ts
mv QOwnNotes_hi_IN.ts QOwnNotes_hi.ts
mv QOwnNotes_ur_PK.ts QOwnNotes_ur.ts
mv QOwnNotes_fi_FI.ts QOwnNotes_fi.ts
mv QOwnNotes_el_GR.ts QOwnNotes_el.ts
mv QOwnNotes_gl_ES.ts QOwnNotes_gl.ts
mv QOwnNotes_no_NO.ts QOwnNotes_no.ts
mv QOwnNotes_da_DK.ts QOwnNotes_da.ts
mv QOwnNotes_ro_RO.ts QOwnNotes_ro.ts
mv QOwnNotes_sk_SK.ts QOwnNotes_sk.ts
mv QOwnNotes_sl_SI.ts QOwnNotes_sl.ts
mv QOwnNotes_eu_ES.ts QOwnNotes_eu.ts
mv QOwnNotes_sr_SP.ts QOwnNotes_sr.ts
mv QOwnNotes_fa_IR.ts QOwnNotes_fa.ts
