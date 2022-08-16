# Connessioni

onNoteStored
------------

### Chiamata al metodo e parametri
```js
/**
  * Questa funzione viene chiamata quando una nota viene archiviata su
  * disco
  * Non è possibile modificare le note memorizzate, poiché sarebbe tutto
  * complicato dal fatto che probabilmente le stai contemporaneamente
  * anche modificando a mano
  *
  * @param {NoteApi} note - l'oggetto nota della nota memorizzata
 */
function onNoteStored(note);
```

Puoi dare un'occhiata all'esempio [on-note-opened.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/on-note-opened.qml).

noteOpenedHook
--------------

### Chiamata al metodo e parametri
```js
/**
  * Questa funzione viene chiamata dopo che una nota è stata aperta
  *
  * @param {NoteApi} note - l'oggetto nota che è stato aperto
 */
function noteOpenedHook(note);
```

Puoi dare un'occhiata all'esempio [on-note-opened.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/on-note-opened.qml).

noteDoubleClickedHook
---------------------

### Chiamata al metodo e parametri
```js
/**
  * Questa funzione viene chiamata dopo aver fatto doppio clic su una
  * nota
  *
  * @param {NoteApi} note - l'oggetto nota su cui è stato fatto clic
  */
funzione noteDoubleClickedHook (note);
```

Potresti voler dare un'occhiata all'esempio [external-note-open.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/external-note-open.qml).

insertMediaHook
---------------

Questa funzione viene chiamata quando un file multimediale viene inserito nella nota corrente.

Se questa funzione è definita in più script, vince il primo script che restituisce una stringa non vuota.

### Chiamata al metodo e parametri
```js
/**
  * @param fileName string il percorso del file multimediale di origine
  *                        prima che fosse copiato nella cartella
  *                        multimediale
  * @param markdownText string il testo del markdown del file
  *                            multimediale, ad es.
  *                            ![mia-immagine](media/mia-immagine-4101461585.jpg)
  * @return string il nuovo testo di markdown del file multimediale
 */
function insertMediaHook(fileName, markdownText);
```

Puoi dare un'occhiata all'esempio [example.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/example.qml).

insertAttachmentHook
--------------------

Questa funzione viene chiamata quando un file allegato viene inserito nella nota corrente.

Se questa funzione è definita in più script, vince il primo script che restituisce una stringa non vuota.

### Chiamata al metodo e parametri
```js
/**
  * @param fileName string il percorso del file allegato di origine
  *                        prima che fosse copiato nella cartella degli
  *                        allegati
  * @param markdownText string il testo del markdown del file allegato,
  *                            ad es.
  *                            [mio-file.txt](allegati/mio-file-4245650967.txt)
  * @return string il nuovo testo di markdown del file allegato
 */
function insertAttachmentHook(fileName, markdownText);
```

Puoi dare un'occhiata all'esempio [example.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/example.qml).

insertingFromMimeDataHook
-------------------------

### Chiamata al metodo e parametri
```js
/**
  * Questa funzione viene chiamata quando html o un file multimediale
  * viene incollato in una nota con `Ctrl + Maiusc + V`
  *
  * @param text testo dell'oggetto QMimeData
  * @param html html dell'oggetto QMimeData
  * @returns la stringa che deve essere inserita al posto del testo
  * dall'oggetto QMimeData
 */
function insertingFromMimeDataHook(text, html);
```

Puoi dare un'occhiata all'esempio [esempio.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/example.qml), [inserisci-titolo-con-link-da-github-url.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/insert-headline-with-link-from-github-url.qml) o [note-testo-dalle-17-mail.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/note-text-from-5pm-mail.qml).

handleNoteTextFileNameHook
--------------------------

### Chiamata al metodo e parametri
```js
/**
  * Questa funzione viene chiamata quando una nota viene archiviata su
  * disco se "Consenti al nome del file della nota di essere diverso dal
  * titolo" è abilitato nelle impostazioni
  *
  * Ti permette di modificare il nome del file della nota
  * Tieni presente che devi preoccuparti tu stesso dei nomi duplicati!
   *
  * Restituisce una stringa vuota se il nome del file della nota non
  * deve essere modificato
  *
  * @param {NoteApi} note - l'oggetto nota della nota memorizzata
  * @return {string} il nome del file della nota
 */
function handleNoteTextFileNameHook(note);
```

Puoi dare un'occhiata all'esempio [esempio.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/example.qml) o [use-tag-names-in-filename.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/use-tag-names-in-filename.qml).

handleNoteNameHook
------------------

### Chiamata al metodo e parametri
```js
/**
 * Questa funzione viene chiamata quando il nome della nota è
 * determinato per una nota
 *
 * Ti permette di modificare il nome della nota che viene visualizzata
 *
 * Restituisce una stringa vuota se il nome della nota non deve essere
 * modificato
 *
 * @param {NoteApi} note - the note object of the stored note
 * @return {string} the name of the note
 */
function handleNoteNameHook(note);
```

Puoi dare un'occhiata all'esempio [example.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/example.qml).

Potrebbe non essere una buona idea usare questo hook se l'impostazione per usare il nome del file come nome della nota è attiva.

handleNewNoteHeadlineHook
-------------------------

### Chiamata al metodo e parametri
```js
/**
  * Questa funzione viene chiamata prima della creazione di una nota
  *
  * Ti permette di modificare il titolo della nota prima che venga creata
  * Nota che devi prenderti cura di un nome di nota univoco, altrimenti
  * la nuova nota non verrà creata, sarà solo trovata nell'elenco delle note
  *
  * È possibile utilizzare questa funzione per creare modelli di note
  *
 * @param headline text that would be used to create the headline
 * @return {string} the headline of the note
 */
function handleNewNoteHeadlineHook(headline);
```

Puoi dare un'occhiata all'esempio [custom-new-note-headline.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-new-note-headline.qml).

preNoteToMarkdownHtmlHook
-------------------------

### Chiamata al metodo e parametri
```js
/**
  * Questa funzione viene chiamata prima che venga generato il markdown html di una nota
  *
  * Ti permette di modificare ciò che viene passato al convertitore markdown in html
  *
  * La funzione può ad esempio essere utilizzata in più script per il rendering del codice (come LaTeX math o mermaid)
  * alla sua rappresentazione grafica per l'anteprima
  *
  * La nota non verrà modificata in questo processo
  *
  * @param {NoteApi} note - l'oggetto nota
  * @param {string} markdown - il markdown che sta per essere convertito in html
  * @param {bool} forExport - true se l'html viene utilizzato per un'esportazione, false per l'anteprima
  * @return {string} il markdown modificato o una stringa vuota se non deve essere modificato nulla
  */
function preNoteToMarkdownHtmlHook(note, markdown, forExport);
```

Puoi dare un'occhiata all'esempio [preview-styling.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/preview-styling.qml).

noteToMarkdownHtmlHook
----------------------

### Chiamata al metodo e parametri
```js
/**
  * Questa funzione viene chiamata quando viene generato il markdown html di una nota
  *
  * Ti permette di modificare questo html
  * Questo è ad esempio chiamato prima dall'anteprima della nota
  *
  * La funzione può essere utilizzata in più script per modificare l'html dell'anteprima
 *
 * @param {NoteApi} note - the note object
 * @param {string} html - the html that is about to being rendered
 * @param {bool} forExport - true if the html is used for an export, false for the preview
 * @return {string} the modified html or an empty string if nothing should be modified
 */
function noteToMarkdownHtmlHook(note, html, forExport);
```

Puoi dare un'occhiata all'esempio [example.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/example.qml) o [preview-styling.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/preview-styling.qml).

Fai riferimento al documento [Sottoinsieme di HTML supportato](http://doc.qt.io/qt-5/richtext-html-subset.html) per un elenco di tutti gli stili CSS supportati.

encryptionHook
--------------

### Chiamata al metodo e parametri
```js
/**
 * Questa funzione viene chiamata quando il testo deve essere
 * crittografato o decrittografato
 *
 * @param text string il testo da crittografare o decrittografare
 * @param password string la password
 * @param decrypt bool se falso, si chiede di criptare, se vero si
 *                     chiede di descrittare
 * @return il testo crittografato/decrittografato
 */
function encryptionHook(text, password, decrypt);
```

Puoi dare un'occhiata all'esempio [encryption-keybase.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/encryption-keybase.qml), [encryption-pgp.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/encryption-pgp.qml) o [encryption-rot13.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/encryption-rot13.qml).

noteTaggingHook
---------------

Puoi implementare il tuo meccanismo di etichettatura delle note, ad esempio con un testo speciale nella tua nota come `@tag1`, `@tag2`, `@tag3`.

### Chiamata al metodo e parametri
```js
/**
  * Gestisce l'etichettatura per una nota
  *
  * Questa funzione viene chiamata quando si aggiunge, toglie o rinomina
  * un'etichetta in una nota oppure sono elencate le etichette di una
  * nota
  *
  * @param note
  * @param action può essere "add", "remove", "rename" o "list"
  * @param tagName nome dell'etichetta da aggiungere, rimuovere o
                   rinominare
  * @param newTagName nome dell'etichetta da rinominare se action =
                      "rename"
  * @return nota stringa di testo o elenco di stringhe di nomi di
  *         etichette (se action = "list")
 */
function noteTaggingHook(note, action, tagName, newTagName);
```

-   non appena viene attivato uno script che implementa la nuova funzione `noteTaggingHook` l'etichettatura delle note sarà gestita da quella funzione
-   le seguenti funzionalità dovrebbero essere disponibili tramite l'interfaccia utente QOwnNotes
    -   inizialmente importando le etichette `@tag` dalle note e sovrascrivendo l'assegnazione dell'etichetta corrente
        -   non perderai il tuo albero delle etichette, solo il precedente assegnamento alle note
        -   puoi comunque spostare le etichette in altre etichette
        -   se ci sono più etichette collo stesso nome nel tuo albero delle etichette, verrà assegnata la prima corrispondenza
    -   l'aggiunta di un'etichetta a una nota aggiungerà l'etichetta al testo della nota
    -   la rimozione di un'etichetta da una nota rimuoverà l'etichetta dal testo della nota
    -   la rimozione di etichette dall'elenco rimuoverà le etichette dalle tue note
    -   rinominare le etichette nell'elenco rinominerà le etichette nelle note
    -   l'etichettatura collettiva delle note nell'elenco delle note aggiungerà tali etichette alle note
    -   la rimozione collettiva di etichette dalle note nell'elenco delle note rimuoverà tali etichette dalle note
    -   l'applicazione attiverà una serie di azioni `aggiungi` e `rimuovi` per tutte le etichette selezionate e i loro figli su tutte le note se le etichette vengono spostate nel pannello delle etichette

Potresti voler dare un'occhiata all'esempio [note-tagging.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/note-tagging.qml) per implementare il tuo meccanismo di etichettatura.

Assicurati che la tua azione `list` sia veramente veloce, perché sarà eseguita per ogni nota ogni volta che la cartella delle note viene ricaricata!
:::

noteTaggingByObjectHook
----------------------

Analogamente a [noteTaggingHook](#notetagginghook) puoi implementare il tuo meccanismo di etichettatura delle note, ma non sei vincolato ai nomi delle etichette nella radice dell'albero delle etichette. In questo modo puoi utilizzare l'intero albero delle etichette invece che solo un elenco di etichette.

Con `noteTaggingByObjectHook` ottieni un oggetto `TagApi` come parametro, invece del nome di un'etichetta. E come risultato per l'azione `list` è necessario fornire un elenco di ID etichetta.

Ciò significa anche che devi creare tu stesso le etichette mancanti per poter fornire un elenco di ID etichetta già esistenti per l'azione `list`.

### Chiamata al metodo e parametri
```js
/**
 * Gestisce l'etichettatura per una nota
 *
 * Questa funzione viene chiamata quando si aggiunge, toglie o rinomina
 * un'etichetta in una nota oppure sono elencate le etichette di una
 * nota
 *
 * @param note
 * @param action può essere "add", "remove", "rename" o "list"
 * @param tag nome dell'etichetta da aggiungere, rimuovere o rinominare
 * @param newTagName nome dell'etichetta da rinominare se action =
 *        "rename"
 */
function noteTaggingByObjectHook(note, action, tag, newTagName);
```

Puoi dare un'occhiata all'esempio [note-tagging-by-object.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/note-tagging-by-object.qml) per implementare il tuo meccanismo di etichettatura.

autocompletionHook
------------------

È possibile restituire un elenco di stringhe da aggiungere all'elenco di completamento automatico quando viene richiamato il completamento automatico (ad esempio premendo <kbd>Ctrl + Spazio</kbd>).

### Chiamata al metodo e parametri
```js
/**
 * Chiama la funzione autocompletionHook per tutti i componenti dello
 * script
 * Questa funzione viene chiamata quando viene richiamato il
 * completamento automatico in una nota
 *
 * @return QStringList testi per l'elenco di completamento automatico
 */
function callAutocompletionHook();
```

Puoi dare un'occhiata all'esempio [autocompletion.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/autocompletion.qml).

websocketRawDataHook
--------------------

Questo hook viene chiamato quando i dati vengono inviati dall'estensione del browser QOwnNotes Web Companion tramite il menu contestuale del browser web.

### Chiamata al metodo e parametri
```js
/**
 * @param requestType può essere "page" o "selection"
 * @param pageUrl l'URL della pagina web in cui è stata effettuata la
 *                richiesta
 * @param pageTitle il titolo della pagina web in cui è stata effettuata
 *                  la richiesta
 * @param rawData i dati che sono stati trasmessi, html per requestType
 *                "page" o testo normale per requestType "selezione"
 * @param screenshotDataUrl l'URL dei dati dello screenshot se la pagina
 *                          web in cui è stata effettuata la richiesta
 * @return true se i dati sono stati gestiti da un hook
 */
function callHandleWebsocketRawDataHook(requestType, pageUrl, pageTitle, rawData, screenshotDataUrl);
```

Puoi dare un'occhiata agli esempi [websocket-raw-data-new-note.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/websocket-raw-data-new-note.qml) e [websocket-raw-data-selection-in-note.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/websocket-raw-data-selection-in-note.qml).

onDetachedProcessCallback
-------------------------

Questo hook viene chiamato quando un thread dello script [startDetachedProcess](methods-and-objects.html#starting-an-external-program-in-the-background) completa l'esecuzione.

### Chiamata al metodo e parametri
```js
/**
 * Questa funzione viene chiamata quando un thread di script completa
 * l'esecuzione.
 * Suggerimento: thread[1]==0 aiuta a capire se l'insieme di processi di
 * un certo identificatore è terminato.
 *
 * @param {QString} callbackIdentifier - l'id fornito chiamando
 *                                       startDetachedProcess()
 * @param {QString} resultSet - il risultato del processo
 * @param {QVariantList} cmd - l'array dell'intero comando
 *                             [0-percorso dell'eseguibile,
 *                              1-parametri,
 *                              2-codice di uscita]
 * @param {QVariantList} thread - l'array di informazioni sul thread
 *                                [0-parametro callback fornito,
 *                                 1-thread rimanenti per questo id]
 */
function onDetachedProcessCallback(callbackIdentifier, resultSet, cmd, thread);
```

Potresti voler dare un'occhiata all'esempio [callback-example.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/callback.qml).

windowStateChangedHook
--------------

### Chiamata al metodo e parametri
```js
/**
 * Questa funzione viene chiamata dopo che è stato attivato un evento
 * WindowStateChange
 *
 * @param {QString} windowState - il nuovo stato della finestra, il
 *                                valore del parametro può essere
 *                                "minimized", "maximized",
 *                                "fullscreen", "active" o "nostate"
 */
function windowStateChangedHook(windowState);
```

Potresti voler dare un'occhiata all'esempio [window-state-changed.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/window-state-changed.qml).

workspaceSwitchedHook
----------------------

This hook is called when workspaces are switched.

### Chiamata al metodo e parametri
```js
/**
 * This function is called when workspaces are switched
 *
 * @param oldUuid old uuid of workspace
 * @param newUuid new uuid of workspace
 */
function workspaceSwitchedHook(oldUuid, newUuid);
```

Potresti voler dare un'occhiata all'esempio [websocket-raw-data-new-note.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/workspaces.qml).
