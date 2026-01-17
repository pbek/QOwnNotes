# استيراد الملاحظات

## Evernote

There is an Evernote import dialog you can reach in the `Note / Import` menu to import _Enex_ files.

سيستورد أيضا الصور والملحقات والوسوم والبيانات الوصفية والتنسيق الأساسي للنصوص.

## Joplin

يمكنك الوصول إلى صندوق حوار استيراد Joplin من قائمة `ملاحظة > استيراد`.

## Obsidian

1. Install the community plugin ["Better Markdown Links"](https://github.com/mnaoumov/obsidian-better-markdown-links) to convert all _wikilink and Obsidian flavoured markdown links (OFM)_ and make Obsidian operate with _standard markdown links_.

2. Open the same files (same vault) in QOwnNotes. Install the QOwnNotes script [link-importer-and-converter-for-obsidian-github-pandoc-markor-and-others](https://github.com/qownnotes/scripts/tree/main/link-importer-and-converter-for-obsidian-github-pandoc-markor-and-others) and press the import button "ObsidianImport" in the script panel to convert all files.

Exporting from QOwnNotes to Obsidian is not needed.

::: warning
Warning _Caution(!)_: The plugin `"Better Markdown Links"` silently adapts links when Obsidian is opened or when folders, files or headings are renamed. Press the "ObsidianImport" button in QOwnNotes everytime you have changed files in Obsidian, otherwise some links might not be recognised by QOwnNotes anymore!
:::

## VS Codium / Github / Markor / Typora / Pandoc and others (Github flavoured markdown GFM)

Githubflavoured markdown (GFM) uses hyphenstyle link headings (spaces converted to hyphens). A reference to a heading such as `## Heading with spaces` for example will look like `[notelink](β_note.md#heading-with-spaces)`.

QOwnNotes and Obsidian (Obsidian flavoured markdown OFM) use _urlencoded (percentencoded) heading fragments_ (spaces converted to %20 and keeping capital letters), so the same link will look different `[notelink](%CE%B2_note.md#Heading%20with%20spaCes)`.

To convert between GFM and QOwnNotes/Obsidian (OFM), just open the files in QOwnNotes. Install the QOwnNotes script [link-importer-and-converter-for-obsidian-github-pandoc-markor-and-others](https://github.com/qownnotes/scripts/tree/main/link-importer-and-converter-for-obsidian-github-pandoc-markor-and-others) and press the import button "GFMImport" in the script panel to convert all files

Exporting from QOwnNotes to GFM works for a single page (useful for a consecutive conversion with pandoc) or for all files.

::: warning
Warning _Caution_: Possible loss of information in heading links in special cases! Assume you have a note `β_note.md` with several identical headings like `## Advantages` within the _same (!) note_ . GFM allows to reference each heading with `[notelink](%CE%B2_note.md#advantages), [notelink](%CE%B2_note.md#advantages-1), [notelink](%CE%B2_note.md#advantages-2), ...`. _Neither QOwnNotes nor Obsidian allow for this!_ QOwnnotes and Obsidian both need unique heading names within the same document to be able to reference each of them! As this feature is not supported at all, after the conversion those links will all point to the first occuring heading with that particular name `[notelink](%CE%B2_note.md#advantages)` (this is especially relevant in long documents)! As a workaround just add text to the headings and update the links to the headings first (make them unique).
:::

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
