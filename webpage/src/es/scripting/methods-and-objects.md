# Métodos y objetos que proporciona QOwnNotes

## Iniciar un programa externo en segundo plano

### Llamada y parámetros del método

```cpp
/**
  * Envoltorio de QML para iniciar un proceso separado
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

## Iniciar un programa externo y esperar la salida

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
var resultado = script.startSynchronousProcess(
  "/ruta/a/mi/programa", 
  ["mi parámetro"], 
  "datos", 
  "/ruta/en/la/cual/ejecutar"
);
```

Puede que quiera echar un vistazo al ejemplo [encryption-keybase.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/encryption-keybase.qml).

## Obtener la ruta de la carpeta de notas actual

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

## Obtener la nota actual

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

## Iniciar sesión en el widget de registro

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

## Descarga de una URL a una cadena

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

## Descarga de una URL a la carpeta multimedia

### Llamada y parámetros del método

```cpp
/**
 * Envoltorio de QML para descargar una URL a la carpeta multimedia y devolver la URL
 * de los medios o el texto de la imagen de Markdown de los medios relativo a la nota actual
 *
 * @param {QString} URL
 * @param {bool} returnUrlOnly si es verdadero, solo se devolverá la URL del medio (por defecto es falso)
 * @return {QString} el Markdown o URL de los medios
 */
QString downloadUrlToMedia(QUrl url, bool returnUrlOnly);
```

### Ejemplo

```js
var markdown = script.downloadUrlToMedia(
  "http://latex.codecogs.com/gif.latex?\frac{1}{1+sin(x)}",
);
```

Puede que quiera echar un vistazo al ejemplo [paste-latex-image.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/paste-latex-image.qml).

## Insertar un archivo multimedia en la carpeta multimedia

### Llamada y parámetros del método

```cpp
/**
  * Envoltorio de QML para insertar un archivo multimedia en la carpeta multimedia y regresar
  * la URL de medios o el texto de la imagen de Markdown de los medios en relación con la nota actual
  *
  * @param {QString} mediaFilePath
  * @param {bool} returnUrlOnly si es verdadero, solo se devolverá la URL del medio (por defecto es falso)
  * @return {QString} el Markdown o URL de los medios
  */
QString ScriptingService::insertMediaFile (QString mediaFilePath,
                                         bool returnUrlOnly);
```

### Ejemplo

```js
var markdown = script.insertMediaFile("/ruta/a/tu/imagen.png");
```

Puede que quiera echar un vistazo al ejemplo [scribble.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/scribble.qml).

## Insertar un archivo adjunto en la carpeta de archivos adjuntos

### Llamada y parámetros del método

```cpp
 * Envoltorio de QML para instartar un archivo adjunto en la carpeta `attachments`
 * devolviendo la URL del adjunto o el texto de Markdown del adjunto
 * relativo a la nota actual
 *
 * @param {QString} attachmentFilePath
 * @param {QString} fileName para usar en el Markdown
 * @param {bool} returnUrlOnly si es verdadero, solo se devolverá la URL del adjunto
 * (por defecto es falso)
 * @return {QString} el Markdown o URL de los medios
 */
QString ScriptingService::insertAttachmentFile(const QString &attachmentFilePath,
                                               const QString &fileName,
                                               bool returnUrlOnly);
```

### Ejemplo

```js
var markdown = script.insertAttachmentFile("/ruta/a/tu/archivo.png");
```

## Regenerar la vista previa de la nota

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

## Registro de una acción personalizada

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
 * @param useInNoteEditContextMenu si es verdadero, usar la acción del menú contextual
 * de la edición de la nota (por defecto: falso)
 * @param hideButtonInToolbar si es verdadero, el botón no se mostrará en el
 * barra de herramientas de acciones personalizadas (por defecto: falso)
 * @param useInNoteListContextMenu si es verdadero, usar la acción del menú contextual 
 * de la lista de la nota (por defecto: falso)
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
También puede asignar accesos directos locales y globales a sus acciones personalizadas en la _Configuración de accesos directos_.
:::

::: warning
Tenga en cuenta que los [iconos de los temas de escritorio gratuitos](https://specifications.freedesktop.org/icon-naming-spec/icon-naming-spec-latest.html) están disponibles en su mayoría solamente en Linux. Entonces, si realmente desea usar un icono en macOS o Windows, debe proporcionar uno con su script. Para obtener la ruta de su secuencia de comandos para establecer una ruta adecuada para su icono, puede usar la propiedad [scriptDirPath](methods-and-objects.md#reading-the-path-to-the-directory-of-your-script).
:::

### Ejemplo

```js
import QtQml 2.0
import QOwnNotesTypes 1.0

Script {
    /**
     * Inicializar las acciones personalizadas
     */
    function init() {
        // añadir una acción personalizada sin un botón
        script.registerCustomAction("miaccionpersonalizada1", "Texto del menú");

        // añadir una acción personalizada con un botón
        script.registerCustomAction("miaccionpersonalizada2", "Texto del menú", "Texto del botón");

        // añadir una acción personalizada con un botón y un icono de un tema de freedesktop
        script.registerCustomAction("miaccionpersonalizada3", "Texto del menú", "Texto del botón", "task-new");

        // añadir una acción personalizada con un botón y un icono desde un archivo
        script.registerCustomAction("miaccionpersonalizada4", "Texto del menú", "Texto del botón", "/usr/share/icons/breeze/actions/24/view-calendar-tasks.svg");
    }

    /**
     * Esta función se invoca cuando se desencadena una acción personalizada
     * en el menú o por medio de un botón
     *
     * @param identifier string el identificador definido en registerCustomAction
     */
    function customActionInvoked(identifier) {
        switch (identifier) {
            case "miaccionpersonalizada1":
                script.log("Acción 1");
            break;
            case "miaccionpersonalizada2":
                script.log("Acción 2");
            break;
            case "miaccionpersonalizada3":
                script.log("Acción 3");
            break;
            case "miaccionpersonalizada4":
                script.log("Acción 4");
            break;
        }
    }
}
```

Para más ejemplos, por favor revise [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-actions.qml).

::: tip
También puede activar una acción personalizada después de iniciar la aplicación con el parámetro `--action customAction_<identifier>`. Para mayor información, por favor consulte [Activar acciones del menú después del inicio](../getting-started/cli-parameters.md#trigger-menu-actions-after-startup).
:::

## Registrar una etiqueta

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
script.registerLabel(
  "etiqueta-html",
  "Texto HTML <strong>fuerte</strong> <br />con tres líneas<br />y un <a href='https://www.qownnotes.org'>enlace a un sitio web</a>.",
);

script.registerLabel(
  "etiqueta-larga",
  "otro texto muy largo, otro texto muy largo, otro texto muy largo, otro texto muy largo, otro texto muy largo, otro texto muy largo, otro texto muy largo, otro texto muy largo, otro texto muy largo, otro texto muy largo, otro texto muy largo que se envolverá",
);

script.registerLabel("contraetiqueta");
```

Las etiquetas serán visibles en el _Panel de secuencia de órdenes_, el cual necesita habilitar en el menú _Ventanas/Paneles_.

Puede utilizar texto sin formato o html en las etiquetas. Se podrá seleccionar el texto y se podrá hacer clic en los enlaces.

Puede que después quiera echar un vistazo al script de ejemplo [scripting-label-demo.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/scripting-label-demo.qml).

## Configurar el texto de una etiqueta registrada

### Llamada y parámetros del método

```cpp
/**
 * Establece el texto de una etiqueta registrada
 *
 * @param identifier el identificador de la etiqueta
 * @param text el texto que se muestra en la etiqueta
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
La secuencia de órdenes también soporta **enlaces externos**, así como **enlaces internos** en notas, tareas, barajas de tarjetas y más. Es posible que comprenda lo que se admite tomando un vistazo al [manejo de URL](https://github.com/pbek/QOwnNotes/blob/964acf69b6382f8ee8252c640c5048f8f4644622/src/utils/urlhandler.cpp#L16-L75).
:::

## Crear una nota nueva

### Llamada y parámetros del método

```cpp
/**
 * Crea una nota nueva
 *
 * @param text el texto de la nota
 */
void ScriptingService::createNote(QString text);
```

### Ejemplo

```js
script.createNote("Titulo de mi nota\n===\n\nMi texto");
```

Puede que quiera echar un vistazo al ejemplo [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-actions.qml).

::: tip
Si desactivó que el título de su nota determine el nombre del archivo de la nota, debe luego renombrar archivo de nota por su cuenta de la siguiente manera:

```js
var nota = script.currentNote();
nota.renameNoteFile("su-nombre-de-archivo");
```

:::

## Acceder al portapapeles

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

## Escribir texto en la edición de texto de la nota

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
// escribir texto en la edición de texto de la nota
script.noteTextEditWrite("Mi texto personalizado");
```

Puede que quiera echar un vistazo a la acción personalizada `transformTextRot13` en el ejemplo [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-actions.qml).

Puede usar esto junto con `noteTextEditSelectAll` para sobrescribir todo el texto de la nota actual.

## Leer el texto seleccionado en la edición de texto de la nota

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
// leer el texto seleccionado desde la edición de texto de la nota
var text = script.noteTextEditSelectedText();
```

Puede que quiera echar un vistazo a la acción personalizada `transformTextRot13` en el ejemplo [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-actions.qml).

## Seleccionar todo el texto en la edición de texto de la nota

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

## Seleccionar la línea actual en la edición de texto de la nota

### Llamada y parámetros del método

```cpp
/**
 * Selecciona la línea actual en la edición de texto de la nota
 */
void ScriptingService::noteTextEditSelectCurrentLine();
```

### Ejemplo

```js
script.noteTextEditSelectCurrentLine();
```

## Seleccionar la palabra actual en la edición de texto de la nota

### Llamada y parámetros del método

```cpp
/**
 * Selecciona la palabra actual en la edición de texto de la nota
 */
void ScriptingService::noteTextEditSelectCurrentWord();
```

### Ejemplo

```js
script.noteTextEditSelectCurrentWord();
```

## Establecer el texto actualmente seleccionado en la edición de texto de nota

### Llamada y parámetros del método

```cpp
/**
 * Establece el texto seleccionado actualmente en la edición de texto de la nota
 *
 * @param start
 * @param end
 */
void ScriptingService::noteTextEditSetSelection(int start, int end);
```

### Ejemplo

```js
// expande la selección actual en un carácter
script.noteTextEditSetSelection(
  script.noteTextEditSelectionStart() - 1,
  script.noteTextEditSelectionEnd() + 1,
);
```

## Obtener la posición inicial de la selección actual en la edición de texto de la nota

### Llamada y parámetros del método

```cpp
/**
 * Devuelve la posición inicial de la selección actual en la edición de texto de la nota
 */
int ScriptingService::noteTextEditSelectionStart();
```

### Ejemplo

```js
script.log(script.noteTextEditSelectionStart());
```

## Obtener la posición final de la selección actual en la edición de texto de la nota

### Llamada y parámetros del método

```cpp
/**
 * Devuelve la posición final de la selección actual en la edición de texto de la nota
 */
int ScriptingService::noteTextEditSelectionEnd();
```

### Ejemplo

```js
script.log(script.noteTextEditSelectionEnd());
```

## Establecer el cursor de texto en la edición de texto de la nota en una posición determinada

### Llamada y parámetros del método

```cpp
/**
 * Establece el cursor de texto en la edición de texto de la nota en una posición determinada
 * 0 sería el comienzo de la nota
 * caso especial: -1 sería el final de la nota
 *
 * @param position
 */
void ScriptingService::noteTextEditSetCursorPosition(int position);
```

### Ejemplo

```js
// salta al undécimo carácter de la nota
script.noteTextEditSetCursorPosition(10);

// salta al final de la nota
script.noteTextEditSetCursorPosition(-1);
```

## Obtener la posición actual del cursor de texto en la edición de texto de la nota

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

## Leer la palabra actual desde el editor de texto de notas

### Llamada y parámetros del método

```cpp
/**
  * Lee la palabra actual en la edición de texto de la nota
  *
  * @param withPreviousCharacters también obtiene más caracteres al principio
  *                               para obtener caracteres como "@" que no son
  *                               caracteres de palabra
  * @return
  */
QString ScriptingService::noteTextEditCurrentWord(bool withPreviousCharacters);
```

### Ejemplo

```js
// lee la palabra actual en la edición de texto de la nota
var text = script.noteTextEditCurrentWord();
```

Puede que quiera echar un vistazo al ejemplo [autocompletion.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/autocompletion.qml).

## Leer el bloque actual desde la edición de texto de la nota

### Llamada y parámetros del método

```cpp
/**
 * Lee el bloque actual en la edición de texto de la nota
 *
 * @return
 */
QString ScriptingService::noteTextEditCurrentBlock();
```

### Ejemplo

```js
// Lee el bloque actual en la edición de texto de la nota
var text = script.noteTextEditCurrentBlock();
```

Puede que quiera echar un vistazo al ejemplo [ai-autocompletion.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/ai-autocompletion.qml).

## Usar un prompt de finalización en el modelo de IA seleccionado actualmente

El prompt de finalización de IA es una función que le permite utilizar un mensaje de finalización en el modelo de IA seleccionado actualmente, como ChatGPT, Claude o sistemas como Ollama.

El sistema de IA necesita ser activado en la barra de herramientas de IA o en el menú principal para que esto funcione.

### Llamada y parámetros del método

```cpp
/**
 * Envoltorio de QML para usar el Finalizador de IA
 *
 * @param prompt
 * @return {QString} el resultado del finalizador
 */
QString ScriptingService::aiComplete(const QString& prompt);
```

### Ejemplo

```js
// Solicita al modelo de IA seleccionado actualmente que complete el prompt
var text = script.aiComplete("Dime, ¿cómo te sientes hoy?");
```

Puede que quiera echar un vistazo al ejemplo [ai-autocompletion.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/ai-autocompletion.qml).

## Comprobar si la plataforma es Linux, OS X o Windows

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

## Etiquetar la nota actual

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
// agrega una etiqueta "favorito" a la nota actual
script.tagCurrentNote("favorito");
```

Puede que quiera echar un vistazo a la acción personalizada `favoriteNote` en el ejemplo [favorite-note.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/favorite-note.qml).

## Crear o recuperar una etiqueta por su nombre lista de ruta de navegación

### Llamada y parámetros del método

```cpp
/**
  * Obtiene o crea una etiqueta por su "lista de ruta de navegación" de nombres de etiquetas
  * Element nameList [0] sería el más alto en el árbol (con parentId: 0)
  *
  * @param nameList
  * @param createMissing {bool} si es verdadero (por defecto) se crearán todas las etiquetas faltantes
  * @return TagApi objeto de la etiqueta más profunda de la lista de ruta de navegación de nombres
  */
TagApi *ScriptingService::getTagByNameBreadcrumbList(
    const QStringList &nameList, bool createMissing);
```

### Ejemplo

```js
// crea todas las etiquetas hasta el tercer nivel y devuelve el objeto de etiqueta para
// la etiqueta "nivel3", que se vería así en el árbol de etiquetas:
// nivel1 > nivel2 > nivel3
var tag = script.getTagByNameBreadcrumbList (["nivel1", "nivel2", "nivel3"]);
```

## Buscar etiquetas por nombre

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
// busca todas las etiquetas que contengan la palabra juego en ellas
var tags = script.searchTagsByName("juego");
```

Puede que quiera echar un vistazo al ejemplo [autocompletion.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/autocompletion.qml).

## Buscar notas por texto de nota

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
var noteIds = script.fetchNoteIdsByNoteTextPart("mitexto");

noteIds.forEach(function (noteId){
  var nota = script.fetchNoteById(noteId);

  // hacer algo más con la nota
});
```

Puede que quiera echar un vistazo al ejemplo [unique-note-id.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/unique-note-id.qml).

## Agregar una hoja de estilo personalizada

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

El widget base para casi todo es [QWidget](https://doc.qt.io/qt-5/qwidget.html). Así que simplemente estilizar `QWidget` con, por ejemplo, `QWidget {background-color: black; color: blanco;` significaría que todo tiene un color de fondo negro y un color de primer plano blanco.

::: tip
El [style.qss](https://github.com/pbek/QOwnNotes/blob/main/src/libraries/qdarkstyle/style.qss) de [qdarkstyle](https://github.com/pbek/QOwnNotes/blob/main/src/libraries/qdarkstyle) también podría ser una buena referencia para los estilos que puede cambiar.
:::

Eche un vistazo a la [Hoja de referencia de estilo](http://doc.qt.io/qt-5/stylesheet-reference.html) para obtener una referencia de los estilos que se encuentran disponibles.

Si desea inyectar estilos en la vista previa de html para alterar la forma en que se previsualizan las notas, por favor eche un vistazo a [notetomarkdownhtmlhook](hooks.html#notetomarkdownhtmlhook).

::: tip
Si realmente desea ver cómo se ven los cuadros de diálogo y cuáles son sus nombres puede descargar [Qt Creator](https://www.qt.io/product/development-tools) y abrir los archivos `*.ui` en él.
:::

## Recargar el motor de secuencias de órdenes

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

## Obtener una nota por su nombre de archivo

### Llamada y parámetros del método

```cpp
/**
  * Obtiene una nota por su nombre de archivo
  *
  * @param fileName string el nombre del archivo de la nota (obligatorio)
  * @param noteSubFolderId integer ID de la subcarpeta de notas
  * @return NoteApi *
  */
NoteApi * ScriptingService::fetchNoteByFileName (QString fileName,
                                                 int noteSubFolderId);
```

### Ejemplo

```js
// busca nota por nombre de archivo
script.fetchNoteByFileName("mi nota.md");
```

## Obtener una nota por su ID

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
// busca nota por ID
script.fetchNoteById(243);
```

Puede que quiera echar un vistazo al ejemplo [export-notes-as-one-html.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/export-notes-as-one-html.qml).

## Comprobar si una nota existe por su nombre de archivo

### Llamada y parámetros del método

```cpp
/**
  * Comprueba si existe un archivo de notas por su nombre de archivo
  *
  * @param fileName string el nombre del archivo de la nota (obligatorio)
  * @param ignoreNoteId integer ID de una nota para ignorar en el cheque
  * @param noteSubFolderId integer ID de la subcarpeta de notas
  * @return bool
  */
bool ScriptingService::noteExistsByFileName(QString fileName,
                                            int ignoreNoteId,
                                            int noteSubFolderId);
```

### Ejemplo

```js
// verifica si la nota existe, pero ignora el ID de "note"
script.noteExistsByFileName("mi nota.md", note.id);
```

Puede que quiera echar un vistazo al ejemplo [use-tag-names-in-filename.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/use-tag-names-in-filename.qml).

## Copiar texto en el portapapeles

### Llamada y parámetros del método

```cpp
/**
 * Copia texto en el portapapeles como texto sin formato o datos MIME HTML
 *
 * @param text string texto a poner en el portapapeles
 * @param asHtml bool si es verdadero, el texto se establecerá como datos MIME HTML
 */
void ScriptingService::setClipboardText(QString text, bool asHtml);
```

### Ejemplo

```js
// copia texto al portapapeles
script.setClipboardText("texto a copiar");
```

Puede que quiera echar un vistazo al ejemplo [selected-markdown-to-bbcode.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/selected-markdown-to-bbcode.qml).

## Ir a una nota

### Llamada y parámetros del método

```cpp
/**
 * Establece la nota actual si la nota es visible en la lista de notas
 *
 * @param note NoteApi nota a la que ir
 * @param asTab bool si es verdadero, la nota será abierta en una nueva pestaña (si no está ya abierta)
 */
void ScriptingService::setCurrentNote(NoteApi *note, bool asTab = false);
```

### Ejemplo

```js
// ir a la nota
script.setCurrentNote(note);

// abrir nota en una nueva pestaña (si no está ya abierta)
script.setCurrentNote(note, true);
```

Puede que quiera echar un vistazo al ejemplo [journal-entry.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/journal-entry.qml).

## Saltar a una subcarpeta de notas

### Llamada y parámetros del método

```cpp
/**
  * Salta a una subcarpeta de notas
  *
  * @param noteSubFolderPath {QString} ruta de la subcarpeta, relativa a la carpeta de notas
  * @param separator {QString} separador entre partes de la ruta, por defecto "/"
  * @return verdadero si el salto fue exitoso
  */
bool ScriptingService::jumpToNoteSubFolder(const QString &noteSubFolderPath,
                                            QString separator);
```

### Ejemplo

```js
// salta a la subcarpeta de notas "una subcarpeta"
script.jumpToNoteSubFolder("una subcarpeta");

// salta a la subcarpeta de notas "sub" dentro de "una subcarpeta"
script.jumpToNoteSubFolder("una subcarpeta/sub");
```

::: tip
Puede crear una nueva subcarpeta de notas en la subcarpeta actual llamando a [`mainWindow.createNewNoteSubFolder`](classes.html#example-2).
:::

## Mostrar un cuadro de mensaje de información

### Llamada y parámetros del método

```cpp
/**
 * Muestra un cuadro de mensaje de información
 *
 * @param text
 * @param title (opcional)
 */
void ScriptingService::informationMessageBox(QString text, QString title);
```

### Ejemplo

```js
// muestra un cuadro de mensaje de información
script.informationMessageBox ("El texto que quiero mostrar", "Algún título opcional");
```

## Mostrar un cuadro de mensaje de pregunta

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
  * @param buttons botones que deben mostrarse (opcional)
  * @param defaultButton botón predeterminado que se seleccionará (opcional)
  * @return id del botón presionado
  */
int ScriptingService::questionMessageBox (
         QString text, QString title, int buttons, int defaultButton);
```

### Ejemplo

```js
// mostrar un cuadro de mensaje de pregunta con una solicitud y un botón de ayuda
// vease: https://doc.qt.io/qt-5/qmessagebox.html#StandardButton-enum
var resultado = script.questionMessageBox(
  "El texto que quiero mostrar",
  "Algún título opcional",
  0x01000000|0x02000000,
  0x02000000,
);
script.log(resultado);
```

Para obtener información sobre los botones, consulte [StandardButton](https://doc.qt.io/qt-5/qmessagebox.html#StandardButton-enum).

Puede que quiera echar un vistazo al ejemplo [input-dialogs.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/input-dialogs.qml).

## Mostrar un cuadro de diálogo de archivo abierto

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
var nombreDeArchivo = script.getOpenFileName(
  "Por favor seleccione una imagen",
  "/home/user/images",
  "Images (*.png *.xpm *.jpg)",
);
```

## Mostrar un cuadro de diálogo para guardar un archivo

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
var fileName = script.getSaveFileName(
  "Seleccione el archivo HTML para guardar",
  "output.html",
  "HTML (*.html)",
);
```

Puede que quiera echar un vistazo al ejemplo [export-notes-as-one-html.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/export-notes-as-one-html.qml).

## Variables de la configuración del script de registro

Debe definir sus variables de configuración como propiedades en su script y registrarlas en una propiedad llamada `settingsVariables`.

Luego, el usuario puede establecer estas propiedades en la configuración del script.

### Ejemplo

```js
// debe definir sus variables registradas para que pueda acceder a ellas más tarde
property string miCadena;
property string miCadenaSecreta;
property bool miBooleano;
property string miTexto;
property int miEntero;
property string miArchivo;
property string miDirectorio;
property string miSelección;

// registre sus variables de configuración para que el usuario pueda establecerlas en la configuración del script
//
// desafortunadamente QVariantHash no existe en Qt, solamente podemos usar
// QVariantMap (que no tiene un orden arbitrario) o QVariantList (que por
// lo menos puede ser ordenado arbitrariamente)
property variant settingsVariables: [
    {
        "identifier": "miCadena",
        "name": "Soy una edición de línea",
        "description": "Por favor introduzca una cadena válida:",
        "type": "string",
        "default": "Mi valor predeterminado",
    },
    {
        "identifier": "miCadenaSecreta",
        "name": "Soy un campo de contraseña",
        "description": "Por favor introduzca una cadena válida:",
        "type": "string-secret",
    },
    {
        "identifier": "miBooleano",
        "name": "Soy una casilla",
        "description": "Algo de descripción",
        "text": "Marque esta casilla",
        "type": "boolean",
        "default": true,
    },
    {
        "identifier": "miTexto",
        "name": "Soy una caja de texto",
        "description": "Por favor introduzca su texto:",
        "type": "text",
        "default": "Este puede ser un texto realmente muy largo\ncon múltiples líneas.",
    },
    {
        "identifier": "miEntero",
        "name": "Soy un selector de número",
        "description": "Por favor introduzca un número:",
        "type": "integer",
        "default": 42,
    },
    {
        "identifier": "miArchivo",
        "name": "Soy un selector de archivo",
        "description": "Por favor seleccione el archivo:",
        "type": "file",
        "default": "pandoc",
    },
    {
        "identifier": "miDirectorio",
        "name": "Soy un selector de directorio",
        "description": "Por favor seleccione el directorio:",
        "type": "directory",
        "default": "/home",
    },
    {
        "identifier": "miSelección",
        "name": "Soy un selector de elemento",
        "description": "Por favor seleccione un elemento:",
        "type": "selection",
        "default": "option2",
        "items": {"option1": "Texto para opción 1", "option2": "Texto para opción 2", "option3": "Texto para opción 3"},
    }
];
```

Además, puede sobreescribir `settingsVariables` con una función especial `registerSettingsVariables ()` como esta:

### Ejemplo

```js
/**
 * Registra las variables de configuración nuevamente
 *
 * Use este método si desea usar código para anular sus variables, como al establecer
 * valores predeterminados dependiendo del sistema operativo.
 */
function registerSettingsVariables() {
  if (script.platformIsWindows()) {
    // sobreescribir el valor predeterminado de miArchivo
    settingsVariables[3].default = "pandoc.exe"
  }
}
```

Puede que quiera echar un vistazo al ejemplo [variables.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/variables.qml).

## Almacenar y cargar variables persistentes

### Llamada y parámetros del método

```cpp
/**
 * Guarda una variable persistente
 * Estas variables son accesibles globalmente en todos los scripts
 * Por favor use un prefijo significativo en su clave, como "PersistentVariablesTest/myVar"
 *
 * @param key {QString}
 * @param value {QVariant}
 */
void ScriptingService::setPersistentVariable(const QString &key,
                                             const QVariant &value);

/**
 * Carga una variable persistente
 * Estas variables son accesibles globalmente en todos los scripts
 *
 * @param key {QString}
 * @param defaultValue {QVariant} return valor si la configuración no existe (opcional)
 * @return
 */
QVariant ScriptingService::getPersistentVariable(const QString &key,
                                                 const QVariant &defaultValue);
```

### Ejemplo

```js
// almacenar una variable persistente
script.setPersistentVariable("PruebaDeVariablesPersistentes/miVariable", result);

// cargar y registrar una variable persistente
script.log(
  script.getPersistentVariable(
    "PruebaDeVariablesPersistentes/miVariable",
    "no hay nada todavía",
  ),
);
```

Asegúrese de usar un prefijo significativo en su clave como `PersistentVariablesTest/myVar` porque las variables son accesibles desde todos los scripts.

Puede que quiera echar un vistazo al ejemplo [persistent-variables.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/persistent-variables.qml).

## Cargar variables de configuración de la aplicación

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

Tenga en cuenta que la configuración en realidad puede estar vacía. Debe ocuparse de eso por su propia cuenta. `defaultValue` solo se usa si la configuración no existe en absoluto.

## Crear un directorio de caché

Puede almacenar en caché los archivos en la ubicación de caché predeterminada de su sistema.

### Llamada y parámetros del método

```cpp
/**
 * Devuelve un directorio de caché para un script
 *
 * @param {QString} subDir la subcarpeta a crear y usar
 * @return {QString} la ruta del directorio de caché
 */
QString ScriptingService::cacheDir(const QString &subDir) const;
```

### Ejemplo

```js
// crear el directorio de caché para my-script-id
var cacheDirForScript = script.cacheDir("my-script-id");
```

## Borrar un directorio de caché

Puede borrar el directorio de caché de su script pasando su nombre a clearCacheDir().

### Llamada y parámetros del método

```cpp
/**
 * Borra el directorio de caché de un script
 *
 * @param {QString} subDir la subcarpeta a limpiar
 * @return {bool} verdadero en caso de éxito
 */
bool ScriptingService::clearCacheDir (const QString & subDir) const;
```

### Ejemplo

```js
// borrar el directorio de caché de mi-ID-de-script
script.clearCacheDir("mi-ID-de-script");
```

## Leer la ruta al directorio de su script

Si necesita obtener la ruta al directorio donde se coloca su script para por ejemplo, cargar otros archivos, debe registrar una `cadena de propiedad scriptDirPath;`. Esta propiedad se establecerá con la ruta al directorio del script.

### Ejemplo

```js
import QtQml 2.0
import QOwnNotesTypes 1.0

Script {
     // la ruta al directorio del script se establecerá aquí
     property string scriptDirPath;

     function init () {
         script.log (scriptDirPath);
     }
}
```

## Conversión de separadores de ruta en nativos

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

## Conversión de separadores de ruta a partir de los nativos

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

## Obtener el separador de directorios nativo

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

## Obtener una lista de las rutas de todas las notas seleccionadas

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

## Obtener una lista de los ID de todas las notas seleccionadas

### Llamada y parámetros del método

```cpp
/**
 * Regresa una lista de todos los ID de todas las notas seleccionadas
 *
 * @return {QList<int>} lista de los ID de las notas seleccionadas
 */
QList<int> ScriptingService::selectedNotesIds();
```

### Ejemplo

```js
// devuelve una lista de los ID de todas las notas seleccionadas
script.log(script.selectedNotesIds());
```

Puede que quiera echar un vistazo al ejemplo [export-notes-as-one-html.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/export-notes-as-one-html.qml).

## Activar una acción del menú

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
Puede obtener los nombres de los objetos de la acción del menú desde [mainwindow.ui](https://github.com/pbek/QOwnNotes/blob/main/src/mainwindow.ui). Simplemente busque el título del menú en inglés. Tenga en cuenta que estos textos pueden cambiar con el tiempo.
:::

## Abrir un diálogo de entrada con un cuadro de selección

### Llamada y parámetros del método

```cpp
/**
  * Abre un cuadro de diálogo de entrada con un cuadro de selección
  *
  * @param title {QString} título del diálogo
  * @param label {QString} texto de la etiqueta del diálogo
  * @param items {QStringList} lista de elementos para seleccionar
  * @param current {int} índice actual del elemento que debe estar seleccionado (predeterminado: 0)
  * @param editable {bool} si es verdadero, el texto del cuadro de diálogo puede ser editado (predeterminado: falso)
  * @return {QString} texto del elemento seleccionado
  */
QString ScriptingService::inputDialogGetItem(
         const QString &título, const QString &etiqueta, const QStringList &elementos,
         int actual, bool editable);
```

Se devolverá una cadena vacía si se pulsa en `Cancelar` o se presiona `Escape`.

### Ejemplo

```js
var resultado = script.inputDialogGetItem("cuadro combinado", "Por favor seleccione un elemento", [
  "Elemento 1",
  "Elemento 2",
  "Elemento 3",
]);
script.log(resultado);
```

Puede que quiera echar un vistazo al ejemplo [input-dialogs.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/input-dialogs.qml).

## Abrir un diálogo de entrada con una edición de línea

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
var resultado = script.inputDialogGetText(
  "edición de línea",
  "Por favor ingrese un nombre",
  "texto actual",
);
script.log(resultado);
```

## Abrir un diálogo de entrada con un editor de texto multirrenglón

### Llamada y parámetros del método

```cpp
/**
 * Abre un cuadro de diálogo de entrada con una edición de texto multilínea.
 *
 * @param title {QString} título del diálogo
 * @param label {QString} texto de etiqueta del diálogo
 * @param text {QString} texto en el diálogo (opcional)
 * @return
 */
QString ScriptingService::inputDialogGetMultiLineText(
        const QString &title, const QString &label, const QString &text);
```

Se devolverá una cadena vacía si se pulsa en `Cancelar` o se presiona `Escape`.

### Ejemplo

```js
var resultado = script.inputDialogGetMultiLineText(
  "edición multi línea",
  "Por favor ingrese un texto",
  "texto actual"
);
script.log(resultado);
```

## Abrir un diálogo para mostrar las diferencias entre dos textos

### Llamada y parámetros del método

```cpp
/**
* Abre un diálogo para mostrar las diferencias entre dos textos y permite que el usuario edite el resultado
*
* @param title {QString} título del diálogo
* @param label {QString} texto de etiqueta del diálogo
* @param text1 {QString} primer texto
* @param text2 {QString} segundo texto
* @return
  */
  QString ScriptingService::textDiffDialog(const QString &title, const QString &label,
                                           const QString &text1, const QString &text2);
```

`text2` es el texto que será capaz de editar en el diálogo. Se devolverá una cadena vacía si se pulsa en `Cancelar` o se presiona `Escape`.

### Ejemplo

```js
const texto = script.noteTextEditSelectedText();
const promptIA = "Traduce el texto a inglés";
const resultadoIA = script.aiComplete(promptIA + ":\n\n" + texto);

var resultado = script.textDiffDialog(
  "Herramienta de texto con IA",
  "Texto resultante",
  texto,
  resultadoIA,
);
script.log(resultado);
```

## Comprobar si un archivo existe

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

## Leer texto de un archivo

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
if(script.fileExists(rutaArchivo)){
  var datos = script.readFromFile(rutaArchivo);
  script.log(datos);
}
```

## Escribir texto en un archivo

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

## Trabajar con zócalos web

Puede controlar QOwnNotes de forma remota utilizando `WebSocketServer`.

Por favor eche un vistazo al ejemplo [websocket-server.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/websocket-server.qml). Puede probar el servidor de zócalos conectándose a él en [Prueba de zócalos web](https://www.websocket.org/echo.html?location=ws://127.0.0.1:35345).

También puede escuchar los zócalos con `WebSocket`. Por favor eche un vistazo al ejemplo [websocket-client.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/websocket-client.qml).

Tenga en cuenta que debe tener instalada la biblioteca QML `websocket` de Qt para usar esto. Por ejemplo, en Ubuntu Linux puede instalar `qml-module-qtwebsockets`.

## Agregar una regla de resaltado para el editor

Puede inyectar directamente reglas de resaltado en el editor definiendo expresiones regulares y asignándolas a un estado de resaltado.

### Llamada y parámetros del método

```cpp
/**
 * Agrega una regla de resaltado al resaltador de sintaxis del editor
 *
 * @param pattern {QString} el patrón de expresión regular a ser subrayado
 * @param shouldContain {QString} string una cadena que el texto subrayado debe contener para que el patrón sea analizado
 * @param state {int} el estado del resaltador de sintaxis que será usado
 * @param capturingGroup {int} el grupo de captura del patrón que se utilizará para resaltar (por defecto: 0)
 * @param maskedGroup {int} el grupo de captura del patrón que se utilizará para el enmascaramiento (por defecto: 0)
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
| HorizontalRule            | 21  |
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

### Ejemplo

```js
// Subraya una línea de texto como "BLOQUE: algún texto" como cita en bloque (estado 18)
script.addHighlightingRule("^BLOQUE: (.+)", "BLOQUE:", 18);

// Enmascara (estado 24) todos los caracteres después de 32 caracteres en una línea
// capturingGroup 1 significa que la expresión de la primera parte entre corchetes del patrón se resaltará
// maskedGroup -1 significa que no se debería hacer ningún enmascaramiento
script.addHighlightingRule("^.{32}(.+)", "", 24, 1, -1);
```

También puede echar un vistazo a los ejemplos en [highlighting.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/highlighting.qml).
