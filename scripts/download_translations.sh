#! /usr/bin/env nix-shell
#! nix-shell --pure -i bash -p crowdin-cli xmlstarlet
# shellcheck shell=bash
#
# Use this script in $ProjectFileDir$ to download the translations from Crowdin
#
# To update the translation files use:
# cd src && /usr/bin/lupdate -verbose QOwnNotes.pro
#
# To upload new source files use:
# crowdin upload
#
# Needed tools:
# - crowdin cli: https://support.crowdin.com/cli-tool/
#

TRANSLATION_PATH="src/languages"

echo "Downloading translation from Crowdin..."
# see: https://support.crowdin.com/cli-tool/
crowdin download || exit 1
echo

# we have to do this because this currently doesn't work in crowdin-cli
echo "Renaming translation files..."
pushd ${TRANSLATION_PATH} || exit 1

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
mv QOwnNotes_ha_HG.ts QOwnNotes_ha.ts
mv QOwnNotes_bg_BG.ts QOwnNotes_bg.ts
mv QOwnNotes_is_IS.ts QOwnNotes_is.ts
mv QOwnNotes_pa_IN.ts QOwnNotes_pa.ts
mv QOwnNotes_sq_AL.ts QOwnNotes_sq.ts
mv QOwnNotes_he_IL.ts QOwnNotes_he.ts
mv QOwnNotes_yi_DE.ts QOwnNotes_yi.ts
mv QOwnNotes_th_TH.ts QOwnNotes_th.ts
mv QOwnNotes_sn_ZW.ts QOwnNotes_sn.ts
mv QOwnNotes_km_KH.ts QOwnNotes_km.ts
mv QOwnNotes_si_LK.ts QOwnNotes_si.ts
mv QOwnNotes_zu_ZA.ts QOwnNotes_zu.ts
mv QOwnNotes_xh_ZA.ts QOwnNotes_xh.ts
mv QOwnNotes_ms_MY.ts QOwnNotes_ms.ts
mv QOwnNotes_mi_NZ.ts QOwnNotes_mi.ts
mv QOwnNotes_ga_IE.ts QOwnNotes_ga.ts
mv QOwnNotes_uz_UZ.ts QOwnNotes_uz.ts
mv QOwnNotes_vi_VN.ts QOwnNotes_vi.ts
mv QOwnNotes_lv_LV.ts QOwnNotes_lv.ts
mv QOwnNotes_ku_TR.ts QOwnNotes_ku.ts
mv QOwnNotes_lt_LT.ts QOwnNotes_lt.ts
mv QOwnNotes_bs_BA.ts QOwnNotes_bs.ts
mv QOwnNotes_mk_MK.ts QOwnNotes_mk.ts
mv QOwnNotes_ko_KR.ts QOwnNotes_ko.ts
mv QOwnNotes_et_EE.ts QOwnNotes_et.ts
# Delete until there is some translation
rm QOwnNotes_be_BY.ts

# Remove the location XML tag blocks, because they take up a lot of space and change all the time
echo "Remove location XML blocks in translation files..."
find ./*.ts -type f ! -name QOwnNotes_en.ts -exec xmlstarlet ed -L -d "//location" {} \;

# crowdin changes all whitespaces at every download, so we want to remove the
# leading whitespaces in all (but the source) translation files to save space
echo "Remove leading spaces in translation files..."
find ./*.ts -type f ! -name QOwnNotes_en.ts -exec sed -i 's/^[[:space:]]*//' {} \;

popd || exit 1

echo "Remove all translated webpage folders, but German, Hungarian, Persian and Dutch..."
# Remove all active languages here!
find webpage/src -type d -regextype posix-egrep -regex ".+src\/(am|be|bg|bn|bs|ca|ceb|cs|da|en|el|et|eu|fi|fil|ga|gl|ha|he|hi|hil|hr|id|is|ja|km|ku|lt|lv|mi|mk|ms|no|pa|pcm|pt|ro|ru|si|sk|sl|sn|sq|sr|sv|ta|th|tl|tlh|tr|uk|ur|uz|vi|xh|yi|zh|zu)$" -exec rm -Rf "{}" \;

#
# Fix Crowdin translation bugs
#

fixCrowdinTranslationProblems() {
  sed -i -e 's/::: Tip/::: tip/g' "$1"
  sed -i -e 's/::: tipp/::: tip/g' "$1"
  sed -i -e 's/::: suggerimento/::: tip/g' "$1"
  sed -i -e 's/::: Trinkgeld/::: tip/g' "$1"
  sed -i -e 's/::: Warnung/::: warning/g' "$1"
  sed -i -e 's/::: warning /::: warning\n/g' "$1"
  sed -i -e 's/::: tip /::: tip\n/g' "$1"
  sed -i -e ':a' -e 'N' -e '$!ba' -e 's/::: tip\nInfo /::: tip Info\n/g' "$1"
  sed -i -e ':a' -e 'N' -e '$!ba' -e 's/::: tip\nImportant /::: tip Important\n/g' "$1"
  sed -i -e 's/ :::$/\n:::/g' "$1"
  sed -i -e ':a' -e 'N' -e '$!ba' -e 's/~~~\n~~~/```\n~~~/g' "$1"
}

echo "Fix Crowdin translation bugs..."
export -f fixCrowdinTranslationProblems
# Add all active languages here!
find webpage/src -type f -regextype posix-egrep -regex ".+src\/(de|nl|fa|hu|es|fr|it|ar|pl)\/.+\.md" -exec bash -c 'fixCrowdinTranslationProblems "$0"' {} \;
