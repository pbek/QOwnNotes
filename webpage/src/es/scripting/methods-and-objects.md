# Métodos y objetos que QOwnNotes proporciona

Iniciar un programa externo en segundo plano
----------------------------------------------


### Parámetros y llamada al método
```cpp
/**
 * QML wrapper to start a detached process
 *
 * @param executablePath the path of the executable
 * @param parameters a list of parameter strings
 * @param callbackIdentifier an identifier to be used in the onDetachedProcessCallback() function (optional)
 * @param callbackParameter an additional parameter for loops or the like (optional)
 * @param processData data written to the process if the callback is used (optional)
 * @return true on success, false otherwise
 */
bool startDetachedProcess(QString executablePath, QStringList parameters,
                            QString callbackIdentifier, QVariant callbackParameter,
                            QByteArray processData);
```

### Ejemplo

Ejemplo simple:

```js
script.startDetachedProcess("/path/to/my/program", ["my parameter"]);
```

Ejecutando muchos procesos:

```js
for (var i = 0; i < 100; i++) {
    var dur = Math.floor(Math.random() * 10) + 1;
    script.startDetachedProcess("sleep", [`${dur}s`], "my-callback", i);
}

function onDetachedProcessCallback(callbackIdentifier, resultSet, cmd, thread) {
    if (callbackIdentifier == "my-callback") {
        script.log(`#${thread[1]} i[${thread[0]}] t${cmd[1]}`);
    }
}
```

Es posible que desee echar un vistazo al ejemplo [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml), [callback.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/callback.qml) or [execute-command-after-note-update.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/execute-command-after-note-update.qml).

También es posible que desee echar un vistazo al gancho [onDetachedProcessCallback](hooks.html#ondetachedprocesscallback).

::: tip
También puede asignar accesos directos locales y globales a sus acciones personalizadas en la *Configuración de accesos directos*.
:::

Iniciar un programa externo y esperar la salida
----------------------------------------------------

### Parámetros y llamada al método
```cpp
/**
 * QML wrapper to start a synchronous process
 *
 * @param executablePath the path of the executable
 * @param parameters a list of parameter strings
 * @param data the data that will be written to the process (optional)
 * @return the text that was returned by the process
QByteArray startSynchronousProcess(QString executablePath, QStringList parameters, QByteArray data);
```

### Ejemplo
```js
var result = script.startSynchronousProcess("/path/to/my/program", ["my parameter"], "data");
```

Es posible que desee echar un vistazo al ejemplo [encryption-keybase.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/encryption-keybase.qml).

Obtener la ruta de la carpeta de notas actual
-------------------------------------------

### Parámetros y llamada al método
```cpp
/**
 * QML wrapper to get the current note folder path
 *
 * @return the path of the current note folder
 */
QString currentNoteFolderPath();
```

### Ejemplo
```js
var path = script.currentNoteFolderPath();
```

Es posible que desee echar un vistazo al ejemplo [absolute-media-links.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/absolute-media-links.qml).

Obtener la nota actual
------------------------

### Parámetros y llamada al método
```cpp
/**
 * QML wrapper to get the current note
 *
 * @returns {NoteApi} the the current note object
 */
NoteApi currentNote();
```

### Ejemplo
```js
var note = script.currentNote();
```

Es posible que desee echar un vistazo al ejemplo [acciones-personalizadas.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml).

Iniciar sesión en el widget de registro
-------------------------

### Parámetros y llamada al método
```cpp
/**
 * QML wrapper to log to the log widget
 *
 * @param text
 */
void log(QString text);
```

### Ejemplo
```js
script.log("my text");
```

Descarga de una URL a una cadena
------------------------------

### Parámetros y llamada al método
```cpp
/**
  * Envoltorio QML para descargar una URL y devolverla como texto
  *
  * @param url
  * @return {QString} el contenido de la URL descargada
  */
QString downloadUrlToString(QUrl url);
```

### Ejemplo
```js
var html = script.downloadUrlToString("https://www.qownnotes.org");
```

Es posible que desee echar un vistazo al ejemplo [insertar-título-con-enlace-de-github-url.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/insert-headline-with-link-from-github-url.qml).

Descarga de una URL a la carpeta multimedia
--------------------------------------

### Parámetros y llamada al método
```cpp
/**
 * QML wrapper to download an url to the media folder and returning the media
 * url or the markdown image text of the media relative to the current note
 *
 * @param {QString} url
 * @param {bool} returnUrlOnly if true only the media url will be returned (default false)
 * @return {QString} the media markdown or url
 */
QString downloadUrlToMedia(QUrl url, bool returnUrlOnly);
```

### Ejemplo
```js
var markdown = script.downloadUrlToMedia("http://latex.codecogs.com/gif.latex?\frac{1}{1+sin(x)}");
```

Es posible que desee echar un vistazo al ejemplo [paste-latex-image.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/paste-latex-image.qml).

Insertar un archivo multimedia en la carpeta multimedia
--------------------------------------------

### Parámetros y llamada al método
```cpp
/**
 * QML wrapper to insert a media file into the media folder and returning
 * the media url or the markdown image text of the media  relative to the current note
 *
 * @param {QString} mediaFilePath
 * @param {bool} returnUrlOnly if true only the media url will be returned (default false)
 * @return {QString} the media markdown or url
 */
QString ScriptingService::insertMediaFile(QString mediaFilePath,
                                        bool returnUrlOnly);
```

### Ejemplo
```js
var markdown = script.insertMediaFile("/path/to/your/image.png");
```

Es posible que desee echar un vistazo al ejemplo [scribble.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/scribble.qml).

Regenerando la vista previa de la nota
-----------------------------

Actualiza la vista previa de la nota.

### Parámetros y llamada al método
```cpp
/**
 * Regenerates the note preview
 */
QString ScriptingService::regenerateNotePreview();
```

### Ejemplo
```js
script.regenerateNotePreview();
```

Es posible que desee echar un vistazo al ejemplo [scribble.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/scribble.qml).

Registro de una acción personalizada
---------------------------

### Parámetros y llamada al método
```cpp
/**
 * Registers a custom action
 *
 * @param identifier the identifier of the action
 * @param menuText the text shown in the menu
 * @param buttonText the text shown in the button
 *                   (no button will be viewed if empty)
 * @param icon the icon file path or the name of a freedesktop theme icon
 *             you will find a list of icons here:
 *             https://specifications.freedesktop.org/icon-naming-spec/icon-naming-spec-latest.html
 * @param useInNoteEditContextMenu if true use the action in the note edit
 *                                 context menu (default: false)
 * @param hideButtonInToolbar if true the button will not be shown in the
 *                            custom action toolbar (default: false)
 * @param useInNoteListContextMenu if true use the action in the note list
 *                                 context menu (default: false)
 */
void ScriptingService::registerCustomAction(QString identifier,
                                            QString menuText,
                                            QString buttonText,
                                            QString icon,
                                            bool useInNoteEditContextMenu,
                                            bool hideButtonInToolbar,
                                            bool useInNoteListContextMenu);
```

### Ejemplo
```js
// add a custom action without a button
script.registerCustomAction("mycustomaction1", "Menu text");

// add a custom action with a button
script.registerCustomAction("mycustomaction1", "Menu text", "Button text");

// add a custom action with a button and freedesktop theme icon
script.registerCustomAction("mycustomaction1", "Menu text", "Button text", "task-new");

// add a custom action with a button and an icon from a file
script.registerCustomAction("mycustomaction1", "Menu text", "Button text", "/usr/share/icons/breeze/actions/24/view-calendar-tasks.svg");
```

You may then want to use the identifier with function `customActionInvoked` in a script like [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml).

Registrar una etiqueta
-------------------

### Parámetros y llamada al método
```cpp
/**
 * Registers a label to write to
 *
 * @param identifier the identifier of the label
 * @param text the text shown in the label (optional)
 */
void ScriptingService::registerLabel(QString identifier, QString text);
```

### Ejemplo
```js
script.registerLabel("html-label", "<strong>Strong</strong> HTML text<br />with three lines<br />and a <a href='https://www.qownnotes.org'>link to a website</a>.");

script.registerLabel("long-label", "another very long text, another very long text, another very long text, another very long text, another very long text, another very long text, another very long text, another very long text, another very long text, another very long text, another very long text that will wrap");

script.registerLabel("counter-label");
```

Las etiquetas estarán visibles en el widget de la base de secuencias de comandos.

Puede utilizar texto sin formato o html en las etiquetas. Se podrá seleccionar el texto y se podrá hacer clic en los enlaces.

A continuación, es posible que desee echar un vistazo al script de ejemplo [scripting-label-demo.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/scripting-label-demo.qml).

Configurar el texto de una etiqueta registrada
--------------------------------------

### Parámetros y llamada al método
```cpp
/**
 * Sets the text of a registered label
 *
 * @param identifier the identifier of the label
 * @param text the text shown in the label
 */
void ScriptingService::setLabelText(QString identifier, QString text);
```

### Ejemplo
```js
script.setLabelText("counter-label", "counter text");
```

Puede utilizar texto sin formato o html en las etiquetas. Se podrá seleccionar el texto y se podrá hacer clic en los enlaces.

A continuación, es posible que desee echar un vistazo al script de ejemplo [scripting-label-demo.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/scripting-label-demo.qml).

Creando una nota nueva
-------------------

### Parámetros y llamada al método
```cpp
/**
 * Creates a new note
 *
 * @param text the note text
 */
void ScriptingService::createNote(QString text);
```

### Ejemplo
```js
script.createNote("My note headline\n===\n\nMy text");
```

Es posible que desee echar un vistazo al ejemplo [acciones-personalizadas.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml).

Accediendo al portapapeles
-----------------------

### Parámetros y llamada al método
```cpp
/**
 * Returns the content of the clipboard as text or html
 *
 * @param asHtml returns the clipboard content as html instead of text
 */
QString ScriptingService::clipboard(bool asHtml);
```

### Ejemplo
```js
var clipboardText = script.clipboard();
var clipboardHtml = script.clipboard(true);
```

Es posible que desee echar un vistazo al ejemplo [acciones-personalizadas.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml).

Escribir texto en la edición de texto de la nota
--------------------------------

### Parámetros y llamada al método
```cpp
/**
 * Writes text to the current cursor position in the note text edit
 *
 * @param text
 */
void ScriptingService::noteTextEditWrite(QString text);
```

### Ejemplo
```js
// write text to the note text edit
script.noteTextEditWrite("My custom text");
```

Es posible que desee ver la acción personalizada `transformTextRot13` en el ejemplo [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml).

Puede usar esto junto con `noteTextEditSelectAll` para sobrescribir todo el texto de la nota actual.

Leer el texto seleccionado en la edición de texto de la nota
--------------------------------------------

### Parámetros y llamada al método
```cpp
/**
 * Reads the selected text in the note text edit
 *
 * @return
 */
QString ScriptingService::noteTextEditSelectedText();
```

### Ejemplo
```js
// read the selected text from the note text edit
var text = script.noteTextEditSelectedText();
```

Es posible que desee ver la acción personalizada `transformTextRot13` en el ejemplo [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml).

Seleccionar todo el texto en la edición de texto de la nota
-------------------------------------

### Parámetros y llamada al método
```cpp
/**
 * Selects all text in the note text edit
 */
void ScriptingService::noteTextEditSelectAll();
```

### Ejemplo
```js
script.noteTextEditSelectAll();
```

Puede usar esto junto con `noteTextEditWrite` para sobrescribir todo el texto de la nota actual.

Seleccione la línea actual en la edición de texto de la nota
---------------------------------------------

### Parámetros y llamada al método
```cpp
/**
 * Selects the current line in the note text edit
 */
void ScriptingService::noteTextEditSelectCurrentLine();
```

### Ejemplo
```js
script.noteTextEditSelectCurrentLine();
```

Seleccione la palabra actual en la edición de texto de la nota
---------------------------------------------

### Parámetros y llamada al método
```cpp
/**
 * Selects the current line in the note text edit
 */
void ScriptingService::noteTextEditSelectCurrentWord();
```

### Ejemplo
```js
script.noteTextEditSelectCurrentWord();
```

Establecer el texto seleccionado actualmente en la edición de texto de nota
-----------------------------------------------------

### Parámetros y llamada al método
```cpp
/**
 * Sets the currently selected text in the note text edit
 *
 * @param start
 * @param end
 */
void ScriptingService::noteTextEditSetSelection(int start, int end);
```

### Ejemplo
```js
// expands the current selection by one character
script.noteTextEditSetSelection(
    script.noteTextEditSelectionStart() - 1,
    script.noteTextEditSelectionEnd() + 1);
```

Obtener la posición inicial de la selección actual en la edición de texto de la nota
---------------------------------------------------------------------

### Parámetros y llamada al método
```cpp
/**
 * Returns the start position of the current selection in the note text edit
 */
int ScriptingService::noteTextEditSelectionStart();
```

### Ejemplo
```js
script.log(script.noteTextEditSelectionStart());
```

Obtener la posición final de la selección actual en la edición de texto de la nota
-------------------------------------------------------------------

### Parámetros y llamada al método
```cpp
/**
  * Devuelve la posición final de la selección actual en la edición de texto de nota
  */
int ScriptingService::noteTextEditSelectionEnd();
```

### Ejemplo
```js
script.log(script.noteTextEditSelectionEnd());
```

Coloque el cursor de texto en la edición de texto de la nota en una posición determinada
---------------------------------------------------------------

### Parámetros y llamada al método
```cpp
/**
 * Sets the text cursor in the note text edit to a certain position
 * 0 would be the beginning of the note
 * special case: -1 would be the end of the note
 *
 * @param position
 */
void ScriptingService::noteTextEditSetCursorPosition(int position);
```

### Ejemplo
```js
// jump to the 11th character in the note
script.noteTextEditSetCursorPosition(10);

// jump to the end of the note
script.noteTextEditSetCursorPosition(-1);
```

Obtener la posición actual del cursor de texto en la edición de texto de la nota
-----------------------------------------------------------------

### Parámetros y llamada al método
```cpp
/**
 * Returns the current position of the text cursor in the note text edit
 * 0 would be the beginning of the note
 */
int ScriptingService::noteTextEditCursorPosition();
```

### Ejemplo
```js
script.log(script.noteTextEditCursorPosition());
```

Leer la palabra actual desde el editor de texto de notas
---------------------------------------------

### Parámetros y llamada al método
```cpp
/**
 * Reads the current word in the note text edit
 *
 * @param withPreviousCharacters also get more characters at the beginning
 *                               to get characters like "@" that are not
 *                               word-characters
 * @return
 */
QString ScriptingService::noteTextEditCurrentWord(bool withPreviousCharacters);
```

### Ejemplo
```js
// read the current word in the note text edit
var text = script.noteTextEditCurrentWord();
```

Es posible que desee echar un vistazo al ejemplo [ autocompletado.qml ](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/autocompletion.qml).

Compruebe si la plataforma es Linux, OS X o Windows
------------------------------------------------

### Parámetros y llamada al método
```cpp
bool ScriptingService::platformIsLinux();
bool ScriptingService::platformIsOSX();
bool ScriptingService::platformIsWindows();
```

### Ejemplo
```js
if (script.platformIsLinux()) {
    // only will be executed if under Linux
}
```

Etiquetar la nota actual
--------------------

### Parámetros y llamada al método
```cpp
/**
 * Tags the current note with a tag named tagName
 *
 * @param tagName
 */
void ScriptingService::tagCurrentNote(QString tagName);
```

### Ejemplo
```js
// add a "favorite" tag to the current note
script.tagCurrentNote("favorite");
```

Es posible que desee ver la acción personalizada `favoriteNote` en el ejemplo [favorite-note.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/favorite-note.qml).

Crear o recuperar una etiqueta por su nombre lista de ruta de navegación
-------------------------------------------------

### Parámetros y llamada al método
```cpp
/**
 * Fetches or creates a tag by its "breadcrumb list" of tag names
 * Element nameList[0] would be highest in the tree (with parentId: 0)
 *
 * @param nameList
 * @param createMissing {bool} if true (default) all missing tags will be created
 * @return TagApi object of deepest tag of the name breadcrumb list
 */
TagApi *ScriptingService::getTagByNameBreadcrumbList(
    const QStringList &nameList, bool createMissing);
```

### Ejemplo
```js
// creates all tags until the 3rd level and returns the tag object for
// tag "level3", which would look like that in the tag tree:
// level1 > level2 > level3
var tag = script.getTagByNameBreadcrumbList(["level1", "level2", "level3"]);
```

Buscar etiquetas por nombre
-----------------------

### Parámetros y llamada al método
```cpp
/**
 * Fetches all tags by doing a substring search on the name field
 *
 * @param name {QString} name to search for
 * @return {QStringList} list of tag names
 */
QStringList ScriptingService::searchTagsByName(QString name);
```

### Ejemplo
```js
// searches for all tags with the word game in it
var tags = script.searchTagsByName("game");
```

Es posible que desee echar un vistazo al ejemplo [ autocompletado.qml ](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/autocompletion.qml).

Buscar notas por texto de nota
-----------------------------

### Parámetros y llamada al método
```cpp
/**
 * Returns a list of note ids of all notes with a certain text in the note text
 *
 * Unfortunately there is no easy way to use a QList<NoteApi*> in QML, so we
 * can only transfer the note ids
 *
 * @return {QList<int>} list of note ids
 */
QList<int> ScriptingService::fetchNoteIdsByNoteTextPart(QString text);
```

### Ejemplo
```js
var noteIds = script.fetchNoteIdsByNoteTextPart("mytext");

noteIds.forEach(function (noteId){
    var note = script.fetchNoteById(noteId);

    // do something with the note
});
```

Es posible que desee echar un vistazo al ejemplo [unique-note-id.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/unique-note-id.qml).

Agregar una hoja de estilo personalizada
-----------------------

### Parámetros y llamada al método
```cpp
/**
 * Adds a custom stylesheet to the application
 *
 * @param stylesheet
 */
void ScriptingService::addStyleSheet(QString stylesheet);
```

### Ejemplo
```js
// make the text in the note list bigger
script.addStyleSheet("QTreeWidget#noteTreeWidget {font-size: 30px;}");
```

Es posible que desee echar un vistazo al ejemplo [hoja de estilo personalizada.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-stylesheet.qml).

Puede obtener los nombres de los objetos de los archivos `*.ui`, por ejemplo [ mainwindow.ui ](https://github.com/pbek/QOwnNotes/blob/develop/src/mainwindow.ui).

Eche un vistazo a [Referencia de hoja de estilo](http://doc.qt.io/qt-5/stylesheet-reference.html) para obtener una referencia de los estilos disponibles.

Si desea inyectar estilos en la vista previa html para modificar la forma en que se obtienen las notas, mire [notetomarkdownhtmlhook](hooks.html#notetomarkdownhtmlhook).

Recarga del motor de secuencias de comandos
------------------------------

### Parámetros y llamada al método
```cpp
/**
 * Reloads the scripting engine
 */
void ScriptingService::reloadScriptingEngine();
```

### Ejemplo
```js
// reload the scripting engine
script.reloadScriptingEngine();
```

Obteniendo una nota por su nombre de archivo
--------------------------------

### Parámetros y llamada al método
```cpp
/**
 * Fetches a note by its file name
 *
 * @param fileName string the file name of the note (mandatory)
 * @param noteSubFolderId integer id of the note subfolder
 * @return NoteApi*
 */
NoteApi* ScriptingService::fetchNoteByFileName(QString fileName,
                                                int noteSubFolderId);
```

### Ejemplo
```js
// fetch note by file name
script.fetchNoteByFileName("my note.md");
```

Obteniendo una nota por su identificación
-------------------------

### Parámetros y llamada al método
```cpp
/**
 * Fetches a note by its id
 *
 * @param id int the id of the note
 * @return NoteApi*
 */
NoteApi* ScriptingService::fetchNoteById(int id);
```

### Ejemplo
```js
// fetch note by id
script.fetchNoteById(243);
```

Es posible que desee echar un vistazo al ejemplo [exportar notas como uno-html.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/export-notes-as-one-html.qml).

Comprobando si una nota existe por su nombre de archivo
------------------------------------------

### Parámetros y llamada al método
```cpp
/**
 * Checks if a note file exists by its file name
 *
 * @param fileName string the file name of the note (mandatory)
 * @param ignoreNoteId integer id of a note to ignore in the check
 * @param noteSubFolderId integer id of the note subfolder
 * @return bool
 */
bool ScriptingService::noteExistsByFileName(QString fileName,
                                            int ignoreNoteId,
                                            int noteSubFolderId);
```

### Ejemplo
```js
// check if note exists, but ignore the id of "note"
script.noteExistsByFileName("my note.md", note.id);
```

You may want to take a look at the example [use-tag-names-in-filename.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/use-tag-names-in-filename.qml).

Copiar texto en el portapapeles
-------------------------------

### Parámetros y llamada al método
```cpp
/**
 * Copies text into the clipboard as plain text or html mime data
 *
 * @param text string text to put into the clipboard
 * @param asHtml bool if true the text will be set as html mime data
 */
void ScriptingService::setClipboardText(QString text, bool asHtml);
```

### Ejemplo
```js
// copy text to the clipboard
script.setClipboardText("text to copy");
```

Es posible que desee echar un vistazo al ejemplo [marcada-seleccionada-a-bbcode.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/selected-markdown-to-bbcode.qml).

Saltando a una nota
-----------------

### Parámetros y llamada al método
```cpp
/**
 * Sets the current note if the note is visible in the note list
 *
 * @param note NoteApi note to jump to
 */
void ScriptingService::setCurrentNote(NoteApi *note);
```

### Ejemplo
```js
// jump to the note
script.setCurrentNote(note);
```

Es posible que desee echar un vistazo al ejemplo [entrada-diario.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/journal-entry.qml).

Saltar a una subcarpeta de notas
---------------------------

### Parámetros y llamada al método
```cpp
/**
 * Jumps to a note subfolder
 *
 * @param noteSubFolderPath {QString} path of the subfolder, relative to the note folder
 * @param separator {QString} separator between parts of the path, default "/"
 * @return true if jump was successful
 */
bool ScriptingService::jumpToNoteSubFolder(const QString &noteSubFolderPath,
                                            QString separator);
```

### Ejemplo
```js
// jump to the note subfolder "a sub folder"
script.jumpToNoteSubFolder("a sub folder");

// jump to the note subfolder "sub" inside of "a sub folder"
script.jumpToNoteSubFolder("a sub folder/sub");
```

::: tip
Puede crear una nueva subcarpeta de notas en la subcarpeta actual llamando a [`mainWindow.createNewNoteSubFolder`](classes.html#example-2).
:::

Mostrar un cuadro de mensaje de información
----------------------------------

### Parámetros y llamada al método
```cpp
/**
 * Shows an information message box
 *
 * @param text
 * @param title (optional)
 */
void ScriptingService::informationMessageBox(QString text, QString title);
```

### Ejemplo
```js
// show a information message box
script.informationMessageBox("The text I want to show", "Some optional title");
```

Mostrando un cuadro de mensaje de pregunta
------------------------------

### Parámetros y llamada al método
```cpp
/**
 * Shows a question message box
 *
 * For information about buttons see:
 * https://doc.qt.io/qt-5/qmessagebox.html#StandardButton-enum
 *
 * @param text
 * @param title (optional)
 * @param buttons buttons that should be shown (optional)
 * @param defaultButton default button that will be selected (optional)
 * @return id of pressed button
 */
int ScriptingService::questionMessageBox(
        QString text, QString title, int buttons, int defaultButton);
```

### Ejemplo
```js
// show a question message box with an apply and a help button
// see: https://doc.qt.io/qt-5/qmessagebox.html#StandardButton-enum
var result = script.questionMessageBox(
    "The text I want to show", "Some optional title", 0x01000000|0x02000000, 0x02000000);
script.log(result);
```

Para obtener información sobre los botones, consulte [Botón estándar](https://doc.qt.io/qt-5/qmessagebox.html#StandardButton-enum).

Quizás también quieras echar un vistazo al ejemplo [input-dialogs.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/input-dialogs.qml).

Mostrar un cuadro de diálogo de archivo abierto
---------------------------

### Parámetros y llamada al método
```cpp
/**
 * Shows an open file dialog
 *
 * @param caption (optional)
 * @param dir (optional)
 * @param filter (optional)
 * @return QString
 */
QString ScriptingService::getOpenFileName(QString caption, QString dir,
                                            QString filter);
```

### Ejemplo
```js
// show an open file dialog
var fileName = script.getOpenFileName("Please select an image", "/home/user/images", "Images (*.png *.xpm *.jpg)");
```

Mostrar un cuadro de diálogo para guardar archivo
--------------------------

### Parámetros y llamada al método
```cpp
/**
 * Shows a save file dialog
 *
 * @param caption (optional)
 * @param dir (optional)
 * @param filter (optional)
 * @return QString
 */
QString ScriptingService::getSaveFileName(QString caption, QString dir,
                                            QString filter);
```

### Ejemplo
```js
// show a save file dialog
var fileName = script.getSaveFileName("Please select HTML file to save", "output.html", "HTML (*.html)");
```

Es posible que desee echar un vistazo al ejemplo [exportar notas como uno-html.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/export-notes-as-one-html.qml).

Registro de variables de configuración de script
-------------------------------------

Debe definir sus variables de configuración como propiedades en su script y registrarlas en una propiedad llamada `settingsVariables`.

Luego, el usuario puede establecer estas propiedades en la configuración del script.

### Ejemplo
```js
// you have to define your registered variables so you can access them later
property string myString;
property bool myBoolean;
property string myText;
property int myInt;
property string myFile;
property string mySelection;

// register your settings variables so the user can set them in the script settings
// use this property if you don't need
//
// unfortunately there is no QVariantHash in Qt, we only can use
// QVariantMap (that has no arbitrary ordering) or QVariantList (which at
// least can be ordered arbitrarily)
property variant settingsVariables: [
    {
        "identifier": "myString",
        "name": "I am a line edit",
        "description": "Please enter a valid string:",
        "type": "string",
        "default": "My default value",
    },
    {
        "identifier": "myBoolean",
        "name": "I am a checkbox",
        "description": "Some description",
        "text": "Check this checkbox",
        "type": "boolean",
        "default": true,
    },
    {
        "identifier": "myText",
        "name": "I am textbox",
        "description": "Please enter your text:",
        "type": "text",
        "default": "This can be a really long text\nwith multiple lines.",
    },
    {
        "identifier": "myInt",
        "name": "I am a number selector",
        "description": "Please enter a number:",
        "type": "integer",
        "default": 42,
    },
    {
        "identifier": "myFile",
        "name": "I am a file selector",
        "description": "Please select the file:",
        "type": "file",
        "default": "pandoc",
    },
    {
        "identifier": "mySelection",
        "name": "I am an item selector",
        "description": "Please select an item:",
        "type": "selection",
        "default": "option2",
        "items": {"option1": "Text for option 1", "option2": "Text for option 2", "option3": "Text for option 3"},
    }
];
```

Además, puede anular las `settingsVariables` con una función especial `registerSettingsVariables ()` como esta:

### Ejemplo
```js
/**
 * Registers the settings variables again
 *
 * Use this method if you want to use code to override your variables, like setting
 * default values depended on the operating system.
 */
function registerSettingsVariables() {
    if (script.platformIsWindows()) {
        // override the myFile default value
        settingsVariables[3].default = "pandoc.exe"
    }
}
```

Quizás también quieras echar un vistazo al ejemplo [variables.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/variables.qml).

Almacenar y cargar variables persistentes
----------------------------------------

### Parámetros y llamada al método
```cpp
/**
 * Stores a persistent variable
 * These variables are accessible globally over all scripts
 * Please use a meaningful prefix in your key like "PersistentVariablesTest/myVar"
 *
 * @param key {QString}
 * @param value {QVariant}
 */
void ScriptingService::setPersistentVariable(const QString &key,
                                                const QVariant &value);

/**
 * Loads a persistent variable
 * These variables are accessible globally over all scripts
 *
 * @param key {QString}
 * @param defaultValue {QVariant} return value if the setting doesn't exist (optional)
 * @return
 */
QVariant ScriptingService::getPersistentVariable(const QString &key,
                                                    const QVariant &defaultValue);
```

### Ejemplo
```js
// store persistent variable
script.setPersistentVariable("PersistentVariablesTest/myVar", result);

// load and log persistent variable
script.log(script.getPersistentVariable("PersistentVariablesTest/myVar", "nothing here yet"));
```

Asegúrese de usar un prefijo significativo en su clave como `PersistentVariablesTest / myVar` porque las variables son accesibles desde todos los scripts.

Quizás también quieras echar un vistazo al ejemplo [persistent-variables.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/persistent-variables.qml).

Loading application settings variables
--------------------------------------

### Parámetros y llamada al método
```cpp
/**
 * Loads an application settings variable
 *
 * @param key {QString}
 * @param defaultValue {QVariant} return value if the setting doesn't exist (optional)
 * @return
 */
QVariant ScriptingService::getApplicationSettingsVariable(const QString &key,
                                                            const QVariant &defaultValue);
```

### Ejemplo
```js
// load and log an application settings variable
script.log(script.getApplicationSettingsVariable("gitExecutablePath"));
```

Tenga en cuenta que la configuración en realidad puede estar vacía, debe ocuparse de eso usted mismo. `defaultValue` solo se usa si la configuración no existe en absoluto.

Creando un directorio de caché
--------------------------

Puede almacenar en caché los archivos en la ubicación de caché predeterminada de su sistema.

### Parámetros y llamada al método
```cpp
/**
 * Returns a cache directory for a script
 *
 * @param {QString} subDir the subfolder to create and use
 * @return {QString} the cache dir path
 */
QString ScriptingService::cacheDir(const QString &subDir) const;
```

### Ejemplo
```js
// create the cache directory for my-script-id
var cacheDirForScript = script.cacheDir("my-script-id");
```

Borrar un directorio de caché
--------------------------

Puede borrar los archivos de caché de su script pasando su nombre a clearCacheDir().

### Parámetros y llamada al método
```cpp
/**
 * Clears the cache directory for a script
 *
 * @param {QString} subDir the subfolder to clear
 * @return {bool} true on success
 */
bool ScriptingService::clearCacheDir(const QString &subDir) const;
```

### Ejemplo
```js
// clear cache directory of my-script-id 
script.clearCacheDir("my-script-id");
```

Leer la ruta al directorio de su script
------------------------------------------------

Si necesita obtener la ruta al directorio donde se coloca su script para, por ejemplo, cargar otros archivos, debe registrar un `cadena de propiedad scriptDirPath;`. Esta propiedad se establecerá con la ruta al directorio del script.

### Ejemplo
```js
import QtQml 2.0
import QOwnNotesTypes 1.0

Script {
    // the path to the script's directory will be set here
    property string scriptDirPath;

    function init() {
        script.log(scriptDirPath);
    }
}
```

Conversión de separadores de ruta en nativos
-----------------------------------------

### Parámetros y llamada al método
```cpp
/**
 * Returns path with the '/' separators converted to separators that are
 * appropriate for the underlying operating system.
 *
 * On Windows, toNativeDirSeparators("c:/winnt/system32") returns
 * "c:\winnt\system32".
 *
 * @param path
 * @return
 */
QString ScriptingService::toNativeDirSeparators(QString path);
```

### Ejemplo
```js
// will return "c:\winnt\system32" on Windows
script.log(script.toNativeDirSeparators("c:/winnt/system32"));
```

Conversión de separadores de ruta de las nativas
-------------------------------------------

### Parámetros y llamada al método
```cpp
/**
 * Returns path using '/' as file separator.
 * On Windows, for instance, fromNativeDirSeparators("c:\\winnt\\system32")
 * returns "c:/winnt/system32".
 *
 * @param path
 * @return
 */
QString ScriptingService::fromNativeDirSeparators(QString path);
```

### Ejemplo
```js
// will return "c:/winnt/system32" on Windows
script.log(script.fromNativeDirSeparators("c:\\winnt\\system32"));
```

Obtener el separador de directorios nativo
--------------------------------------

### Parámetros y llamada al método
```cpp
/**
 * Returns the native directory separator "/" or "\" on Windows
 *
 * @return
 */
QString ScriptingService::dirSeparator();
```

### Ejemplo
```js
// will return "\" on Windows
script.log(script.dirSeparator());
```

Obtener una lista de las rutas de todas las notas seleccionadas
-------------------------------------------------

### Parámetros y llamada al método
```cpp
/**
 * Returns a list of the paths of all selected notes
 *
 * @return {QStringList} list of selected note paths
 */
QStringList ScriptingService::selectedNotesPaths();
```

### Ejemplo
```js
// returns a list of the paths of all selected notes
script.log(script.selectedNotesPaths());
```

Es posible que desee echar un vistazo al ejemplo [external-note-diff.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/external-note-diff.qml).

Obtener una lista de los identificadores de todas las notas seleccionadas
-----------------------------------------------

### Parámetros y llamada al método
```cpp
/**
 * Returns a list of the ids of all selected notes
 *
 * @return {QList<int>} list of selected note ids
 */
QList<int> ScriptingService::selectedNotesIds();
```

### Ejemplo
```js
// returns a list of the ids of all selected notes
script.log(script.selectedNotesIds());
```

Es posible que desee echar un vistazo al ejemplo [exportar notas como uno-html.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/export-notes-as-one-html.qml).

Activar una acción de menú
------------------------

### Parámetros y llamada al método
```cpp
/**
 * Triggers a menu action
 *
 * @param objectName {QString} object name of the action to trigger
 * @param checked {QString} only trigger the action if checked-state is
 *                          different than this parameter (optional, can be 0 or 1)
 */
void ScriptingService::triggerMenuAction(QString objectName, QString checked);
```

### Ejemplo
```js
// toggle the read-only mode
script.triggerMenuAction("actionAllow_note_editing");

// disable the read-only mode
script.triggerMenuAction("actionAllow_note_editing", 1);
```

Es posible que desee echar un vistazo al ejemplo [disable-readonly-mode.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/disable-readonly-mode.qml).

Puede obtener los nombres de los objetos de la acción del menú en [mainwindow.ui](https://github.com/pbek/QOwnNotes/blob/develop/src/mainwindow.ui).

Opening an input dialog with a select box
-----------------------------------------

### Parámetros y llamada al método
```cpp
/**
 * Opens an input dialog with a select box
 *
 * @param title {QString} title of the dialog
 * @param label {QString} label text of the dialog
 * @param items {QStringList} list of items to select
 * @param current {int} index of the item that should be selected (default: 0)
 * @param editable {bool} if true the text in the dialog can be edited (default: false)
 * @return {QString} text of the selected item
 */
QString ScriptingService::inputDialogGetItem(
        const QString &title, const QString &label, const QStringList &items,
        int current, bool editable);
```

### Ejemplo
```js
var result = script.inputDialogGetItem(
    "combo box", "Please select an item", ["Item 1", "Item 2", "Item 3"]);
script.log(result);
```

Es posible que desee echar un vistazo al ejemplo [input-dialogs.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/input-dialogs.qml).

Abrir un diálogo de entrada con una edición de línea
----------------------------------------

### Parámetros y llamada al método
```cpp
/**
 * Opens an input dialog with a line edit
 *
 * @param title {QString} title of the dialog
 * @param label {QString} label text of the dialog
 * @param text {QString} text in the dialog (optional)
 * @return
 */
QString ScriptingService::inputDialogGetText(
        const QString &title, const QString &label, const QString &text);
```

### Ejemplo
```js
var result = script.inputDialogGetText(
    "line edit", "Please enter a name", "current text");
script.log(result);
```

Comprobando si existe un archivo
-------------------------

### Parámetros y llamada al método
```cpp
/**
 * Check if a file exists
 * @param filePath
 * @return
 */
bool ScriptingService::fileExists(QString &filePath);
```

### Ejemplo
```js
var result = script.fileExists(filePath);
script.log(result);
```

Leer texto de un archivo
------------------------

### Parámetros y llamada al método
```cpp
/**
 * Read text from a file
 *
 * @param filePath {QString} path of the file to load
 * @param codec {QString} file encoding (default: UTF-8)
 * @return the file data or null if the file does not exist
 */
QString ScriptingService::readFromFile(const QString &filePath, const QString &codec)
```

### Ejemplo
```js
if(script.fileExists(filePath)){
    var data = script.readFromFile(filePath);
    script.log(data);
}
```


Escribir texto en un archivo
----------------------

### Parámetros y llamada al método
```cpp
/**
 * Writes a text to a file
 *
 * @param filePath {QString}
 * @param data {QString}
 * @param createParentDirs {bool} optional (default: false)
 * @return
 */
bool ScriptingService::writeToFile(const QString &filePath, const QString &data, bool createParentDirs);
```

### Ejemplo
```js
var result = script.writeToFile(filePath, html);
script.log(result);
```

Es posible que desee echar un vistazo al ejemplo [exportar notas como uno-html.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/export-notes-as-one-html.qml).

Trabajando con websockets
-----------------------

Puede controlar QOwnNotes de forma remota utilizando `WebSocketServer`.

Por favor, mire el ejemplo [websocket-server.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/websocket-server.qml). Puede probar el servidor de socket conectándose a él en [Websocket prueba](https://www.websocket.org/echo.html?location=ws://127.0.0.1:35345).

También puede escuchar sockets con `WebSocket`. Por favor, mira el ejemplo [websocket-client.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/websocket-client.qml).

Tenga en cuenta que debe tener instalada la biblioteca QML `websocket` de Qt para usar esto. Por ejemplo, en Ubuntu Linux puede instalar `módulo-qml-qtwebsockets`.
