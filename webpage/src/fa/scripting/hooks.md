# هوک ها

onNoteStored
------------

### فراخوانی شگرد و پارامترها
```js
/**
 * This function is called when a note gets stored to disk
 * You cannot modify stored notes, that would be a mess since
 * you are most likely editing them by hand at the same time
 *
 * @param {NoteApi} note - the note object of the stored note
 */
function onNoteStored(note);
```

شاید بخواهید به مثال [on-note-opened.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/on-note-opened.qml) نگاهی بیندازید.

noteOpenedHook
--------------

### فراخوانی شگرد و پارامترها
```js
/**
 * This function is called after a note was opened
 *
 * @param {NoteApi} note - the note object that was opened
 */
function noteOpenedHook(note);
```

برای نمونه می توانید به [on-note-opened.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/on-note-opened.qml) نگاهی بیندازید.

noteDoubleClickedHook
---------------------

### فراخوانی شگرد و پارامترها
```js
/**
 * This function is called after a note was double clicked
 *
 * @param {NoteApi} note - the note object that was clicked
 */
function noteDoubleClickedHook(note);
```

به عنوان مثال می توانید به [external-note-open.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/external-note-open.qml) نگاهی بیندازید.

insertMediaHook
---------------

این تابع هنگامی فراخوانی می شود که یک پرونده رسانه در یادداشت کنونی درج شده باشد.

اگر این تابع در چندین اسکریپ تعریف شود، در این صورت اسکریپت نخست که یک رشته غیر خالی را بر می گرداند، برنده می شود.

### فراخوانی شگرد و پارامترها
```js
/**
 * @param fileName string the file path of the source media file before it was copied to the media folder
 * @param markdownText string the markdown text of the media file, e.g. ![my-image](media/my-image-4101461585.jpg)
 * @return string the new markdown text of the media file
 */
function insertMediaHook(fileName, markdownText);
```

شاید بخواهید به مثال [example.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/example.qml) نگاهی بیندازید.

insertAttachmentHook
--------------------

این تابع هنگامی فراخوانی می شود که یک پرونده پیوست در یادداشت کنونی درج شده باشد.

اگر این تابع در چندین اسکریپ تعریف شود، در این صورت اسکریپت نخست که یک رشته غیر خالی را بر می گرداند، برنده می شود.

### فراخوانی شگرد و پارامترها
```js
/**
 * @param fileName string the file path of the source attachment file before it was copied to the attachment folder
 * @param markdownText string the markdown text of the attachment file, e.g. [my-file.txt](attachments/my-file-4245650967.txt)
 * @return string the new markdown text of the attachment file
 */
function insertAttachmentHook(fileName, markdownText);
```

به عنوان مثال می توانید [example.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/example.qml) نگاهی بیندازید.

insertingFromMimeDataHook
-------------------------

### فراخوانی شگرد و پارامترها
```js
/**
 * This function is called when html or a media file is pasted to a note with `Ctrl + Shift + V`
 *
 * @param text text of the QMimeData object
 * @param html html of the QMimeData object
 * @returns the string that should be inserted instead of the text from the QMimeData object
 */
function insertingFromMimeDataHook(text, html);
```

شاید بخواهید نگاهی به مثال [example.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/example.qml)، [insert-headline-with-link-from-github-url.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/insert-headline-with-link-from-github-url.qml) یا [note-text-from-5pm-mail.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/note-text-from-5pm-mail.qml) بیندازید.

handleNoteTextFileNameHook
--------------------------

### فراخوانی شگرد و پارامترها
```js
/**
 * This function is called when a note gets stored to disk if
 * "Allow note file name to be different from headline" is enabled
 * in the settings
 *
 * It allows you to modify the name of the note file
 * Keep in mind that you have to care about duplicate names yourself!
 *
 * Return an empty string if the file name of the note should
 * not be modified
 *
 * @param {NoteApi} note - the note object of the stored note
 * @return {string} the file name of the note
 */
function handleNoteTextFileNameHook(note);
```

در صورت تمایل می توانید مثال [example.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/example.qml) یا [use-tag-names-in-filename.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/use-tag-names-in-filename.qml) را ملاحظه کنید.

handleNoteNameHook
------------------

### فراخوانی شگرد و پارامترها
```js
/**
 * This function is called when the note name is determined for a note
 *
 * It allows you to modify the name of the note that is viewed
 *
 * Return an empty string if the name of the note should not be modified
 *
 * @param {NoteApi} note - the note object of the stored note
 * @return {string} the name of the note
 */
function handleNoteNameHook(note);
```

شاید بخواهید به مثال [example.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/example.qml) نگاهی بیندازید.

در صورتی که تنظیماتی که از نام پرونده به عنوان نام یادداشت استفاده می کند فعال باشد، استفاده از این هوک ایده خوبی به نظر نمی رسد.

handleNewNoteHeadlineHook
-------------------------

### فراخوانی شگرد و پارامترها
```js
/**
 * This function is called before a note is created
 *
 * It allows you to modify the headline of the note before it is created
 * Note that you have to take care about a unique note name, otherwise
 * the new note will not be created, it will just be found in the note list
 *
 * You can use this function for creating note templates
 *
 * @param headline text that would be used to create the headline
 * @return {string} the headline of the note
 */
function handleNewNoteHeadlineHook(headline);
```

شاید بخواهید به مثال [custom-new-note-headline.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-new-note-headline.qml) نگاهی بیندازید.

preNoteToMarkdownHtmlHook
-------------------------

### فراخوانی شگرد و پارامترها
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

شاید بخواهید به مثال [preview-styling.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/preview-styling.qml) نگاهی بیندازید.

noteToMarkdownHtmlHook
----------------------

### فراخوانی شگرد و پارامترها
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

در صورت تمایل می توانید نگاهی به مثال [example.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/example.qml) یا [preview-styling.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/preview-styling.qml) بیندازید.

لطفاً برای لیستی از همه سبک های پشتیبانی شده css به مستندات [زیرمجموعه پشیبانی شده HTML](http://doc.qt.io/qt-5/richtext-html-subset.html) مراجعه کنید.

encryptionHook
--------------

### فراخوانی شگرد و پارامترها
```js
/**
 * This function is called when text has to be encrypted or decrypted
 *
 * @param text string the text to encrypt or decrypt
 * @param password string the password
 * @param decrypt bool if false encryption is demanded, if true decryption is demanded
 * @return the encrypted decrypted text
 */
function encryptionHook(text, password, decrypt);
```

شاید بخواهید نگاهی به مثال [encryption-keybase.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/encryption-keybase.qml)، [encryption-pgp.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/encryption-pgp.qml) یا [encryption-rot13.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/encryption-rot13.qml) بیندازید.

noteTaggingHook
---------------

شما می توانید مکانیسم برچسب گذاری یادداشت خود را مثلاً با متن خاصی در یادداشت تان مانند  `@tag1`، `@tag2` یا  `@tag3` پیاده سازی کنید.

### فراخوانی شگرد و پارامترها
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

-   به محض اینکه اسکریپتی فعال می شود که تابع جدید `noteTaggingHook` را پیاده سازی می کند، برچسب گذاری یادداشت با آن تابع انجام می شود.
-   قابلیت های پیش رو بایستی از طریق رابط کاربری QOwnNotes فعالیت کند
    -   ورود اولیه برچسب هایی نظیر `@tag` از یادداشت شما و بازنویسی برچسب زنی کنونی شما
        -   شما ساختار درختی برچسب هایتان را از دست نخواهید داد (تنها تخصیص قبلی به یادداشت ها)
        -   همچنان می توانید برچسب ها را به سایر برچسب ها جابجا کنید
        -   اگر بیش از یک برچسب در ساختار درختی برچسب شما نام یکسانی داشته باشد، مورد نخست در نظر گرفته خواهد شد
    -   افزودن یک برچسب به یک یادداشت، برچسب را به متن یادداشت اضافه خواهد کرد
    -   حذف یک برچسب از یک یادداشت، برچسب را از متن یادداشت حذف خواهد کرد
    -   حذف برچسب ها در لیست برچسب، آن برچسب ها را از یادداشت های شما حذف خواهد کرد
    -   تغییر نام برچسب ها در لیست برچسب، آن برچسب ها را در یادداشت های شما تغییر نام خواهد داد
    -   برچسب گذاری انبوه یادداشت ها در لیست یادداشت آن برچسب ها را به یادداشت های شما اضافه خواهد کرد
    -   حذف انبوه برچسب ها از یادداشت ها در لیست یادداشت، آن برچسب ها را از یادداشت های شما حذف خواهد کرد
    -   در صورتی که برچسب ها در پنل برچسب جابجا ‌شوند، برنامه یکسری اقدامات `افزایش` و `حذف` برای همه برچسب های منتخب و فرزندان آن ها در تمام یادداشت ها انجام خواهد داد.

شاید بخواهید برای پیاده سازی مکانیسم برچسب گذاری یادداشت خود به مثال [note-tagging.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/note-tagging.qml) نگاهی بیندازید.

::: warning
مطمئن شوید که فعالیت `لیست` شما به واقع سریع است زیرا برای هر یادداشت و در هر زمانی که پوشه یادداشت مجدداً بارگذاری می شود، اجرا خواهد شد!
:::

noteTaggingByObjectHook
----------------------

به همین ترتیب برای [noteTaggingHook](#notetagginghook)، شما می توانید مکانیسم برچسب گذاری یادداشت خود را پیاده سازی کنید اما به اسامی برچسب در ریشه ساختار درختی برچسب متصل نیستید. از این رو می ‌توانید از کل ساختار درختی برچسب به جای تنها یک لیست برچسب استفاده کنید.

با `noteTaggingByObjectHook` به جای یک نام برچسب یک مقصد `TagApi` به عنوان پارامتر دریافت می کنید. علاوه بر این به عنوان نتیجه ای برای فعالیت `لیست`، باید لیستی از شناسه های برچسب تهیه کنید.

این همچنین به این معنی است که باید برچسب های از دست رفته را خودتان ایجاد کنید تا بتوانید لیستی از شناسه های برچسب موجود را برای فعالیت `لیست` فراهم کنید.

### فراخوانی شگرد و پارامترها
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

می توانید برای پیاده سازی مکانیسم برچسب گذاری یادداشت خود مثال [note-tagging-by-object.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/note-tagging-by-object.qml) را ملاحظه کنید.

autocompletionHook
------------------

هنگامی که تکمیل خودکار (برای مثال با فشردن <kbd>Ctrl + فاصله</kbd>) فراخوانی می شود، شما می توانید لیستی از رشته ها را برای افزودن به لیست تکمیل خودکار برگردانید.

### فراخوانی شگرد و پارامترها
```js
/**
 * Calls the autocompletionHook function for all script components
 * This function is called when autocompletion is invoked in a note
 *
 * @return QStringList of text for the autocomplete list
 */
function callAutocompletionHook();
```

شاید بخواهید به مثال [autocompletion.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/autocompletion.qml) نگاهی بیندازید.

websocketRawDataHook
--------------------

این هوک هنگامی فراخوانی می شود که داده ها از افزونه مرورگر همراه وب QOwnNotes از طریق منوی زمینه مرورگر وب ارسال شده باشد.

### فراخوانی شگرد و پارامترها
```js
/**
 * @param requestType can be "page" or "selection"
 * @param pageUrl the url of the webpage where the request was made
 * @param pageTitle the page title of the webpage where the request was made
 * @param rawData the data that was transmitted, html for requestType "page" or plain text for requestType "selection"
 * @param screenshotDataUrl the data url of the screenshot if the webpage where the request was made
 * @return true if data was handled by a hook
 */
function callHandleWebsocketRawDataHook(requestType, pageUrl, pageTitle, rawData, screenshotDataUrl);
```

شاید بخواهید نگاهی به مثال های [websocket-raw-data-new-note.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/websocket-raw-data-new-note.qml) و [websocket-raw-data-selection-in-note.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/websocket-raw-data-selection-in-note.qml) بیندازید.

onDetachedProcessCallback
-------------------------

این هوک زمانی فراخوانی می شود که بند اسکریپت [startDetachedProcess](methods-and-objects.html#starting-an-external-program-in-the-background) در حال اجرا شدن باشد.

### فراخوانی شگرد و پارامترها
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

شاید بخواهید مثال [callback-example.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/callback.qml) را ملاحظه کنید.

windowStateChangedHook
--------------

### فراخوانی شگرد و پارامترها
```js
/**
 * This function is called after a WindowStateChange event was triggered
 *
 * @param {QString} windowState - the new window state, parameter value can be "minimized", "maximized", "fullscreen", "active" or "nostate"
 */
function windowStateChangedHook(windowState);
```

شاید بخواهید به مثال [window-state-changed.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/window-state-changed.qml) نگاهی بیندازید.

workspaceSwitchedHook
----------------------

این هوک زمانی فراخوانده می شود که وضعیت محیط های کار تغییر کند.

### فراخوانی شگرد و پارامترها
```js
/**
 * This function is called when workspaces are switched
 *
 * @param oldUuid old uuid of workspace
 * @param newUuid new uuid of workspace
 */
function workspaceSwitchedHook(oldUuid, newUuid);
```

شاید بخواهید مثال [websocket-raw-data-new-note.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/workspaces.qml) را ملاحظه کنید.
