# التدقيق الإملائي

أصلح أخطاءك الإملائية!

::: tip Info
QOwnNotes has a builtin **Dictionary Manager** (in the _Edit_ menu) which you can use to download dictionaries or delete them (if you are missing your language see below on how to request adding a dictionary). إذا أردت تنزيل القواميس يدويًا، يمكنك اتباع الخطوات التالية.
:::

- في **لينكس** يمكنك تثبيت قواميس Hunspell للغتك باستخدام مدير الحزم الخاص بتوزيعتك
  - If you need to add additional language files you can do so in `~/.local/share/hunspell` or `/usr/share/hunspell`.
  - If you are using the Snap package of QOwnNotes you can also use `~/hunspell` for language files, since the Snap can't access `~/.local`

- في **ويندوز** عليك نسخ قواميس Hunspell للغتك إلى مجلد `dicts` في مجلد تطبيق QOwnNotes أو في `C:\Users\[username]\dicts`
  - يمكنك تنزيل القواميس من [قواميس ليبر أوفيس](https://github.com/LibreOffice/dictionaries)
  - في **ماك أو إس** عليك نسخ قواميس Hunspell للغتك إلى <code dir="ltr">/Library/Spelling</code> أو <code dir="ltr">~/Library/Spelling</code>. إذا لم يكن هذا المجلد موجودا، عليك إنشاؤه.

يمكن تنزيل القواميس من [قواميس QOwnNotes](https://github.com/qownnotes/dictionaries). استخدم الجدول الذي في نهاية هذا المستند لتجد لغتك والرمز المناظر لها.

إذا لم يكن قاموس لغتك متاحًا في **مدير القواميس** أو في الرابط الذي بالأعلى، برجاء البحث عنه على الإنترنت أو فتح مشكلة. سنتأكد من إضافته إلى الإصدارة التالية من QOwnNotes إذا وُجد.

::: tip
معلومة إذا لم تستطع العثور على لغتك في هذا الجدول، فللأسف ليس لها قاموس في ملفات ليبر أوفيس — إذا أردت إنشاء قاموس، اذهب إلى صفحة [تطوير القواميس على موقع منظمة المستند (بالإنجليزية)](https://wiki.documentfoundation.org/Development/Dictionaries)
:::

| م   | رمز اللغة | اللغة                             |
| --- | --------- | --------------------------------- |
| 1   | af_ZA     | الأفريكانية (جنوب أفريقيا)        |
| 2   | an_ES     | الأراغونية (إسبانيا)              |
| 3   | ar        | العربية                           |
| 4   | be_BY     | البيلاروسية                       |
| 5   | bg_BG     | البلغارية                         |
| 6   | bn_BD     | البنغالية                         |
| 7   | bo        | التبتية                           |
| 8   | br_FR     | البريتانية (فرنسا)                |
| 9   | bs_BA     | البوسنية                          |
| 10  | ca        | الكاتالونية                       |
| 11  | cs_CZ     | التشيكية                          |
| 12  | da_DK     | الدانماركية                       |
| 13  | de        | الألمانية                         |
| 14  | el_GR     | اليونانية                         |
| 15  | en        | الإنجليزية                        |
| 16  | es        | الإسبانية                         |
| 17  | et_EE     | الإستونية                         |
| 18  | gd_GB     | الغيلية                           |
| 19  | gl        | الغاليسية                         |
| 20  | gu_IN     | الكجراتية                         |
| 21  | gug       | الغوارانية                        |
| 22  | he_IL     | العبرية                           |
| 23  | hi_IN     | الهندية                           |
| 24  | hr_HR     | الكرواتية                         |
| 25  | id        | الإندونيسية                       |
| 26  | it_IT     | الإيطالية                         |
| 27  | kmr_Latn  | الكردية (تركيا)                   |
| 28  | lo_LA     | اللاوية                           |
| 29  | lt_LT     | الليتوانية                        |
| 30  | lv_LV     | اللاتفية                          |
| 31  | ne_NP     | النيبالية                         |
| 32  | nl_NL     | الهولندية                         |
| 33  | no        | النرويجية                         |
| 34  | oc_FR     | الأكستانية                        |
| 35  | pl_PL     | البولندية                         |
| 36  | pt_BR     | البرتغالية البرازيلية             |
| 37  | pt_PT     | البرتغالية                        |
| 38  | ro        | الرومانية                         |
| 39  | ru_RU     | الروسية                           |
| 40  | si_LK     | السنهالية                         |
| 41  | sk_SK     | السلوفاكية                        |
| 42  | sl_Sl     | السلوفينية                        |
| 43  | sq_AL     | الألبانية                         |
| 44  | sr        | الصربية                           |
| 45  | sv_SE     | السويدية                          |
| 46  | sw_TZ     | السواحلية                         |
| 47  | te_IN     | التيلوغوية                        |
| 48  | th_TH     | التايلاندية                       |
| 49  | tr_TR     | التركية                           |
| 51  | uk_UA     | الأوكرانية                        |
| 52  | vi        | الفيتنامية                        |
| 53  | zu_ZA     | الزولوية (قاموس الإملاء غير متاح) |

::: tip
شكر نستخدم [Sonnet](https://github.com/KDE/sonnet) مع [Hunspell](https://hunspell.github.io/) للتدقيق الإملائي
:::
