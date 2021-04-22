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
  * @param callbackIdentifier un identificatore da utilizzare nella funzione onDetachedProcessCallback () (opzionale)
  * @param callbackParameter un parametro aggiuntivo per i loop o simili (opzionale)
  * @param processData dati scritti nel processo se viene utilizzata la richiamata (opzionale)
  * @return true in caso di successo, false in caso contrario
 */
bool startDetachedProcess(QString executablePath, QStringList parameters,
                            QString callbackIdentifier, QVariant callbackParameter,
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

Puoi dare un'occhiata all'esempio [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml), [callback.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/callback.qml) or [execute-command-after-note-update.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/execute-command-after-note-update.qml).

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
 * QML wrapper to get the current note folder path
 *
 * @return the path of the current note folder
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
 * QML wrapper to get the current note
 *
 * @returns {NoteApi} the the current note object
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
 * QML wrapper to log to the log widget
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
 * QML wrapper to download an url and returning it as text
 *
 * @param url
 * @return {QString} the content of the downloaded url
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
  * @param {bool} returnUrlOnly if true solo l'URL multimediale verrà restituito (valore predefinito false)
  * @return {QString} il markdown multimediale o l'URL
```

### Esempio
```js
var markdown = script.insertMediaFile("/path/to/your/image.png");
```

Puoi dare un'occhiata all'esempio [scribble.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/scribble.qml).

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
/ **
  * Registra un'azione personalizzata
  *
  * @param identifier l'identificatore dell'azione
  * @param menu Inserisce il testo mostrato nel menu
  * @param button Inserisce il testo mostrato nel pulsante
  * (nessun pulsante verrà visualizzato se vuoto)
  * @param icon il percorso del file dell'icona o il nome di un'icona del tema freedesktop
  * troverai un elenco di icone qui:
  * https://specifications.freedesktop.org/icon-naming-spec/icon-naming-spec-latest.html
  * @param useInNoteEditContextMenu se true usa l'azione nella modifica della nota
  * menu contestuale (predefinito: falso)
  * @param hideButtonInToolbar se true il pulsante non verrà mostrato nel file
  * barra degli strumenti delle azioni personalizzate (impostazione predefinita: false)
  * @param useInNoteListContextMenu se true usa l'azione nell'elenco delle note
  * menu contestuale (predefinito: false)
  */
void ScriptingService :: registerCustomAction (identificatore QString,
                                            QString menuText,
                                            QString buttonText,
                                            QString icon,
                                            bool useInNoteEditContextMenu,
                                            bool hideButtonInToolbar,
                                            bool useInNoteListContextMenu);
```

### Esempio
```js
// aggiunge un'azione personalizzata senza un pulsante
script.registerCustomAction("mycustomaction1", "Menu text");

// aggiunge un'azione personalizzata con un pulsante
script.registerCustomAction("mycustomaction1", "Menu text", "Button text");

// aggiungi un'azione personalizzata con un pulsante e un'icona del tema freedesktop
script.registerCustomAction("mycustomaction1", "Menu text", "Button text", "task-new");

// aggiunge un'azione personalizzata con un pulsante e un'icona da un file
script.registerCustomAction("mycustomaction1", "Menu text", "Button text", "/usr/share/icons/breeze/actions/24/view-calendar-tasks.svg");
```

È quindi possibile utilizzare l'identificatore con funzione `customActionInvoked` in a script like [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml).

Registrazione di un'etichetta
-------------------

### Chiamata al metodo e parametri
```cpp
/**
  * Registra un'etichetta su cui scrivere
  *
  * @param identifier l'identificatore dell'etichetta
  * @param text il testo mostrato nell'etichetta (opzionale)
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
  * Imposta il testo di un'etichetta registrata
  *
  * @param identifier l'identificatore dell'etichetta
  * @param text il testo mostrato nell'etichetta
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
  * Crea una nuova nota
  *
  * @param text il testo della nota
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

Scrivi il testo nella modifica del testo della nota
--------------------------------

### Chiamata al metodo e parametri
```cpp
/**
  * Scrive il testo nella posizione corrente del cursore nella modifica del testo della nota
 *
 * @param text
 */
void ScriptingService::noteTextEditWrite(QString text);
```

### Esempio
```js
// scrive il testo nella modifica del testo della nota
script.noteTextEditWrite("My custom text");
```

Potresti voler esaminare l'azione personalizzata  `transformTextRot13` nel file esempio [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml).

Puoi usarlo insieme a `noteTextEditSelectAll` per sovrascrivere l'intero testo della nota corrente.

Leggi il testo selezionato nella modifica del testo della nota
--------------------------------------------

### Chiamata al metodo e parametri
```cpp
/**
  * Legge il testo selezionato nella modifica del testo della nota
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

Seleziona tutto il testo nella modifica del testo della nota
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

Seleziona la riga corrente nella modifica del testo della nota
---------------------------------------------

### Chiamata al metodo e parametri
```cpp
/**
  * Seleziona la riga corrente nella modifica del testo della nota
  */
void ScriptingService::noteTextEditSelectCurrentLine();
```

### Esempio
```js
script.noteTextEditSelectCurrentLine();
```

Seleziona la parola corrente nella modifica del testo della nota
---------------------------------------------

### Chiamata al metodo e parametri
```cpp
/**
  * Seleziona la riga corrente nella modifica del testo della nota
  */
void ScriptingService::noteTextEditSelectCurrentWord();
```

### Esempio
```js
script.noteTextEditSelectCurrentWord();
```

Imposta il testo attualmente selezionato nella modifica del testo della nota
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

Ottieni la posizione iniziale della selezione corrente nella modifica del testo della nota
---------------------------------------------------------------------

### Chiamata al metodo e parametri
```cpp
/**
  * Restituisce la posizione iniziale della selezione corrente nella modifica del testo della nota
  */
int ScriptingService::noteTextEditSelectionStart();
```

### Esempio
```js
script.log(script.noteTextEditSelectionStart());
```

Ottieni la posizione finale della selezione corrente nella modifica del testo della nota
-------------------------------------------------------------------

### Chiamata al metodo e parametri
```cpp
/**
  * Restituisce la posizione finale della selezione corrente nella modifica del testo della nota
  */
int ScriptingService::noteTextEditSelectionEnd();
```

### Esempio
```js
script.log(script.noteTextEditSelectionEnd());
```

Impostare il cursore del testo nella modifica del testo della nota su una determinata posizione
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

Ottieni la posizione corrente del cursore del testo nella modifica del testo della nota
-----------------------------------------------------------------

### Chiamata al metodo e parametri
```cpp
/**
  * Restituisce la posizione corrente del cursore del testo nella modifica del testo della nota
  * 0 sarebbe l'inizio della nota
  */
int ScriptingService::noteTextEditCursorPosition();
```

### Esempio
```js
script.log(script.noteTextEditCursorPosition());
```

Leggi la parola corrente dalla modifica del testo della nota
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
  * Tag la nota corrente con un tag denominato tagName
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

You might want to look at the custom action `favoriteNote` in the example [favorite-note.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/favorite-note.qml).

Create or fetch a tag by its name breadcrumb list
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

Cerca i tag per nome
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
  * Restituisce un elenco di ID nota di tutte le note con un determinato testo nel testo della nota
  *
  * Sfortunatamente non esiste un modo semplice per usare una QList<NoteApi*> in QML, quindi noi
  * può trasferire solo gli ID della nota
  *
  * @return {QList<int>} elenco di ID note
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
  * Aggiunge un foglio di stile personalizzato all'applicazione
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

È possibile ottenere i nomi degli oggetti dai file `*.ui`, ad esempio [mainwindow.ui](https://github.com/pbek/QOwnNotes/blob/develop/src/mainwindow.ui).

Take a look at [Style Sheet Reference](http://doc.qt.io/qt-5/stylesheet-reference.html) for a reference of what styles are available.

Se vuoi inserire stili nell'anteprima html per modificare il modo in cui le note vengono visualizzate in anteprima, guarda [notetomarkdownhtmlhook](hooks.html#notetomarkdownhtmlhook).

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

Recupero di una nota in base al nome del file
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

Puoi dare un'occhiata all'esempio [export-notes-as-one-html.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/export-notes-as-one-html.qml).

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

Saltando a una nota
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

Puoi dare un'occhiata all'esempio [journal-entry.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/journal-entry.qml).

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
È possibile creare una nuova sottocartella delle note nella sottocartella corrente chiamando [`mainWindow.createNewNoteSubFolder`](classes.html#example-2).
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

Per informazioni sui pulsanti vedere [StandardButton](https://doc.qt.io/qt-5/qmessagebox.html#StandardButton-enum).

Potresti anche dare un'occhiata all'esempio [input-dialogs.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/input-dialogs.qml).

Visualizzazione di una finestra di dialogo Apri file
---------------------------

### Chiamata al metodo e parametri
```cpp
/**
  * Mostra una finestra di dialogo di apertura del file
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

Puoi dare un'occhiata all'esempio [esporta-note-come-un-html.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/export-notes-as-one-html.qml).

Registrazione delle variabili delle impostazioni dello script
-------------------------------------

Si prega di impostare le variabili di settaggio nel proprio script e registrarle in una proprietà nominata `settingsVariables`.

L'utente può quindi impostare queste proprietà nelle impostazioni dello script.

### Example
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

Inoltre puoi sovrascrivere `settingsVariables` con una funzione speciale `registerSettingsVariables()` come questa:

### Esempio
```js
/**
  * Registra nuovamente le variabili di impostazione
  *
  * Usa questo metodo se vuoi usare il codice per sovrascrivere le tue variabili, come l'impostazione
  * i valori predefiniti dipendono dal sistema operativo.
 */
function registerSettingsVariables() {
    if (script.platformIsWindows()) {
        // override the myFile default value
        settingsVariables[3].default = "pandoc.exe"
    }
}
```

Potresti anche dare un'occhiata all'esempio [variables.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/variables.qml).

Memorizzazione e caricamento di variabili persistenti
----------------------------------------

### Chiamata al metodo e parametri
```cpp
/**
  * Memorizza una variabile persistente
  * Queste variabili sono accessibili globalmente su tutti gli script
  * Utilizza un prefisso significativo nella tua chiave come "PersistentVariablesTest / myVar"
 *
 * @param key {QString}
 * @param value {QVariant}
 */
void ScriptingService::setPersistentVariable(const QString &key,
                                                const QVariant &value);


/ **
  * Carica una variabile persistente
  * Queste variabili sono accessibili globalmente su tutti gli script
  *
 * @param key {QString}
 * @param defaultValue {QVariant} valore di ritorno se l'impostazione non esiste (opzionale)
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

Assicurati di utilizzare un prefisso significativo nella tua chiave come `PersistentVariablesTest/myVar` perché le variabili sono accessibili da tutti gli script.

Potresti anche dare un'occhiata all'esempio [variabili-persistenti.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/persistent-variables.qml).

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

Tieni presente che le impostazioni in realtà possono essere vuote, devi occupartene tu stesso. `defaultValue` is only used if the setting doesn't exist at all.

Creazione di una directory della cache
--------------------------

È possibile memorizzare nella cache i file nella posizione cache predefinita del sistema.

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

Puoi cancellare i file della cache del tuo script passando il suo nome a clearCacheDir().

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

Se è necessario ottenere il percorso della directory in cui è posizionato lo script, ad esempio per caricare altri file, è necessario registrare un file `property string scriptDirPath;`. Questa proprietà verrà impostata con il percorso della directory dello script.

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

### Example
```js
// returns a list of the paths of all selected notes
script.log(script.selectedNotesPaths());
```

Puoi dare un'occhiata all'esempio [external-note-diff.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/external-note-diff.qml).

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

Puoi dare un'occhiata all'esempio [export-notes-as-one-html.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/export-notes-as-one-html.qml).

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

Puoi dare un'occhiata all'esempio [disable-readonly-mode.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/disable-readonly-mode.qml).

È possibile ottenere i nomi degli oggetti dell'azione del menu da [mainwindow.ui](https://github.com/pbek/QOwnNotes/blob/develop/src/mainwindow.ui).

Apertura di una finestra di dialogo di input con una casella di selezione
-----------------------------------------

### Chiamata al metodo e parametri
```cpp
/**
  * Apre una finestra di dialogo di input con una casella di selezione
  *
  * @param title {QString} titolo della finestra di dialogo
  * @param label {QString} testo dell'etichetta della finestra di dialogo
  * @param items {QStringList} elenco di elementi da selezionare
  * @param current {int} indice dell'elemento che dovrebbe essere selezionato (predefinito: 0)
  * @param editable {bool} se true il testo nella finestra di dialogo può essere modificato (impostazione predefinita: false)
  * @return {QString} testo dell'elemento selezionato
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

Potresti dare un'occhiata all'esempio [input-dialogs.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/input-dialogs.qml).

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
  * Scrive un testo in un file
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

Puoi dare un'occhiata all'esempio [export-notes-as-one-html.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/export-notes-as-one-html.qml).

Lavorare con websocket
-----------------------

È possibile controllare in remoto QOwnNotes utilizzando `WebSocketServer`.

Si prega di dare un'occhiata all'esempio [websocket-server.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/websocket-server.qml). Puoi testare il server socket collegandoti ad esso su [Websocket test](https://www.websocket.org/echo.html?location=ws://127.0.0.1:35345).

Puoi anche ascoltare i socket con `WebSocket`. Per favore guarda l'esempio [websocket-client.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/websocket-client.qml).

Keep in mind that you need to have Qt's QML `websocket` library installed to use this. Ad esempio in Ubuntu Linux puoi installare `qml-module-qtwebsockets`.
