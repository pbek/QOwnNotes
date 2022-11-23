# Metodi e oggetti forniti da QOwnNotes

Avvio di un programma esterno in background
----------------------------------------------


### Chiamata al metodo e parametri
```cpp
/**
 * Wrapper QML per avviare un processo separato
 *
 * @param executablePath the path of the executable
 * @param parameters a list of parameter strings
 * @param callbackIdentifier an identifier to be used in the onDetachedProcessCallback() function (optional)
 * @param callbackParameter an additional parameter for loops or the like (optional)
 * @param processData data written to the process if the callback is used (optional)
 * @param workingDirectory the working directory to execute the process in (optional, only works without callback)
 * @return true on success, false otherwise
 */
bool startDetachedProcess(QString executablePath, QStringList parameters,
                            QString callbackIdentifier, QVariant callbackParameter,
                            QByteArray processData, QString workingDirectory);
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

Avvio di un programma esterno e attesa dell'output
----------------------------------------------------

### Chiamata al metodo e parametri
```cpp
/**
 * Wrapper QML per avviare un processo sincrono
 *
 * @param executablePath the path of the executable
 * @param parameters a list of parameter strings
 * @param data the data that will be written to the process (optional)
 * @param workingDirectory the working directory to execute the process in (optional)
 * @return the text that was returned by the process
QByteArray startSynchronousProcess(QString executablePath, QStringList parameters, QByteArray data, QString workingDirectory);
```

### Esempio
```js
var result = script.startSynchronousProcess("/path/to/my/program", ["my parameter"], "data", "/path/to/execute/in");
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
 * @returns {NoteApi} the current note object
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

Puoi dare un'occhiata all'esempio [insert-headline-with-link-from-github-url.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/insert-headline-with-link-from-github-url.qml).

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
var markdown = script.insertMediaFile("/percorso/del/tuo/file.png");
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

::: tip
Puoi anche assegnare scorciatoie locali e globali alle tue azioni personalizzate nelle *Impostazioni dei collegamenti*.
:::

::: warning
Ricorda che le [icone a tema freedesktop](https://specifications.freedesktop.org/icon-naming-spec/icon-naming-spec-latest.html) sono principalmente disponibili solo in Linux. Quindi se vuoi davvero usare un'icona in macOS o Windows, devi fornirne una col tuo script. Per fare in modo che il tuo script definisca un percorso corretto per la tua icona puoi usare la [proprietà scriptDir](methods-and-objects.md#reading-the-path-to-the-directory-of-your-script).
:::

### Esempio

```js
importa QtQml 2.0
import QOwnNotesTypes 1.0

Script {
     /**
      * Inizializza le azioni personalizzate
      */
     funzione init() {
         // aggiungi un'azione personalizzata senza un pulsante
        script.registerCustomAction("mycustomaction1", "Menu text");

         // aggiungi un'azione personalizzata con un pulsante
        script.registerCustomAction("mycustomaction2", "Menu text", "Button text");

         // aggiungi un'azione personalizzata con un pulsante e l'icona del tema freedesktop
        script.registerCustomAction("mycustomaction3", "Menu text", "Button text", "task-new");
```

Per altri esempi guarda [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml).

::: tip
Puoi anche attivare un'azione personalizzata dopo che l'applicazione è stata avviata con il parametro `--action customAction_<identifier>`. Per maggiori informazioni dai un'occhiata a [Attiva le azioni del menu dopo l'avvio](../getting-started/cli-parameters.md#trigger-menu-actions-after-startup).
:::

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

script.registerLabel("etichetta lunga", "un altro testo molto lungo, un altro testo molto lungo, un altro testo molto lungo, un altro testo molto lungo, un altro testo molto lungo, un altro testo molto lungo, un altro testo molto lungo, un altro testo molto lungo , un altro testo molto lungo, un altro testo molto lungo, un altro testo molto lungo che andrà a capo");

script.registerLabel("counter-label");
```

The labels will be visible in the *Scripting panel*, which you need to enable in the *Window / Panels* menu.

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
 * Restituisce il contenuto degli appunti come testo o html
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
 * Scrive il testo nella posizione corrente del cursore nella modifica del testo della nota
 *
 * @param text
 */
void ScriptingService::noteTextEditWrite(QString text);
```

### Esempio
```js
// scrive il testo nella finestra di modifica del testo della nota
script.noteTextEditWrite("My custom text");
```

Potresti voler esaminare l'azione personalizzata `transformTextRot13` nel file esempio [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml).

Puoi usarlo insieme a `noteTextEditSelectAll` per sovrascrivere l'intero testo della nota corrente.

Leggi il testo selezionato nella finestra di modifica del testo della nota
--------------------------------------------

### Chiamata al metodo e parametri
```cpp
/**
  * Legge il testo selezionato nella finestra di modifica del testo della nota
 *
 * @return
 */
QString ScriptingService::noteTextEditSelectedText();

```

### Esempio
```js
// legge il testo selezionato nella finestra di modifica del testo della nota
var text = script.noteTextEditSelectedText();
```

Potresti voler esaminare l'azione personalizzata `transformTextRot13` nel file esempio [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml).

Seleziona tutto il testo nella finestra di modifica del testo della nota
-------------------------------------

### Chiamata al metodo e parametri
```cpp
/**
 * Seleziona tutto il testo nella finestra di modifica del testo della nota
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
  * Seleziona la riga corrente nella finestra di modifica del testo della nota
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
 * Seleziona la parola corrente nella modifica del testo della nota
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
 * Imposta il testo attualmente selezionato nella modifica del testo della nota
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
 * Restituisce la posizione iniziale della selezione corrente nella modifica del testo della nota
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
 * Restituisce la posizione finale della selezione corrente nella modifica del testo della nota
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
 * Imposta il cursore del testo nella modifica del testo della nota su una determinata posizione
  * 0 sarebbe l'inizio della nota
  * caso speciale: -1 sarebbe la fine della nota
 *
 * @param position
 */
void ScriptingService::noteTextEditSetCursorPosition(int position);
```

### Esempio
```js
// salta all'undicesimo carattere nella nota
script.noteTextEditSetCursorPosition(10);

// salta alla fine della nota
script.noteTextEditSetCursorPosition(-1);
```

Ottieni la posizione corrente del cursore del testo nella finestra di modifica del testo della nota
-----------------------------------------------------------------

### Chiamata al metodo e parametri
```cpp
/**
  * Restituisce la posizione corrente del cursore del testo nella finestra di modifica del testo della nota
  * 0 sarebbe l'inizio della nota
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
 * Legge la parola corrente nella modifica del testo della nota
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
// legge la parola corrente dalla finestra di modifica del testo della nota
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
    // Verrà eseguito solo se sotto Linux
}
```

Etichetta la nota corrente
--------------------

### Chiamata al metodo e parametri
```cpp
/**
  * Contrassegna la nota corrente con un'entichetta denominata tagName
  *
  * @param tagName
  */
void ScriptingService::tagCurrentNote(QString tagName);
```

### Esempio
```js
// aggiungi un'etichetta "preferito" alla nota corrente
script.tagCurrentNote("preferito");
```

Potresti voler dare un'occhiata all'azione personalizzata `favoriteNote` nell'esempio [favorite-note.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/favorite-note.qml).

Crea o recupera un'etichetta dal una lista di elementi
-------------------------------------------------

### Chiamata al metodo e parametri
```cpp
/**
 * Recupera o crea un'etichetta tramite la lista dei suoi nomi
 * L'elemento nameList[0] è il più alto nella gerarchia (con parentId:
 * 0)
 *
 * @param nameList
 * @param createMissing {bool} se true (default) tutte le etichette
 *                             mancanti vengono create
 * @return TagApi oggetto associateo all'etichetta alla massima
 *                profondità
 */
TagApi *ScriptingService::getTagByNameBreadcrumbList(
    const QStringList &nameList, bool createMissing);
```

### Esempio
```js
// crea tutti i tag fino al 3° livello e restituisce l'oggetto tag per
// tag "level3", which would look like that in the tag tree:
// level1 > level2 > level3
var tag = script.getTagByNameBreadcrumbList(["level1", "level2", "level3"]);
```

Cerca le etichette per nome
-----------------------

### Chiamata al metodo e parametri
```cpp
/**
 * Recupera tutti i tag eseguendo una ricerca di sottostringa nel campo del nome
 *
 * @param name {QString} name to search for
 * @return {QStringList} list of tag names
 */
QStringList ScriptingService::searchTagsByName(QString name);
```

### Esempio
```js
// cerca tutti i tag che contengono il gioco di parole
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
 * Sfortunatamente non esiste un modo semplice per utilizzare una QList<NoteApi*> in QML, quindi possiamo trasferire solo gli ID delle note
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

    // fare qualcosa con la nota
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
// ingrandisci il testo nell'elenco delle note
script.addStyleSheet("QTreeWidget#noteTreeWidget {font-size: 30px;}");
```

Puoi dare un'occhiata all'esempio [custom-stylesheet.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-stylesheet.qml).

Puoi ottenere i nomi dei widget dai file `*.ui`, ad esempio la finestra principale è [mainwindow.ui](https://github.com/pbek/QOwnNotes/blob/develop/src/mainwindow.ui).

La documentazione Qt (ad esempio [QMainWindow](https://doc.qt.io/qt-5/qmainwindow.html)) può aiutarti a vedere come sono correlati tra loro i widget (cerca `Eredita` nelle pagine).

Il widget di base per quasi tutto è [QWidget](https://doc.qt.io/qt-5/qwidget.html). Quindi basta disegnare `QWidget` con, ad esempio, `QWidget {background-color: black; color: white;}` significherebbe che tutto ha un colore di sfondo nero e un colore di primo piano bianco.

::: tip
Il [style.qss](https://github.com/pbek/QOwnNotes/blob/develop/src/libraries/qdarkstyle/style.qss) di [qdarkstyle](https://github.com/pbek/QOwnNotes/blob/develop/src/libraries/qdarkstyle) potrebbe anche essere un buon riferimento per gli stili che puoi modificare.
:::

Dai un'occhiata a [Riferimento foglio di stile](http://doc.qt.io/qt-5/stylesheet-reference.html) per un riferimento su quali stili sono disponibili.

Se vuoi inserire stili nell'anteprima HTML per modificare il modo in cui le note vengono visualizzate in anteprima, guarda [notetomarkdownhtmlhook](hooks.html#notetomarkdownhtmlhook).

::: tip
Se vuoi effettivamente vedere come appaiono le finestre di dialogo e quali sono i loro nomi, puoi scaricare [Qt Creator](https://www.qt.io/product/development-tools) e aprire i file `*.ui` al suo interno.
:::

Ricaricamento del motore di scripting
------------------------------

### Chiamata al metodo e parametri
```cpp
/**
  * Ricarica il motore di scripting
 */
void ScriptingService::reloadScriptingEngine();
```

### Esempio
```js
// ricaricare il motore di scripting
script.reloadScriptingEngine();
```

Recupero di una nota in base al nome del file
--------------------------------

### Chiamata al metodo e parametri
```cpp
/**
 * Recupera una nota in base al nome del file
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
// recupera la nota in base al nome del file
script.fetchNoteByFileName("my note.md");
```

Recupero di una nota dal suo ID
-------------------------

### Chiamata al metodo e parametri
```cpp
/**
 * Recupera una nota in base al suo ID
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
 * Verifica se esiste un file di note in base al nome del file
 *
 * @param fileName string the file name of the note (mandatory)
 * @param ignoreNoteId ID intero di una nota da ignorare nell'assegno
 * @param noteSubFolderId ID intero della sottocartella della nota
 * @return bool
 */
bool ScriptingService::noteExistsByFileName(QString fileName,
                                            int ignoreNoteId,
                                            int noteSubFolderId);
```

### Esempio
```js
// controlla se la nota esiste, ma ignora l'id di "note"
script.noteExistsByFileName("my note.md", note.id);
```

Puoi dare un'occhiata all'esempio [paste-latex-image.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/use-tag-names-in-filename.qml).

Copia del testo negli appunti
-------------------------------

### Chiamata al metodo e parametri
```cpp
/**
 * Copia il testo negli appunti come testo normale o dati mime html
 *
 * @param text string text to put into the clipboard
 * @param asHtml bool if true the text will be set as html mime data
 */
void ScriptingService::setClipboardText(QString text, bool asHtml);
```

### Esempio
```js
// copia il testo negli appunti
script.setClipboardText("text to copy");
```

Puoi dare un'occhiata all'esempio [disable-readonly-mode.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/selected-markdown-to-bbcode.qml).

Andare a una nota
-----------------

### Chiamata al metodo e parametri
```cpp
/**
 * Sets the current note if the note is visible in the note list
 *
 * @param note NoteApi note to jump to
 * @param asTab bool if true the note will be opened in a new tab (if not already open)
 */
void ScriptingService::setCurrentNote(NoteApi *note, bool asTab = false);
```

### Esempio
```js
// jump to the note
script.setCurrentNote(note);

// open note in new tab (if not already open)
script.setCurrentNote(note, true);
```

Puoi dare un'occhiata all'esempio [journal-entry.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/journal-entry.qml).

Passaggio a una sottocartella di note
---------------------------

### Chiamata al metodo e parametri
```cpp
/**
 * Salta a una sottocartella di note
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
// passa alla sottocartella delle note "una sottocartella"
script.jumpToNoteSubFolder("a sub folder");

// passa alla sottocartella delle note "sub" all'interno di "una sottocartella"
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
 * Mostra una finestra di messaggio informativo
 *
 * @param text
 * @param title (optional)
 */
void ScriptingService::informationMessageBox(QString text, QString title);
```

### Esempio
```js
// mostrare una finestra di messaggio informativo
script.informationMessageBox("The text I want to show", "Some optional title");
```

Visualizzazione di una finestra di messaggio interrogativo
------------------------------

### Chiamata al metodo e parametri
```cpp
/**
 * Mostra una finestra di messaggio di domanda
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
// mostra una finestra di messaggio con una domanda con un pulsante Applica e un pulsante di aiuto
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
 * Mostra una finestra di dialogo del file aperto
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
// mostra una finestra di dialogo del file aperto
var fileName = script.getOpenFileName("Please select an image", "/home/user/images", "Images (*.png *.xpm *.jpg)");
```

Visualizzazione di una finestra di dialogo Salva file
--------------------------

### Chiamata al metodo e parametri
```cpp
/**
 * Mostra una finestra di dialogo di salvataggio del file
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
// mostra una finestra di dialogo di salvataggio del file
var fileName = script.getSaveFileName("Please select HTML file to save", "output.html", "HTML (*.html)");
```

Puoi dare un'occhiata all'esempio [export-notes-as-one-html.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/export-notes-as-one-html.qml).

Registrazione delle variabili delle impostazioni dello script
-------------------------------------

Si prega di impostare le variabili di settaggio nel proprio script e registrarle in una proprietà nominata `settingsVariables`.

L'utente può quindi impostare queste proprietà nelle impostazioni dello script.

### Esempio
```js
// you have to define your registered variables so you can access them later
property string myString;
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

Inoltre puoi sovrascrivere `settingsVariables` con una funzione speciale `registerSettingsVariables()` come questa:

### Esempio
```js
/**
  * Registra nuovamente le variabili delle impostazioni
  *
  * Utilizzare questo metodo se si desidera utilizzare il codice per sovrascrivere le variabili, ad esempio l'impostazione
  * valori predefiniti a seconda del sistema operativo.
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
// memorizzare variabile persistente
script.setPersistentVariable("PersistentVariablesTest/myVar", result);

// caricare e registrare la variabile persistente
script.log(script.getPersistentVariable("PersistentVariablesTest/myVar", "nothing here yet"));
```

Assicurati di utilizzare un prefisso significativo nella tua chiave come `PersistentVariablesTest/myVar` perché le variabili sono accessibili da tutti gli script.

Potresti anche dare un'occhiata all'esempio [variabili-persistenti.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/persistent-variables.qml).

Caricamento delle variabili delle impostazioni dell'applicazione
--------------------------------------

### Chiamata al metodo e parametri
```cpp
/**
 * Carica una variabile delle impostazioni dell'applicazione
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
// caricare e registrare una variabile delle impostazioni dell'applicazione
script.log(script.getApplicationSettingsVariable("gitExecutablePath"));
```

Tieni presente che le impostazioni in realtà possono essere vuote, devi occupartene tu stesso. `defaultValue` viene utilizzato solo se l'impostazione non esiste affatto.

Creazione di una directory della cache
--------------------------

È possibile memorizzare nella cache i file nella posizione cache predefinita del sistema.

### Chiamata al metodo e parametri
```cpp
/**
 * Restituisce una directory cache per uno script
 *
 * @param {QString} subDir the subfolder to create and use
 * @return {QString} the cache dir path
 */
QString ScriptingService::cacheDir(const QString &subDir) const;
```

### Esempio
```js
// crea la directory della cache per my-script-id
var cacheDirForScript = script.cacheDir("my-script-id");
```

Cancellazione di una directory della cache
--------------------------

Puoi cancellare la directory della cache del tuo script passando il suo nome a clearCacheDir().

### Chiamata al metodo e parametri
```cpp
/**
 * Cancella la directory della cache per uno script
 *
 * @param {QString} subDir the subfolder to clear
 * @return {bool} true on success
 */
bool ScriptingService::clearCacheDir(const QString &subDir) const;
```

### Esempio
```js
// svuota la directory della cache di my-script-id
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
* Restituisce il percorso con i separatori '/' convertiti in separatori che sono
* appropriato per il sistema operativo sottostante.
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
// tornerà "c:\winnt\system32" on Windows
script.log(script.toNativeDirSeparators("c:/winnt/system32"));
```

Conversione dei separatori di percorso da quelli nativi
-------------------------------------------

### Chiamata al metodo e parametri
```cpp
/**
 * Restituisce il percorso usando '/' come separatore di file.
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
 * Restituisce il separatore di directory nativo "/" o "\" su Windows
 *
 * @return
 */
QString ScriptingService::dirSeparator();
```

### Esempio
```js
// tornerà "\" on Windows
script.log(script.dirSeparator());
```

Ottenere un elenco dei percorsi di tutte le note selezionate
-------------------------------------------------

### Chiamata al metodo e parametri
```cpp
/**
 * Restituisce un elenco dei percorsi di tutte le note selezionate
 *
 * @return {QStringList} list of selected note paths
 */
QStringList ScriptingService::selectedNotesPaths();
```

### Esempio
```js
// restituisce un elenco dei percorsi di tutte le note selezionate
script.log(script.selectedNotesPaths());
```

Puoi dare un'occhiata all'esempio [external-note-diff.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/external-note-diff.qml).

Ottenere un elenco degli ID di tutte le note selezionate
-----------------------------------------------

### Chiamata al metodo e parametri
```cpp
/**
 * Restituisce un elenco degli ID di tutte le note selezionate
 *
 * @return {QList<int>} list of selected note ids
 */
QList<int> ScriptingService::selectedNotesIds();
```

### Esempio
```js
// restituisce un elenco degli ID di tutte le note selezionate
script.log(script.selectedNotesIds());
```

Puoi dare un'occhiata all'esempio [esporta-note-come-un-html.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/export-notes-as-one-html.qml).

Attivazione di un'azione di menu
------------------------

### Chiamata al metodo e parametri
```cpp
/**
 * Attiva un'azione di menu
 *
 * @param objectName {QString} object name of the action to trigger
 * @param checked {QString} only trigger the action if checked-state is
 *                          different than this parameter (optional, can be 0 or 1)
 */
void ScriptingService::triggerMenuAction(QString objectName, QString checked);
```

### Esempio
```js
// attiva la modalità di sola lettura
script.triggerMenuAction("actionAllow_note_editing");

// disabilitare la modalità di sola lettura
script.triggerMenuAction("actionAllow_note_editing", 1);
```

Puoi dare un'occhiata all'esempio [disable-readonly-mode.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/disable-readonly-mode.qml).

::: tip
Puoi ottenere i nomi degli oggetti dell'azione del menu da [mainwindow.ui](https://github.com/pbek/QOwnNotes/blob/develop/src/mainwindow.ui). Basta cercare il titolo del menu in inglese. Nota che questi testi possono cambiare nel tempo.
:::

Apertura di una finestra di dialogo di input con una casella di selezione
-----------------------------------------

### Chiamata al metodo e parametri
```cpp
/**
 * Apre una finestra di dialogo di immissione con una casella di selezione
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

Potresti dare un'occhiata all'esempio [input-dialogs.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/input-dialogs.qml).

Apertura di una finestra di dialogo di input con una riga di modifica
----------------------------------------

### Chiamata al metodo e parametri
```cpp
/**
 * Apre una finestra di dialogo di immissione con una modifica della riga
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
 * Controlla se esiste un file
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
 * Leggi il testo da un file
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

Puoi controllare in remoto QOwnNotes utilizzando `WebSocketServer`.

Si prega di dare un'occhiata all'esempio [websocket-server.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/websocket-server.qml). Puoi testare il server socket collegandoti ad esso su [Websocket test](https://www.websocket.org/echo.html?location=ws://127.0.0.1:35345).

Puoi anche ascoltare i socket con `WebSocket`. Per favore guarda l'esempio [websocket-client.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/websocket-client.qml).

Tieni presente che per utilizzarla è necessario che sia installata la libreria QML `websocket` di Qt. Ad esempio in Ubuntu Linux puoi installare `qml-module-qtwebsockets`.

Aggiunta di una regola di evidenziazione per l'editor
-----------------------------------------

Puoi inserire direttamente le regole di evidenziazione nell'editor definendo espressioni regolari e assegnandole a uno stato di evidenziazione.

### Chiamata al metodo e parametri
```cpp
/**
 * Aggiunge una regola di evidenziazione all'evidenziatore della sintassi dell'editor
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

### Highlighting states

| Nome                       | Nr. |
| -------------------------- | --- |
| NoState                    | -1  |
| Collegamento               | 0   |
| Immagine                   | 3   |
| CodeBlock                  | 4   |
| CodeBlockComment           | 5   |
| Corsivo                    | 7   |
| Grassetto                  | 8   |
| Liste                      | 9   |
| Comment                    | 11  |
| H1                         | 12  |
| H2                         | 13  |
| H3                         | 14  |
| H4                         | 15  |
| H5                         | 16  |
| H6                         | 17  |
| BlockQuote                 | 18  |
| HorizontalRuler            | 21  |
| Tabella                    | 22  |
| InlineCodeBlock            | 23  |
| MaskedSyntax               | 24  |
| CurrentLineBackgroundColor | 25  |
| BrokenLink                 | 26  |
| FrontmatterBlock           | 27  |
| TrailingSpace              | 28  |
| CheckBoxUnChecked          | 29  |
| CheckBoxChecked            | 30  |
| StUnderline                | 31  |

### Esempio
```js
// Evidenzia una riga di testo come "BLOCCO: del testo" come citazione in blocco (stato 18)
script.addHighlightingRule("^BLOCK: (.+)", "BLOCK:", 18);

// Mask out (state 24) all characters after 32 characters in a line
// capturingGroup 1 means the expression from the first bracketed part of the pattern will be highlighted
// maskedGroup -1 means that no masking should be done
script.addHighlightingRule("^.{32}(.+)", "", 24, 1, -1);
```

Puoi anche dare un'occhiata agli esempi in [highlighting.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/highlighting.qml).
