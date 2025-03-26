# استيراد الملاحظات

## Evernote

There is an Evernote import dialog you can reach in the `Note / Import` menu to import _Enex_ files.

سيستورد أيضا الصور والملحقات والوسوم والبيانات الوصفية والتنسيق الأساسي للنصوص.

## Joplin

يمكنك الوصول إلى صندوق حوار استيراد Joplin من قائمة `ملاحظة > استيراد`.

## صيغ عديدة

تستطيع أداة سطر الأوامر [jimmy](https://github.com/marph91/jimmy) تحويل صيغ عديدة إلى صيغة ماركداون (مثل [Google Keep](https://marph91.github.io/jimmy/formats/google_keep/)، و&nbsp;[Synology Note Station](https://marph91.github.io/jimmy/formats/synology_note_station/)، وغيرهما [الكثير](https://marph91.github.io/jimmy/)). للاحتفاظ بكل ما يمكن من البيانات الوصفية، ننصح بتطبيق [تطويعات إضافية](https://marph91.github.io/jimmy/import_instructions/#qownnotes).

لو واجهتك مشكلة في التحويل، يمكنك إنشاء مسألة في [GitHub](https://github.com/marph91/jimmy/issues).

## Google Keep

> استخدم Google Takeout لتنزيل مهام Keep
>
> استخرج محتويات الملف المضغوط
>
> نزّل keep_to_markdown.py إلى جوار مجلد Keep الذي استخرجته
>
>     python keep_to_markdown.py Keep out/
>
> أبدل الاسم Keep في الأمر باسم المجلد الذي فيه الملاحظات بصيغة JSON. ومجلد out سيُنشأ إذا لم يكن موجودا.

من <https://gitlab.com/-/snippets/2002921>

## تومبوي

يمكنك استيراد ملاحظاتك من تومبوي باستخدام بُريمج يُسمى [Trombone](https://github.com/samba/trombone).

ربما تحتاج إلى تثبيت python2 حتى يعمل. سيحوّل ملاحظاتك في تومبوي إلى ملف Evernote بالامتداد `enex`، والذي يمكنك عندئذٍ استيراده إلى QOwnNotes.

إلى من يريد فعل ذلك: تأكد أولا من أن Python2 مثبتة لديك، وربما تحتاج أيضا إلى تثبيت `python-is-python2` (يمكنك إزالتها فيما بعد). قد يكون فعل ذلك أسهل أو أأمن في جذر بديل (chroot) أو حاوية Docker، إذا أمكنك ذلك (سنشرح هذا هنا فيما بعد).

```bash
sudo apt install python2 python-is-python2
```

نزّل ملف trombone من GitHub، واستخرج محتوياته، وانتقل إلى المجلد الناتج:

```bash
cd ~/Downloads/trombone-master

sudo make
sudo make install
```

ثم انتقل إلى المجلد الذي فيه ملاحظات تومبوي الخاصة بك:

```bash
 cd ~/.local/share/tomboy/
```

ثم نفّذ هذا الأمر:

```bash
find ./ -type f -name '*.note' -print0 | xargs -0 trombone > EXPORT.enex
```

إذا واجهت أخطاء بخصوص يونيكود بسبب ملفات ملاحظات معينة، عليك فقط أن تزيل هذه الملاحظات وتنفّذ أمر التحويل مجددا حتى يعطيك رسالة تقول <code dir="ltr">Saving...</code> . ستجد ملفًا اسمه `EXPORT.enex` والذي يمكنك عندئذٍ استيراده إلى QOwnNotes.

عند الاستيراد إلى QOwnNotes، قد تود تعطيل جميع الخصائص المراد استيرادها، ربما باستثناء تاريخ الإنشاء والتحديث، لأن تومبوي لا يدعم تلك الخصائص.
