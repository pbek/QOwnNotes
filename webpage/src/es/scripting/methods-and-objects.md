# Métodos y objetos que QOwnNotes proporciona

Iniciar un programa externo en segundo plano
----------------------------------------------


### Parámetros y llamada al método
```cpp
/**
  * Envoltorio QML para iniciar un proceso separado
  *
  * @param ejecutablePath la ruta del ejecutable
  * @param parameters una lista de cadenas de parámetros
  * @param callbackIdentifier un identificador que se utilizará en la función onDetachedProcessCallback () (opcional)
  * @param callbackParameter un parámetro adicional para bucles o similares (opcional)
  * @param processData datos escritos en el proceso si se usa la devolución de llamada (opcional)
  * @return verdadero en caso de éxito, falso en caso contrario
  */
bool startDetachedProcess (QString ejecutablePath, parámetros QStringList,
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
  * Envoltorio QML para iniciar un proceso sincrónico
  *
  * @param ejecutablePath la ruta del ejecutable
  * @param parameters una lista de cadenas de parámetros
  * @param data los datos que se escribirán en el proceso (opcional)
  * @return el texto que fue devuelto por el proceso
QByteArray startSynchronousProcess (QString ejecutablePath, parámetros QStringList, datos QByteArray);
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
  * Envoltorio QML para obtener la ruta de la carpeta de notas actual
  *
  * @return la ruta de la carpeta de notas actual
  */
QString currentNoteFolderPath ();
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
  * Envoltorio QML para obtener la nota actual
  *
  * @returns {NoteApi} el objeto de nota actual
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
  * Envoltorio QML para iniciar sesión en el widget de registro
  *
  * @param texto
  */
registro vacío (texto QString);
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
  * Envoltorio QML para descargar una URL a la carpeta de medios y devolver los medios
  * URL o el texto de la imagen de rebaja de los medios en relación con la nota actual
  *
  * @param {QString} URL
  * @param {bool} returnUrlOnly si es verdadero, solo se devolverá la URL del medio (por defecto es falso)
  * @return {QString} el descuento de medios o la URL
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
  * Envoltorio QML para insertar un archivo multimedia en la carpeta multimedia y regresar
  * la URL de medios o el texto de la imagen de rebaja de los medios en relación con la nota actual
  *
  * @param {QString} mediaFilePath
  * @param {bool} returnUrlOnly si es verdadero, solo se devolverá la URL del medio (por defecto es falso)
  * @return {QString} el descuento de medios o la URL
  */
QString ScriptingService :: insertMediaFile (QString mediaFilePath,
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
 * Registra una acción personalizada
 *
 * @param identifier el identificador de la acción
 * @param menuText el texto que se muestra en el menú
 * @param button Envía el texto que se muestra en el botón
 * (no se verá ningún botón si está vacío)
 * @param icon la ruta del archivo del icono o el nombre de un icono de tema de freedesktop
 * encontrará una lista de iconos aquí:
 * https://specifications.freedesktop.org/icon-naming-spec/icon-naming-spec-latest.html
 * @param useInNoteEditContextMenu si es verdadero, use la acción en la edición de notas
 * menú contextual (predeterminado: falso)
 * @param hideButtonInToolbar si es verdadero, el botón no se mostrará en el
 * barra de herramientas de acción personalizada (predeterminado: falso)
 * @param useInNoteListContextMenu si es verdadero, use la acción en la lista de notas
 * menú contextual (predeterminado: falso)
 */
void ScriptingService :: registerCustomAction (identificador QString,
                                            QString menuText,
                                            QString buttonText,
                                            Icono de QString,
                                            bool useInNoteEditContextMenu,
                                            bool hideButtonInToolbar,
                                            bool useInNoteListContextMenu);
```

### Ejemplo
```js
// agrega una acción personalizada sin un botón
script.registerCustomAction("mycustomaction1", "Menu text");

// agrega una acción personalizada con un botón
script.registerCustomAction("mycustomaction1", "Menu text", "Button text");

// agregue una acción personalizada con un botón y un ícono de tema de escritorio libre
script.registerCustomAction("mycustomaction1", "Menu text", "Button text", "task-new");

// agrega una acción personalizada con un botón y un ícono de un archivo
script.registerCustomAction("mycustomaction1", "Menu text", "Button text", "/usr/share/icons/breeze/actions/24/view-calendar-tasks.svg");
```

A continuación, es posible que desee utilizar el identificador con función `customActionInvoked` en un script como [acciones-personalizadas.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml).

Registrar una etiqueta
-------------------

### Parámetros y llamada al método
```cpp
/**
  * Registra una etiqueta para escribir
  *
  * @param identifier el identificador de la etiqueta
  * @param text el texto que se muestra en la etiqueta (opcional)
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
  * Establece el texto de una etiqueta registrada
  *
  * @param identifier el identificador de la etiqueta
  * @param envía el texto que se muestra en la etiqueta
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

::: tip
Si desactivó que el título de su nota determina el nombre del archivo de la nota, luego debe cambiar el nombre de su archivo de nota usted mismo, así:

```js
var note = script.currentNote();
note.renameNoteFile('your-filename');
```
:::

Accediendo al portapapeles
-----------------------

### Parámetros y llamada al método
```cpp
/**
  * Devuelve el contenido del portapapeles como texto o html
  *
  * @param asHtml devuelve el contenido del portapapeles como html en lugar de texto
  */
QString ScriptingService :: portapapeles (bool asHtml);
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
  * Selecciona la línea actual en la edición de texto de la nota
  */
void ScriptingService :: noteTextEditSelectCurrentLine();
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
  * Selecciona la línea actual en la edición de texto de la nota
  */
void ScriptingService :: noteTextEditSelectCurrentWord();
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
  * Establece el texto seleccionado actualmente en la edición de texto de nota
  *
  * @param inicio
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
  * Devuelve la posición inicial de la selección actual en la edición de texto de nota
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
  * Establece el cursor de texto en la edición de texto de la nota en una posición determinada
  * 0 sería el comienzo de la nota
  * caso especial: -1 sería el final de la nota
  *
  * posición @param
  */
void ScriptingService::noteTextEditSetCursorPosition(int position);
```

### Ejemplo
```js
// saltar al undécimo carácter de la nota
script.noteTextEditSetCursorPosition(10);

// saltar al final de la nota
script.noteTextEditSetCursorPosition(-1);
```

Obtener la posición actual del cursor de texto en la edición de texto de la nota
-----------------------------------------------------------------

### Parámetros y llamada al método
```cpp
/**
  * Devuelve la posición actual del cursor de texto en la edición de texto de la nota
  * 0 sería el comienzo de la nota
  */
int ScriptingService :: noteTextEditCursorPosition();
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
  * Lee la palabra actual en la edición de texto de la nota
  *
  * @param withPreviousCharacters también obtiene más caracteres al principio
  * para obtener caracteres como "@" que no son
  * caracteres de palabra
  * @regreso
  */
QString ScriptingService::noteTextEditCurrentWord(bool withPreviousCharacters);
```

### Ejemplo
```js
// leer la palabra actual en el texto de la nota editar
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
if (script.platformIsLinux ()) {
     // solo se ejecutará si está bajo Linux
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

You might want to look at the custom action `favoriteNote` in the example [favorite-note.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/favorite-note.qml).

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

You may want to take a look at the example [autocompletion.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/autocompletion.qml).

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
// agranda el texto en la lista de notas
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
  * Recarga el motor de secuencias de comandos
  */
void ScriptingService::reloadScriptingEngine();
```

### Ejemplo
```js
// recarga el motor de secuencias de comandos
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
// buscar nota por id
script.fetchNoteById(243);
```

Es posible que desee echar un vistazo al ejemplo [exportar notas como uno-html.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/export-notes-as-one-html.qml).

Comprobando si una nota existe por su nombre de archivo
------------------------------------------

### Parámetros y llamada al método
```cpp
/**
  * Comprueba si existe un archivo de notas por su nombre de archivo
  *
  * @param fileName cadena el nombre del archivo de la nota (obligatorio)
  * @param ignoreNoteId ID entero de una nota para ignorar en el cheque
  * @param noteSubFolderId ID entero de la subcarpeta de notas
  * @return bool
  */
bool ScriptingService::noteExistsByFileName(QString fileName,
                                            int ignoreNoteId,
                                            int noteSubFolderId);
```

### Ejemplo
```js
// verifica si la nota existe, pero ignora el id de "nota"
script.noteExistsByFileName("my note.md", note.id);
```

You may want to take a look at the example [use-tag-names-in-filename.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/use-tag-names-in-filename.qml).

Copiar texto en el portapapeles
-------------------------------

### Parámetros y llamada al método
```cpp
/**
  * Copia texto en el portapapeles como texto sin formato o datos mime html
  *
  * @param texto de cadena de texto para poner en el portapapeles
  * @param asHtml bool si es verdadero, el texto se establecerá como datos mime html
  */
void ScriptingService::setClipboardText(QString text, bool asHtml);
```

### Ejemplo
```js
// copiar texto al portapapeles
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
// saltar a la nota
script.setCurrentNote(note);
```

Es posible que desee echar un vistazo al ejemplo [entrada-diario.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/journal-entry.qml).

Saltar a una subcarpeta de notas
---------------------------

### Parámetros y llamada al método
```cpp
/**
  * Salta a una subcarpeta de notas
  *
  * @param noteSubFolderPath {QString} ruta de la subcarpeta, relativa a la carpeta de notas
  * @param separator {QString} separador entre partes de la ruta, predeterminado "/"
  * @return verdadero si el salto fue exitoso
  */
bool ScriptingService::jumpToNoteSubFolder(const QString &noteSubFolderPath,
                                            QString separator);
```

### Ejemplo
```js
// saltar a la subcarpeta de notas "una subcarpeta"
script.jumpToNoteSubFolder("a sub folder");

// saltar a la subcarpeta de notas "sub" dentro de "una subcarpeta"
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
  * Muestra un cuadro de mensaje de información
  *
  * @param texto
  * @param title (opcional)
  */
void ScriptingService::informationMessageBox(QString text, QString title);
```

### Ejemplo
```js
// muestra un cuadro de mensaje de información
script.informationMessageBox ("El texto que quiero mostrar", "Algún título opcional");
```

Mostrando un cuadro de mensaje de pregunta
------------------------------

### Parámetros y llamada al método
```cpp
/**
  * Muestra un cuadro de mensaje de pregunta
  *
  * Para obtener información sobre los botones, consulte:
  * https://doc.qt.io/qt-5/qmessagebox.html#StandardButton-enum
  *
  * @param texto
  * @param title (opcional)
  * Botones de @param botones que deben mostrarse (opcional)
  * @param defaultButton botón predeterminado que se seleccionará (opcional)
  * @return id del botón presionado
  */
int ScriptingService :: questionMessageBox (
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
// tienes que definir tus variables registradas para poder acceder a ellas más tarde
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
  * Registra las variables de configuración nuevamente
  *
  * Use este método si desea usar código para anular sus variables, como configurar
  * los valores predeterminados dependen del sistema operativo.
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
  * Almacena una variable persistente
  * Estas variables son accesibles globalmente en todos los scripts
  * Utilice un prefijo significativo en su clave como "PersistentVariablesTest / myVar"
  *
  * @param key {QString}
  * @param value {QVariant}
  */
void ScriptingService :: setPersistentVariable (const QString & clave,
                                                 const QVariant & value);

/**
 * Loads a persistent variable
 * These variables are accessible globally over all scripts
  *
  * @param key {QString}
  * @param defaultValue {QVariant} valor de retorno si la configuración no existe (opcional)
  * @regreso
  * /
QVariant ScriptingService::getPersistentVariable(const QString &key,
                                                    const QVariant &defaultValue);
```

### Ejemplo
```js
// almacenar variable persistente
script.setPersistentVariable("PersistentVariablesTest/myVar", result);

// cargar y registrar variable persistente
script.log(script.getPersistentVariable("PersistentVariablesTest/myVar", "no hay nada todavía"));
```

Asegúrese de usar un prefijo significativo en su clave como `PersistentVariablesTest / myVar` porque las variables son accesibles desde todos los scripts.

Quizás también quieras echar un vistazo al ejemplo [persistent-variables.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/persistent-variables.qml).

Cargando variables de configuración de la aplicación
--------------------------------------

### Parámetros y llamada al método
```cpp
/**
  * Carga una variable de configuración de la aplicación
  *
  * @param key {QString}
  * @param defaultValue {QVariant} valor de retorno si la configuración no existe (opcional)
 * @return
 */
QVariant ScriptingService::getApplicationSettingsVariable(const QString &key,
                                                            const QVariant &defaultValue);
```

### Ejemplo
```js
// cargar y registrar una variable de configuración de la aplicación
script.log(script.getApplicationSettingsVariable ("gitExecutablePath"));
```

Tenga en cuenta que la configuración en realidad puede estar vacía, debe ocuparse de eso usted mismo. `defaultValue` solo se usa si la configuración no existe en absoluto.

Creando un directorio de caché
--------------------------

Puede almacenar en caché los archivos en la ubicación de caché predeterminada de su sistema.

### Parámetros y llamada al método
```cpp
/**
  * Devuelve un directorio de caché para un script
  *
  * @param {QString} subDir la subcarpeta para crear y usar
  * @return {QString} la ruta del directorio de caché
  */
QString ScriptingService::cacheDir(const QString &subDir) const;
```

### Ejemplo
```js
// crea el directorio de caché para my-script-id
var cacheDirForScript = script.cacheDir("my-script-id");
```

Borrar un directorio de caché
--------------------------

Puede borrar los archivos de caché de su script pasando su nombre a clearCacheDir().

### Parámetros y llamada al método
```cpp
/**
  * Borra el directorio de caché de un script
  *
  * @param {QString} subDir la subcarpeta para borrar
  * @return {bool} verdadero en caso de éxito
  */
bool ScriptingService :: clearCacheDir (const QString & subDir) const;
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
  * Devuelve la ruta con los separadores '/' convertidos en separadores que son
  * apropiado para el sistema operativo subyacente.
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
