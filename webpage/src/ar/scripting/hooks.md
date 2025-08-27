# الخطاطيف

## onNoteStored (خطاف حفظ الملاحظة)

### نداء الدالة ومُعامِلاتها

```js
/**
 * تُنادى هذه الدالة عندما تخزن ملاحظة على قرص التخزين
 * لا يمكنك تعديل الملاحظات المخزنة؛ سيسبب هذا فوضى
 * لأنك على الأرجح ستكون مُعدِّلا لها يدويا في الوقت نفسه
 *
 * @param {NoteApi} note
 *       كائن الملاحظة الخاص بالملاحظة المخزنة
 */
function onNoteStored(note);
```

ربما تحب أن تلقي نظرة على المثال [on-note-opened.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/on-note-opened.qml).

## noteOpenedHook (خطاف فتح ملاحظة)

### نداء الدالة ومُعامِلاتها

```js
/**
 * تُنادى هذه الدالة عند فتح ملاحظة
 *
 * @param {NoteApi} note
 *       كائن الملاحظة الخاص بالملاحظة المفتوحة
 */
function noteOpenedHook(note);
```

ربما تحب أن تلقي نظرة على المثال [on-note-opened.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/on-note-opened.qml).

## noteDoubleClickedHook (خطاف النقر المزدوج على ملاحظة)

### نداء الدالة ومُعامِلاتها

```js
/**
 * تُنادى هذه الدالة عند النقر المزدوج على ملاحظة
 *
 * @param {NoteApi} note
 *       كائن الملاحظة الخاص بالملاحظة التي نقر المستخدم عليها
 */
function noteDoubleClickedHook(note);
```

ربما تحب أن تلقي نظرة على المثال [external-note-open.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/external-note-open.qml).

## insertMediaHook (خطاف إضافة وسائط)

تُنادى هذه الدالة عندما يُضاف ملف وسائط إلى الملاحظة الحالية.

إذا كانت هذه الدالة مُعرَّفة في أكثر من بُريمج، فإن أول بُريمج يُعيد سلسلة نصية غير فارغة يفوز.

### نداء الدالة ومُعامِلاتها

```js
/**
 * @param fileName
 *       سلسلة نصية: المسار الأصلي لملف الوسائط قبل نسخه إلى مجلد الوسائط
 * @param markdownText
 *       سلسلة نصية: نص ماركداون لملف الوسائط؛ مثال
 *       ![my-image](media/my-image-4101461585.jpg)
 * @return
 *        سلسلة نصية: نص ماركداون الجديد لملف الوسائط
 */
function insertMediaHook(fileName, markdownText);
```

ربما تحب أن تلقي نظرة على المثال [example.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/example.qml).

## insertAttachmentHook (خطاف إضافة مرفق)

تُنادى هذه الدالة عندما يُضاف ملف مرفق إلى الملاحظة الحالية.

إذا كانت هذه الدالة مُعرَّفة في أكثر من بُريمج، فإن أول بُريمج يُعيد سلسلة نصية غير فارغة يفوز.

### نداء الدالة ومُعامِلاتها

```js
/**
 * @param fileName
 *       سلسلة نصية: المسار الأصلي للملف المرفق قبل نسخه إلى مجلد المرفقات
 * @param markdownText
 *       سلسلة نصية: نص ماركداون للملف المرفق؛ مثال
 *       [my-file.txt](attachments/my-file-4245650967.txt)
 * @return
 *        سلسلة نصية: نص ماركداون الجديد للملف المرفق
 */
function insertAttachmentHook(fileName, markdownText);
```

ربما تحب أن تلقي نظرة على المثال [example.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/example.qml).

## insertingFromMimeDataHook (خطاف إلصاق هتمل أو وسائط)

### نداء الدالة ومُعامِلاتها

```js
/**
 * تُنادى هذه الدالة عندما يتم لصق هتمل أو ملف وسائط بضغط
 * `Ctrl + Shift + V`
 *
 * @param text
 *       نص كائن بيانات المايم (QMimeData)
 * @param html
 *       هتمل كائن بيانات المايم (QMimeData)
 * @return
 *        السلسلة النصية التي يجب إضافتها بدلا من نص كائن بيانات المايم
 */
function insertingFromMimeDataHook(text, html);
```

ربما تحب أن تلقي نظرة على الأمثلة [example.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/example.qml) أو [insert-headline-with-link-from-github-url.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/insert-headline-with-link-from-github-url.qml) أو [note-text-from-5pm-mail.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/note-text-from-5pm-mail.qml).

## handleNoteTextFileNameHook (خطاف التعامل مع اسم ملف الملاحظة)

### نداء الدالة ومُعامِلاتها

```js
/**
 * تُنادى هذه الدالة عندما تخزن ملاحظة على قرص التخزين إذا كان
 * الخيار «اسمح لاسم ملف الملاحظة أن يكون مختلفا عن العنوان» مفعلا
 * في الإعدادات
 *
 * تتيح لك هذه الدالة تعديل اسم ملف الملاحظة
 * تذكر أن عليك الاهتمام بتفادي تكرار أسماء الملفات بنفسك!‏
 *
 * أعد سلسلة نصية فارغة إذا كان اسم ملف الملاحظة لا يجب أن يتغيّر
 *
 * @param {NoteApi} note
 *       كائن الملاحظة الخاص بالملاحظة المخزنة
 * @return {string}
 *        سلسلة نصية: اسم ملف الملاحظة
 */
function handleNoteTextFileNameHook(note);
```

ربما تحب أن تلقي نظرة على المثالين [example.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/example.qml) أو [use-tag-names-in-filename.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/use-tag-names-in-filename.qml).

## handleNoteNameHook (خطاف التعامل مع اسم الملاحظة)

### نداء الدالة ومُعامِلاتها

```js
/**
 * تُنادى هذه الدالة عندما يتحدد اسم الملاحظة
 *
 * وتسمح لك بتعديل اسمها المعروض
 *
 * أعد سلسلة نصية فارغة إذا كان اسم الملاحظة لا يحتاج تغيير
 *
 * @param {NoteApi} note
 *       كائن الملاحظة الخاص بالملاحظة المخزنة
 * @return {string}
 *        سلسلة نصية: اسم الملاحظة
 */
function handleNoteNameHook(note);
```

The note name is mainly used when showing the note in the lists.

ربما تحب أن تلقي نظرة على المثال [example.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/example.qml).

ليس مستحسنا استخدام هذا الخطاف إذا فعّل المستخدم خيار السماح لاسم الملف أن يختلف عن اسم الملاحظة.

## handleNewNoteHeadlineHook (خطاف التعامل مع عنوان الملاحظة الجديدة)

### نداء الدالة ومُعامِلاتها

```js
/**
 * تُنادى هذه الدالة قبل إنشاء ملاحظة
 *
 * تتيح لك هذه الدالة تعديل عنوان الملاحظة الرئيسي قبل إنشائها
 * لاحظ أن عليك الاهتمام بإنشاء اسم ملاحظة فريد بنفسك
 * وإلا فلن يتم إنشاء ملاحظة جديدة، بل مجرد إيجادها في قائمة الملاحظات
 *
 * يمكنك استخدام هذه الدالة لإنشاء قوالب للملاحظات
 *
 * @param headline
 *       النص الذي سيستخدم لإنشاء العنوان الرئيسي
 * @return {string}
 *        سلسلة نصية: عنوان الملاحظة الرئيسي
 */
function handleNewNoteHeadlineHook(headline);
```

ربما تحب أن تلقي نظرة على المثال [custom-new-note-headline.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-new-note-headline.qml).

## preNoteToMarkdownHtmlHook (خطاف ما قبل تحويل الملاحظة من ماركداون إلى HTML)

### نداء الدالة ومُعامِلاتها

```js
/**
 * This function is called before the markdown html of a note is generated
 *
 * It allows you to modify what is passed to the markdown to html converter
 *
 * The function can for example be used in multiple scripts to render code (like LaTeX math or mermaid)
 * to its graphical representation for the preview
 *
 * The note will not be changed in this process
 *
 * @param {NoteApi} note - the note object
 * @param {string} markdown - the markdown that is about to being converted to html
 * @param {bool} forExport - true if the html is used for an export, false for the preview
 * @return {string} the modified markdown or an empty string if nothing should be modified
 */
function preNoteToMarkdownHtmlHook(note, markdown, forExport);
```

ربما تحب أن تلقي نظرة على المثال [preview-styling.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/preview-styling.qml).

## noteToMarkdownHtmlHook (خطاف تحويل الملاحظة من ماركداون إلى HTML)

### نداء الدالة ومُعامِلاتها

```js
/**
 * This function is called when the markdown html of a note is generated
 *
 * It allows you to modify this html
 * This is for example called before by the note preview
 *
 * The function can be used in multiple scripts to modify the html of the preview
 *
 * @param {NoteApi} note - the note object
 * @param {string} html - the html that is about to being rendered
 * @param {bool} forExport - true if the html is used for an export, false for the preview
 * @return {string} the modified html or an empty string if nothing should be modified
 */
function noteToMarkdownHtmlHook(note, html, forExport);
```

ربما تحب أن تلقي نظرة على المثالين [example.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/example.qml) أو [preview-styling.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/preview-styling.qml).

برجاء الاطلاع على توثيق [الجزء المدعوم من HTML](http://doc.qt.io/qt-5/richtext-html-subset.html) لقائمة بجميع خصائص CSS المدعومة.

## encryptionHook (خطاف التعمية)

### نداء الدالة ومُعامِلاتها

```js
/**
 * تُنادى هذه الدالة عند الاحتياج إلى تعمية نص أو فك تعميته
 *
 * @param text
 *       سلسلة نصية: النص الذي سيُعمّى أو ستُفك تعميته
 * @param password
 *       سلسلة نصية: كلمة المرور
 * @param decrypt bool if false encryption is demanded, if true decryption is demanded
 *       قيمة منطقية: «صادق» إذا كان المطلوب فك التعمية، و«كاذب» إذا كان المطلوب التعمية
 * @return
 *       سلسلة نصية: النص المعّمى أو المفكوكة تعميته
 */
function encryptionHook(text, password, decrypt);
```

ربما تحب أن تلقي نظرة على الأمثلة [encryption-keybase.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/encryption-keybase.qml) أو [encryption-pgp.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/encryption-pgp.qml) أو [encryption-rot13.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/encryption-rot13.qml).

## noteTaggingHook (خطاف وسم الملاحظات)

يمكنك تنفيذ آلية وسم خاصة بك، مثلا باستخدام نص بصيغة خاصة في ملاحظاتك، مثل `@tag1` و&nbsp;`@tag2` و&nbsp;`@tag3`.

### نداء الدالة ومُعامِلاتها

```js
/**
 * Handles note tagging for a note
 *
 * This function is called when tags are added to, removed from or renamed in
 * a note or the tags of a note should be listed
 *
 * @param note
 * @param action can be "add", "remove", "rename" or "list"
 * @param tagName tag name to be added, removed or renamed
 * @param newTagName tag name to be renamed to if action = "rename"
 * @return note text string or string-list of tag names (if action = "list")
 */
function noteTaggingHook(note, action, tagName, newTagName);
```

- as soon as a script is activated that implements the new function `noteTaggingHook` note tagging will be handled by that function
- ستعمل الخصائص التالية من خلال واجهة مستخدم QOwnNotes
  - initially importing tags like `@tag` from your notes and overwriting your current tag assignment
    - you will not lose your tags tree, just the former assignment to notes
    - ستظل قادرًا على نقل الوسوم إلى وسوم أخرى
    - if more than one tag has the same name in your tag tree the first hit will be assigned
  - إضافة وسم إلى ملاحظة سيضيف الوسم إلى نص الملاحظة
  - إزالة وسم من ملاحظة سيزيل الوسم من نص الملاحظة
  - removing of tags in the tag list will remove those tags from your notes
  - renaming of tags in the tag list will rename those tags in your notes
  - bulk tagging of notes in the note list will add those tags to your notes
  - bulk removing of tags from notes in the note list will remove those tags from your notes
  - the application will trigger a series of `add` and `remove` actions for all selected tags and their children on all notes if tags are moved in the tag panel

ربما تحب أن تلقي نظرة على المثال [note-tagging.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/note-tagging.qml) لتنفيذ آلية وسم خاصة بك.

::: warning
Make sure your `list` action is really fast, because it will be executed for every note every time the note folder or the tag tree is reloaded!

That is every time you find `reloadTagTree` in [mainwindow.cpp](https://github.com/pbek/QOwnNotes/blob/main/src/mainwindow.cpp).
:::

## noteTaggingByObjectHook (خطاف وسم الملاحظات الكائني)

مثل [خطاف وسم الملاحظات (noteTaggingHook)](#notetagginghook)، يمكنك تنفيذ آلية وسم خاصة بك، لكنك لست مربوطًا بأسماء وسوم في جذر شجرة الوسوم. يمكنك بهذه الطريقة استخدام شجرة الوسوم كلها بدلا من قائمة وسوم فقط.

تحصل باستخدامك `خطاف وسم الملاحظات الكائني (noteTaggingByObjectHook)` على كائن `TagApi` كمُعامِل، بدلا من اسم وسم. وعليك توفير قائمة مُعرِّفات الوسوم من أجل نتيجة إجراء السرد (`list`).

يعني هذا أيضا أن عليك إنشاء الوسوم المفقودة بنفسك حتى تتمكن من توفير قائمة بمُعرِّفات الوسوم الموجودة بالفعل من أجل إجراء السرد.

### نداء الدالة ومُعامِلاتها

```js
/**
 * Handles note tagging for a note
 *
 * This function is called when tags are added to, removed from or renamed in
 * a note or the tags of a note should be listed
 *
 * @param note
 * @param action can be "add", "remove", "rename" or "list"
 * @param tag to be added, removed or renamed
 * @param newTagName tag name to be renamed to if action = "rename"
 * @return note text string or string-list of tag ids (if action = "list")
 */
function noteTaggingByObjectHook(note, action, tag, newTagName);
```

ربما تحب أن تلقي نظرة على المثال [note-tagging-by-object.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/note-tagging-by-object.qml) لتنفيذ آلية وسم خاصة بك.

## autocompletionHook (خطاف الإكمال التلقائي)

يمكنك إرجاع قائمة سلاسل نصية لإضافتها إلى قائمة الإكمال التلقائي عندما يُستدعى الإكمال التلقائي (مثلا بضغط <kbd>Ctrl</kbd> + <kbd>Space</kbd>).

### نداء الدالة ومُعامِلاتها

```js
/**
 * Calls the autocompletionHook function for all script components
 * This function is called when autocompletion is invoked in a note
 *
 * @return QStringList of text for the autocomplete list
 */
function callAutocompletionHook();
```

ربما تحب أن تلقي نظرة على المثال [autocompletion.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/autocompletion.qml).

## websocketRawDataHook (خطاف البيانات الخام من مقبس الويب)

يُنادى هذا الخطاف عندما تُرسل بيانات من إضافة متصفح رفيقة ويب QOwnNotes من خلال قائمة سياق المتصفح.

### نداء الدالة ومُعامِلاتها

```js
/**
 * @param requestType can be "page" or "selection"
 * @param pageUrl the url of the webpage where the request was made
 * @param pageTitle the page title of the webpage where the request was made
 * @param rawData the data that was transmitted, html for requestType "page" or plain text for requestType "selection"
 * @param screenshotDataUrl the data url of the screenshot of the webpage where the request was made
 * @return true if data was handled by a hook
 */
function callHandleWebsocketRawDataHook(requestType, pageUrl, pageTitle, rawData, screenshotDataUrl);
```

ربما تحب أن تلقي نظرة على المثالين [websocket-raw-data-new-note.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/websocket-raw-data-new-note.qml) و&nbsp;[websocket-raw-data-selection-in-note.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/websocket-raw-data-selection-in-note.qml).

## onDetachedProcessCallback (عند رد عملية منفصلة)

يُنادى هذا الخطاف عندما ينتهي تنفيذ عملية منفصلة بدأها بُريمج في خيط تنفيذ بالدالة [startDetachedProcess](methods-and-objects.html#starting-an-external-program-in-the-background).

### نداء الدالة ومُعامِلاتها

```js
/**
 * This function is called when a script thread is done executing.
 * Hint: thread[1]==0 helps to determine if a bulk of started processes for a certain identifier is done.
 *
 * @param {QString} callbackIdentifier - the provided id when calling startDetachedProcess()
 * @param {QString} resultSet - the result of the process
 * @param {QVariantList} cmd - the entire command array [0-executablePath, 1-parameters, 2-exitCode]
 * @param {QVariantList} thread - the thread information array [0-passed callbackParameter, 1-remaining threads for this identifier]
 */
function onDetachedProcessCallback(callbackIdentifier, resultSet, cmd, thread);
```

ربما تحب أن تلقي نظرة على المثال [callback-example.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/callback.qml).

## windowStateChangedHook (خطاف تغيّر حالة النافذة)

### نداء الدالة ومُعامِلاتها

```js
/**
 * تُنادى هذه الدالة عند حدث تغيير حالة النافذة
 *
 * @param {QString} windowState
 *       الحالة الجديدة للنافذة؛ القيم المحتملة هي
 *       "minimized", "maximized", "fullscreen", "active", "nostate"
 */
function windowStateChangedHook(windowState);
```

ربما تحب أن تلقي نظرة على المثال [window-state-changed.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/window-state-changed.qml).

## workspaceSwitchedHook (خطاف تبديل مساحة العمل)

يُنادى هذا الخطاف عند تبديل مساحة العمل.

### نداء الدالة ومُعامِلاتها

```js
/**
 * تُنادى هذه الدالة عند تبديل مساحات العمل
 *
 * @param oldUuid
 *       المعرف العالمي الفريد لمساحة العمل القديمة
 * @param newUuid
 *       المعرف العالمي الفريد لمساحة العمل الجديدة
 */
function workspaceSwitchedHook(oldUuid, newUuid);
```

ربما تحب أن تلقي نظرة على المثال [websocket-raw-data-new-note.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/workspaces.qml).

## openAiBackendsHook (خطاف خدمات الذكاء المصطنع)

يُنادى هذا الخطاف عند إعادة تحميل تهيئة خدمات OpenAI. يحدث هذا مثلا عند إعادة تحميل محرك البرمجة.

You can use it to provide config for custom OpenAI backends, like your own OpenAI API compatible LLMs, for example ChatGPT, Claude or systems like Ollama.

### نداء الدالة ومُعامِلاتها

```js
/**
 * This function is called when the OpenAI service config is reloaded
 * It returns a list of objects with config parameters for new OpenAI backends
 */
function openAiBackendsHook() {
  return [
    {
      id: "my-custom-ai",
      name: "My Custom AI",
      baseUrl: "http://localhost:5000",
      apiKey: "kDFJkjk3asdm",
      models: ["gpt-3.5-turbo", "gpt-4.0-turbo"],
    },
    {
      id: "my-custom-ai2",
      name: "My Custom AI 2",
      baseUrl: "http://localhost:5001",
      apiKey: "lOikf7eNdb9",
      models: ["gpt-3.5-turbo2", "gpt-4.0-turbo2"],
    },
  ];
}
```

::: tip
يمكنك الاستفادة من [تسجيل متغيرات إعدادات البُريمج](./methods-and-objects.md#registering-script-settings-variables) لحفظ إعدادات خدمات الذكاء المصطنع ضمن إعدادات البريمج.
:::

ربما تحب أن تلقي نظرة على المثال [custom-openai-backends.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-openai-backends.qml).
