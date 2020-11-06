# Hooks

onNoteStored
------------

### Methodenaufruf und Parameter
```js
/ **
  * Diese Funktion wird aufgerufen, wenn eine Mediendatei in die Notiz eingefügt wird
  * Wenn diese Funktion in mehreren Skripten definiert ist, gewinnt das erste Skript, das eine nicht leere Zeichenfolge zurückgibt
  * *
  * @param fileName string Der Dateipfad der Quellmediendatei, bevor sie in den Medienordner kopiert wurde
  * @param mediaMarkdownText Zeichenfolge der Markdown-Text der Mediendatei, z. ! [mein-Bild] (Datei: //media/505671508.jpg)
  * @return string der neue Markdown-Text der Mediendatei
  * /
Funktion insertMediaHook (Dateiname, mediaMarkdownText);
```

Vielleicht möchten Sie sich das Beispiel ansehen [on-note-opened.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/on-note-opened.qml).

noteOpenedHook
--------------

### Methodenaufruf und Parameter
```js
/ **
  * Diese Funktion wird aufgerufen, nachdem eine Notiz geöffnet wurde
  * *
  * @param {NoteApi} note - das geöffnete Notizobjekt
  * /
Funktion noteOpenedHook (note);
```

Vielleicht möchten Sie sich das Beispiel ansehen [on-note-opened.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/on-note-opened.qml).

noteDoubleClickedHook
---------------------

### Methodenaufruf und Parameter
```js
/ **
  * Diese Funktion wird aufgerufen, nachdem auf eine Notiz doppelklickt wurde
  * *
  * @param {NoteApi} note - das Notizobjekt, auf das geklickt wurde
  * /
Funktion noteDoubleClickedHook (note);
```

Vielleicht möchten Sie sich das Beispiel ansehen [external-note-open.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/external-note-open.qml).

insertMediaHook
---------------

This function is called when a media file is inserted into the current note.

If this function is defined in multiple scripts, then the first script that returns a non-empty string wins.

### Methodenaufruf und Parameter
```js
/**
 * @param fileName string the file path of the source media file before it was copied to the media folder
 * @param markdownText string the markdown text of the media file, e.g. ![my-image](media/my-image-4101461585.jpg)
 * @return string the new markdown text of the media file
 */
function insertMediaHook(fileName, markdownText);
```

You may want to take a look at the example [example.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/example.qml).

insertAttachmentHook
--------------------

This function is called when an attachment file is inserted into the current note.

If this function is defined in multiple scripts, then the first script that returns a non-empty string wins.

### Methodenaufruf und Parameter
```js
/**
 * @param fileName string the file path of the source attachment file before it was copied to the attachment folder
 * @param markdownText string the markdown text of the attachment file, e.g. [my-file.txt](attachments/my-file-4245650967.txt)
 * @return string the new markdown text of the attachment file
 */
function insertAttachmentHook(fileName, markdownText);
```

You may want to take a look at the example [example.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/example.qml).

insertingFromMimeDataHook
-------------------------

### Methodenaufruf und Parameter
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

You may want to take a look at the example [example.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/example.qml), [insert-headline-with-link-from-github-url.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/insert-headline-with-link-from-github-url.qml) or [note-text-from-5pm-mail.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/note-text-from-5pm-mail.qml).

handleNoteTextFileNameHook
--------------------------

### Methodenaufruf und Parameter
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

You may want to take a look at the example [example.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/example.qml) or [use-tag-names-in-filename.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/use-tag-names-in-filename.qml).

handleNoteNameHook
------------------

### Methodenaufruf und Parameter
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

You may want to take a look at the example [example.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/example.qml).

It may not be a good idea to use this hook if the setting to use the file name as note name is active.

handleNewNoteHeadlineHook
-------------------------

### Methodenaufruf und Parameter
```js
/**
 * This function is called before a note note is created
 *
 * It allows you to modify the headline of the note before it is created
 * Note that you have to take care about a unique note name, otherwise
 * the new note will not be created, it will just be found in the note list
 *
 * You can use this method for creating note templates
 *
 * @param headline text that would be used to create the headline
 * @return {string} the headline of the note
 */
function handleNewNoteHeadlineHook(headline);
```

You may want to take a look at the example [custom-new-note-headline.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-new-note-headline.qml).

preNoteToMarkdownHtmlHook
-------------------------

### Methodenaufruf und Parameter
```js
/**
 * This function is called before the markdown html of a note is generated
 *
 * It allows you to modify what is passed to the markdown to html converter
 *
 * The method can for example be used in multiple scripts to render code (like LaTeX math or mermaid)
 * to its graphical representation for the preview
 *
 * The note will not be changed in this process
 *
 * @param {NoteApi} note - the note object
 * @param {string} markdown - the markdown that is about to being converted to html
 * @param {string} forExport - true if the html is used for an export, false for the preview
 * @return {string} the modified markdown or an empty string if nothing should be modified
 */
function preNoteToMarkdownHtmlHook(note, markdown, forExport);
```

You may want to take a look at the example [preview-styling.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/preview-styling.qml).

noteToMarkdownHtmlHook
----------------------

### Methodenaufruf und Parameter
```js
/**
 * This function is called when the markdown html of a note is generated
 *
 * It allows you to modify this html
 * This is for example called before by the note preview
 *
 * The method can be used in multiple scripts to modify the html of the preview
 *
 * @param {NoteApi} note - the note object
 * @param {string} html - the html that is about to being rendered
 * @param {string} forExport - true if the html is used for an export, false for the preview
 * @return {string} the modified html or an empty string if nothing should be modified
 */
function noteToMarkdownHtmlHook(note, html, forExport);
```

You may want to take a look at the example [example.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/example.qml) or [preview-styling.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/preview-styling.qml).

Please refer to the [Supported HTML Subset](http://doc.qt.io/qt-5/richtext-html-subset.html) documentation for a list of all supported css styles.

encryptionHook
--------------

### Methodenaufruf und Parameter
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

You may want to take a look at the example [encryption-keybase.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/encryption-keybase.qml), [encryption-pgp.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/encryption-pgp.qml) or [encryption-rot13.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/encryption-rot13.qml).

noteTaggingHook
---------------

You can implement your own note tagging mechanism for example with special text in your note like `@tag1`, `@tag2`, `@tag3`.

### Method call and parameters
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

-   Sobald ein Skript aktiviert ist, das die neue Funktion ` noteTaggingHook </ 0> implementiert, wird das Note-Tagging von dieser Funktion verarbeitet</li>
<li><p spaces-before="0">  Die folgenden Funktionen sollten über die QOwnNotes-Benutzeroberfläche funktionieren</p>

<ul>
<li><p spaces-before="0">  Importieren Sie zunächst Tags wie <code> @tag ` aus Ihren Notizen und überschreiben Sie Ihre aktuelle Tag-Zuweisung</p>
        -   Sie werden Ihren Tag-Baum nicht verlieren, sondern nur die frühere Zuordnung zu Notizen
        -   Sie können Tags weiterhin in andere Tags verschieben
        -   Wenn mehr als ein Tag denselben Namen in Ihrem Tag-Baum hat, wird der erste Treffer zugewiesen
    -   Durch Hinzufügen eines Tags zu einer Notiz wird das Tag zum Notiztext hinzugefügt
    -   Durch Entfernen eines Tags aus einer Notiz wird das Tag aus dem Notiztext entfernt
    -   removing of tags in the tag list will remove those tags from your notes
    -   renaming of tags in the tag list will rename those tags in your notes
    -   bulk tagging of notes in the note list will add those tags to your notes
    -   bulk removing of tags from notes in the note list will remove those tags from your notes
    -   the application will trigger a series of `add` and `remove` actions for all selected tags and their children on all notes if tags are moved in the tag panel</li> </ul>

You may want to take a look at the example [note-tagging.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/note-tagging.qml) to implement your own tagging mechanism.

::: warning
Make sure your `list` action is really fast, because it will be executed for every note every time the note folder is reloaded!
:::

noteTaggingByObjectHook
----------------------

Similarly to [noteTaggingHook](#notetagginghook) you can implement your own note tagging mechanism, but you are not bound to tag names in the tag tree root. This way you can make use of the whole tag tree instead of only a tag list.

With `noteTaggingByObjectHook` you get a `TagApi` object as parameter, instead of a tag name. And as result for the `list` action you need to provide a list of tag ids.

This also means you need to create missing tags yourself to be able to provide a list of already existing tag ids for the `list` action.

### Methodenaufruf und Parameter
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

You may want to take a look at the example [note-tagging-by-object.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/note-tagging-by-object.qml) to implement your own tagging mechanism.

autocompletionHook
------------------

You can return a list of strings to be added to the autocompletion list when the autocompletion is invoked (for example by pressing <kbd>Ctrl + Space</kbd>).

### Methodenaufruf und Parameter
```js
/**
 * Calls the autocompletionHook function for all script components
 * This function is called when autocompletion is invoked in a note
 *
 * @return QStringList of text for the autocomplete list
 */
function callAutocompletionHook();
```

You may want to take a look at the example [autocompletion.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/autocompletion.qml).

websocketRawDataHook
--------------------

This hook is called when data is sent from the QOwnNotes Web Companion browser extension via the web browser's context menu.

### Methodenaufruf und Parameter
```js
/**
 * @param requestType can be "page" or "selection"
 * @param pageUrl the url of the webpage where the request was made
 * @param pageTitle the page title of the webpage where the request was made
 * @param rawData the data that was transmitted, html for requestType "page" or plain text for requestType "selection"
 * @param screenshotDataUrl the data url of the screenshot if the webpage where the request was made
 * @return true if data was handled by a hook
 */
bool ScriptingService::callHandleWebsocketRawDataHook(
    const QString &requestType, const QString &pageUrl,
    const QString &pageTitle, const QString &rawData,
    const QString &screenshotDataUrl);
```

You may want to take a look at the examples [websocket-raw-data-new-note.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/websocket-raw-data-new-note.qml) and [websocket-raw-data-selection-in-note.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/websocket-raw-data-selection-in-note.qml).

onDetachedProcessCallback
-------------------------

This hook is called when a script thread of [startDetachedProcess](methods-and-objects.html#starting-an-external-program-in-the-background) is done executing.

### Method call and parameters
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
You may want to take a look at the example [callback-example.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/callback.qml).
