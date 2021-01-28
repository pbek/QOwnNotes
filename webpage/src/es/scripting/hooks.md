# Manos

onNoteStored
------------

### Parámetros y llamada al método
```js
/**
  * Esta función se llama cuando una nota se almacena en el disco
  * No puede modificar las notas almacenadas, eso sería un desastre ya que
  * lo más probable es que los esté editando a mano al mismo tiempo
  *
  * @param {NoteApi} nota: el objeto de nota de la nota almacenada
  */
función onNoteStored (nota);
```

Es posible que desee echar un vistazo al ejemplo [on-note-open.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/on-note-opened.qml).

noteOpenedHook
--------------

### Parámetros y llamada al método
```js
/**
  * Esta función se llama después de que se abrió una nota
  *
  * @param {NoteApi} note: el objeto de nota que se abrió
 */
function noteOpenedHook(note);
```

Es posible que desee echar un vistazo al ejemplo [on-note-open.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/on-note-opened.qml).

noteDoubleClickedHook
---------------------

### Parámetros y llamada al método
```js
/**
  * Esta función se llama después de hacer doble clic en una nota
  *
  * @param {NoteApi} nota: el objeto de nota en el que se hizo clic
  */
function noteDoubleClickedHook(note);
```

Es posible que desee echar un vistazo al ejemplo [external-note-open.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/external-note-open.qml).

insertMediaHook
---------------

Esta función se llama cuando se inserta un archivo multimedia en la nota actual.

Si esta función se define en varios scripts, entonces gana el primer script que devuelve una cadena no vacía.

### Parámetros y llamada al método
```js
/**
 * @param fileName string the file path of the source media file before it was copied to the media folder
 * @param markdownText string the markdown text of the media file, e.g. ![my-image](media/my-image-4101461585.jpg)
 * @return string the new markdown text of the media file
 */
function insertMediaHook(fileName, markdownText);
```

Es posible que desee echar un vistazo al ejemplo [example.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/example.qml).

insertAttachmentHook
--------------------

Esta función se llama cuando se inserta un archivo adjunto en la nota actual.

Si esta función se define en varios scripts, entonces gana el primer script que devuelve una cadena no vacía.

### Parámetros y llamada al método
```js
/**
 * @param fileName string the file path of the source attachment file before it was copied to the attachment folder
 * @param markdownText string the markdown text of the attachment file, e.g. [my-file.txt](attachments/my-file-4245650967.txt)
 * @return string the new markdown text of the attachment file
 */
function insertAttachmentHook(fileName, markdownText);
```

Es posible que desee echar un vistazo al ejemplo [ejemplo.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/example.qml).

insertingFromMimeDataHook
-------------------------

### Parámetros y llamada al método
```js
/**
  * Esta función se llama cuando se pega html o un archivo multimedia en una nota con `Ctrl + Shift + V`
  *
  * @param texto texto del objeto QMimeData
  * @param html html del objeto QMimeData
  * @ devuelve la cadena que se debe insertar en lugar del texto del objeto QMimeData
 */
function insertingFromMimeDataHook(text, html);

```

Es posible que desee echar un vistazo al ejemplo [ejemplo.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/example.qml), [insertar-título-con-enlace-de-github-url.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/insert-headline-with-link-from-github-url.qml) o [note-text-from-5pm-mail.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/note-text-from-5pm-mail.qml).

handleNoteTextFileNameHook
--------------------------

### Parámetros y llamada al método
```js
/**
  * Esta función se llama cuando una nota se almacena en el disco si
  * "Permitir que el nombre del archivo de nota sea diferente del título" está habilitado
  * en la configuración
  *
  * Te permite modificar el nombre del archivo de notas
  * ¡Tenga en cuenta que usted mismo debe preocuparse por los nombres duplicados!
 *
  * Devuelve una cadena vacía si el nombre del archivo de la nota debe
  * no modificarse
*
  * @param {NoteApi} nota: el objeto de nota de la nota almacenada
  * @return {string} el nombre del archivo de la nota
  */
function handleNoteTextFileNameHook (nota);
```

Es posible que desee echar un vistazo al ejemplo [ejemplo.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/example.qml) o [use-tag-names-in-filename.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/use-tag-names-in-filename.qml).

handleNoteNameHook
------------------

### Method call and parameters
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

### Method call and parameters
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

### Method call and parameters
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

### Method call and parameters
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

### Method call and parameters
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

-   as soon as a script is activated that implements the new function `noteTaggingHook` note tagging will be handled by that function
-   following features should work via the QOwnNotes user interface
    -   initially importing tags like `@tag` from your notes and overwriting your current tag assignment
        -   you will not loose your tags tree, just the former assignment to notes
        -   you can still move tags into other tags
        -   if more than one tag has the same name in your tag tree the first hit will be assigned
    -   adding a tag to a note will add the tag to the note text
    -   removing a tag from a note will remove the tag from the note text
    -   removing of tags in the tag list will remove those tags from your notes
    -   renaming of tags in the tag list will rename those tags in your notes
    -   bulk tagging of notes in the note list will add those tags to your notes
    -   bulk removing of tags from notes in the note list will remove those tags from your notes
    -   the application will trigger a series of `add` and `remove` actions for all selected tags and their children on all notes if tags are moved in the tag panel

You may want to take a look at the example [note-tagging.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/note-tagging.qml) to implement your own tagging mechanism.

::: warning
Make sure your `list` action is really fast, because it will be executed for every note every time the note folder is reloaded!
:::

noteTaggingByObjectHook
----------------------

Similarly to [noteTaggingHook](#notetagginghook) you can implement your own note tagging mechanism, but you are not bound to tag names in the tag tree root. This way you can make use of the whole tag tree instead of only a tag list.

With `noteTaggingByObjectHook` you get a `TagApi` object as parameter, instead of a tag name. And as result for the `list` action you need to provide a list of tag ids.

This also means you need to create missing tags yourself to be able to provide a list of already existing tag ids for the `list` action.

### Parámetros y llamada al método
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

### Parámetros y llamada al método
```js
/**
 * Calls the autocompletionHook function for all script components
 * This function is called when autocompletion is invoked in a note
 *
 * @return QStringList of text for the autocomplete list
 */
function callAutocompletionHook();
```

Es posible que desee echar un vistazo al ejemplo [ autocompletado.qml ](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/autocompletion.qml).

websocketRawDataHook
--------------------

Este enlace se llama cuando se envían datos desde la extensión del navegador QOwnNotes Web Companion a través del menú contextual del navegador web.

### Parámetros y llamada al método
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

Es posible que desee echar un vistazo a los ejemplos [websocket-raw-data-new-note.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/websocket-raw-data-new-note.qml) y [websocket-raw-data-selection-in-note.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/websocket-raw-data-selection-in-note.qml).

onDetachedProcessCallback
-------------------------

Este gancho se llama cuando un hilo de script de [startDetachedProcess](methods-and-objects.html#starting-an-external-program-in-the-background) termina de ejecutarse.

### Parámetros y llamada al método
```js
/**
  * Esta función se llama cuando se termina de ejecutar un hilo de script.
 * Hint: thread[1]==0 helps to determine if a bulk of started processes for a certain identifier is done.
 *
 * @param {QString} callbackIdentifier - the provided id when calling startDetachedProcess()
 * @param {QString} resultSet - the result of the process
 * @param {QVariantList} cmd - the entire command array [0-executablePath, 1-parameters, 2-exitCode]
 * @param {QVariantList} thread - the thread information array [0-passed callbackParameter, 1-remaining threads for this identifier]
 */
function onDetachedProcessCallback(callbackIdentifier, resultSet, cmd, thread);
```

Es posible que desee echar un vistazo al ejemplo [callback-example.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/callback.qml).

windowStateChangedHook
--------------

### Parámetros y llamada al método
```js
/**
 * This function is called after a WindowStateChange event was triggered
 *
 * @param {QString} windowState - the new window state, parameter value can be "minimized", "maximized", "fullscreen", "active" or "nostate"
 */
function windowStateChangedHook(windowState);
```

Es posible que desee echar un vistazo al ejemplo [window-state-changed.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/window-state-changed.qml).
