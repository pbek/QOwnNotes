# Ganchos

## onNoteStored

### Llamada y parámetros del método

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

Puede que quiera echar un vistazo al ejemplo [on-note-opened.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/on-note-opened.qml).

## noteOpenedHook

### Llamada y parámetros del método

```js
/**
  * Esta función se llama después de que se abrió una nota
  *
  * @param {NoteApi} note: el objeto de nota que se abrió
 */
function noteOpenedHook(note);
```

Puede que quiera echar un vistazo al ejemplo [on-note-opened.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/on-note-opened.qml).

## noteDoubleClickedHook

### Llamada y parámetros del método

```js
/**
  * Esta función se llama después de hacer doble clic en una nota
  *
  * @param {NoteApi} nota: el objeto de nota en el que se hizo clic
  */
function noteDoubleClickedHook(note);
```

Puede que quiera echar un vistazo al ejemplo [external-note-open.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/external-note-open.qml).

## insertMediaHook

Esta función se llama cuando se inserta un archivo multimedia en la nota actual.

Si esta función se define en varios scripts, entonces gana el primer script que devuelve una cadena no vacía.

### Llamada y parámetros del método

```js
/**
 * @param fileName cadena la ruta del archivo de medios de origen antes de que se copiara a la carpeta de medios
 * @param markdownText cadena el texto de Markdown del archivo multimedia, p. ej. ! [mi-imagen] (media/mi-imagen-4101461585.jpg)
 * @ return string el nuevo texto de Markdown del archivo multimedia
 */
function insertMediaHook (fileName, markdownText);
```

Puede que quiera echar un vistazo al ejemplo [example.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/example.qml).

## insertAttachmentHook

Esta función se llama cuando se inserta un archivo adjunto en la nota actual.

Si esta función se define en varios scripts, entonces gana el primer script que devuelve una cadena no vacía.

### Llamada y parámetros del método

```js
/**
 * @param fileName string la ruta del archivo adjunto de origen antes de que se copiara a la carpeta de adjuntos
 * @param markdownText string el texto del archivo adjunto en Markdown, p. ej. [mi-archivo.txt] (attachments/mi-archivo-4245650967.txt)
 * @return string el nuevo texto del archivo adjunto en Markdown
 */
function insertAttachmentHook (fileName, markdownText);
```

Puede que quiera echar un vistazo al ejemplo [example.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/example.qml).

## insertingFromMimeDataHook

### Llamada y parámetros del método

```js
/**
 * Esta función se llama cuando se pega html o un archivo multimedia en una nota con `Ctrl + Shift + V`
 *
 * @param text texto del objeto QMimeData
 * @param html html del objeto QMimeData
 * @return la cadena que debería ser insertada en lugar del texto del objeto QMimeData
 */
function insertingFromMimeDataHook(text, html);
```

Puede que quiera echar un vistazo al ejemplo [example.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/example.qml), [insert-headline-with-link-from-github-url.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/insert-headline-with-link-from-github-url.qml) o [note-text-from-5pm-mail.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/note-text-from-5pm-mail.qml).

## handleNoteTextFileNameHook

### Llamada y parámetros del método

```js
/**
 * Esta función se llama cuando una nota se almacena en el disco si
 * "Permitir que el nombre del archivo de nota sea diferente del título" está habilitado
 * en la configuración
 *
 * Le permite modificar el nombre del archivo de notas
 * ¡Tenga en cuenta que debe preocuparse por los nombres duplicados por su propia cuenta!
 *
 * Devuelve una cadena vacía si el nombre del archivo de la nota
 * no debe modificarse
*
 * @param {NoteApi} note: el objeto de nota de la nota almacenada
 * @return {string} el nombre del archivo de la nota
 */
function handleNoteTextFileNameHook (note);
```

Puede que quiera echar un vistazo al ejemplo [example.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/example.qml) o [use-tag-names-in-filename.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/use-tag-names-in-filename.qml).

## handleNoteNameHook

### Llamada y parámetros del método

```js
/**
 * Esta función se llama cuando se determina el nombre de la nota para una nota
 *
 * Le permite modificar el nombre de la nota que se visualiza
 *
 * Devuelve una cadena vacía si el nombre de la nota no debe modificarse
 *
 * @param {NoteApi} nota: el objeto de nota de la nota almacenada
 * @return {string} el nombre de la nota
 */
function handleNoteNameHook(note);
```

El nombre de la nota se utiliza principalmente cuando se muestra la nota en las listas.

Puede que quiera echar un vistazo al ejemplo [example.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/example.qml).

Puede que no sea una buena idea usar este gancho si la configuración para usar el nombre de archivo como nombre de nota está activa.

## handleNewNoteHeadlineHook

### Llamada y parámetros del método

```js
/**
 * Esta función se llama antes de que se cree una nota.
 *
* Le permite modificar el título de la nota antes de crearla
 * Tenga en cuenta que debe tener cuidado con un nombre de nota único, de lo contrario
 * la nueva nota no se creará, solo se encontrará en la lista de notas
 *
 * Puede usar esta función para crear plantillas de notas
 *
 * @param headline texto que sería usado para crear el título
 * @return {string} el título de la nota
 */
function handleNewNoteHeadlineHook(headline);
```

Puede que quiera echar un vistazo al ejemplo [custom-new-note-headline.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-new-note-headline.qml).

## preNoteToMarkdownHtmlHook

### Llamada y parámetros del método

```js
/**
 * Esta función se llama antes de que se genere el HTML de Markdown de una nota
 *
 * Le permite modificar lo que pasa al convertidor de Markdown a HTML
 *
 * La función se puede usar, por ejemplo, en múltiples secuencias de órdenes para rasterizar código (como LaTeX math o mermaid)
 * a su representación gráfica para la vista previa
 *
 * La nota no será modificada en este proceso
 *
 * @param {NoteApi} note - el objeto de la nota
 * @param {string} markdown - el Markdown que está a punto de ser convertido a HTML
 * @param {bool} forExport - verdadero si el HTML es usado para un export, falso para la vista previa
 * @return {string} el Markdown modificado o una cadena vacía si nada debe ser modificado
 */
function preNoteToMarkdownHtmlHook(note, markdown, forExport);
```

Puede que quiera echar un vistazo al ejemplo [preview-styling.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/preview-styling.qml).

## noteToMarkdownHtmlHook

### Llamada y parámetros del método

```js
/**
 * Esta función se llama cuando se genera el HTML de Markdown de una nota
 *
 * Le permite modificar este HTML
 * Esto es, por ejemplo, llamado antes por la vista previa de la nota
 *
 * La función se puede usar en múltiples secuencias de órdenes para modificar el HTML de la vista previa
 *
 * @param {NoteApi} note - el objeto de la nota
 * @param {string} html - el HTML que está a punto de ser renderizado
 * @param {bool} forExport - verdadero si el HTML es usado para un export, falso para la vista previa
 * @return {string} el HTML modificado o una cadena vacía si nada debe ser modificado
 */
function noteToMarkdownHtmlHook(note, html, forExport);
```

Puede que quiera echar un vistazo al ejemplo [example.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/example.qml) o [preview-styling.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/preview-styling.qml).

Por favor consule la documentación del [Subconjunto de HTML compatible ](http://doc.qt.io/qt-5/richtext-html-subset.html) para obtener una lista de todos los estilos de CSS admitidos.

## encryptionHook

### Llamada y parámetros del método

```js
/**
 * Esta función se llama cuando el texto debe cifrarse o descifrarse
 *
 * @param text cadena el texto para cifrar o descifrar
 * @param contraseña cadena la contraseña
 * @param decrypt bool si es falso, se exige el cifrado; si es verdadero se exige el descifrado
 * @return el texto cifrado/descifrado
 */
function encryptionHook(text, password, decrypt);
```

Puede que quiera echar un vistazo al ejemplo [encryption-keybase.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/encryption-keybase.qml), [encryption-pgp.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/encryption-pgp.qml) o [encryption-rot13.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/encryption-rot13.qml).

## noteTaggingHook

Puede implementar su propio mecanismo de etiquetado de notas, por ejemplo, con texto especial en su nota, como `@etiqueta1`, `@etiqueta2`, `@etiqueta3`.

### Llamada y parámetros del método

```js
/**
 * Maneja el etiquetado de una nota
 *
 * Esta función se llama cuando las etiquetas se agregan, eliminan o renombran en
 * debe incluirse una nota o las etiquetas de una nota
 *
 * @param nota
 * @param action puede ser "add" (agregar), "remove" (eliminar), "rename" (renombrar) o "list" (listar)
 * @param tagName nombre de la etiqueta que se agregará, eliminará o cambiará de nombre
 * @param newTagName nombre de la etiqueta a la que se le cambiará el nombre si action = "rename"
 * @return cadena de texto o lista de cadenas de nombres de etiquetas de la nota (si action = "list")
 */
function noteTaggingHook(note, action, tagName, newTagName);
```

- tan pronto como se active un script que implemente la nueva función `noteTaggingHook`, el etiquetado de notas será manejado por esa función
- las siguientes funciones deberían funcionar a través de la interfaz de usuario de QOwnNotes
  - inicialmente importando etiquetas como `@etiqueta` de sus notas y sobrescribiendo su asignación de etiqueta actual
    - no perderá su árbol de etiquetas, solo la asignación anterior a las notas
    - aún puede mover etiquetas a otras etiquetas
    - si más de una etiqueta tiene el mismo nombre en su árbol de etiquetas, se asignará el primer resultado
  - agregar una etiqueta a una nota agregará la etiqueta al texto de la nota
  - eliminar una etiqueta de una nota eliminará la etiqueta del texto de la nota
  - la eliminación de etiquetas en la lista de etiquetas eliminará esas etiquetas de sus notas
  - el renombramiento de las etiquetas en la lista de etiquetas renombrará esas etiquetas en sus notas
  - el etiquetado masivo de notas en la lista de notas agregará esas etiquetas a sus notas
  - la eliminación masiva de etiquetas de notas en la lista de notas eliminará esas etiquetas de sus notas
  - la aplicación activará una serie de acciones `agregar` y `eliminar` para todas las etiquetas seleccionadas y sus hijos en todas las notas si las etiquetas se mueven en el panel de etiquetas

Puede que quiera echar un vistazo al ejemplo [note-tagging.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/note-tagging.qml) para implementar su mecanismo de etiquetamiento propio.

::: warning
Make sure your `list` action is really fast, because it will be executed for every note every time the note folder or the tag tree is reloaded!

That is every time you find `reloadTagTree` in [mainwindow.cpp](https://github.com/pbek/QOwnNotes/blob/main/src/mainwindow.cpp).
:::

## noteTaggingByObjectHook

De manera similar a [noteTaggingHook](#notetagginghook), puede implementar su propio mecanismo de etiquetado de notas, pero no está obligado a etiquetar nombres en la raíz del árbol de etiquetas. De esta forma, puede utilizar todo el árbol de etiquetas en lugar de solo una lista de etiquetas.

Con `noteTaggingByObjectHook`, obtiene un objeto `TagApi` como parámetro, en lugar de un nombre de etiqueta. Y como resultado de la acción `list`, debe proporcionar una lista de identificadores de etiquetas.

Esto también significa que debe crear usted mismo las etiquetas faltantes para poder proporcionar una lista de los identificadores de etiquetas ya existentes para la acción `list`.

### Llamada y parámetros del método

```js
/**
  * Maneja el etiquetado de notas para una nota
  *
  * Esta función se llama cuando las etiquetas se agregan, eliminan o renombran en
  * debe incluirse una nota o las etiquetas de una nota
  *
  * @param nota
  * La acción @param puede ser "agregar", "eliminar", "renombrar" o "listar"
  * Etiqueta @param para agregar, eliminar o renombrar
  * @param newTagName nombre de la etiqueta a la que se le cambiará el nombre si action = "rename"
  * @return nota cadena de texto o lista de cadenas de identificadores de etiquetas (si acción = "lista")
  */
function noteTaggingByObjectHook(note, action, tag, newTagName);
```

Puede que quiera echar un vistazo al ejemplo [on-note-opened.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/note-tagging-by-object.qml) para implementar su mecanismo de etiquetado propio.

## autocompletionHook

Puede devolver una lista de cadenas para agregar a la lista de finalización automática cuando se invoca la finalización automática (por ejemplo, presionando <kbd>Ctrl + Espacio</kbd>).

### Llamada y parámetros del método

```js
/**
 * Llama a la función autocompletionHook para todos los componentes del script
 * Esta función se llama cuando se invoca la finalización automática en una nota
 *
 * @return QStringList de texto para la lista de finalización automática
 */
function callAutocompletionHook();
```

Puede que quiera echar un vistazo al ejemplo [autocompletion.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/autocompletion.qml).

## websocketRawDataHook

Este gancho se llama cuando se envían datos desde la extensión del navegador QOwnNotes Web Companion a través del menú contextual del navegador web.

### Llamada y parámetros del método

```js
/**
 * @param requestType puede ser "página" o "selección"
 * @param pageUrl la URL de la página web donde se realizó la solicitud
 * @param pageTitle el título de la página web donde se realizó la solicitud
 * @param rawData los datos que se transmitieron, HTML para requestType "página" o texto sin formato para requestType "selección"
 * @param screenshotDataUrl la URL de datos de la captura de pantalla de la página web donde se realizó la solicitud
 * @return verdadero si los datos fueron manejados por un gancho
 */
function callHandleWebsocketRawDataHook(requestType, pageUrl, pageTitle, rawData, screenshotDataUrl);
```

Puede que quiera echar un vistazo a los ejemplos [websocket-raw-data-new-note.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/websocket-raw-data-new-note.qml) y [websocket-raw-data-selection-in-note.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/websocket-raw-data-selection-in-note.qml).

## onDetachedProcessCallback

Este gancho se llama cuando un hilo de script de [startDetachedProcess](methods-and-objects.html#starting-an-external-program-in-the-background) termina de ejecutarse.

### Llamada y parámetros del método

```js
/**
  * Esta función se llama cuando se termina de ejecutar un hilo de script.
 * Sugerencia: thread[1]==0 ayuda a determinar si se realiza una gran parte de los procesos iniciados para un determinado identificador.
 *
  * @param {QString} callbackIdentifier: la identificación proporcionada al llamar a startDetachedProcess ()
  * @param {QString} resultSet - el resultado del proceso
  * @param {QVariantList} cmd: la matriz de comandos completa [0-ejecutablePath, 1-parameters, 2-exitCode]
  * @param {QVariantList} hilo: la matriz de información del hilo [0-pass callbackParameter, 1-hilos restantes para este identificador]
 */
function onDetachedProcessCallback(callbackIdentifier, resultSet, cmd, thread);
```

Puede que quiera echar un vistazo al ejemplo [callback-example.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/callback.qml).

## windowStateChangedHook

### Llamada y parámetros del método

```js
/**
* Esta función se llama después de que se activara un evento WindowStateChange
  *
  * @param {QString} windowState: el nuevo estado de la ventana, el valor del parámetro puede ser "minimizado", "maximizado", "pantalla completa", "activo" o "nostate"
  */
function windowStateChangedHook (windowState);
```

Puede que quiera echar un vistazo al ejemplo [window-state-changed.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/window-state-changed.qml).

## workspaceSwitchedHook

Este gancho se llama cuando se cambian los espacios de trabajo.

### Llamada y parámetros del método

```js
/**
 * Esta función se llama cuando se cambian los espacios de trabajo
 *
 * @param oldUuid uuid actual del espacio de trabajo
 * @param newUuid uuid nuevo del espacio de trabajo
 */
function workspaceSwitchedHook(oldUuid, newUuid);
```

Puede que quiera echar un vistazo al ejemplo [websocket-raw-data-new-note.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/workspaces.qml).

## openAiBackendsHook

Este gancho se llama cuando el servicio de OpenAI se vuelve a cargar. Por ejemplo, esto también sucede cuando el motor de scripts se vuelve a cargar.

Puede utilizarlo para proporcionar configuración para backends de OpenAI personalizados, como sus propios LLM compatibles con la API de OpenAI. Por ejemplo, ChatGPT, Claude o sistemas como Ollama.

### Llamada y parámetros del método

```js
/**
 * Esta función se llama cuando se recarga la configuración del servicio de OpenAI
 * Devuelve una lista de objetos con parámetros de configuración para nuevos motores de OpenAI
 */
function openAiBackendsHook() {
  return [
    {
      id: "my-ia-personalizada",
      name: "My IA personalizada",
      baseUrl: "http://localhost:5000",
      apiKey: "kDFJkjk3asdm",
      models: ["gpt-3.5-turbo", "gpt-4.0-turbo"],
    },
    {
      id: "mi-ia-personalizada2",
      name: "My IA personalizada 2",
      baseUrl: "http://localhost:5001",
      apiKey: "lOikf7eNdb9",
      models: ["gpt-3.5-turbo2", "gpt-4.0-turbo2"],
    },
  ];
}
```

::: tip
Puede usar las [variables de la configuración del script de registro](./methods-and-objects.md#registering-script-settings-variables) para almacenar la configuración del motor de OpenAI en la configuración del script.
:::

Puede que quiera echar un vistazo al ejemplo [custom-openai-backends.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-openai-backends.qml).
