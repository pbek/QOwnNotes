# Métodos y objetos que QOwnNotes proporciona

Iniciar un programa externo en segundo plano
----------------------------------------------


### Llamada y parámetros del método
```cpp
/**
  * Envoltorio QML para iniciar un proceso separado
  *
  * @param executablePath la ruta del ejecutable
  * @param parámetros una lista de cadenas de parámetros
  * @param callbackIdentifier un identificador que se usará en la función onDetachedProcessCallback() (opcional)
  * @param callbackParameter un parámetro adicional para bucles o similares (opcional)
  * @param processData datos escritos en el proceso si se utiliza la devolución de llamada (opcional)
  * @param workingDirectory el directorio de trabajo para ejecutar el proceso (opcional, solo funciona sin devolución de llamada)
  * @return verdadero en caso de éxito, falso en caso contrario
  */
bool startDetachedProcess(QString executablePath, parámetros QStringList,
                             QString callbackIdentifier, QVariant callbackParameter,
                             QByteArray processData, QString directorio de trabajo);
```

### Ejemplo

Ejemplo simple:

```js
script.startDetachedProcess("/ruta/a/mi/programa", ["mi parámetro"]);
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

Puede que quiera echar un vistazo al ejemplo [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-actions.qml), [callback.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/callback.qml) o [execute-command-after-note-update.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/execute-command-after-note-update.qml).

También es posible que desee echar un vistazo al gancho [onDetachedProcessCallback](hooks.html#ondetachedprocesscallback).

Iniciar un programa externo y esperar la salida
----------------------------------------------------

### Llamada y parámetros del método
```cpp
/**
 * Envoltorio de QML para iniciar un proceso sincrónico
 *
 * @param executablePath la ruta del ejecutable
 * @param parameters una lista de cadenas de parámetros
 * @param data los datos que se escribirán en el proceso (opcional)
 * @param workingDirectory el directorio de trabajo para ejecutar el proceso (opcional)
 * @return el texto que devolvió el proceso
QByteArray startSynchronousProcess(QString executablePath, QStringList parameters, QByteArray data, QString workingDirectory);
```

### Ejemplo
```js
var resultado = script.startSynchronousProcess("/ruta/a/mi/programa", ["mi parámetro"], "datos", "/ruta/en/la/cual/ejecutar");
```

Puede que quiera echar un vistazo al ejemplo [encryption-keybase.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/encryption-keybase.qml).

Obtener la ruta de la carpeta de notas actual
-------------------------------------------

### Llamada y parámetros del método
```cpp
/**
 * Envoltorio de QML para obtener la ruta de la carpeta de notas actual
 *
 * @return la ruta de la carpeta de notas actual
 */
QString currentNoteFolderPath ();
```

### Ejemplo
```js
var path = script.currentNoteFolderPath();
```

Puede que quiera echar un vistazo al ejemplo [absolute-media-links.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/absolute-media-links.qml).

Obtener la nota actual
------------------------

### Llamada y parámetros del método
```cpp
/**
 * Envoltorio de QML para obtener la nota actual
 *
 * @returns {NoteApi} el objeto de nota actual
 */
NoteApi currentNote();
```

### Ejemplo
```js
var note = script.currentNote();
```

Puede que quiera echar un vistazo al ejemplo [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-actions.qml).

Iniciar sesión en el widget de registro
-------------------------

### Llamada y parámetros del método
```cpp
/**
 * Envoltorio de QML para iniciar sesión en el widget de registro
 *
 * @param text texto
 */
void log(QString text);
```

### Ejemplo
```js
script.log("mi texto");
```

Descarga de una URL a una cadena
------------------------------

### Llamada y parámetros del método
```cpp
/**
 * Envoltorio de QML para descargar una URL y devolverla como texto
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

Puede que quiera echar un vistazo al ejemplo [insert-headline-with-link-from-github-url.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/insert-headline-with-link-from-github-url.qml).

Descarga de una URL a la carpeta multimedia
--------------------------------------

### Llamada y parámetros del método
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

Puede que quiera echar un vistazo al ejemplo [paste-latex-image.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/paste-latex-image.qml).

Insertar un archivo multimedia en la carpeta multimedia
--------------------------------------------

### Llamada y parámetros del método
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
var markdown = script.insertMediaFile("/ruta/a/tu/imagen.png");
```

Puede que quiera echar un vistazo al ejemplo [scribble.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/scribble.qml).

Insertar un archivo adjunto en la carpeta de archivos adjuntos
--------------------------------------------------------

### Llamada y parámetros del método
```cpp
 * QML wrapper to insert an attachment file into the `attachments` folder and
 * returning the attachment url or the markdown text of the attachment
 * relative to the current note
 *
 * @param {QString} attachmentFilePath
 * @param {QString} fileName to use in the markdown
 * @param {bool} returnUrlOnly if true only the attachment url will be returned
 * (default false)
 * @return {QString} the attachment markdown or url
 */
QString ScriptingService::insertAttachmentFile(const QString &attachmentFilePath,
                                               const QString &fileName,
                                               bool returnUrlOnly);
```

### Ejemplo
```js
var markdown = script.insertAttachmentFile("/ruta/a/tu/archivo.png");
```

Regenerar la vista previa de la nota
-----------------------------

Actualiza la vista previa de la nota.

### Llamada y parámetros del método
```cpp
/**
 * Regenera la vista previa de la nota
 */
QString ScriptingService::regenerateNotePreview();
```

### Ejemplo
```js
script.regenerateNotePreview();
```

Puede que quiera echar un vistazo al ejemplo [scribble.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/scribble.qml).

Registro de una acción personalizada
---------------------------

### Llamada y parámetros del método
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
void ScriptingService:: registerCustomAction (identificador QString,
                                            QString menuText,
                                            QString buttonText,
                                            Icono de QString,
                                            bool useInNoteEditContextMenu,
                                            bool hideButtonInToolbar,
                                            bool useInNoteListContextMenu);
```

::: tip
También puede asignar accesos directos locales y globales a sus acciones personalizadas en la *Configuración de accesos directos*.
:::

::: warning
Tenga en cuenta que los [iconos de temas de escritorio gratuitos](https://specifications.freedesktop.org/icon-naming-spec/icon-naming-spec-latest.html) están disponibles en su mayoría solo en Linux. Entonces, si realmente desea usar un ícono en macOS o Windows, debe proporcionar uno con su script. Para obtener la ruta de su secuencia de comandos para establecer una ruta adecuada para su icono, puede usar la propiedad [scriptDirPath](methods-and-objects.md#reading-the-path-to-the-directory-of-your-script).
:::

### Ejemplo

```js
import QtQml 2.0
import QOwnNotesTypes 1.0

Script {
    /**
     * Initializes the custom actions
     */
    function init() {
        // add a custom action without a button
        script.registerCustomAction("mycustomaction1", "Menu text");

        // add a custom action with a button
        script.registerCustomAction("mycustomaction2", "Menu text", "Button text");

        // add a custom action with a button and freedesktop theme icon
        script.registerCustomAction("mycustomaction3", "Menu text", "Button text", "task-new");

        // add a custom action with a button and an icon from a file
        script.registerCustomAction("mycustomaction4", "Menu text", "Button text", "/usr/share/icons/breeze/actions/24/view-calendar-tasks.svg");
    }

    /**
     * This function is invoked when a custom action is triggered
     * in the menu or via button
     * 
     * @param identifier string the identifier defined in registerCustomAction
     */
    function customActionInvoked(identifier) {
        switch (identifier) {
            case "mycustomaction1":
                script.log("Action 1");
            break;
            case "mycustomaction2":
                script.log("Action 2");
            break;
            case "mycustomaction3":
                script.log("Action 3");
            break;
            case "mycustomaction4":
                script.log("Action 4");
            break;
        }
    }
}
```

Para más ejemplos, por favor revise [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-actions.qml).

::: tip
También puede activar una acción personalizada después de iniciar la aplicación con el parámetro `--action customAction_<identifier>`. Para mayor información, por favor consulte [Activar acciones del menú después del inicio](../getting-started/cli-parameters.md#trigger-menu-actions-after-startup).
:::

Registrar una etiqueta
-------------------

### Llamada y parámetros del método
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
script.registerLabel("html-label", "<strong>Fuerte</strong> texto HTML<br />con tres líneas<br />y un <a href='https://www.qownnotes.org'>enlace a un sitio web</a>.");

script.registerLabel("etiqueta larga", "otro texto muy largo, otro texto muy largo, otro texto muy largo, otro texto muy largo, otro texto muy largo, otro texto muy largo, otro texto muy largo, otro texto muy largo , otro texto muy largo, otro texto muy largo, otro texto muy largo que se envolverá");

script.registerLabel("contra-etiqueta");
```

The labels will be visible in the *Scripting panel*, which you need to enable in the *Window / Panels* menu.

Puede utilizar texto sin formato o html en las etiquetas. Se podrá seleccionar el texto y se podrá hacer clic en los enlaces.

Puede que después quiera echar un vistazo al script de ejemplo [scripting-label-demo.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/scripting-label-demo.qml).

Configurar el texto de una etiqueta registrada
--------------------------------------

### Llamada y parámetros del método
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

Puede que después quiera echar un vistazo al script de ejemplo [scripting-label-demo.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/scripting-label-demo.qml).

::: tip
The scripting label also supports **external links**, as well as **internal links** to notes, tasks, deck cards and more. You might get a grasp of what's supported by taking a look at [URL handling](https://github.com/pbek/QOwnNotes/blob/964acf69b6382f8ee8252c640c5048f8f4644622/src/utils/urlhandler.cpp#L16-L75).
:::

Crear una nota nueva
-------------------

### Llamada y parámetros del método
```cpp
/**
 * Crea una nota nueva
 *
 * @param text the note text
 */
void ScriptingService::createNote(QString text);
```

### Ejemplo
```js
script.createNote("My note headline\n===\n\nMy text");
```

Puede que quiera echar un vistazo al ejemplo [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-actions.qml).

::: tip
If you turned off that your note headline determines the note filename then you have to rename your note file yourself afterward, like this:

```js
var note = script.currentNote();
note.renameNoteFile('su-nombre-de-archivo');
```
:::

Acceder al portapapeles
-----------------------

### Llamada y parámetros del método
```cpp
/**
 * Devuelve el contenido del portapapeles como texto o html
 *
 * @param asHtml devuelve el contenido del portapapeles como html en vez de texto
 */
QString ScriptingService::clipboard(bool asHtml);
```

### Ejemplo
```js
var clipboardText = script.clipboard();
var clipboardHtml = script.clipboard(true);
```

Puede que quiera echar un vistazo al ejemplo [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-actions.qml).

Escribir texto en la edición de texto de la nota
--------------------------------

### Llamada y parámetros del método
```cpp
/**
 * Escribe texto en la posición actual del cursor en la edición de texto de la nota
 *
 * @param text
 */
void ScriptingService::noteTextEditWrite(QString text);
```

### Ejemplo
```js
// escribir texto en la nota editar texto
script.noteTextEditWrite("My custom text");
```

You might want to look at the custom action `transformTextRot13` in the example [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-actions.qml).

Puede usar esto junto con `noteTextEditSelectAll` para sobrescribir todo el texto de la nota actual.

Leer el texto seleccionado en la edición de texto de la nota
--------------------------------------------

### Llamada y parámetros del método
```cpp
/**
 * Lee el texto seleccionado en la edición de texto de la nota
 *
 * @return
 */
QString ScriptingService::noteTextEditSelectedText();
```

### Ejemplo
```js
// leer el texto seleccionado de la edición de texto de la nota
var text = script.noteTextEditSelectedText();
```

You might want to look at the custom action `transformTextRot13` in the example [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-actions.qml).

Seleccionar todo el texto en la edición de texto de la nota
-------------------------------------

### Llamada y parámetros del método
```cpp
/ **
 * Selecciona todo el texto en la edición de texto de la nota
 */
void ScriptingService::noteTextEditSelectAll();
```

### Ejemplo
```js
script.noteTextEditSelectAll();
```

Puede usar esto junto con `noteTextEditWrite` para sobrescribir todo el texto de la nota actual.

Seleccionar la línea actual en la edición de texto de la nota
---------------------------------------------

### Llamada y parámetros del método
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

Seleccionar la palabra actual en la edición de texto de la nota
---------------------------------------------

### Llamada y parámetros del método
```cpp
/**
 * Selecciona la palabra actual en la edición de texto de nota
 */
void ScriptingService::noteTextEditSelectCurrentWord();
```

### Ejemplo
```js
script.noteTextEditSelectCurrentWord();
```

Establecer el texto actualmente seleccionado en la edición de texto de nota
-----------------------------------------------------

### Llamada y parámetros del método
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
// expande la selección actual en un carácter
script.noteTextEditSetSelection(
    script.noteTextEditSelectionStart() - 1,
    script.noteTextEditSelectionEnd() + 1);
```

Obtener la posición inicial de la selección actual en la edición de texto de la nota
---------------------------------------------------------------------

### Llamada y parámetros del método
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

### Llamada y parámetros del método
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

Establecer el cursor de texto en la edición de texto de la nota en una posición determinada
---------------------------------------------------------------

### Llamada y parámetros del método
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

### Llamada y parámetros del método
```cpp
/**
 * Devuelve la posición actual del cursor de texto en la edición de texto de la nota
 * 0 sería el comienzo de la nota
 */
int ScriptingService::noteTextEditCursorPosition();
```

### Ejemplo
```js
script.log(script.noteTextEditCursorPosition());
```

Leer la palabra actual desde el editor de texto de notas
---------------------------------------------

### Llamada y parámetros del método
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

Puede que quiera echar un vistazo al ejemplo [autocompletion.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/autocompletion.qml).

Read the current block from the note text edit
----------------------------------------------

### Llamada y parámetros del método
```cpp
/**
 * Reads the current block in the note text edit
 *
 * @return
 */
QString ScriptingService::noteTextEditCurrentBlock();
```

### Ejemplo
```js
// Read the current block in the note text edit
var text = script.noteTextEditCurrentBlock();
```

Puede que quiera echar un vistazo al ejemplo [ai-autocompletion.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/ai-autocompletion.qml).

Use a completion prompt on the currently selected AI model
----------------------------------------------------------

The AI completion prompt is a feature that allows you to use a completion prompt on the currently selected AI model.

The AI system needs to be enabled in the AI toolbar or main menu for this to work.

### Llamada y parámetros del método
```cpp
/**
 * QML wrapper to use the AI Completer
 *
 * @param prompt
 * @return {QString} the result of the completer
 */
QString ScriptingService::aiComplete(const QString& prompt);
```

### Ejemplo
```js
// Ask the currently selected AI model to complete the prompt
var text = script.aiComplete("Tell me how do you feel today?");
```

Puede que quiera echar un vistazo al ejemplo [ai-autocompletion.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/ai-autocompletion.qml).

Comprobar si la plataforma es Linux, OS X o Windows
------------------------------------------------

### Llamada y parámetros del método
```cpp
bool ScriptingService::platformIsLinux();
bool ScriptingService::platformIsOSX();
bool ScriptingService::platformIsWindows();
```

### Ejemplo
```js
if (script.platformIsLinux()) {
    // Solo se ejecutará si se está en Linux
}
```

Etiquetar la nota actual
--------------------

### Llamada y parámetros del método
```cpp
/**
  * Etiqueta la nota actual con una etiqueta llamada tagName
  *
  * @param tagName
  */
void ScriptingService::tagCurrentNote(QString tagName);
```

### Ejemplo
```js
// agrega una etiqueta de "favorito" a la nota actual
script.tagCurrentNote("favorite");
```

You might want to look at the custom action `favoriteNote` in the example [favorite-note.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/favorite-note.qml).

Crear o recuperar una etiqueta por su nombre lista de ruta de navegación
-------------------------------------------------

### Llamada y parámetros del método
```cpp
/**
  * Obtiene o crea una etiqueta por su "lista de ruta de navegación" de nombres de etiquetas
  * Element nameList [0] sería el más alto en el árbol (con parentId: 0)
  *
  * @param nameList
  * @param createMissing {bool} si es verdadero (predeterminado) se crearán todas las etiquetas faltantes
  * @return TagApi objeto de la etiqueta más profunda de la lista de ruta de navegación de nombres
  */
TagApi *ScriptingService::getTagByNameBreadcrumbList(
    const QStringList &nameList, bool createMissing);
```

### Ejemplo
```js
// crea todas las etiquetas hasta el tercer nivel y devuelve el objeto de etiqueta para
// etiqueta "level3", que se vería así en el árbol de etiquetas:
// nivel1 > nivel2 > nivel3
var tag = script.getTagByNameBreadcrumbList (["nivel1", "nivel2", "nivel3"]);
```

Buscar etiquetas por nombre
-----------------------

### Llamada y parámetros del método
```cpp
/**
  * Obtiene todas las etiquetas haciendo una búsqueda de subcadenas en el campo de nombre
  *
  * @param name {QString} nombre para buscar
  * @return {QStringList} lista de nombres de etiquetas
  */
QStringList ScriptingService::searchTagsByName(QString name);
```

### Ejemplo
```js
// busca todas las etiquetas que contengan el juego de palabras var tags = script.searchTagsByName("game");
```

Puede que quiera echar un vistazo al ejemplo [autocompletion.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/autocompletion.qml).

Buscar notas por texto de nota
-----------------------------

### Llamada y parámetros del método
```cpp
/**
  * Devuelve una lista de ID de nota de todas las notas con un texto determinado en el texto de la nota
  *
  * Desafortunadamente, no hay una manera fácil de usar QList<NoteApi*> en QML, por lo que
  * solo se pueden transferir los ID de las notas
  *
  * @return {QList<int>} lista de ID de notas
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

Puede que quiera echar un vistazo al ejemplo [unique-note-id.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/unique-note-id.qml).

Agregar una hoja de estilo personalizada
-----------------------

### Llamada y parámetros del método
```cpp
/**
 * Agrega una hoja de estilo personalizada a la aplicación.
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

Puede que quiera echar un vistazo al ejemplo [custom-stylesheet.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-stylesheet.qml).

Puede obtener los nombres de los widgets a partir de los archivos `*.ui`. Por ejemplo, la ventana principal se encuentra en [mainwindow.ui](https://github.com/pbek/QOwnNotes/blob/main/src/mainwindow.ui).

La documentación de Qt (por ejemplo [QMainWindow](https://doc.qt.io/qt-5/qmainwindow.html)) puede ayudarle a visualizar cómo los widgets se relacionan entre sí (busque `Hereda` en las páginas).

El widget base para casi todo es [QWidget](https://doc.qt.io/qt-5/qwidget.html). Así que simplemente diseña `QWidget` con, por ejemplo, `QWidget {background-color: black; color: blanco;` significaría que todo tiene un color de fondo negro y un color de primer plano blanco.

::: tip
The [style.qss](https://github.com/pbek/QOwnNotes/blob/main/src/libraries/qdarkstyle/style.qss) of [qdarkstyle](https://github.com/pbek/QOwnNotes/blob/main/src/libraries/qdarkstyle) might also be a good reference for styles you can change.
:::

Take a look at [Style Sheet Reference](http://doc.qt.io/qt-5/stylesheet-reference.html) for a reference of what styles are available.

Si desea inyectar estilos en la vista previa de html para alterar la forma en que se previsualizan las notas, por favor eche un vistazo a [notetomarkdownhtmlhook](hooks.html#notetomarkdownhtmlhook).

::: tip
If you actually want to see how the dialogs look and what their names are you could download [Qt Creator](https://www.qt.io/product/development-tools) and open the `*.ui` files in it.
:::

Recargar el motor de secuencias de órdenes
------------------------------

### Llamada y parámetros del método
```cpp
/**
 * Recarga el motor de secuencias de órdenes
 */
void ScriptingService::reloadScriptingEngine();
```

### Ejemplo
```js
// recarga el motor de secuencias de órdenes
script.reloadScriptingEngine();
```

Obtener una nota por su nombre de archivo
--------------------------------

### Llamada y parámetros del método
```cpp
/**
  * Obtiene una nota por su nombre de archivo
  *
  * @param fileName cadena el nombre del archivo de la nota (obligatorio)
  * @param noteSubFolderId ID entero de la subcarpeta de notas
  * @return NoteApi *
  */
NoteApi * ScriptingService :: fetchNoteByFileName (QString fileName,
                                                 int noteSubFolderId);
```

### Ejemplo
```js
// buscar nota por nombre de archivo
script.fetchNoteByFileName("my note.md");
```

Obtener una nota por su ID
-------------------------

### Llamada y parámetros del método
```cpp
/**
 * Obtiene una nota por su identificador
 *
 * @param id int el identificador de la nota
 * @return NoteApi*
 */
NoteApi* ScriptingService::fetchNoteById(int id);
```

### Ejemplo
```js
// buscar nota por id
script.fetchNoteById(243);
```

Puede que quiera echar un vistazo al ejemplo [export-notes-as-one-html.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/export-notes-as-one-html.qml).

Comprobar si una nota existe por su nombre de archivo
------------------------------------------

### Llamada y parámetros del método
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

Puede que quiera echar un vistazo al ejemplo [use-tag-names-in-filename.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/use-tag-names-in-filename.qml).

Copiar texto en el portapapeles
-------------------------------

### Llamada y parámetros del método
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
// copiar texto al portapapeles
script.setClipboardText("text to copy");
```

Puede que quiera echar un vistazo al ejemplo [selected-markdown-to-bbcode.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/selected-markdown-to-bbcode.qml).

Ir a una nota
-----------------

### Llamada y parámetros del método
```cpp
/**
 * Sets the current note if the note is visible in the note list
 *
 * @param note NoteApi note to jump to
 * @param asTab bool if true the note will be opened in a new tab (if not already open)
 */
void ScriptingService::setCurrentNote(NoteApi *note, bool asTab = false);
```

### Ejemplo
```js
// jump to the note
script.setCurrentNote(note);

// open note in new tab (if not already open)
script.setCurrentNote(note, true);
```

Puede que quiera echar un vistazo al ejemplo [journal-entry.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/journal-entry.qml).

Saltar a una subcarpeta de notas
---------------------------

### Llamada y parámetros del método
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

### Llamada y parámetros del método
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

Mostrar un cuadro de mensaje de pregunta
------------------------------

### Llamada y parámetros del método
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
// mostrar un cuadro de mensaje de pregunta con una solicitud y un botón de ayuda
// ver: https://doc.qt.io/qt-5/qmessagebox.html#StandardButton-enum
var result = script.questionMessageBox(
"El texto que quiero mostrar", "Algún título opcional", 0x01000000|0x02000000, 0x02000000);
script.log(result);
```

Para obtener información sobre los botones, consulte [Botón estándar](https://doc.qt.io/qt-5/qmessagebox.html#StandardButton-enum).

Puede que quiera echar un vistazo al ejemplo [input-dialogs.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/input-dialogs.qml).

Mostrar un cuadro de diálogo de archivo abierto
---------------------------

### Llamada y parámetros del método
```cpp
/**
  * Muestra un cuadro de diálogo para abrir un archivo
  *
 * @param caption (opcional)
 * @param dir (opcional)
 * @param filter (opcional)
 * @return QString
 */
QString ScriptingService::getOpenFileName(QString caption, QString dir,
                                            QString filter);
```

### Ejemplo
```js
// mostrar un diálogo de archivo abierto
var fileName = script.getOpenFileName("Por favor seleccione una imagen", "/home/user/images", "Images (*.png *.xpm *.jpg)");
```

Mostrar un cuadro de diálogo para guardar archivo
--------------------------

### Llamada y parámetros del método
```cpp
/**
 * Muestra un cuadro de diálogo para guardar un archivo
 *
 * @param caption (opcional)
 * @param dir (opcional)
 * @param filter (opcional)
 * @return QString
 */
QString ScriptingService::getSaveFileName(QString caption, QString dir,
                                            QString filter);
```

### Ejemplo
```js
// mostrar un cuadro de diálogo para guardar archivos
var fileName = script.getSaveFileName("Seleccione el archivo HTML para guardar", "output.html", "HTML (*.html)");
```

Puede que quiera echar un vistazo al ejemplo [export-notes-as-one-html.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/export-notes-as-one-html.qml).

Registro de variables de configuración de script
-------------------------------------

Debe definir sus variables de configuración como propiedades en su script y registrarlas en una propiedad llamada `settingsVariables`.

Luego, el usuario puede establecer estas propiedades en la configuración del script.

### Ejemplo
```js
// you have to define your registered variables so you can access them later
property string myString;
property string myStringSecret;
property bool myBoolean;
property string myText;
property int myInt;
property string myFile;
property string myDirectory;
property string mySelection;

// register your settings variables so the user can set them in the script settings
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
        "identifier": "myStringSecret",
        "name": "I am a password field",
        "description": "Please enter a valid string:",
        "type": "string-secret",
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
        "identifier": "myDirectory",
        "name": "I am a directory selector",
        "description": "Please select the directory:",
        "type": "directory",
        "default": "/home",
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

In addition, you can override the `settingsVariables` with a special function `registerSettingsVariables()` like this:

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

Puede que quiera echar un vistazo al ejemplo [variables.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/variables.qml).

Almacenar y cargar variables persistentes
----------------------------------------

### Llamada y parámetros del método
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
// almacenar variable persistente
script.setPersistentVariable("PersistentVariablesTest/myVar", result);

// cargar y registrar variable persistente
script.log(script.getPersistentVariable("PersistentVariablesTest/myVar", "no hay nada todavía"));
```

Asegúrese de usar un prefijo significativo en su clave como `PersistentVariablesTest / myVar` porque las variables son accesibles desde todos los scripts.

Puede que quiera echar un vistazo al ejemplo [persistent-variables.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/persistent-variables.qml).

Cargando variables de configuración de la aplicación
--------------------------------------

### Llamada y parámetros del método
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

Crear un directorio de caché
--------------------------

Puede almacenar en caché los archivos en la ubicación de caché predeterminada de su sistema.

### Llamada y parámetros del método
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

Sie können das Cache-Verzeichnis Ihres Skripts löschen, indem Sie seinen Namen an clearCacheDir() übergeben.

### Llamada y parámetros del método
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
// borrar el directorio de caché de my-script-id
script.clearCacheDir("my-script-id");
```

Leer la ruta al directorio de su script
------------------------------------------------

Si necesita obtener la ruta al directorio donde se coloca su script para, por ejemplo, cargar otros archivos, debe registrar un `cadena de propiedad scriptDirPath;`. Esta propiedad se establecerá con la ruta al directorio del script.

### Ejemplo
```js
importar QtQml 2.0
importar QOwnNotesTypes 1.0

Script {
     // la ruta al directorio del script se establecerá aquí
     property string scriptDirPath;

     function init () {
         script.log (scriptDirPath);
     }
}
```

Conversión de separadores de ruta en nativos
-----------------------------------------

### Llamada y parámetros del método
```cpp
/**
 * Devuelve la ruta con los separadores '/' convertidos en separadores que son
 * apropiados para el sistema operativo subyacente.
 *
 * En Windows, toNativeDirSeparators("c:/winnt/system32") devuelve
 * "c:\winnt\system32".
 *
 * @param path
 * @return
 */
QString ScriptingService::toNativeDirSeparators(QString path);
```

### Ejemplo
```js
// Devolverá "c:\winnt\system32" en Windows
script.log(script.toNativeDirSeparators("c:/winnt/system32"));
```

Conversión de separadores de ruta de las nativas
-------------------------------------------

### Llamada y parámetros del método
```cpp
/**
 * Devuelve la ruta usando '/' como separador de archivos.
 * En Windows, por ejemplo, fromNativeDirSeparators("c:\\winnt\\system32")
 * devuelve "c:/winnt/system32".
 *
 * @param path
 * @return
 */
QString ScriptingService::fromNativeDirSeparators(QString path);
```

### Ejemplo
```js
// Devolverá "c:/winnt/system32" en Windows
script.log(script.fromNativeDirSeparators("c:\\winnt\\system32"));
```

Obtener el separador de directorios nativo
--------------------------------------

### Llamada y parámetros del método
```cpp
/**
 * Devuelve el separador de directorio nativo "/" o "\" en Windows
 *
 * @return
 */
QString ScriptingService::dirSeparator();
```

### Ejemplo
```js
// Devolverá "\" en Windows
script.log(script.dirSeparator());
```

Obtener una lista de las rutas de todas las notas seleccionadas
-------------------------------------------------

### Llamada y parámetros del método
```cpp
/**
 * Devuelve una lista de las rutas de todas las notas seleccionadas
 *
 * @return {QStringList} lista de rutas de las notas seleccionadas
 */
QStringList ScriptingService::selectedNotesPaths();
```

### Ejemplo
```js
// Devuelve una lista de las rutas de todas las notas seleccionadas
script.log(script.selectedNotesPaths());
```

Puede que quiera echar un vistazo al ejemplo [external-note-diff.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/external-note-diff.qml).

Obtener una lista de los identificadores de todas las notas seleccionadas
-----------------------------------------------

### Llamada y parámetros del método
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
// devuelve una lista de los id de todas las notas seleccionadas
script.log(script.identificadores de notas seleccionadas());
```

Puede que quiera echar un vistazo al ejemplo [export-notes-as-one-html.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/export-notes-as-one-html.qml).

Activar una acción del menú
------------------------

### Llamada y parámetros del método
```cpp
/**
 * Activa una acción del menú
 *
 * @param objectName {QString} nombre del objeto de la acción a invocar
 * @param checked {QString} solamente invocar la acción si el estado verificado es
 *                          diferente a este parámetro (opcional, puede ser 0 o 1)
 */
void ScriptingService::triggerMenuAction(QString objectName, QString checked);
```

### Ejemplo
```js
// alterna el modo de solo lectura
script.triggerMenuAction("actionAllow_note_editing");

// deshabilita el modo de solo lectura
script.triggerMenuAction("actionAllow_note_editing", 1);
```

Puede que quiera echar un vistazo al ejemplo [disable-readonly-mode.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/disable-readonly-mode.qml).

::: tip
You can get the object names of the menu action from [mainwindow.ui](https://github.com/pbek/QOwnNotes/blob/main/src/mainwindow.ui). Simplemente busque el título del menú en inglés. Tenga en cuenta que estos textos pueden cambiar con el tiempo.
:::

Opening an input dialog with a select box
-----------------------------------------

### Llamada y parámetros del método
```cpp
/**
  * Abre un cuadro de diálogo de entrada con un cuadro de selección
  *
  * @param title {QString} título del diálogo
  * @param label {QString} texto de la etiqueta del diálogo
  * @param items {QStringList} lista de elementos para seleccionar
  * @param índice actual {int} del elemento que debe seleccionarse (predeterminado: 0)
  * @param editable {bool} si es verdadero, el texto del cuadro de diálogo se puede editar (predeterminado: falso)
  * @return {QString} texto del elemento seleccionado
  */
QString ScriptingService::inputDialogGetItem(
         const QString &título, const QString &etiqueta, const QStringList &elementos,
         int actual, bool editable);
```

Se devolverá una cadena vacía si se pulsa en `Cancelar` o se presiona `Escape`.

### Ejemplo
```js
var result = script.inputDialogGetItem(
    "combo box", "Please select an item", ["Item 1", "Item 2", "Item 3"]);
script.log(result);
```

Puede que quiera echar un vistazo al ejemplo [input-dialogs.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/input-dialogs.qml).

Abrir un diálogo de entrada con una edición de línea
----------------------------------------

### Llamada y parámetros del método
```cpp
/**
 * Abre un cuadro de diálogo de entrada con una edición de línea.
 *
 * @param title {QString} título del diálogo
 * @param label {QString} texto de etiqueta del diálogo
 * @param text {QString} texto en el diálogo (opcional)
 * @return
 */
QString ScriptingService::inputDialogGetText(
        const QString &title, const QString &label, const QString &text);
```

Se devolverá una cadena vacía si se pulsa en `Cancelar` o se presiona `Escape`.

### Ejemplo
```js
var result = script.inputDialogGetText(
    "line edit", "Por favor ingrese un nombre", "current text");
script.log(result);
```

Abrir un diálogo de entrada con un editor de texto multirrenglón
---------------------------------------------------

### Llamada y parámetros del método
```cpp
/**
 * Opens an input dialog with a multi-line text edit
 *
 * @param title {QString} title of the dialog
 * @param label {QString} label text of the dialog
 * @param text {QString} text in the dialog (optional)
 * @return
 */
QString ScriptingService::inputDialogGetMultiLineText(
        const QString &title, const QString &label, const QString &text);
```

Se devolverá una cadena vacía si se pulsa en `Cancelar` o se presiona `Escape`.

### Ejemplo
```js
var result = script.inputDialogGetMultiLineText(
    "multi-line edit", "Please enter a text", "current text");
script.log(result);
```

Opening a dialog to show the differences between two texts
----------------------------------------------------------

### Llamada y parámetros del método
```cpp
/**
* Opens a dialog to show the differences between two texts and lets the user edit the result
*
* @param title {QString} title of the dialog
* @param label {QString} label text of the dialog
* @param text1 {QString} first text
* @param text2 {QString} second text
* @return
  */
  QString ScriptingService::textDiffDialog(const QString &title, const QString &label,
                                           const QString &text1, const QString &text2);
```

`text2` is the text you will be able to edit in the dialog. An empty string will be returned, if `Cancel` was clicked or `Escape` was pressed.

### Ejemplo
```js
const text = script.noteTextEditSelectedText();
const aiPrompt = "Translate the text to English";
const aiResult = script.aiComplete(aiPrompt + ":\n\n" + text);

var result = script.textDiffDialog("AI Text Tool", "Resulting text", text, aiResult);
script.log(result);
```

Comprobar si un archivo existe
-------------------------

### Llamada y parámetors del método
```cpp
/**
 * Comprueba si existe un archivo
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

### Llamada y parámetros del método
```cpp
/**
 * Lee texto de un archivo
 *
 * @param filePath {QString} ruta del archivo a cargar
 * @param codec {QString} codificación del archivo (por defecto: UTF-8)
 * @return los datos del archivo o null si el archivo no existe
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

### Llamada y parámetors del método
```cpp
/**
 * Escribe un texto en un archivo
 *
 * @param filePath {QString}
 * @param data {QString}
 * @param createParentDirs {bool} opcional (por defecto: falso)
 * @return
 */
bool ScriptingService::writeToFile(const QString &filePath, const QString &data, bool createParentDirs);
```

### Ejemplo
```js
var result = script.writeToFile(filePath, html);
script.log(result);
```

Puede que quiera echar un vistazo al ejemplo [export-notes-as-one-html.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/export-notes-as-one-html.qml).

Trabajar con zócalos web
-----------------------

Puede controlar QOwnNotes de forma remota utilizando `WebSocketServer`.

Por favor eche un vistazo al ejemplo [websocket-server.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/websocket-server.qml). Puede probar el servidor de zócalos conectándose a él en [Prueba de zócalos web](https://www.websocket.org/echo.html?location=ws://127.0.0.1:35345).

También puede escuchar los zócalos con `WebSocket`. Por favor eche un vistazo al ejemplo [websocket-client.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/websocket-client.qml).

Tenga en cuenta que debe tener instalada la biblioteca QML `websocket` de Qt para usar esto. Por ejemplo, en Ubuntu Linux puede instalar `qml-module-qtwebsockets`.

Agregar una regla de resaltado para el editor
-----------------------------------------

Puede inyectar directamente reglas de resaltado en el editor definiendo reglas regulares expresiones y asignarles un estado de resaltado.

### Method call and parameters
```cpp
/**
 * Agrega una regla de resaltado al resaltador de sintaxis del editor
 *
 * @param pattern {QString} the regular expression pattern to highlight
 * @param shouldContain {QString} a string that must be contained in the highlighted text for the pattern to be parsed
 * @param state {int} the state of the syntax highlighter to use
 * @param capturingGroup {int} the capturing group for the pattern to use for highlighting (default: 0)
 * @param maskedGroup {int} the capturing group for the pattern to use for masking (default: 0)
 */
void ScriptingService::addHighlightingRule(const QString &pattern,
                                           const QString &shouldContain,
                                           int state,
                                           int capturingGroup,
                                           int maskedGroup);
```

### Estados del resaltador

| Nombre                     | No. |
| -------------------------- | --- |
| NoState                    | -1  |
| Enlace                     | 0   |
| Imagen                     | 3   |
| CodeBlock                  | 4   |
| CodeBlockComment           | 5   |
| Cursiva                    | 7   |
| Negrita                    | 8   |
| Liza                       | 9   |
| Comentario                 | 11  |
| H1                         | 12  |
| H2                         | 13  |
| H3                         | 14  |
| H4                         | 15  |
| H5                         | 16  |
| H6                         | 17  |
| Cita en bloque             | 18  |
| HorizontalRuler            | 21  |
| Tabla                      | 22  |
| InlineCodeBlock            | 23  |
| MaskedSyntax               | 24  |
| CurrentLineBackgroundColor | 25  |
| BrokenLink                 | 26  |
| FrontmatterBlock           | 27  |
| TrailingSpace              | 28  |
| CheckBoxUnChecked          | 29  |
| CheckBoxChecked            | 30  |
| StUnderline                | 31  |

### Example
```js
// Highlight a text line like "BLOCK: some text" as blockquote (state 18)
script.addHighlightingRule("^BLOCK: (.+)", "BLOCK:", 18);

// Mask out (state 24) all characters after 32 characters in a line
// capturingGroup 1 means the expression from the first bracketed part of the pattern will be highlighted
// maskedGroup -1 means that no masking should be done
script.addHighlightingRule("^.{32}(.+)", "", 24, 1, -1);
```

También puede echar un vistazo a los ejemplos en [highlighting.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/highlighting.qml).
