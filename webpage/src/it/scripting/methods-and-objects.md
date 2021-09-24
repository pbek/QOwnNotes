# Metodi e oggetti forniti da QOwnNotes

Avvio di un programma esterno in background
----------------------------------------------


### Chiamata al metodo e parametri
```cpp
/**
  * Wrapper QML per avviare un processo scollegato
  *
  * @param executablePath il percorso dell'eseguibile
  * @param parameters un elenco di stringhe di parametri
  * @param callbackIdentifier un identificatore da utilizzare nella
  * funzione onDetachedProcessCallback() (opzionale)
  * @param callbackParameter un parametro aggiuntivo per i loop o
  * simili (opzionale)
  * @param processData dati scritti nel processo se viene utilizzata
  * la richiamata (opzionale)
  * @return true in caso di successo, false in caso contrario
 */
bool startDetachedProcess(QString executablePath,
                          QStringList parameters,
                          QString callbackIdentifier,
                          QVariant callbackParameter,
                          QByteArray processData);
```

### Esempio

Esempio semplice:

```js
script.startDetachedProcess("/path/to/my/program", ["my parameter"]);
```

Esecuzione di molti processi:

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

Puoi dare un'occhiata all'esempio [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml), [callback.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/callback.qml) o [execute-command-after-note-update.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/execute-command-after-note-update.qml).

Potresti anche voler dare un'occhiata all'hook [onDetachedProcessCallback](hooks.html#ondetachedprocesscallback).

::: tip
Puoi anche assegnare scorciatoie locali e globali alle tue azioni personalizzate nelle *Impostazioni dei collegamenti*.
:::

Avvio di un programma esterno e attesa dell'output
----------------------------------------------------

### Chiamata al metodo e parametri
```cpp
/**
  * Wrapper QML per avviare un processo sincrono
  *
  * @param executablePath il percorso dell'eseguibile
  * @param parameters un elenco di stringhe di parametri
  * @param data i dati che verranno scritti nel processo (facoltativo)
  * @return il testo che è stato restituito dal processo
QByteArray startSynchronousProcess(QString executablePath, QStringList parameters, QByteArray data);
```

### Esempio
```js
var result = script.startSynchronousProcess("/path/to/my/program", ["my parameter"], "data");
```

Puoi dare un'occhiata all'esempio [encryption-keybase.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/encryption-keybase.qml).

Ottenere il percorso della cartella della nota corrente
-------------------------------------------

### Chiamata al metodo e parametri
```cpp
/**
 * Wrapper QML per ottenere il percorso della cartella della nota corrente
 *
 * @return il percorso della cartella della nota corrente
 */
QString currentNoteFolderPath();
```

### Esempio
```js
var path = script.currentNoteFolderPath();
```

Puoi dare un'occhiata all'esempio [absolute-media-links.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/absolute-media-links.qml).

Ottenere la nota corrente
------------------------

### Chiamata al metodo e parametri
```cpp
/**
 * Wrapper QML per ottenere la nota corrente
 *
 * @returns {NoteApi} l'oggetto nota corrente
 */
NoteApi currentNote();
```

### Esempio
```js
var note = script.currentNote();
```

Puoi dare un'occhiata all'esempio [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml).

Accesso al widget di registro
-------------------------

### Chiamata al metodo e parametri
```cpp
/**
 * Wrapper QML per accedere al registro
 *
 * @param text
 */
void log(QString text);
```

### Esempio
```js
script.log("my text");
```

Download di un URL in una stringa
------------------------------

### Chiamata al metodo e parametri
```cpp
/**
 * Wrapper QML per scaricare un url e ritornarlo come testo
 *
 * @param url
 * @return {QString} il contenuto del url  scaricato
 */
QString downloadUrlToString(QUrl url);
```

### Esempio
```js
var html = script.downloadUrlToString("https://www.qownnotes.org");
```

Puoi dare un'occhiata all'esempio [inserisci-titolo-con-link-da-github-url.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/insert-headline-with-link-from-github-url.qml).

Download di un URL nella cartella multimediale
--------------------------------------

### Chiamata al metodo e parametri
```cpp
/**
  * Wrapper QML per scaricare un URL nella cartella multimediale e restituire il supporto
  * url o il testo dell'immagine di markdown del supporto relativo alla nota corrente
  *
  * URL @param {QString}
  * @param {bool} returnUrlOnly if true solo l'URL multimediale verrà restituito (valore predefinito false)
  * @return {QString} il markdown multimediale o l'URL
 */
QString downloadUrlToMedia(QUrl url, bool returnUrlOnly);
```

### Esempio
```js
var markdown = script.downloadUrlToMedia("http://latex.codecogs.com/gif.latex?\frac{1}{1+sin(x)}");
```

Puoi dare un'occhiata all'esempio [paste-latex-image.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/paste-latex-image.qml).

Inserimento di un file multimediale nella cartella multimediale
--------------------------------------------

### Chiamata al metodo e parametri
```cpp
/**
  * QML wrapper per inserire un file multimediale nella cartella multimediale e tornare
  * l'URL del supporto o il testo dell'immagine di markdown del supporto relativo alla nota corrente
  *
  * @param {QString} mediaFilePath
  * @param {bool} returnUrlOnly se true solo l'URL multimediale verrà restituito (valore predefinito false)
  * @return {QString} il markdown multimediale o l'URL
```

### Esempio
```js
var markdown = script.insertMediaFile("/path/to/your/image.png");
```

Puoi dare un'occhiata all'esempio [scribble.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/scribble.qml).

Inserimento di un file allegato nella cartella allegati
--------------------------------------------------------

### Chiamata al metodo e parametri
```cpp
  * Wrapper QML per inserire un file allegato nella cartella
 * `attachments`e restituire l'URL dell'allegato o il testo markdown
 * dell'allegato relativi alla nota corrente
 *
 * @param {QString} attachmentFilePath
 * @param {QString} nome del file da usare nel markdown
 * @param {bool} returnUrlOnly se true sarà ritornato solo l'URL
 *                             dell'allegato (default false)
 * @return {QString} il markdown dell'allegato o l'URL
 */
QString ScriptingService::insertAttachmentFile(const QString &attachmentFilePath,
                                               const QString &fileName,
                                               bool returnUrlOnly);
```

### Esempio
```js
var markdown = script.insertAttachmentFile("/percorso/del/tuo/file.png");
```

Rigenerazione dell'anteprima della nota
-----------------------------

Aggiorna l'anteprima della nota.

### Chiamata al metodo e parametri
```cpp
/**
 * Rigenera l'anteprima della nota
 */
QString ScriptingService::regenerateNotePreview();
```

### Esempio
```js
script.regenerateNotePreview();
```

Puoi dare un'occhiata all'esempio [scribble.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/scribble.qml).

Registrazione di un'azione personalizzata
---------------------------

### Chiamata al metodo e parametri
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

### Esempio
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

È quindi possibile utilizzare l'identificatore con funzione `customActionInvoked` in a script like [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml).

::: tip
Puoi anche attivare un'azione personalizzata dopo che l'applicazione è stata avviata con il parametro `--action customAction_<identifier>`. Per maggiori informazioni dai un'occhiata a [Attiva le azioni del menu dopo l'avvio](../getting-started/cli-parameters.md#trigger-menu-actions-after-startup).
:::

Registrazione di un'etichetta
-------------------

### Chiamata al metodo e parametri
```cpp
/**
 * Registers a label to write to
 *
 * @param identifier the identifier of the label
 * @param text the text shown in the label (optional)
 */
void ScriptingService::registerLabel(QString identifier, QString text);
```

### Esempio
```js
script.registerLabel("html-label", "<strong>Strong</strong> HTML text<br />with three lines<br />and a <a href='https://www.qownnotes.org'>link to a website</a>.");

script.registerLabel("long-label", "another very long text, another very long text, another very long text, another very long text, another very long text, another very long text, another very long text, another very long text, another very long text, another very long text, another very long text that will wrap");

script.registerLabel("counter-label");
```

Le etichette saranno visibili nel widget del dock di scripting.

È possibile utilizzare sia testo normale che html nelle etichette. Il testo sarà selezionabile e sarà possibile fare clic sui collegamenti.

Potresti quindi voler dare un'occhiata allo script di esempio [scripting-label-demo.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/scripting-label-demo.qml).

Impostazione del testo di un'etichetta registrata
--------------------------------------

### Chiamata al metodo e parametri
```cpp
/**
 * Sets the text of a registered label
 *
 * @param identifier the identifier of the label
 * @param text the text shown in the label
 */
void ScriptingService::setLabelText(QString identifier, QString text);
```

### Esempio
```js
script.setLabelText("counter-label", "counter text");
```

È possibile utilizzare sia testo normale che html nelle etichette. Il testo sarà selezionabile e sarà possibile fare clic sui collegamenti.

Potresti quindi voler dare un'occhiata allo script di esempio [scripting-label-demo.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/scripting-label-demo.qml).

Creazione di una nuova nota
-------------------

### Chiamata al metodo e parametri
```cpp
/**
 * Creates a new note
 *
 * @param text the note text
 */
void ScriptingService::createNote(QString text);
```

### Esempio
```js
script.createNote("My note headline\n===\n\nMy text");
```

Puoi dare un'occhiata all'esempio [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml).

::: tip
Se hai disattivato che il titolo della nota determina il nome del file della nota, devi rinominare il file della nota da solo in seguito, in questo modo:

```js
var note = script.currentNote();
note.renameNoteFile('your-filename');
```
:::

Accesso agli appunti
-----------------------

### Chiamata al metodo e parametri
```cpp
/**
 * Returns the content of the clipboard as text or html
 *
 * @param asHtml returns the clipboard content as html instead of text
 */
QString ScriptingService::clipboard(bool asHtml);
```

### Esempio
```js
var clipboardText = script.clipboard();
var clipboardHtml = script.clipboard(true);
```

Puoi dare un'occhiata all'esempio [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml).

Scrivi il testo nella finestra di modifica del testo della nota
--------------------------------

### Chiamata al metodo e parametri
```cpp
/**
 * Writes text to the current cursor position in the note text edit
 *
 * @param text
 */
void ScriptingService::noteTextEditWrite(QString text);
```

### Esempio
```js
// write text to the note text edit
script.noteTextEditWrite("My custom text");
```

Potresti voler esaminare l'azione personalizzata  `transformTextRot13` nel file esempio [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml).

Puoi usarlo insieme a `noteTextEditSelectAll` per sovrascrivere l'intero testo della nota corrente.

Read the selected text in the note text edit
--------------------------------------------

### Chiamata al metodo e parametri
```cpp
/**
 * Reads the selected text in the note text edit
 *
 * @return
 */
QString ScriptingService::noteTextEditSelectedText();
```

### Esempio
```js
// read the selected text from the note text edit
var text = script.noteTextEditSelectedText();
```

Potresti voler esaminare l'azione personalizzata  `transformTextRot13` nel file esempio [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml).

Seleziona tutto il testo nella finestra di modifica del testo della nota
-------------------------------------

### Chiamata al metodo e parametri
```cpp
/**
 * Selects all text in the note text edit
 */
void ScriptingService::noteTextEditSelectAll();
```

### Esempio
```js
script.noteTextEditSelectAll();
```

Puoi usarlo insieme a `noteTextEditWrite` per sovrascrivere l'intero testo della nota corrente.

Seleziona la riga corrente nella finestra di modifica del testo della nota
---------------------------------------------

### Chiamata al metodo e parametri
```cpp
/**
 * Selects the current line in the note text edit
 */
void ScriptingService::noteTextEditSelectCurrentLine();
```

### Esempio
```js
script.noteTextEditSelectCurrentLine();
```

Seleziona la parola corrente nella finestra di modifica del testo della nota
---------------------------------------------

### Chiamata al metodo e parametri
```cpp
/**
 * Selects the current line in the note text edit
 */
void ScriptingService::noteTextEditSelectCurrentWord();
```

### Esempio
```js
script.noteTextEditSelectCurrentWord();
```

Imposta il testo attualmente selezionato nella finestra di modifica del testo della nota
-----------------------------------------------------

### Chiamata al metodo e parametri
```cpp
/**
 * Sets the currently selected text in the note text edit
 *
 * @param start
 * @param end
 */
void ScriptingService::noteTextEditSetSelection(int start, int end);
```

### Esempio
```js
// expands the current selection by one character
script.noteTextEditSetSelection(
    script.noteTextEditSelectionStart() - 1,
    script.noteTextEditSelectionEnd() + 1);
```

Ottieni la posizione iniziale della selezione corrente nella finestra di modifica del testo della nota
---------------------------------------------------------------------

### Chiamata al metodo e parametri
```cpp
/**
 * Returns the start position of the current selection in the note text edit
 */
int ScriptingService::noteTextEditSelectionStart();
```

### Esempio
```js
script.log(script.noteTextEditSelectionStart());
```

Ottieni la posizione finale della selezione corrente nella finestra di modifica del testo della nota
-------------------------------------------------------------------

### Chiamata al metodo e parametri
```cpp
/**
 * Returns the end position of the current selection in the note text edit
 */
int ScriptingService::noteTextEditSelectionEnd();
```

### Esempio
```js
script.log(script.noteTextEditSelectionEnd());
```

Impostare il cursore del testo nella finestra di modifica del testo della nota su una determinata posizione
---------------------------------------------------------------

### Chiamata al metodo e parametri
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

### Esempio
```js
// jump to the 11th character in the note
script.noteTextEditSetCursorPosition(10);

// jump to the end of the note
script.noteTextEditSetCursorPosition(-1);
```

Ottieni la posizione corrente del cursore del testo nella finestra di modifica del testo della nota
-----------------------------------------------------------------

### Chiamata al metodo e parametri
```cpp
/**
 * Returns the current position of the text cursor in the note text edit
 * 0 would be the beginning of the note
 */
int ScriptingService::noteTextEditCursorPosition();
```

### Esempio
```js
script.log(script.noteTextEditCursorPosition());
```

Leggi la parola corrente dalla finestra di modifica del testo della nota
---------------------------------------------

### Chiamata al metodo e parametri
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

### Esempio
```js
// read the current word in the note text edit
var text = script.noteTextEditCurrentWord();
```

Puoi dare un'occhiata all'esempio [autocompletion.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/autocompletion.qml).

Controlla se la piattaforma è Linux, OS X o Windows
------------------------------------------------

### Chiamata al metodo e parametri
```cpp
bool ScriptingService::platformIsLinux();
bool ScriptingService::platformIsOSX();
bool ScriptingService::platformIsWindows();
```

### Esempio
```js
if (script.platformIsLinux()) {
    // only will be executed if under Linux
}
```

Contrassegna la nota corrente
--------------------

### Chiamata al metodo e parametri
```cpp
/**
 * Tags the current note with a tag named tagName
 *
 * @param tagName
 */
void ScriptingService::tagCurrentNote(QString tagName);
```

### Esempio
```js
// add a "favorite" tag to the current note
script.tagCurrentNote("favorite");
```

Potresti voler dare un'occhiata all'azione personalizzata `favoriteNote` nell'esempio [favorite-note.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/favorite-note.qml).

Crea o recupera un'etichetta dal una lista di elementi
-------------------------------------------------

### Chiamata al metodo e parametri
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

### Esempio
```js
// creates all tags until the 3rd level and returns the tag object for
// tag "level3", which would look like that in the tag tree:
// level1 > level2 > level3
var tag = script.getTagByNameBreadcrumbList(["level1", "level2", "level3"]);
```

Cerca le etichette per nome
-----------------------

### Chiamata al metodo e parametri
```cpp
/**
 * Fetches all tags by doing a substring search on the name field
 *
 * @param name {QString} name to search for
 * @return {QStringList} list of tag names
 */
QStringList ScriptingService::searchTagsByName(QString name);
```

### Esempio
```js
// searches for all tags with the word game in it
var tags = script.searchTagsByName("game");
```

Puoi dare un'occhiata all'esempio [autocompletion.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/autocompletion.qml).

Cerca note in base al testo della nota
-----------------------------

### Chiamata al metodo e parametri
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

### Esempio
```js
var noteIds = script.fetchNoteIdsByNoteTextPart("mytext");

noteIds.forEach(function (noteId){
    var note = script.fetchNoteById(noteId);

    // do something with the note
});
```

Puoi dare un'occhiata all'esempio [unique-note-id.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/unique-note-id.qml).

Aggiungi un foglio di stile personalizzato
-----------------------

### Chiamata al metodo e parametri
```cpp
/**
 * Adds a custom stylesheet to the application
 *
 * @param stylesheet
 */
void ScriptingService::addStyleSheet(QString stylesheet);
```

### Esempio
```js
// make the text in the note list bigger
script.addStyleSheet("QTreeWidget#noteTreeWidget {font-size: 30px;}");
```

Puoi dare un'occhiata all'esempio [custom-stylesheet.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-stylesheet.qml).

You can get the widget names from the `*.ui` files, for example the main window is [mainwindow.ui](https://github.com/pbek/QOwnNotes/blob/develop/src/mainwindow.ui).

The Qt documentation (for example [QMainWindow](https://doc.qt.io/qt-5/qmainwindow.html)) can help you to see how the widgets are related to each other (search for `Inherits` on the pages).

The base widget for almost everything is [QWidget](https://doc.qt.io/qt-5/qwidget.html). So just styling `QWidget` with for example `QWidget {background-color: black; color: white;}` would mean everything has a black background color and a white foreground color.

::: tip
The [style.qss](https://github.com/pbek/QOwnNotes/blob/develop/src/libraries/qdarkstyle/style.qss) of [qdarkstyle](https://github.com/pbek/QOwnNotes/blob/develop/src/libraries/qdarkstyle) might also be a good reference for styles you can change.
:::

Take a look at [Style Sheet Reference](http://doc.qt.io/qt-5/stylesheet-reference.html) for a reference of what styles are available.

If you want to inject styles into html preview to alter the way notes are previewed please look at [notetomarkdownhtmlhook](hooks.html#notetomarkdownhtmlhook).

::: tip
If you actually want to see how the dialogs look and what the names are you could download [Qt Creator](https://www.qt.io/product/development-tools) and open the `*.ui` files in it.
:::

Ricaricamento del motore di scripting
------------------------------

### Chiamata al metodo e parametri
```cpp
/**
 * Reloads the scripting engine
 */
void ScriptingService::reloadScriptingEngine();
```

### Esempio
```js
// reload the scripting engine
script.reloadScriptingEngine();
```

Fetching a note by its file name
--------------------------------

### Chiamata al metodo e parametri
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

### Esempio
```js
// fetch note by file name
script.fetchNoteByFileName("my note.md");
```

Recupero di una nota dal suo ID
-------------------------

### Chiamata al metodo e parametri
```cpp
/**
 * Fetches a note by its id
 *
 * @param id int the id of the note
 * @return NoteApi*
 */
NoteApi* ScriptingService::fetchNoteById(int id);
```

### Esempio
```js
// fetch note by id
script.fetchNoteById(243);
```

You may want to take a look at the example [export-notes-as-one-html.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/export-notes-as-one-html.qml).

Controllo dell'esistenza di una nota tramite il nome del file
------------------------------------------

### Chiamata al metodo e parametri
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

### Esempio
```js
// check if note exists, but ignore the id of "note"
script.noteExistsByFileName("my note.md", note.id);
```

You may want to take a look at the example [use-tag-names-in-filename.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/use-tag-names-in-filename.qml).

Copia del testo negli appunti
-------------------------------

### Chiamata al metodo e parametri
```cpp
/**
 * Copies text into the clipboard as plain text or html mime data
 *
 * @param text string text to put into the clipboard
 * @param asHtml bool if true the text will be set as html mime data
 */
void ScriptingService::setClipboardText(QString text, bool asHtml);
```

### Esempio
```js
// copy text to the clipboard
script.setClipboardText("text to copy");
```

You may want to take a look at the example [selected-markdown-to-bbcode.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/selected-markdown-to-bbcode.qml).

Andare a una nota
-----------------

### Chiamata al metodo e parametri
```cpp
/**
 * Sets the current note if the note is visible in the note list
 *
 * @param note NoteApi note to jump to
 */
void ScriptingService::setCurrentNote(NoteApi *note);
```

### Esempio
```js
// jump to the note
script.setCurrentNote(note);
```

You may want to take a look at the example [journal-entry.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/journal-entry.qml).

Passaggio a una sottocartella di note
---------------------------

### Chiamata al metodo e parametri
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

### Esempio
```js
// jump to the note subfolder "a sub folder"
script.jumpToNoteSubFolder("a sub folder");

// jump to the note subfolder "sub" inside of "a sub folder"
script.jumpToNoteSubFolder("a sub folder/sub");
```

::: tip
You can create a new note subfolder in the current subfolder by calling [`mainWindow.createNewNoteSubFolder`](classes.html#example-2).
:::

Visualizzazione di una finestra di messaggio informativo
----------------------------------

### Chiamata al metodo e parametri
```cpp
/**
 * Shows an information message box
 *
 * @param text
 * @param title (optional)
 */
void ScriptingService::informationMessageBox(QString text, QString title);
```

### Esempio
```js
// show a information message box
script.informationMessageBox("The text I want to show", "Some optional title");
```

Visualizzazione di una finestra di messaggio informativo
------------------------------

### Chiamata al metodo e parametri
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

### Esempio
```js
// show a question message box with an apply and a help button
// see: https://doc.qt.io/qt-5/qmessagebox.html#StandardButton-enum
var result = script.questionMessageBox(
    "The text I want to show", "Some optional title", 0x01000000|0x02000000, 0x02000000);
script.log(result);
```

For information about buttons see [StandardButton](https://doc.qt.io/qt-5/qmessagebox.html#StandardButton-enum).

You may also want to take a look at the example [input-dialogs.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/input-dialogs.qml).

Visualizzazione di una finestra di dialogo Apri file
---------------------------

### Chiamata al metodo e parametri
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

### Esempio
```js
// show an open file dialog
var fileName = script.getOpenFileName("Please select an image", "/home/user/images", "Images (*.png *.xpm *.jpg)");
```

Visualizzazione di una finestra di dialogo per il salvataggio del file
--------------------------

### Chiamata al metodo e parametri
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

### Esempio
```js
// show a save file dialog
var fileName = script.getSaveFileName("Please select HTML file to save", "output.html", "HTML (*.html)");
```

You may want to take a look at the example [export-notes-as-one-html.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/export-notes-as-one-html.qml).

Registrazione delle variabili delle impostazioni dello script
-------------------------------------

You need to define your settings variables as properties in your script and register them in a property named `settingsVariables`.

The user can then set these properties in the script settings.

### Esempio
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

In addition you can override the `settingsVariables` with a special function `registerSettingsVariables()` like this:

### Esempio
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

You may also want to take a look at the example [variables.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/variables.qml).

Memorizzazione e caricamento di variabili persistenti
----------------------------------------

### Chiamata al metodo e parametri
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

### Esempio
```js
// store persistent variable
script.setPersistentVariable("PersistentVariablesTest/myVar", result);

// load and log persistent variable
script.log(script.getPersistentVariable("PersistentVariablesTest/myVar", "nothing here yet"));
```

Please make sure to use a meaningful prefix in your key like `PersistentVariablesTest/myVar` because the variables are accessible from all scripts.

You may also want to take a look at the example [persistent-variables.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/persistent-variables.qml).

Caricamento delle variabili delle impostazioni dell'applicazione
--------------------------------------

### Chiamata al metodo e parametri
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

### Esempio
```js
// load and log an application settings variable
script.log(script.getApplicationSettingsVariable("gitExecutablePath"));
```

Keep in mind that settings actually can be empty, you have to take care about that yourself. `defaultValue` is only used if the setting doesn't exist at all.

Creazione di una directory della cache
--------------------------

You can cache files at the default cache location of your system.

### Chiamata al metodo e parametri
```cpp
/**
 * Returns a cache directory for a script
 *
 * @param {QString} subDir the subfolder to create and use
 * @return {QString} the cache dir path
 */
QString ScriptingService::cacheDir(const QString &subDir) const;
```

### Esempio
```js
// create the cache directory for my-script-id
var cacheDirForScript = script.cacheDir("my-script-id");
```

Cancellazione di una directory della cache
--------------------------

You can clear the cache files of your script by passing its name to clearCacheDir().

### Chiamata al metodo e parametri
```cpp
/**
 * Clears the cache directory for a script
 *
 * @param {QString} subDir the subfolder to clear
 * @return {bool} true on success
 */
bool ScriptingService::clearCacheDir(const QString &subDir) const;
```

### Esempio
```js
// clear cache directory of my-script-id 
script.clearCacheDir("my-script-id");
```

Leggere il percorso della directory del tuo script
------------------------------------------------

If you need to get the path to the directory where your script is placed to for example load other files you have to register a `property string scriptDirPath;`. This property will be set with the path to the script's directory.

### Esempio
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

Conversione dei separatori di percorso in quelli nativi
-----------------------------------------

### Chiamata al metodo e parametri
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

### Esempio
```js
// will return "c:\winnt\system32" on Windows
script.log(script.toNativeDirSeparators("c:/winnt/system32"));
```

Conversione dei separatori di percorso da quelli nativi
-------------------------------------------

### Chiamata al metodo e parametri
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

### Esempio
```js
// will return "c:/winnt/system32" on Windows
script.log(script.fromNativeDirSeparators("c:\\winnt\\system32"));
```

Recupero del separatore di directory nativo
--------------------------------------

### Chiamata al metodo e parametri
```cpp
/**
 * Returns the native directory separator "/" or "\" on Windows
 *
 * @return
 */
QString ScriptingService::dirSeparator();
```

### Esempio
```js
// will return "\" on Windows
script.log(script.dirSeparator());
```

Ottenere un elenco dei percorsi di tutte le note selezionate
-------------------------------------------------

### Chiamata al metodo e parametri
```cpp
/**
 * Returns a list of the paths of all selected notes
 *
 * @return {QStringList} list of selected note paths
 */
QStringList ScriptingService::selectedNotesPaths();
```

### Esempio
```js
// returns a list of the paths of all selected notes
script.log(script.selectedNotesPaths());
```

You may want to take a look at the example [external-note-diff.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/external-note-diff.qml).

Ottenere un elenco degli ID di tutte le note selezionate
-----------------------------------------------

### Chiamata al metodo e parametri
```cpp
/**
 * Returns a list of the ids of all selected notes
 *
 * @return {QList<int>} list of selected note ids
 */
QList<int> ScriptingService::selectedNotesIds();
```

### Esempio
```js
// returns a list of the ids of all selected notes
script.log(script.selectedNotesIds());
```

You may want to take a look at the example [export-notes-as-one-html.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/export-notes-as-one-html.qml).

Attivazione di un'azione di menu
------------------------

### Chiamata al metodo e parametri
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

### Esempio
```js
// toggle the read-only mode
script.triggerMenuAction("actionAllow_note_editing");

// disable the read-only mode
script.triggerMenuAction("actionAllow_note_editing", 1);
```

You may want to take a look at the example [disable-readonly-mode.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/disable-readonly-mode.qml).

::: tip
You can get the object names of the menu action from [mainwindow.ui](https://github.com/pbek/QOwnNotes/blob/develop/src/mainwindow.ui). Just search for the English menu title. Note that these texts can change over time.
:::

Apertura di una finestra di dialogo di input con una casella di selezione
-----------------------------------------

### Chiamata al metodo e parametri
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

### Esempio
```js
var result = script.inputDialogGetItem(
    "combo box", "Please select an item", ["Item 1", "Item 2", "Item 3"]);
script.log(result);
```

You may want to take a look at the example [input-dialogs.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/input-dialogs.qml).

Apertura di una finestra di dialogo di input con una riga di modifica
----------------------------------------

### Chiamata al metodo e parametri
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

### Esempio
```js
var result = script.inputDialogGetText(
    "line edit", "Please enter a name", "current text");
script.log(result);
```

Verificare se esiste un file
-------------------------

### Chiamata al metodo e parametri
```cpp
/**
 * Check if a file exists
 * @param filePath
 * @return
 */
bool ScriptingService::fileExists(QString &filePath);
```

### Esempio
```js
var result = script.fileExists(filePath);
script.log(result);
```

Leggere il testo da un file
------------------------

### Chiamata al metodo e parametri
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

### Esempio
```js
if(script.fileExists(filePath)){
    var data = script.readFromFile(filePath);
    script.log(data);
}
```


Scrittura di testo in un file
----------------------

### Chiamata al metodo e parametri
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

### Esempio
```js
var result = script.writeToFile(filePath, html);
script.log(result);
```

You may want to take a look at the example [export-notes-as-one-html.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/export-notes-as-one-html.qml).

Lavorare con websocket
-----------------------

You can remote control QOwnNotes by using `WebSocketServer`.

Please take a look at the example [websocket-server.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/websocket-server.qml). You can test the socket server by connecting to it on [Websocket test](https://www.websocket.org/echo.html?location=ws://127.0.0.1:35345).

You can also listen to sockets with `WebSocket`. Please take look at the example [websocket-client.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/websocket-client.qml).

Keep in mind that you need to have Qt's QML `websocket` library installed to use this. For example under Ubuntu Linux you can install `qml-module-qtwebsockets`.
