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
  * @param fileName cadena la ruta del archivo de medios de origen antes de que se copiara a la carpeta de medios
  * @param markdownText cadena el texto de la rebaja del archivo multimedia, p. ej. ! [mi-imagen] (media / mi-imagen-4101461585.jpg)
  * @ return string el nuevo texto de rebaja del archivo multimedia
  */
function insertMediaHook (fileName, markdownText);
```

Es posible que desee echar un vistazo al ejemplo [example.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/example.qml).

insertAttachmentHook
--------------------

Esta función se llama cuando se inserta un archivo adjunto en la nota actual.

Si esta función se define en varios scripts, entonces gana el primer script que devuelve una cadena no vacía.

### Parámetros y llamada al método
```js
/**
  * @param fileName cadena la ruta del archivo adjunto de origen antes de que se copiara a la carpeta de adjuntos
  * @param markdownText cadena el texto de la rebaja del archivo adjunto, p. ej. [mi-archivo.txt] (adjuntos / mi-archivo-4245650967.txt)
  * @return cadena el nuevo texto de rebaja del archivo adjunto
  */
function insertAttachmentHook (fileName, markdownText);
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

### Parámetros y llamada al método
```js
/**
  * Esta función se llama cuando se determina el nombre de la nota para una nota
  *
  * Te permite modificar el nombre de la nota que se visualiza
  *
  * Devuelve una cadena vacía si el nombre de la nota no debe modificarse
  *
  * @param {NoteApi} nota: el objeto de nota de la nota almacenada
  * @return {string} el nombre de la nota
  */
function handleNoteNameHook(note);
```

Es posible que desee echar un vistazo al ejemplo [example.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/example.qml).

Puede que no sea una buena idea usar este gancho si la configuración para usar el nombre de archivo como nombre de nota está activa.

handleNewNoteHeadlineHook
-------------------------

### Parámetros y llamada al método
```js
/**
 * Esta función se llama antes de que se cree una nota.
 *
* Te permite modificar el titular de la nota antes de crearla
  * Tenga en cuenta que debe tener cuidado con un nombre de nota único, de lo contrario
  * la nueva nota no se creará, solo se encontrará en la lista de notas
  *
  * Puede usar esta función para crear plantillas de notas
 *
 * @param headline text that would be used to create the headline
 * @return {string} the headline of the note
 */
function handleNewNoteHeadlineHook(headline);
```

Es posible que desee echar un vistazo al ejemplo [título-nuevo-de-nota-personalizado.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-new-note-headline.qml).

preNoteToMarkdownHtmlHook
-------------------------

### Parámetros y llamada al método
```js
/**
  * Esta función se llama antes de que se genere el html de descuento de una nota
  *
  * Te permite modificar lo que se pasa al convertidor markdown to html
  *
  * La función se puede usar, por ejemplo, en múltiples scripts para generar código (como LaTeX math o mermaid)
  * a su representación gráfica para la vista previa
  *
  * La nota no se cambiará en este proceso
 *
 * @param {NoteApi} note - the note object
 * @param {string} markdown - the markdown that is about to being converted to html
 * @param {bool} forExport - true if the html is used for an export, false for the preview
 * @return {string} the modified markdown or an empty string if nothing should be modified
 */
function preNoteToMarkdownHtmlHook(note, markdown, forExport);
```

Es posible que desee echar un vistazo al ejemplo [preview-styling.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/preview-styling.qml).

noteToMarkdownHtmlHook
----------------------

### Parámetros y llamada al método
```js
/**
  * Esta función se llama cuando se genera el html de descuento de una nota
  *
  * Te permite modificar este html
  * Esto es, por ejemplo, llamado antes por la vista previa de la nota
  *
  * La función se puede usar en múltiples scripts para modificar el html de la vista previa
  *
 * @param {NoteApi} note - the note object
 * @param {string} html - the html that is about to being rendered
 * @param {bool} forExport - true if the html is used for an export, false for the preview
 * @return {string} the modified html or an empty string if nothing should be modified
 */
function noteToMarkdownHtmlHook(note, html, forExport);
```

Es posible que desee echar un vistazo al ejemplo [ejemplo.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/example.qml) o [preview-styling.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/preview-styling.qml).

Consulte el [HTML compatible. Documentación del subconjunto](http://doc.qt.io/qt-5/richtext-html-subset.html) para obtener una lista de todos los estilos CSS admitidos.

encryptionHook
--------------

### Parámetros y llamada al método
```js
/**
  * Esta función se llama cuando el texto debe cifrarse o descifrarse
  *
  * @param text cadena el texto para cifrar o descifrar
  * @param contraseña cadena la contraseña
  * @param decrypt bool si se exige un cifrado falso, si se exige un descifrado verdadero
  * @return el texto descifrado cifrado
  */
function encryptionHook(text, password, decrypt);
```

Es posible que desee echar un vistazo al ejemplo [encryption-keybase.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/encryption-keybase.qml), [encryption-pgp.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/encryption-pgp.qml) or [encryption-rot13.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/encryption-rot13.qml).

noteTaggingHook
---------------

Puede implementar su propio mecanismo de etiquetado de notas, por ejemplo, con texto especial en su nota como `@tag1`, `@tag2`, `@tag3`.

### Parámetros y llamada al método
```js
/**
  * Maneja el etiquetado de notas para una nota
  *
  * Esta función se llama cuando las etiquetas se agregan, eliminan o renombran en
  * debe incluirse una nota o las etiquetas de una nota
  *
  * @param nota
  * La acción @param puede ser "agregar", "eliminar", "renombrar" o "listar"
  * @param tagName nombre de la etiqueta que se agregará, eliminará o cambiará de nombre
  * @param newTagName nombre de la etiqueta a la que se le cambiará el nombre si action = "rename"
  * @return nota cadena de texto o lista de cadenas de nombres de etiquetas (si acción = "lista")
  */
function noteTaggingHook(note, action, tagName, newTagName);
```

-   tan pronto como se active un script que implemente la nueva función `noteTaggingHook` el etiquetado de notas será manejado por esa función
-   las siguientes funciones deberían funcionar a través de la interfaz de usuario de QOwnNotes
    -   inicialmente importando etiquetas como `@tag` de sus notas y sobrescribiendo su asignación de etiqueta actual
        -   no perderá su árbol de etiquetas, solo la asignación anterior a las notas
        -   aún puede mover etiquetas a otras etiquetas
        -   si más de una etiqueta tiene el mismo nombre en su árbol de etiquetas, se asignará el primer resultado
    -   agregar una etiqueta a una nota agregará la etiqueta al texto de la nota
    -   eliminar una etiqueta de una nota eliminará la etiqueta del texto de la nota
    -   la eliminación de etiquetas en la lista de etiquetas eliminará esas etiquetas de sus notas
    -   el cambio de nombre de las etiquetas en la lista de etiquetas cambiará el nombre de esas etiquetas en su notas
    -   el etiquetado masivo de notas en la lista de notas agregará esas etiquetas a sus notas
    -   la eliminación masiva de etiquetas de notas en la lista de notas eliminará esas etiquetas de sus notas
    -   la aplicación activará una serie de acciones `agregar` y `eliminar` para todas las etiquetas seleccionadas y sus hijos en todas las notas si las etiquetas se mueven en el panel de etiquetas

Es posible que desee echar un vistazo al ejemplo [note-tagging.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/note-tagging.qml) para implementar su propio mecanismo de etiquetado.

::: warning
Asegúrese de que su acción `list` sea realmente rápida, ¡porque se ejecutará para cada nota cada vez que se recargue la carpeta de notas!
:::

noteTaggingByObjectHook
----------------------

De manera similar a [noteTaggingHook](#notetagginghook), puede implementar su propio mecanismo de etiquetado de notas, pero no está obligado a etiquetar nombres en la raíz del árbol de etiquetas. De esta forma, puede utilizar todo el árbol de etiquetas en lugar de solo una lista de etiquetas.

Con `noteTaggingByObjectHook` obtienes un objeto `TagApi` como parámetro, en lugar de un nombre de etiqueta. Y como resultado de la acción `list`, debe proporcionar una lista de identificadores de etiquetas.

Esto también significa que debe crear usted mismo las etiquetas faltantes para poder proporcionar una lista de los identificadores de etiquetas ya existentes para la acción `list`.

### Parámetros y llamada al método
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

Es posible que desee echar un vistazo al ejemplo [etiquetado-de-notas-por-objeto.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/note-tagging-by-object.qml) para implementar su propio mecanismo de etiquetado.

autocompletionHook
------------------

Puede devolver una lista de cadenas para agregar a la lista de autocompletar cuando se invoca el autocompletado (por ejemplo, presionando <kbd>Ctrl + Espacio</kbd>).

### Parámetros y llamada al método
```js
/**
  * Llama a la función autocompletionHook para todos los componentes del script
  * Esta función se llama cuando se invoca el autocompletado en una nota
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
  * @param requestType puede ser "página" o "selección"
  * @param pageUrl la URL de la página web donde se realizó la solicitud
  * @param pageTitle el título de la página web donde se realizó la solicitud
  * @param rawData los datos que se transmitieron, html para requestType "página" o texto sin formato para requestType "selección"
  * @param screenshotDataUrl la URL de datos de la captura de pantalla si la página web donde se realizó la solicitud
  * @return verdadero si los datos fueron manejados por un gancho
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
 * Sugerencia: thread[1]==0 ayuda a determinar si se realiza una gran parte de los procesos iniciados para un determinado identificador.
 *
  * @param {QString} callbackIdentifier: la identificación proporcionada al llamar a startDetachedProcess ()
  * @param {QString} resultSet - el resultado del proceso
  * @param {QVariantList} cmd: la matriz de comandos completa [0-ejecutablePath, 1-parameters, 2-exitCode]
  * @param {QVariantList} hilo: la matriz de información del hilo [0-pass callbackParameter, 1-hilos restantes para este identificador]
 */
function onDetachedProcessCallback(callbackIdentifier, resultSet, cmd, thread);
```

Es posible que desee echar un vistazo al ejemplo [callback-example.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/callback.qml).

windowStateChangedHook
--------------

### Parámetros y llamada al método
```js
/**
* Esta función se llama después de que se activara un evento WindowStateChange
  *
  * @param {QString} windowState: el nuevo estado de la ventana, el valor del parámetro puede ser "minimizado", "maximizado", "pantalla completa", "activo" o "nostate"
  */
function windowStateChangedHook (windowState);
```

Es posible que desee echar un vistazo al ejemplo [window-state-changed.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/window-state-changed.qml).

workspaceSwitchedHook
----------------------

This hook is called when workspaces are switched.

### Parámetros y llamada al método
```js
/**
 * This function is called when workspaces are switched
 *
 * @param oldUuid old uuid of workspace
 * @param newUuid new uuid of workspace
 */
function workspaceSwitchedHook(oldUuid, newUuid);
```

Es posible que desee echar un vistazo al ejemplo. [websocket-datos-sin procesar-nueva-nota.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/workspaces.qml).
