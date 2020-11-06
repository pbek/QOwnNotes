# Methoden und Objekte, die QOwnNotes bereitstellt

Starten eines externen Programms im Hintergrund
----------------------------------------------


### Methodenaufruf und Parameter
```cpp
/ **
     * QML-Wrapper zum Starten eines getrennten Prozesses
     * *
     * @param executeablePath der Pfad der ausführbaren Datei
     * @param parameters eine Liste von Parameterzeichenfolgen
     * @param callbackIdentifier Ein Bezeichner, der in der Funktion onDetachedProcessCallback () verwendet werden soll (optional).
     * @param callbackParameter ein zusätzlicher Parameter für Schleifen oder ähnliches (optional)
     * @param processData-Daten, die in den Prozess geschrieben werden, wenn der Rückruf verwendet wird (optional)
     * @return true bei Erfolg, andernfalls false
     * /
bool startDetachedProcess (QString ausführbarer Pfad, QStringList-Parameter,
                             QString callbackIdentifier, QVariant callbackParameter,
                             QByteArray processData);
```

### Beispiel

Einfaches Beispiel:

```js
script.startDetachedProcess("/path/to/my/program", ["my parameter"]);
```

Running a lot of processes:

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

Vielleicht möchten Sie sich das Beispiel ansehen [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml), [callback.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/callback.qml) or [execute-command-after-note-update.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/execute-command-after-note-update.qml).

Vielleicht möchten Sie auch einen Blick auf den Hook [ onDetachedProcessCallback ](hooks.html#ondetachedprocesscallback) werfen.

::: tip
Sie können Ihren benutzerdefinierten Aktionen auch lokale und globale Verknüpfungen zuweisen in den * Shortcuts-Einstellungen*.
:::

Starten Sie ein externes Programm und warten Sie auf die Ausgabe
----------------------------------------------------

### Methodenaufruf und Parameter
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

### Beispiel
```js
var result = script.startSynchronousProcess("/path/to/my/program", ["my parameter"], "data");
```

Vielleicht möchten Sie sich das Beispiel ansehen [encryption-keybase.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/encryption-keybase.qml).

Abrufen des Pfads des aktuellen Notizordners
-------------------------------------------

### Methodenaufruf und Parameter
```cpp
/**
    * QML wrapper to get the current note folder path
    *
    * @return the path of the current note folder
    */
QString currentNoteFolderPath();
```

### Beispiel
```js
var path = script.currentNoteFolderPath();
```

Vielleicht möchten Sie sich das Beispiel ansehen [absolute-media-links.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/absolute-media-links.qml).

Abrufen der aktuellen Notiz
------------------------

### Methodenaufruf und Parameter
```cpp
/ **
     * QML-Wrapper, um die aktuelle Notiz zu erhalten
     * *
     * @returns {NoteApi} das aktuelle Notizobjekt
     * /
NoteApi currentNote ();
```

### Beispiel
```js
var note = script.currentNote();
```

Vielleicht möchten Sie sich das Beispiel ansehen [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml).

Protokollierung beim Protokoll-Widget
-------------------------

### Methodenaufruf und Parameter
```cpp
/ **
     * QML-Wrapper zum Protokollieren im Protokoll-Widget
     * *
     * @param Text
     * /
void log (QString-Text);
```

### Beispiel
```js
script.log("my text");
```

Herunterladen einer URL in eine Zeichenfolge
------------------------------

### Methodenaufruf und Parameter
```cpp
/**
    * QML wrapper to download an url and returning it as text
    *
    * @param url
    * @return {QString} the content of the downloaded url
    */
QString downloadUrlToString(QUrl url);
```

### Beispiel
```js
var html = script.downloadUrlToString("https://www.qownnotes.org");
```

You may want to take a look at the example [insert-headline-with-link-from-github-url.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/insert-headline-with-link-from-github-url.qml).

Herunterladen einer URL in den Medienordner
--------------------------------------

### Methodenaufruf und Parameter
```cpp
/ **
     * QML-Wrapper zum Herunterladen einer URL in den Medienordner und zum Zurückgeben des Mediums
     * URL oder der Markdown-Bildtext des Mediums relativ zur aktuellen Notiz
     * *
     * @param {QString} URL
     * @param {bool} returnUrlOnly wenn true, wird nur die Medien-URL zurückgegeben (Standardwert false)
     * @return {QString} den Medienabschlag oder die URL
     * /
QString downloadUrlToMedia (QUrl url, bool returnUrlOnly);
```

### Beispiel
```js
var markdown = script.downloadUrlToMedia("http://latex.codecogs.com/gif.latex?\frac{1}{1+sin(x)}");
```

You may want to take a look at the example [paste-latex-image.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/paste-latex-image.qml).

Einfügen einer Mediendatei in den Medienordner
--------------------------------------------

### Methodenaufruf und Parameter
```cpp
/ **
     * QML-Wrapper zum Einfügen einer Mediendatei in den Medienordner und zum Zurückgeben
     * die Medien-URL oder der Markdown-Bildtext des Mediums relativ zur aktuellen Notiz
     * *
     * @param {QString} mediaFilePath
     * @param {bool} returnUrlOnly wenn true, wird nur die Medien-URL zurückgegeben (Standardwert false)
     * @return {QString} den Medienabschlag oder die URL
     * /
QString ScriptingService :: insertMediaFile (QString mediaFilePath,
                                         bool returnUrlOnly);
```

### Beispiel
```js
var markdown = script.insertMediaFile("/path/to/your/image.png");
```

Vielleicht möchten Sie sich das Beispiel ansehen [scribble.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/scribble.qml).

Die Notizenvorschau erneuern
-----------------------------

Aktualisiert die Notizvorschau.

### Methodenaufruf und Parameter
```cpp
/ **
     * Regeneriert die Notenvorschau
     * /
QString ScriptingService :: regenerateNotePreview ();
```

### Beispiel
```js
script.regenerateNotePreview();
```

Vielleicht möchten Sie sich das Beispiel ansehen [scribble.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/scribble.qml).

Registrieren einer benutzerdefinierten Aktion
---------------------------

### Methodenaufruf und Parameter
```cpp
/ **
    * Registriert eine benutzerdefinierte Aktion
    * *
    * @param bezeichner der bezeichner der aktion
    * @param menuText den im Menü angezeigten Text
    * @param buttonText den in der Schaltfläche angezeigten Text
    * (keine Schaltfläche wird angezeigt, wenn leer)
    * @param icon Der Pfad der Symboldatei oder der Name eines Freedesktop-Themensymbols
    * Eine Liste der Symbole finden Sie hier:
    * https://specifications.freedesktop.org/icon-naming-spec/icon-naming-spec-latest.html
    * @param useInNoteEditContextMenu Wenn true, verwenden Sie die Aktion in der Notizbearbeitung
    * Kontextmenü (Standard: false)
    * @param hideButtonInToolbar Wenn true, wird die Schaltfläche nicht in der angezeigt
    * Symbolleiste für benutzerdefinierte Aktionen (Standard: false)
    * @param useInNoteListContextMenu Wenn true, verwenden Sie die Aktion in der Notizliste
    * Kontextmenü (Standard: false)
    * /
void ScriptingService :: registerCustomAction (QString-ID,
                                            QString menuText,
                                            QString buttonText,
                                            QString-Symbol,
                                            bool useInNoteEditContextMenu,
                                            bool hideButtonInToolbar,
                                            bool useInNoteListContextMenu);
```

### Beispiel
```js
// eine benutzerdefinierte Aktion ohne Schaltfläche hinzufügen
script.registerCustomAction ("mycustomaction1", "Menütext");

// eine benutzerdefinierte Aktion mit einer Schaltfläche hinzufügen
script.registerCustomAction ("mycustomaction1", "Menütext", "Schaltflächentext");

// füge eine benutzerdefinierte Aktion mit einer Schaltfläche und einem Freedesktop-Themensymbol hinzu
script.registerCustomAction ("mycustomaction1", "Menütext", "Schaltflächentext", "task-new");

// eine benutzerdefinierte Aktion mit einer Schaltfläche und einem Symbol aus einer Datei hinzufügen
script.registerCustomAction ("mycustomaction1", "Menütext", "Schaltflächentext", "/usr/share/icons/breeze/actions/24/view-calendar-tasks.svg");
```

You may then want to use the identifier with function `customActionInvoked` in a script like [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml).

Ein Label registrieren
-------------------

### Methodenaufruf und Parameter
```cpp
/ **
     * Registriert ein Etikett, auf das geschrieben werden soll
     * *
     * @param bezeichner der bezeichner des etiketts
     * @param text der auf dem Etikett angezeigte Text (optional)
     * /
void ScriptingService :: registerLabel (QString-ID, QString-Text);
```

### Beispiel
```js
script.registerLabel ("html-label", "<strong> Starker </ strong> HTML-Text <br /> mit drei Zeilen <br /> und einem <a href='https://www.qownnotes.org'> Link zu einer Website </a>. ");

script.registerLabel ("long-label", "ein anderer sehr langer Text, ein anderer sehr langer Text, ein anderer sehr langer Text, ein anderer sehr langer Text, ein anderer sehr langer Text, ein anderer sehr langer Text, ein anderer sehr langer Text, ein anderer sehr langer Text, ein anderer sehr langer Text, ein anderer sehr langer Text, ein anderer sehr langer Text, der umbrochen wird ");

script.registerLabel ("counter-label");
```

Die Beschriftungen werden im Skript-Dock-Widget angezeigt.

You can use both plain text or html in the labels. Der Text kann ausgewählt werden und Links können angeklickt werden.

Vielleicht möchten Sie sich dann das Beispielskript ansehen [scripting-label-demo.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/scripting-label-demo.qml).

Einstellen des Textes eines registrierten Etiketts
--------------------------------------

### Methodenaufruf und Parameter
```cpp
/ **
     * Legt den Text eines registrierten Etiketts fest
     * *
     * @param bezeichner der bezeichner des etiketts
     * @param text der auf dem Etikett angezeigte Text
     * /
void ScriptingService :: setLabelText (QString-ID, QString-Text);
```

### Beispiel
```js
script.setLabelText("counter-label", "counter text");
```

You can use both plain text or html in the labels. Der Text kann ausgewählt werden und Links können angeklickt werden.

Vielleicht möchten Sie sich dann das Beispielskript ansehen [scripting-label-demo.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/scripting-label-demo.qml).

Neue Notiz erstellen
-------------------

### Methodenaufruf und Parameter
```cpp
/ **
     * Erstellt eine neue Notiz
     * *
     * @param text der notentext
     * /
void ScriptingService :: createNote (QString-Text);
```

### Beispiel
```js
script.createNote("My note headline\n===\n\nMy text");
```

Vielleicht möchten Sie sich das Beispiel ansehen [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml).

Zugriff auf die Zwischenablage
-----------------------

### Methodenaufruf und Parameter
```cpp
/ **
     * Gibt den Inhalt der Zwischenablage als Text oder HTML zurück
     * *
     * @param asHtml gibt den Inhalt der Zwischenablage als HTML anstelle von Text zurück
     * /
QString ScriptingService :: clipboard (bool asHtml);
```

### Beispiel
```js
var clipboardText = script.clipboard();
var clipboardHtml = script.clipboard(true);
```

Vielleicht möchten Sie sich das Beispiel ansehen [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml).

Schreiben Sie Text in die Notiztextbearbeitung
--------------------------------

### Methodenaufruf und Parameter
```cpp
/ **
     * Schreibt Text an die aktuelle Cursorposition in der Notiztextbearbeitung
     * *
     * @param Text
     * /
void ScriptingService :: noteTextEditWrite (QString-Text);
```

### Beispiel
```js
/ schreibe Text in die Notiz Textbearbeitung
script.noteTextEditWrite ("Mein benutzerdefinierter Text");
```

Möglicherweise möchten Sie die benutzerdefinierte Aktion ` transformTextRot13 </ 0> im Beispiel anzeigen
<a href="https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml">custom-actions.qml</a>.
</p>

<p spaces-before="0">Sie können dies zusammen mit <code> noteTextEditSelectAll ` verwenden, um den gesamten Text der aktuellen Notiz zu überschreiben.

Lesen Sie den ausgewählten Text in der Notiztextbearbeitung
--------------------------------------------

### Methodenaufruf und Parameter
```cpp
/ **
     * Liest den ausgewählten Text in der Notiztextbearbeitung
     * *
     * @Rückkehr
     * /
QString ScriptingService :: noteTextEditSelectedText ();
```

### Beispiel
```js
// lese den ausgewählten Text aus der Notiztextbearbeitung
var text = script.noteTextEditSelectedText ();
```

Möglicherweise möchten Sie die benutzerdefinierte Aktion ` transformTextRot13 </ 0> im Beispiel anzeigen
<a href="https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml">custom-actions.qml</a>.
</p>

<h2 spaces-before="0">Wählen Sie den gesamten Text in der Notiztextbearbeitung aus</h2>

<h3 spaces-before="0">Methodenaufruf und Parameter</h3>

<pre><code class="cpp">/ **
     * Wählt den gesamten Text in der Notiztextbearbeitung aus
     * /
void ScriptingService :: noteTextEditSelectAll ();
`</pre>

### Beispiel
```js
script.noteTextEditSelectAll();
```

Sie können dies zusammen mit ` noteTextEditWrite ` verwenden, um den gesamten Text der aktuellen Notiz zu überschreiben.

Wählen Sie die aktuelle Zeile in der Notiztextbearbeitung aus
---------------------------------------------

### Methodenaufruf und Parameter
```cpp
/ **
     * Wählt die aktuelle Zeile in der Notiztextbearbeitung aus
     * /
void ScriptingService :: noteTextEditSelectCurrentLine ();
```

### Beispiel
```js
script.noteTextEditSelectCurrentLine();
```

Wählen Sie das aktuelle Wort in der Notiztextbearbeitung aus
---------------------------------------------

### Methodenaufruf und Parameter
```cpp
/ **
     * Wählt die aktuelle Zeile in der Notiztextbearbeitung aus
     * /
void ScriptingService :: noteTextEditSelectCurrentWord ();
```

### Beispiel
```js
script.noteTextEditSelectCurrentWord();
```

Stellen Sie den aktuell ausgewählten Text in der Notiztextbearbeitung ein
-----------------------------------------------------

### Methodenaufruf und Parameter
```cpp
/ **
     * Legt den aktuell ausgewählten Text in der Notiztextbearbeitung fest
     * *
     * @param start
     * @param end
     * /
void ScriptingService :: noteTextEditSetSelection (int start, int end);
```

### Beispiel
```js
// erweitert die aktuelle Auswahl um ein Zeichen
script.noteTextEditSetSelection (
     script.noteTextEditSelectionStart () - 1,
     script.noteTextEditSelectionEnd () + 1);
```

Holen Sie sich die Startposition der aktuellen Auswahl in der Notiztextbearbeitung
---------------------------------------------------------------------

### Methodenaufruf und Parameter
```cpp
/ **
     * Gibt die Startposition der aktuellen Auswahl in der Notiztextbearbeitung zurück
     * /
int ScriptingService :: noteTextEditSelectionStart ();
```

### Beispiel
```js
script.log(script.noteTextEditSelectionStart());
```

Holen Sie sich die Endposition der aktuellen Auswahl in der Notiztextbearbeitung
-------------------------------------------------------------------

### Methodenaufruf und Parameter
```cpp
/ **
      * Gibt die Startposition der Auswahl in der Notiztextbearbeitung zurück
      * /
int ScriptingService :: noteTextEditSelectionStart ();
```

### Beispiel
```js
script.log(script.noteTextEditSelectionEnd());
```

Read the current word from the note text edit
---------------------------------------------

### Methodenaufruf und Parameter
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

### Beispiel
```js
// Lies das aktuelle Wort in der Notiztextbearbeitung
var text = script.noteTextEditCurrentWord ();
```

You may want to take a look at the example [autocompletion.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/autocompletion.qml).

Prüfen Sie, ob die Plattform Linux, OS X oder Windows ist
------------------------------------------------

### Methodenaufruf und Parameter
```cpp
bool ScriptingService::platformIsLinux();
bool ScriptingService::platformIsOSX();
bool ScriptingService::platformIsWindows();
```

### Beispiel
```js
if (script.platformIsLinux ()) {
     // wird nur unter Linux ausgeführt
}}
```

Kennzeichnen Sie die aktuelle Notiz
--------------------

### Methodenaufruf und Parameter
```cpp
/ **
     * Markiert die aktuelle Notiz mit einem Tag namens tagName
     * *
     * @param tagName
     * /
void ScriptingService :: tagCurrentNote (QString tagName);
```

### Beispiel
```js
// füge der aktuellen Notiz ein "Favorit" -Tag hinzu
script.tagCurrentNote ("Favorit");
```

Vielleicht möchten Sie sich die benutzerdefinierte Aktion ` favorNote ` in der Beispiel [ Favoritennotiz.qml ](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/favorite-note.qml).

Erstellen oder holen Sie ein Tag anhand seiner Namens-Breadcrumb-Liste
-------------------------------------------------

### Methodenaufruf und Parameter
```cpp
/ **
     * Ruft ein Tag anhand seiner "Breadcrumb-Liste" von Tag-Namen ab oder erstellt es
     * Element nameList [0] wäre im Baum am höchsten (mit parentId: 0)
     * *
     * @param nameList
     * @param createMissing {bool} Wenn true (Standard), werden alle fehlenden Tags erstellt
     * @return TagApi-Objekt des tiefsten Tags der Breadcrumb-Liste
     * /
TagApi * ScriptingService :: getTagByNameBreadcrumbList (
     const QStringList & amp; nameList, bool createMissing);
```

### Beispiel
```js
// erstellt alle Tags bis zur 3. Ebene und gibt das Tag-Objekt für zurück
// Tag "level3", der im Tag-Baum so aussehen würde:
// level1> level2> level3
var tag = script.getTagByNameBreadcrumbList (["level1", "level2", "level3"]);
```

Suche nach Tags nach Namen
-----------------------

### Methodenaufruf und Parameter
```cpp
/ **
     * Ruft alle Tags ab, indem eine Teilstringsuche im Namensfeld durchgeführt wird
     * *
     * @param name {QString} Name, nach dem gesucht werden soll
     * @return {QStringList} Liste der Tag-Namen
     * /
QStringList ScriptingService :: searchTagsByName (QString-Name);
```

### Beispiel
```js
// sucht nach allen Tags mit dem Wort Spiel darin
var tags = script.searchTagsByName ("Spiel");
```

Vielleicht möchten Sie sich das Beispiel ansehen [autocompletion.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/autocompletion.qml).

Suchen Sie nach Notizen nach Notiztext
-----------------------------

### Methodenaufruf und Parameter
```cpp
/ **
     * Gibt eine Liste der Notiz-IDs aller Notizen mit einem bestimmten Text im Notiztext zurück
     * *
     * Leider gibt es keine einfache Möglichkeit, eine QList <NoteApi*> in QML zu verwenden
     * kann nur die Noten-IDs übertragen
     * *
     * @return {QList <int>} Liste der Noten-IDs
     * /
QList <int> ScriptingService :: fetchNoteIdsByNoteTextPart (QString-Text);
```

### Beispiel
```js
var noteIds = script.fetchNoteIdsByNoteTextPart ("mytext");

noteIds.forEach (Funktion (noteId) {
     var note = script.fetchNoteById (noteId);

     // mach etwas mit der Notiz
});
```

Vielleicht möchten Sie sich das Beispiel ansehen

 unique-note-id.qml </ 0>.</p> 





Fügen Sie ein benutzerdefiniertes Stylesheet hinzu
-----------------------



### Methodenaufruf und Parameter


```cpp
/ **
     * Fügt der Anwendung ein benutzerdefiniertes Stylesheet hinzu
     * *
     * @param Stylesheet
     * /
void ScriptingService :: addStyleSheet (QString-Stylesheet);
```




### Beispiel


```js
// mache den Text in der Notizliste größer
script.addStyleSheet ("QTreeWidget # noteTreeWidget {Schriftgröße: 30px;}");
```


Vielleicht möchten Sie sich das Beispiel ansehen [ custom-stylesheet.qml ](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-stylesheet.qml).

You can get the object names from the `*.ui` files, for example [mainwindow.ui](https://github.com/pbek/QOwnNotes/blob/develop/src/mainwindow.ui).

Schauen Sie sich das [ Stylesheet für Referenz ](http://doc.qt.io/qt-5/stylesheet-reference.html) für eine Referenz, welche Stile verfügbar sind.

Wenn Sie Stile in die HTML-Vorschau einfügen möchten, um die Vorschau von Notizen zu ändern, lesen Sie bitte [ notetomarkdownhtmlhook ](hooks.html#notetomarkdownhtmlhook).





Neuladen der Scripting-Engine
------------------------------



### Methodenaufruf und Parameter


```cpp
/ **
     * Lädt die Scripting-Engine neu
     * /
void ScriptingService :: reloadScriptingEngine ();
```




### Beispiel


```js
// reload the scripting engine
script.reloadScriptingEngine();
```






Abrufen einer Notiz anhand ihres Dateinamens
--------------------------------



### Methodenaufruf und Parameter


```cpp
/ **
     * Ruft eine Notiz anhand ihres Dateinamens ab
     * *
     * @param fileName string der Dateiname der Notiz (obligatorisch)
     * @param noteSubFolderId Ganzzahl-ID des Notiz-Unterordners
     * @return NoteApi *
     * /
NoteApi * ScriptingService :: fetchNoteByFileName (QString fileName,
                                                 int noteSubFolderId);
```




### Beispiel


```js
// Notiz nach Dateiname holen
script.fetchNoteByFileName ("my note.md");
```






Abrufen einer Notiz anhand ihrer ID
-------------------------



### Methodenaufruf und Parameter


```cpp
/ **
     * Ruft eine Notiz anhand ihrer ID ab
     * *
     * @param id int die ID der Notiz
     * @return NoteApi *
     * /
NoteApi * ScriptingService :: fetchNoteById (int id);
```




### Beispiel


```js
// fetch note by id
script.fetchNoteById(243);
```


Vielleicht möchten Sie sich das Beispiel ansehen [export-notes-as-one-html.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/export-notes-as-one-html.qml).





Überprüfen Sie anhand des Dateinamens, ob eine Notiz vorhanden ist
------------------------------------------



### Methodenaufruf und Parameter


```cpp
/ **
     * Überprüft anhand des Dateinamens, ob eine Notizdatei vorhanden ist
     * *
     * @param fileName string der Dateiname der Notiz (obligatorisch)
     * @param ignoreNoteId Ganzzahl-ID einer Notiz, die bei der Prüfung ignoriert werden soll
     * @param noteSubFolderId Ganzzahl-ID des Notiz-Unterordners
     * @return bool
     * /
bool ScriptingService :: noteExistsByFileName (QString-Dateiname,
                                             int ignoreNoteId,
                                             int noteSubFolderId);
```




### Beispiel


```js
// check if note exists, but ignore the id of "note"
script.noteExistsByFileName("my note.md", note.id);
```


You may want to take a look at the example





Kopieren von Text in die Zwischenablage
-------------------------------



### Methodenaufruf und Parameter


```cpp
/ **
     * Kopiert Text als Nur-Text- oder HTML-MIME-Daten in die Zwischenablage
     * *
     * @param Text String Text, der in die Zwischenablage eingefügt werden soll
     * @param asHtml bool Wenn true, wird der Text als HTML-MIME-Daten festgelegt
     * /
void ScriptingService :: setClipboardText (QString-Text, bool asHtml);
```




### Beispiel


```js
// Text in die Zwischenablage kopieren
script.setClipboardText ("zu kopierender Text");
```


Vielleicht möchten Sie sich das Beispiel ansehen [selected-markdown-to-bbcode.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/selected-markdown-to-bbcode.qml).





Zu einer Notiz springen
-----------------



### Methodenaufruf und Parameter


```cpp
/**
    * Sets the current note if the note is visible in the note list
    *
    * @param note NoteApi note to jump to
    */
void ScriptingService::setCurrentNote(NoteApi *note);
```




### Beispiel


```js
// zur Notiz springen
script.setCurrentNote (Anmerkung);
```


Vielleicht möchten Sie sich das Beispiel ansehen 





Zu einem Notiz-Unterordner springen
---------------------------



### Methodenaufruf und Parameter


```cpp
/ **
     * Springt zu einem Notizunterordner
     * *
     * @param noteSubFolderPath {QString} -Pfad des Unterordners relativ zum Notizordner
     * @param separator {QString} Trennzeichen zwischen Teilen des Pfads, Standard "/"
     * @return true, wenn der Sprung erfolgreich war
     * /
bool ScriptingService :: jumpToNoteSubFolder (const QString & amp; noteSubFolderPath,
                                             QString Separator);
```




### Beispiel


```js
// springe zum Notiz-Unterordner "ein Unterordner"
script.jumpToNoteSubFolder ("ein Unterordner");

// springe zum Notiz-Unterordner "sub" innerhalb von "einem Unterordner"
script.jumpToNoteSubFolder ("ein Unterordner / Unter");
```


::: tip
Sie können einen neuen Notizunterordner im aktuellen Unterordner erstellen, indem Sie aufrufen
:::





Anzeigen eines Informationsmeldungsfelds
----------------------------------



### Methodenaufruf und Parameter


```cpp
/ **
     * Zeigt ein Informationsmeldungsfeld an
     * *
     * @param Text
     * @param title (optional)
     * /
void ScriptingService :: informationMessageBox (QString-Text, QString-Titel);
```




### Beispiel


```js
// ein Informationsmeldungsfeld anzeigen
script.informationMessageBox ("Der Text, den ich anzeigen möchte", "Einige optionale Titel");
```






Anzeigen eines Fragenmeldungsfelds
------------------------------



### Methodenaufruf und Parameter


```cpp
/ **
     * Zeigt ein Fragenfeld an
     * *
     * Informationen zu Schaltflächen finden Sie unter:
     * https://doc.qt.io/qt-5/qmessagebox.html#StandardButton-enum
     * *
     * @param Text
     * @param title (optional)
     * @param Schaltflächen Schaltflächen, die angezeigt werden sollen (optional)
     * @param defaultButton Standardschaltfläche, die ausgewählt wird (optional)
     * @return id der gedrückten Taste
     * /
int ScriptingService :: questionMessageBox (
         QString-Text, QString-Titel, int-Schaltflächen, int defaultButton);
```




### Beispiel


```js
// ein Frage-Meldungsfeld mit einer Schaltfläche zum Anwenden und einer Hilfe anzeigen
// siehe: https://doc.qt.io/qt-5/qmessagebox.html#StandardButton-enum
var result = script.questionMessageBox (
     "Der Text, den ich anzeigen möchte", "Einige optionale Titel", 0x01000000 | 0x02000000, 0x02000000);
script.log (Ergebnis);
```


Informationen zu Schaltflächen finden Sie unter [StandardButton](https://doc.qt.io/qt-5/qmessagebox.html#StandardButton-enum).

Vielleicht möchten Sie sich auch das Beispiel ansehen [input-dialogs.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/input-dialogs.qml).





Anzeigen eines geöffneten Dateidialogs
---------------------------



### Methodenaufruf und Parameter


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




### Beispiel


```js
// zeige einen geöffneten Dateidialog
var fileName = script.getOpenFileName ("Bitte wählen Sie ein Bild aus", "/ home / user / images", "Images (* .png * .xpm * .jpg)");
```






Anzeigen eines Dialogfelds zum Speichern von Dateien
--------------------------



### Methodenaufruf und Parameter


```cpp
/ **
     * Zeigt einen Dialog zum Speichern von Dateien an
     * *
     * @param caption (optional)
     * @param dir (optional)
     * @param Filter (optional)
     * @return QString
     * /
QString ScriptingService :: getSaveFileName (QString-Beschriftung, QString-Verzeichnis,
                                             QString-Filter);
```




### Beispiel


```js
// zeige einen Dialog zum Speichern von Dateien
var fileName = script.getSaveFileName ("Bitte wählen Sie die zu speichernde HTML-Datei aus", "output.html", "HTML (* .html)");
```


You may want to take a look at the example [export-notes-as-one-html.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/export-notes-as-one-html.qml).





Registrieren von Skripteinstellungsvariablen
-------------------------------------

Sie müssen Eigenschaften in Ihrem Skript definieren und in einer weiteren Eigenschaft mit dem Namen `settingsVariables ` registrieren.

Der Benutzer kann diese Eigenschaften dann in den Skripteinstellungen festlegen.



### Beispiel


```js
// Sie müssen Ihre registrierten Variablen definieren, damit Sie später darauf zugreifen können
Eigenschaftszeichenfolge myString;
Eigenschaft bool myBoolean;
Eigenschaftszeichenfolge myText;
property int myInt;
Eigenschaftszeichenfolge myFile;
Eigenschaftszeichenfolge mySelection;

// Registrieren Sie Ihre Einstellungsvariablen, damit der Benutzer sie in den Skripteinstellungen festlegen kann
// benutze diese Eigenschaft, wenn du sie nicht brauchst
// //
// Leider gibt es kein QVariantHash in Qt, wir können es nur verwenden
// QVariantMap (das hat keine willkürliche Reihenfolge) oder QVariantList (die bei
// am wenigsten kann beliebig bestellt werden)
EigenschaftenvarianteneinstellungenVariablen: [
    {
        "bezeichner": "myString",
        "name": "Ich bin ein Zeilenbearbeiter",
        "description": "Bitte geben Sie eine gültige Zeichenfolge ein:",
        "type": "string",
        "default": "Mein Standardwert",
    },
    {
        "bezeichner": "myBoolean",
        "name": "Ich bin ein Kontrollkästchen",
        "description": "Some description",
        "text": "Aktivieren Sie dieses Kontrollkästchen",
        "type": "boolean",
        "default": true,
    },
    {
        "bezeichner": "myText",
        "name": "Ich bin ein Textfeld",
        "description": "Bitte geben Sie Ihren Text ein:",
        "Text eingeben",
        "default": "Dies kann ein sehr langer Text mit mehreren Zeilen sein.",
    },
    {
        "bezeichner": "myInt",
        "name": "Ich bin ein Nummernwähler",
        "description": "Bitte geben Sie eine Nummer ein:",
        "Typ": "Ganzzahl",
        "Standard": 42,
    },
    {
        "bezeichner": "myFile",
        "name": "Ich bin ein Datei-Selektor",
        "description": "Bitte wählen Sie die Datei aus:",
        "Typ": "Datei",
        "default": "pandoc",
    },
    {
        "bezeichner": "mySelection",
        "name": "Ich bin ein Artikelwähler",
        "description": "Bitte wählen Sie einen Artikel aus:",
        "Typ": "Auswahl",
        "default": "option2",
        "items": {"option1": "Text für Option 1", "option2": "Text für Option 2", "option3": "Text für Option 3"},
    }}
];
```


Darüber hinaus können Sie die ` settingsVariables ` mit einer speziellen Funktion ` registerSettingsVariables () ` wie folgt überschreiben:



### Beispiel


```js
/ **
  * Registriert die Einstellungsvariablen erneut
  * *
  * Verwenden Sie diese Methode, wenn Sie Code verwenden möchten, um Ihre Variablen zu überschreiben, z. B. Einstellungen
  * Standardwerte abhängig vom Betriebssystem.
 * /
Funktion registerSettingsVariables () {
     if (script.platformIsWindows ()) {
         // überschreibe den myFile Standardwert
         settingsVariables [3] .default = "pandoc.exe"
     }}
}}
```


Vielleicht möchten Sie sich auch das Beispiel ansehen [variables.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/variables.qml).





Speichern und Laden persistenter Variablen
----------------------------------------



### Methodenaufruf und Parameter


```cpp
/ **
     * Speichert eine persistente Variable
     * Auf diese Variablen kann global über alle Skripte zugegriffen werden
     * Bitte verwenden Sie in Ihrem Schlüssel ein aussagekräftiges Präfix wie "PersistentVariablesTest / myVar".
     * *
     * @param key {QString}
     * @param value {QVariant}
     * /
void ScriptingService :: setPersistentVariable (const QString & amp; key,
                                                 const QVariant & amp; value);

/ **
     * Lädt eine persistente Variable
     * Auf diese Variablen kann global über alle Skripte zugegriffen werden
     * *
     * @param key {QString}
     * @param defaultValue {QVariant} Rückgabewert, wenn die Einstellung nicht vorhanden ist (optional)
     * @Rückkehr
     * /
QVariant ScriptingService :: getPersistentVariable (const QString & amp; key,
                                                     const QVariant & amp; defaultValue);
```




### Beispiel


```js
// store persistent variable
script.setPersistentVariable("PersistentVariablesTest/myVar", result);

// load and log persistent variable
script.log(script.getPersistentVariable("PersistentVariablesTest/myVar", "nothing here yet"));
```


Stellen Sie sicher, dass Sie in Ihrem Schlüssel ein aussagekräftiges Präfix wie ` PersistentVariablesTest / myVar ` verwenden, da auf die Variablen von allen Skripten aus zugegriffen werden kann.

Sie können sich auch das Beispiel [ persistent-variables.qml ](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/persistent-variables.qml) ansehen.





Laden von Anwendungseinstellungsvariablen
--------------------------------------



### Methodenaufruf und Parameter


```cpp
/ **
     * Lädt eine Anwendungseinstellungsvariable
     * *
     * @param key {QString}
     * @param defaultValue {QVariant} Rückgabewert, wenn die Einstellung nicht vorhanden ist (optional)
     * @Rückkehr
     * /
QVariant ScriptingService :: getApplicationSettingsVariable (const QString & amp; key,
                                                             const QVariant & amp; defaultValue);
```




### Beispiel


```js
// load and log an application settings variable
script.log(script.getApplicationSettingsVariable("gitExecutablePath"));
```


Keep in mind that settings actually can be empty, you have to take care about that yourself. ` defaultValue ` wird nur verwendet, wenn die Einstellung überhaupt nicht vorhanden ist.





Creating a cache directory
--------------------------

Sie können Dateien am Standard-Cache-Speicherort Ihres Systems zwischenspeichern.



### Methodenaufruf und Parameter


```cpp
/ **
     * Gibt ein Cache-Verzeichnis für ein Skript zurück
     * *
     * @param {QString} subDir der zu erstellende und zu verwendende Unterordner
     * @return {QString} den Cache-Verzeichnispfad
     * /
QString ScriptingService :: cacheDir (const QString & amp; subDir) const;
```




### Beispiel


```js
// create the cache directory for my-script-id
var cacheDirForScript = script.cacheDir("my-script-id");
```






Löschen eines Cache-Verzeichnisses
--------------------------

Sie können die Cache-Dateien Ihres Skripts löschen, indem Sie den Namen an clearCacheDir () übergeben.



### Methodenaufruf und Parameter


```cpp
/ **
     * Löscht das Cache-Verzeichnis für ein Skript
     * *
     * @param {QString} subDir den zu löschenden Unterordner
     * @return {bool} true bei Erfolg
     * /
bool ScriptingService :: clearCacheDir (const QString & amp; subDir) const;
```




### Beispiel


```js
// clear cache directory of my-script-id 
script.clearCacheDir("my-script-id");
```






Lesen Sie den Pfad zum Verzeichnis Ihres Skripts
------------------------------------------------

Wenn Sie den Pfad zu dem Verzeichnis abrufen müssen, in dem sich Ihr Skript befindet, um beispielsweise andere Dateien zu laden, müssen Sie eine ` -Eigenschaftszeichenfolge scriptDirPath; ` registrieren. Diese Eigenschaft wird mit dem Pfad zum Verzeichnis des Skripts festgelegt.



### Beispiel


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






Konvertieren von Pfadtrennzeichen in native
-----------------------------------------



### Methodenaufruf und Parameter


```cpp
/**
    * Returns path with the '/' separators converted to separators that are
    * appropriate for the underlying operating system.
    * *
     * Unter Windows wird toNativeDirSeparators ("c: / winnt / system32") zurückgegeben
     * "c: \ winnt \ system32".
    *
    * @param path
    * @return
    */
QString ScriptingService::toNativeDirSeparators(QString path);
```




### Beispiel


```js
// will return "c:\winnt\system32" on Windows
script.log(script.toNativeDirSeparators("c:/winnt/system32"));
```






Konvertieren von Pfadtrennzeichen von nativen
-------------------------------------------



### Methodenaufruf und Parameter


```cpp
/ **
     * Gibt den Pfad mit '/' als Dateitrennzeichen zurück.
    * Unter Windows beispielsweise fromNativeDirSeparators ("c: \\ winnt \\ system32")
     * gibt "c: / winnt / system32" zurück.
    * *
     * @param Pfad
     * @Rückkehr
     * /
QString ScriptingService :: fromNativeDirSeparators (QString-Pfad);
```




### Beispiel


```js
// gibt unter Windows "c: / winnt / system32" zurück
script.log (script.fromNativeDirSeparators ("c: \\ winnt \\ system32"));
```






Abrufen des nativen Verzeichnis-Trennzeichens
--------------------------------------



### Methodenaufruf und Parameter


```cpp
/ **
     * Gibt das native Verzeichnis-Trennzeichen "/" oder "\" unter Windows zurück
     * *
     * @Rückkehr
     * /
QString ScriptingService :: dirSeparator ();
```




### Beispiel


```js
// gibt unter Windows "\" zurück
script.log (script.dirSeparator ());
```






Abrufen einer Liste der Pfade aller ausgewählten Noten
-------------------------------------------------



### Methodenaufruf und Parameter


```cpp
/ **
     * Gibt eine Liste der Pfade aller ausgewählten Noten zurück
     * *
     * @return {QStringList} Liste der ausgewählten Notenpfade
     * /
QStringList ScriptingService :: selectedNotesPaths ();
```




### Beispiel


```js
// gibt eine Liste der Pfade aller ausgewählten Noten zurück
script.log (script.selectedNotesPaths ());
```


Vielleicht möchten Sie sich das Beispiel ansehen [external-note-diff.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/external-note-diff.qml).





Abrufen einer Liste der IDs aller ausgewählten Notizen
-----------------------------------------------



### Methodenaufruf und Parameter


```cpp
/ **
     * Gibt eine Liste der IDs aller ausgewählten Noten zurück
     * *
     * @return {QList <int>} Liste der ausgewählten Noten-IDs
     * /
QList <int> ScriptingService :: selectedNotesIds ();
```




### Beispiel


```js
// gibt eine Liste der IDs aller ausgewählten Noten zurück
script.log (script.selectedNotesIds ());
```


Vielleicht möchten Sie sich das Beispiel ansehen [export-notes-as-one-html.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/export-notes-as-one-html.qml).





Auslösen einer Menüaktion
------------------------



### Methodenaufruf und Parameter


```cpp
/ **
     * Löst eine Menüaktion aus
     * *
     * @param objectName {QString} Objektname der auszulösenden Aktion
     * @param checked {QString} löst die Aktion nur aus, wenn der Status check aktiviert ist
     * anders als dieser Parameter (optional, kann 0 oder 1 sein)
     * /
void ScriptingService :: triggerMenuAction (QString objectName, QString geprüft);
```




### Beispiel


```js
// toggle the read-only mode
script.triggerMenuAction("actionAllow_note_editing");

// disable the read-only mode
script.triggerMenuAction("actionAllow_note_editing", 1);
```


Vielleicht möchten Sie sich das Beispiel ansehen [disable-readonly-mode.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/disable-readonly-mode.qml).

Sie können die Objektnamen der Menüaktion von abrufen [mainwindow.ui](https://github.com/pbek/QOwnNotes/blob/develop/src/mainwindow.ui).





Öffnen eines Eingabedialogs mit einem Auswahlfeld
-----------------------------------------



### Methodenaufruf und Parameter


```cpp
/ **
     * Öffnet einen Eingabedialog mit einem Auswahlfeld
     * *
     * @param title {QString} Titel des Dialogfelds
     * @param label {QString} Beschriftungstext des Dialogfelds
     * @param items {QStringList} Liste der auszuwählenden Elemente
     * @param aktueller {int} Index des Elements, das ausgewählt werden soll (Standard: 0)
     * @param editable {bool} Wenn true, kann der Text im Dialogfeld bearbeitet werden (Standard: false).
     * @return {QString} Text des ausgewählten Elements
     * /
QString ScriptingService :: inputDialogGetItem (
         const QString & amp; title, const QString & amp; label, const QStringList & amp; items,
         int current, bool editierbar);
```




### Beispiel


```js
var result = script.inputDialogGetItem (
     "Kombinationsfeld", "Bitte wählen Sie einen Artikel aus", ["Artikel 1", "Artikel 2", "Artikel 3"]);
script.log (Ergebnis);
```


Vielleicht möchten Sie sich das Beispiel ansehen [input-dialogs.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/input-dialogs.qml).





Öffnen eines Eingabedialogs mit einer Zeilenbearbeitung
----------------------------------------



### Methodenaufruf und Parameter


```cpp
/ **
     * Öffnet einen Eingabedialog mit einer Zeilenbearbeitung
     * *
     * @param title {QString} Titel des Dialogfelds
     * @param label {QString} Beschriftungstext des Dialogfelds
     * @param text {QString} Text im Dialog (optional)
     * @Rückkehr
     * /
QString ScriptingService :: inputDialogGetText (
         const QString & amp; title, const QString & amp; label, const QString & amp; text);
```




### Beispiel


```js
var result = script.inputDialogGetText (
     "Zeilenbearbeitung", "Bitte geben Sie einen Namen ein", "aktueller Text");
script.log (Ergebnis);
```






Überprüfen, ob eine Datei vorhanden ist
-------------------------



### Methodenaufruf und Parameter


```cpp
/ **
     * Überprüfen Sie, ob eine Datei vorhanden ist
     * @param filePath
     * @Rückkehr
     * /
bool ScriptingService :: fileExists (QString & amp; filePath);
```




### Beispiel


```js
var result = script.fileExists(filePath);
script.log(result);
```






Text aus einer Datei lesen
------------------------



### Methodenaufruf und Parameter


```cpp
/ **
     * Lesen Sie Text aus einer Datei
     * *
     * @param filePath {QString} Pfad der zu ladenden Datei
     * @param codec {QString} Dateikodierung (Standard: UTF-8)
     * @return die Dateidaten oder null, wenn die Datei nicht existiert
     * /
QString ScriptingService :: readFromFile (const QString & amp; filePath, const QString & amp; Codec)
```




### Beispiel


```js
if (script.fileExists (filePath)) {
     var data = script.readFromFile (filePath);
     script.log (Daten);
}}
```








Text in eine Datei schreiben
----------------------



### Methodenaufruf und Parameter


```cpp
/ **
     * Schreibt einen Text in eine Datei
     * *
     * @param filePath {QString}
     * @param data {QString}
     * @param createParentDirs {bool} optional (Standard: false)
     * @Rückkehr
     * /
bool ScriptingService :: writeToFile (const QString & amp; filePath, const QString & amp; data, bool createParentDirs);
```




### Beispiel


```js
var result = script.writeToFile (filePath, html);
script.log (Ergebnis);
```


Vielleicht möchten Sie sich das Beispiel ansehen [export-notes-as-one-html.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/export-notes-as-one-html.qml).





Arbeiten mit Websockets
-----------------------

Sie können QOwnNotes mithilfe von ` WebSocketServer ` fernsteuern.

Please take a look at the example [websocket-server.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/websocket-server.qml). Du kannst die Socket Server Verbindung testen indem du dich mit [Websocket test](https://www.websocket.org/echo.html?location=ws://127.0.0.1:35345) zu ihr verbindest.

Sie können Sockets auch mit ` WebSocket ` abhören. Bitte schauen Sie sich das Beispiel an: [ websocket-client.qml ](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/websocket-client.qml).

Beachten Sie, dass Sie die QML ` Websocket ` -Bibliothek von Qt installiert haben müssen, um dies zu verwenden. Unter Ubuntu Linux können Sie beispielsweise ` qml-module-qtwebsockets ` installieren.
