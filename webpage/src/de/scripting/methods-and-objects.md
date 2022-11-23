# Methoden und Objekte, die QOwnNotes bereitstellt

Starten eines externen Programms im Hintergrund
----------------------------------------------


### Methodenaufruf und Parameter
```cpp
/**
 * QML wrapper to start a detached process
 *
 * @param executablePath der Pfad der ausführbaren Datei
 * @param parameters eine Liste von Parameterstrings
 * @param callbackIdentifier ein Bezeichner, der in der Funktion onDetachedProcessCallback() verwendet werden soll (optional)
 * @param callbackParameter ein zusätzlicher Parameter für Schleifen oder ähnliches (optional)
 * @param processData data in den Prozess geschrieben, wenn der Callback verwendet wird (optional)
 * @param workingDirectory das Arbeitsverzeichnis, in dem der Prozess ausgeführt werden soll (optional, funktioniert nur ohne Callback)
 * @return true bei Erfolg, sonst false
 */
bool startDetachedProcess(QString executablePath, QStringList parameters,
                            QString callbackIdentifier, QVariant callbackParameter,
                            QByteArray processData, QString workingDirectory);
```

### Beispiel

Einfaches Beispiel:

```js
script.startDetachedProcess("/path/to/my/program", ["my parameter"]);
```

Viele Prozesse ausführen:

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

Vielleicht möchten Sie sich das Beispiel ansehen: [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml), [callback.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/callback.qml) oder: [execute-command-after-note-update.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/execute-command-after-note-update.qml).

Vielleicht möchten Sie auch einen Blick auf den Hook [onDetachedProcessCallback](hooks.html#ondetachedprocesscallback) werfen.

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
 * @param workingDirectory the working directory to execute the process in (optional)
 * @return the text that was returned by the process
QByteArray startSynchronousProcess(QString executablePath, QStringList parameters, QByteArray data, QString workingDirectory);
```

### Beispiel
```js
var result = script.startSynchronousProcess("/path/to/my/program", ["my parameter"], "data", "/path/to/execute/in");
```

Vielleicht möchten Sie sich das Beispiel ansehen [encryption-keybase.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/encryption-keybase.qml).

Abrufen des Pfads des aktuellen Notizordners
-------------------------------------------

### Methodenaufruf und Parameter
```cpp
/**
  * QML-Wrapper, um den aktuellen Pfad des Notizordners abzurufen
  *
  * @ den Pfad des aktuellen Notizordners zurückgeben
  */
QString currentNoteFolderPath ();
```

### Beispiel
```js
var path = script.currentNoteFolderPath();
```

Vielleicht möchten Sie sich das Beispiel [absolute-media-links.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/absolute-media-links.qml) ansehen.

Abrufen der aktuellen Notiz
------------------------

### Methodenaufruf und Parameter
```cpp
/**
     * QML-Wrapper, um die aktuelle Notiz zu erhalten
     *
     * @retourniert {NoteApi} das aktuelle Notizobjekt
     */
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
/**
     * QML-Wrapper zum Protokollieren im Protokoll-Widget
     *
     * @param Text
     */
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
  * QML-Wrapper zum Herunterladen einer URL und zum Zurückgeben als Text
  *
  * @param url
  * @return {QString} den Inhalt der heruntergeladenen URL
  */
QString downloadUrlToString (QUrl url);
```

### Beispiel
```js
var html = script.downloadUrlToString("https://www.qownnotes.org");
```

Vielleicht möchten Sie sich das Beispiel ansehen: [insert-headline-with-link-from-github-url.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/insert-headline-with-link-from-github-url.qml).

Herunterladen einer URL in den Medienordner
--------------------------------------

### Methodenaufruf und Parameter
```cpp
/**
     * QML-Wrapper zum Herunterladen einer URL in den Medienordner und zum Zurückgeben des Mediums
     * URL oder der Markdown-Bildtext des Mediums relativ zur aktuellen Notiz
     *
     * @param {QString} URL
     * @param {bool} returnUrlOnly if true, wird nur die Medien-URL zurückgegeben (Standard false).
     * @return {QString} den Medienabschlag oder die URL
     */
QString downloadUrlToMedia (QUrl url, bool returnUrlOnly);
```

### Beispiel
```js
var markdown = script.downloadUrlToMedia("http://latex.codecogs.com/gif.latex?\frac{1}{1+sin(x)}");
```

Vielleicht möchten Sie sich das Beispiel ansehen [paste-latex-image.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/paste-latex-image.qml).

Einfügen einer Mediendatei in den Medienordner
--------------------------------------------

### Methodenaufruf und Parameter
```cpp
/**
 * QML-Wrapper zum Einfügen einer Mediendatei in den Medienordner und zum Zurückgeben
 * die Medien-URL oder der Markdown-Bildtext des Mediums relativ zur aktuellen Notiz
 *
 * @param {QString} mediaFilePath
 * @param {bool} returnUrlOnly wenn true, wird nur die Medien-URL zurückgegeben (Standardwert false)
 * @return {QString} den Medienabschlag oder die URL
 */
QString ScriptingService::insertMediaFile (QString mediaFilePath,
                                         bool returnUrlOnly);
```

### Beispiel
```js
var markdown = script.insertMediaFile("/path/to/your/image.png");
```

Vielleicht möchten Sie sich das Beispiel ansehen [scribble.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/scribble.qml).

Einfügen einer Anhangsdatei in den Anhangsordner
--------------------------------------------------------

### Methodenaufruf und Parameter
```cpp
 * QML-Wrapper zum Einfügen einer Anhangsdatei in den Ordner `attachments` und
  * Rückgabe der Anhangs-URL oder des Markdown-Textes des Anhangs
  * bezogen auf die aktuelle Notiz
  *
  * @param {QString} AnhangFilePath
  * @param {QString} Dateiname für den Markdown
  * @param {bool} returnUrlOnly - Nur wenn true wird nur die Anhangs-URL zurückgegeben
  * (Standardwert falsch)
  * @return {QString} das Markdown oder die URL des Anhangs
  */
QString ScriptingService::insertAttachmentFile(const QString &attachmentFilePath,
                                                const QString &fileName,
                                                bool returnUrlOnly);
```

### Beispiel
```js
var markdown = script.insertAttachmentFile("/path/to/your/file.png");
```

Die Notizenvorschau erneuern
-----------------------------

Aktualisiert die Notizvorschau.

### Methodenaufruf und Parameter
```cpp
/**
 * Erzeugt die Notizvorschau neu
 */
QString ScriptingService::regenerateNotePreview ();
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
/**
    * Registriert eine benutzerdefinierte Aktion
    *
    * @param bezeichner der bezeichner der aktion
    * @param menuText den im Menü angezeigten Text
    * @param buttonText den in der Schaltfläche angezeigten Text
    *               (keine Schaltfläche wird angezeigt, wenn leer)
    * @param icon Der Pfad der Symboldatei oder der Name eines Freedesktop-Themensymbols
    * Eine Liste der Symbole finden Sie hier:
    * https://specifications.freedesktop.org/icon-naming-spec/icon-naming-spec-latest.html
    * @param useInNoteEditContextMenu Wenn true, verwenden Sie die Aktion in der Notizbearbeitung
    *                Kontextmenü (Standard: false)
    * @param hideButtonInToolbar Wenn true, wird die Schaltfläche nicht in der angezeigt
    *                Symbolleiste für benutzerdefinierte Aktionen (Standard: false)
    * @param useInNoteListContextMenu Wenn true, verwenden Sie die Aktion in der Notizliste
    *                Kontextmenü (Standard: false)
    */
void ScriptingService::registerCustomAction (QString-ID,
                                            QString menuText,
                                            QString buttonText,
                                            QString-Symbol,
                                            bool useInNoteEditContextMenu,
                                            bool hideButtonInToolbar,
                                            bool useInNoteListContextMenu);
```

::: tip
Sie können Ihren benutzerdefinierten Aktionen auch lokale und globale Verknüpfungen in den *Verknüpfungseinstellungen* zuweisen.
:::

::: warning
Denken Sie daran, dass [freedesktop theme icons](https://specifications.freedesktop.org/icon-naming-spec/icon-naming-spec-latest.html) meist nur unter Linux verfügbar sind. Wenn Sie also wirklich ein Symbol unter macOS oder Windows verwenden möchten, müssen Sie eines mit Ihrem Skript bereitstellen. Um den Pfad Ihres Skripts zu erhalten, um einen richtigen Pfad für Ihr Symbol festzulegen, können Sie die [scriptDirPath property](methods-and-objects.md#reading-the-path-to-the-directory-of-your-script) verwenden.
:::

### Beispiel

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

Weitere Beispiele finden Sie unter [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml).

::: tip
Sie können eine benutzerdefinierte Aktion auch nach dem Start der Anwendung mit dem Parameter `--action customAction_<identifier>` auslösen. Weitere Informationen finden Sie unter [Menüaktionen nach dem Start auslösen](../getting-started/cli-parameters.md#trigger-menu-actions-after-startup).
:::

Ein Label registrieren
-------------------

### Methodenaufruf und Parameter
```cpp
/**
     * Registriert ein Etikett, auf das geschrieben werden soll
     *
     * @param bezeichner der bezeichner des etiketts
     * @param text der auf dem Etikett angezeigte Text (optional)
     */
void ScriptingService:: registerLabel (QString-ID, QString-Text);
```

### Beispiel
```js
script.registerLabel("html-label", "<strong>Strong</strong> HTML text<br />with three lines<br />and a <a href='https://www.qownnotes.org'>link to a website</a>.");

script.registerLabel ("long-label", "ein anderer sehr langer Text, ein anderer sehr langer Text, ein anderer sehr langer Text, ein anderer sehr langer Text, ein anderer sehr langer Text, ein anderer sehr langer Text, ein anderer sehr langer Text, ein anderer sehr langer Text, ein anderer sehr langer Text, ein anderer sehr langer Text, ein anderer sehr langer Text, der umbrochen wird ");

script.registerLabel ("counter-label");
```

Die Labels sind im *Skript-Bedienfeld* sichtbar, das Sie im Menü *Fenster / Bedienfelder* aktivieren müssen.

Sie können sowohl einfachen Text als auch HTML in den Beschriftungen verwenden. Der Text kann ausgewählt werden und Links können angeklickt werden.

Vielleicht möchten Sie sich dann das Beispielskript ansehen [scripting-label-demo.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/scripting-label-demo.qml).

Einstellen des Textes eines registrierten Etiketts
--------------------------------------

### Methodenaufruf und Parameter
```cpp
/**
 * Legt den Text eines registrierten Etiketts fest
 *
 * @param identifier the identifier of the label
 * @param text the text shown in the label
 */
void ScriptingService::setLabelText(QString identifier, QString text);
```

### Beispiel
```js
script.setLabelText("counter-label", "counter text");
```

Sie können sowohl einfachen Text als auch HTML in den Beschriftungen verwenden. Der Text kann ausgewählt werden und Links können angeklickt werden.

Vielleicht möchten Sie sich dann das Beispielskript ansehen [scripting-label-demo.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/scripting-label-demo.qml).

Neue Notiz erstellen
-------------------

### Methodenaufruf und Parameter
```cpp
/**
     * Erstellt eine neue Notiz
     **
     * @param text der notentext
     */
void ScriptingService::createNote (QString-Text);
```

### Beispiel
```js
script.createNote("My note headline\n===\n\nMy text");
```

Vielleicht möchten Sie sich das Beispiel ansehen [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml).

::: tip
Wenn Sie deaktiviert haben, dass Ihre Notizüberschrift den Dateinamen der Notiz bestimmt, müssen Sie Ihre Notizdatei anschließend selbst wie folgt umbenennen:

```js
var note = script.currentNote();
note.renameNoteFile('your-filename');
```
:::

Zugriff auf die Zwischenablage
-----------------------

### Methodenaufruf und Parameter
```cpp
/**
     * Gibt den Inhalt der Zwischenablage als Text oder HTML zurück
     *
     * @param asHtml gibt den Inhalt der Zwischenablage als HTML anstelle von Text zurück
     */
QString ScriptingService::clipboard (bool asHtml);
```

### Beispiel
```js
var clipboardText = script.clipboard();
var clipboardHtml = script.clipboard(true);
```

Vielleicht möchten Sie sich das Beispiel ansehen [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml).

Schreiben Sie Text in die Notiztext-Bearbeitung
--------------------------------

### Methodenaufruf und Parameter
```cpp
/**
 * Schreibt Text an die aktuelle Cursorposition in der Notiztextbearbeitung
 *
 * @param text
 */
void ScriptingService::noteTextEditWrite(QString text);
```

### Beispiel
```js
// schreibe Text in die Notiz Textbearbeitung
script.noteTextEditWrite("My custom text");
```

Möglicherweise möchten Sie die benutzerdefinierte Aktion `transformTextRot13` im Beispiel [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml) anzeigen.

Sie können dies zusammen mit ` noteTextEditSelectAll ` verwenden, um den gesamten Text der aktuellen Notiz zu überschreiben.

Lesen Sie den ausgewählten Text in der Notiztextbearbeitung
--------------------------------------------

### Methodenaufruf und Parameter
```cpp
/**
 * Liest den ausgewählten Text in der Notiztextbearbeitung
 *
 * @return
 */
QString ScriptingService::noteTextEditSelectedText();
```

### Beispiel
```js
// lese den ausgewählten Text aus der Notiztextbearbeitung
var text = script.noteTextEditSelectedText ();
```

Vielleicht möchten Sie die benutzerdefinierte Aktion `transformTextRot13` im Beispiel [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml) anzeigen.

Wählen Sie den gesamten Text in der Notiztextbearbeitung aus
-------------------------------------

### Methodenaufruf und Parameter
```cpp
/**
 * Wählt den gesamten Text in der Notiztextbearbeitung aus
 */
void ScriptingService::noteTextEditSelectAll();
```

### Beispiel
```js
script.noteTextEditSelectAll();
```

Sie können dies zusammen mit `noteTextEditWrite` verwenden, um den gesamten Text der aktuellen Notiz zu überschreiben.

Wählen Sie die aktuelle Zeile in der Notiztextbearbeitung aus
---------------------------------------------

### Methodenaufruf und Parameter
```cpp
/**
 * Wählt die aktuelle Zeile in der Notiztextbearbeitung aus
 */
void ScriptingService::noteTextEditSelectCurrentLine();
```

### Beispiel
```js
script.noteTextEditSelectCurrentLine();
```

Wählen Sie das aktuelle Wort in der Notiztextbearbeitung aus
---------------------------------------------

### Methodenaufruf und Parameter
```cpp
/**
  * Wählt das aktuelle Wort in der Notiztextbearbeitung aus
  */
void ScriptingService::noteTextEditSelectCurrentWord();
```

### Beispiel
```js
script.noteTextEditSelectCurrentWord();
```

Stellen Sie den aktuell ausgewählten Text in der Notiztextbearbeitung ein
-----------------------------------------------------

### Methodenaufruf und Parameter
```cpp
/**
 * Sets the currently selected text in the note text edit
 *
 * @param start
 * @param end
 */
void ScriptingService::noteTextEditSetSelection(int start, int end);
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
/**
 * Gibt die Startposition der aktuellen Auswahl in der Notiztextbearbeitung zurück
 */
int ScriptingService::noteTextEditSelectionStart();
```

### Beispiel
```js
script.log(script.noteTextEditSelectionStart());
```

Holen Sie sich die Endposition der aktuellen Auswahl in der Notiztextbearbeitung
-------------------------------------------------------------------

### Methodenaufruf und Parameter
```cpp
/**
 * Gibt die Endposition der aktuellen Auswahl in der Notiztextbearbeitung zurück
 */
int ScriptingService::noteTextEditSelectionEnd();
```

### Beispiel
```js
script.log(script.noteTextEditSelectionEnd());
```

Setzen Sie den Textcursor in der Notiztextbearbeitung auf eine bestimmte Position
---------------------------------------------------------------

### Methodenaufruf und Parameter
```cpp
/**
 * Setzt den Textcursor in der Notiztextbearbeitung auf eine bestimmte Position
 * 0 wäre der Anfang der Notiz
 * Spezialfall: -1 wäre das Ende der Notiz
 *
 * @param position
 */
void ScriptingService::noteTextEditSetCursorPosition(int position);
```

### Beispiel
```js
// springe zum 11. Zeichen in der Notiz
script.noteTextEditSetCursorPosition (10);

// zum Ende der Notiz springen
script.noteTextEditSetCursorPosition (-1);
```

Ruft die aktuelle Position des Textcursors in der Notiztextbearbeitung ab
-----------------------------------------------------------------

### Methodenaufruf und Parameter
```cpp
/**
  * Gibt die aktuelle Position des Textcursors in der Notiztextbearbeitung zurück
  * 0 wäre der Anfang der Notiz
  */
int ScriptingService :: noteTextEditCursorPosition ();
```

### Beispiel
```js
script.log(script.noteTextEditCursorPosition());
```

Lesen Sie das aktuelle Wort aus der Notiztextbearbeitung
---------------------------------------------

### Methodenaufruf und Parameter
```cpp
/**
  * Liest das aktuelle Wort in der Notiztextbearbeitung
  *
  * @param withPreviousCharacters erhalten am Anfang auch mehr Zeichen
  *                                    um Zeichen wie "@" zu erhalten, die es nicht sind
  *                                    Wortzeichen
  * @return
  */
QString ScriptingService::noteTextEditCurrentWord (bool withPreviousCharacters);
```

### Beispiel
```js
// Lies das aktuelle Wort in der Notiztextbearbeitung
var text = script.noteTextEditCurrentWord ();
```

Vielleicht möchten Sie sich das Beispiel ansehen [autocompletion.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/autocompletion.qml).

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
if (script.platformIsLinux()) {
    // Wird nur ausgeführt, wenn unter Linux
}
```

Verschlagworten Sie die aktuelle Notiz
--------------------

### Methodenaufruf und Parameter
```cpp
/**
     * Markiert die aktuelle Notiz mit einem Tag namens tagName
     *
     * @param tagName
     */
void ScriptingService::tagCurrentNote (QString tagName);
```

### Beispiel
```js
// fügen Sie der aktuellen Notiz ein "Favorit" -Tag hinzu
script.tagCurrentNote ("Favorit");
```

Vielleicht möchten Sie sich die benutzerdefinierte Aktion `favoriteNote` in der Beispiel [favorite-note.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/favorite-note.qml).

Erstellen oder holen Sie ein Tag anhand seiner Namens-Breadcrumb-Liste
-------------------------------------------------

### Methodenaufruf und Parameter
```cpp
/**
     * Ruft ein Tag anhand seiner "Breadcrumb-Liste" von Tag-Namen ab oder erstellt es
     * Element nameList[0] wäre im Baum am höchsten (mit parentId: 0)
     *
     * @param nameList
     * @param createMissing {bool} Wenn true (Standard), werden alle fehlenden Tags erstellt
     * @return TagApi-Objekt des tiefsten Tags der Breadcrumb-Liste
     */
TagApi * ScriptingService :: getTagByNameBreadcrumbList (
     const QStringList &nameList, bool createMissing);
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
/**
     * Ruft alle Tags ab, indem eine Teilstringsuche im Namensfeld durchgeführt wird
     *
     * @param name {QString} Name, nach dem gesucht werden soll
     * @return {QStringList} Liste der Tag-Namen
 */
QStringList ScriptingService::searchTagsByName(QString name);
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
/**
     * Gibt eine Liste der Notiz-IDs aller Notizen mit einem bestimmten Text im Notiztext zurück
     *
     * Leider gibt es keine einfache Möglichkeit, eine QList<NoteApi*> in QML zu verwenden
     * kann nur die Noten-IDs übertragen
     *
     * @return {QList<int>} Liste der Noten-IDs
     */
QList<int> ScriptingService::fetchNoteIdsByNoteTextPart (QString-Text);
```

### Beispiel
```js
var noteIds = script.fetchNoteIdsByNoteTextPart ("mytext");

noteIds.forEach (Funktion (noteId) {
     var note = script.fetchNoteById (noteId);

     // mach etwas mit der Notiz
});
```

Vielleicht möchten Sie sich das Beispiel ansehen [unique-note-id.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/unique-note-id.qml).

Fügen Sie ein benutzerdefiniertes Stylesheet hinzu
-----------------------

### Methodenaufruf und Parameter
```cpp
/**
     * Fügt der Anwendung ein benutzerdefiniertes Stylesheet hinzu
     *
     * @param Stylesheet
     */
void ScriptingService::addStyleSheet(QString stylesheet);
```

### Beispiel
```js
// mache den Text in der Notizliste größer
script.addStyleSheet ("QTreeWidget # noteTreeWidget {Schriftgröße: 30px;}");
```

Vielleicht möchten Sie sich das Beispiel ansehen [custom-stylesheet.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-stylesheet.qml).

Sie können die Widget-Namen aus den `*.ui`-Dateien abrufen, zum Beispiel ist das Hauptfenster [mainwindow.ui](https://github.com/pbek/QOwnNotes/blob/develop/src/mainwindow.ui).

Die Qt-Dokumentation (zum Beispiel [QMainWindow](https://doc.qt.io/qt-5/qmainwindow.html)) kann Ihnen helfen zu sehen, wie die Widgets miteinander verbunden sind (suchen Sie auf den Seiten nach `Inherits`).

Das Basis-Widget für fast alles ist [QWidget](https://doc.qt.io/qt-5/qwidget.html). Also einfach `QWidget` mit zum Beispiel `QWidget {background-color: black; color: white;}` würde bedeuten, dass alles eine schwarze Hintergrundfarbe und eine weiße Vordergrundfarbe hat.

::: tip
Die [style.qss](https://github.com/pbek/QOwnNotes/blob/develop/src/libraries/qdarkstyle/style.qss) von [qdarkstyle](https://github.com/pbek/QOwnNotes/blob/develop/src/libraries/qdarkstyle) kann auch eine gute Referenz für Stile sein, die Sie ändern können.
:::

Sehen Sie sich die [Stylesheet-Referenz](http://doc.qt.io/qt-5/stylesheet-reference.html) an, um eine Referenz zu den verfügbaren Stilen zu erhalten.

Wenn Sie Stile in die HTML-Vorschau einfügen möchten, um die Vorschau von Notizen zu ändern, sehen Sie sich bitte [notetomarkdownhtmlhook](hooks.html#notetomarkdownhtmlhook) an.

::: tip
Wenn Sie tatsächlich sehen möchten, wie die Dialoge aussehen und wie sie heißen, können Sie [Qt Creator](https://www.qt.io/product/development-tools) herunterladen und die darin enthaltenen `*.ui`-Dateien öffnen.
:::

Neuladen der Scripting-Engine
------------------------------

### Methodenaufruf und Parameter
```cpp
/**
     * Lädt die Scripting-Engine neu
     */
void ScriptingService::reloadScriptingEngine();
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
/**
  * Ruft eine Notiz anhand ihres Dateinamens ab
 *
 * @param fileName string the file name of the note (mandatory)
 * @param noteSubFolderId integer id of the note subfolder
 * @return NoteApi*
 */
NoteApi* ScriptingService::fetchNoteByFileName(QString fileName,
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
/**
     * Ruft eine Notiz anhand ihrer ID ab
     *
     * @param id int die ID der Notiz
     * @return NoteApi*
     */
NoteApi* ScriptingService::fetchNoteById(int id);
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
/**
     * Überprüft anhand des Dateinamens, ob eine Notizdatei vorhanden ist
     *
     * @param fileName string der Dateiname der Notiz (obligatorisch)
     * @param ignoreNoteId Ganzzahl-ID einer Notiz, die bei der Prüfung ignoriert werden soll
     * @param noteSubFolderId Ganzzahl-ID des Notiz-Unterordners
     * @return bool
     */
bool ScriptingService::noteExistsByFileName (QString-Dateiname,
                                             int ignoreNoteId,
                                             int noteSubFolderId);
```

### Beispiel
```js
// prüfe ob eine Notiz existiert, aber ignoriere die ID von "note"
script.noteExistsByFileName("my note.md", note.id);
```

You may want to take a look at the example

Kopieren von Text in die Zwischenablage
-------------------------------

### Methodenaufruf und Parameter
```cpp
/**
     * Kopiert Text als Nur-Text- oder HTML-MIME-Daten in die Zwischenablage
     *
     * @param Text String Text, der in die Zwischenablage eingefügt werden soll
     * @param asHtml bool Wenn true, wird der Text als HTML-MIME-Daten festgelegt
     */
void ScriptingService::setClipboardText(QString-Text, bool asHtml);
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
 * @param asTab bool if true the note will be opened in a new tab (if not already open)
 */
void ScriptingService::setCurrentNote(NoteApi *note, bool asTab = false);
```

### Beispiel
```js
// jump to the note
script.setCurrentNote(note);

// open note in new tab (if not already open)
script.setCurrentNote(note, true);
```

Vielleicht möchten Sie sich das Beispiel ansehen [journal-entry.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/journal-entry.qml).

Zu einem Notiz-Unterordner springen
---------------------------

### Methodenaufruf und Parameter
```cpp
/**
     * Springt zu einem Notizunterordner
     *
     * @param noteSubFolderPath {QString} -Pfad des Unterordners relativ zum Notizordner
     * @param separator {QString} Trennzeichen zwischen Teilen des Pfads, Standard "/"
     * @return true, wenn der Sprung erfolgreich war
     */
bool ScriptingService::jumpToNoteSubFolder (const QString & noteSubFolderPath,
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
Sie können einen neuen Notizunterordner im aktuellen Unterordner erstellen, indem Sie [`mainWindow.createNewNoteSubFolder`](classes.html#example-2) aufrufen.
:::

Anzeigen eines Informationsmeldungsfelds
----------------------------------

### Methodenaufruf und Parameter
```cpp
/ **
     * Zeigt ein Informationsmeldungsfeld an
     *
     * @param Text
     * @param title (optional)
     */
void ScriptingService::informationMessageBox (QString-Text, QString-Titel);
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
     *
     * Informationen zu Schaltflächen finden Sie unter:
     * https://doc.qt.io/qt-5/qmessagebox.html#StandardButton-enum
     *
     * @param Text
     * @param title (optional)
     * @param Schaltflächen Schaltflächen, die angezeigt werden sollen (optional)
     * @param defaultButton Standardschaltfläche, die ausgewählt wird (optional)
     * @return id der gedrückten Taste
     */
int ScriptingService::questionMessageBox (
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
     * Zeigt einen Dialog zum Speichern von Dateien an
     *
     * @param caption (optional)
     * @param dir (optional)
     * @param Filter (optional)
     * @return QString
     */
QString ScriptingService::getSaveFileName (QString-Beschriftung, QString-Verzeichnis,
                                             QString-Filter);
```

### Beispiel
```js
// zeige einen geöffneten Dateidialog
var fileName = script.getOpenFileName ("Bitte wählen Sie ein Bild aus", "/ home / user / images", "Images (*.png *.xpm *.jpg)");
```

Anzeigen eines Dialogfelds zum Speichern von Dateien
--------------------------

### Methodenaufruf und Parameter
```cpp
/**
     * Zeigt einen Dialog zum Speichern von Dateien an
     *
     * @param caption (optional)
     * @param dir (optional)
     * @param Filter (optional)
     * @return QString
     */
QString ScriptingService :: getSaveFileName (QString-Beschriftung, QString-Verzeichnis,
                                             QString-Filter);
```

### Beispiel
```js
// zeige einen Dialog zum Speichern von Dateien
var fileName = script.getSaveFileName("Please select HTML file to save", "output.html", "HTML (*.html)");
```

Vielleicht möchten Sie sich das Beispiel ansehen [export-notes-as-one-html.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/export-notes-as-one-html.qml).

Registrieren von Skripteinstellungsvariablen
-------------------------------------

Sie müssen Ihre Einstellungsvariablen als Eigenschaften in Ihrem Skript definieren und in einer weiteren Eigenschaft mit dem Namen `settingsVariables ` registrieren.

Der Benutzer kann diese Eigenschaften dann in den Skripteinstellungen festlegen.

### Beispiel
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
/**
  * Speichert eine persistente Variable
  * Diese Variablen sind global über alle Skripte zugänglich
  * Bitte verwenden Sie ein aussagekräftiges Präfix in Ihrem Schlüssel wie "PersistentVariablesTest/myVar"
 *
 * @param key {QString}
 * @param value {QVariant}
 */
void ScriptingService::setPersistentVariable(const QString &key,
                                                const QVariant &value);
/**
  * Lädt eine persistente Variable
  * Diese Variablen sind global über alle Skripte zugänglich
 *
 * @param key {QString}
 * @param defaultValue {QVariant} return value if the setting doesn't exist (optional)
 * @return
 */
QVariant ScriptingService::getPersistentVariable(const QString &key,
                                                    const QVariant &defaultValue);
```

### Beispiel
```js
// store persistent variable
script.setPersistentVariable("PersistentVariablesTest/myVar", result);

// load and log persistent variable
script.log(script.getPersistentVariable("PersistentVariablesTest/myVar", "nothing here yet"));
```

Stellen Sie sicher, dass Sie in Ihrem Schlüssel ein aussagekräftiges Präfix wie `PersistentVariablesTest/myVar` verwenden, da auf die Variablen von allen Skripten aus zugegriffen werden kann.

Sie können sich auch das Beispiel [persistent-variables.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/persistent-variables.qml) ansehen.

Laden von Anwendungseinstellungsvariablen
--------------------------------------

### Methodenaufruf und Parameter
```cpp
/**
     * Lädt eine Anwendungseinstellungsvariable
     *
     * @param key {QString}
     * @param defaultValue {QVariant} Rückgabewert, wenn die Einstellung nicht vorhanden ist (optional)
     * @Rückkehr
     */
QVariant ScriptingService::getApplicationSettingsVariable (const QString &key,
                                                             const QVariant &defaultValue);
```

### Beispiel
```js
// load and log an application settings variable
script.log(script.getApplicationSettingsVariable("gitExecutablePath"));
```

Denken Sie daran, dass die Einstellungen tatsächlich leer sein können und Sie sich selbst darum kümmern müssen. `defaultValue` wird nur verwendet, wenn die Einstellung überhaupt nicht vorhanden ist.

Erstellen eines Cache-Verzeichnisses
--------------------------

Sie können Dateien am Standard-Cache-Speicherort Ihres Systems zwischenspeichern.

### Methodenaufruf und Parameter
```cpp
/**
 * Gibt ein Cache-Verzeichnis für ein Skript zurück
 *
 * @param {QString} subDir der zu erstellende und zu verwendende Unterordner
 * @return {QString} den Cache-Verzeichnispfad
 */
QString ScriptingService::cacheDir (const QString & subDir) const;
```

### Beispiel
```js
// create the cache directory for my-script-id
var cacheDirForScript = script.cacheDir("my-script-id");
```

Löschen eines Cache-Verzeichnisses
--------------------------

Sie können das Cache-Verzeichnis Ihres Skripts löschen, indem Sie seinen Namen an clearCacheDir() übergeben.

### Methodenaufruf und Parameter
```cpp
/**
 * Löscht das Cache-Verzeichnis für ein Skript
 *
 * @param {QString} subDir den zu löschenden Unterordner
 * @return {bool} true bei Erfolg
 */
bool ScriptingService::clearCacheDir (const QString &subDir) const;
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
    // hier wird der Pfad zum Verzeichnis des Skripts gesetzt
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
 * Gibt den Pfad mit den '/' Trennzeichen zurück, die in Trennzeichen konvertiert wurden
 * passend für das zugrunde liegende Betriebssystem.
 *
     * Unter Windows wird toNativeDirSeparators ("c: / winnt / system32") zurückgegeben
 * "c:\winnt\system32".
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
/**
 * Gibt den Pfad mit '/' als Dateitrennzeichen zurück.
 * Unter Windows beispielsweise fromNativeDirSeparators("c:\\winnt\\system32")
 * gibt "c:/winnt/system32" zurück.
 *
 * @param path
 * @return
 */
QString ScriptingService::toNativeDirSeparators(QString path);
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
/**
     * Gibt das native Verzeichnis-Trennzeichen "/" oder "\" unter Windows zurück
     *
     * @Rückkehr
     */
QString ScriptingService::dirSeparator ();
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
/**
     * Gibt eine Liste der Pfade aller ausgewählten Noten zurück
     *
     * @return {QStringList} Liste der ausgewählten Notenpfade
     */
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
/**
     * Gibt eine Liste der IDs aller ausgewählten Noten zurück
     *
     * @return {QList <int>} Liste der ausgewählten Noten-IDs
     */
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
/**
     * Löst eine Menüaktion aus
     *
     * @param objectName {QString} Objektname der auszulösenden Aktion
     * @param checked {QString} löst die Aktion nur aus, wenn der Status check aktiviert ist
     * anders als dieser Parameter (optional, kann 0 oder 1 sein)
     */
void ScriptingService::triggerMenuAction (QString objectName, QString geprüft);
```

### Beispiel
```js
// toggle the read-only mode
script.triggerMenuAction("actionAllow_note_editing");

// disable the read-only mode
script.triggerMenuAction("actionAllow_note_editing", 1);
```

Vielleicht möchten Sie sich das Beispiel ansehen [disable-readonly-mode.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/disable-readonly-mode.qml).

::: tip
Die Objektnamen der Menüaktion erhalten Sie von [mainwindow.ui](https://github.com/pbek/QOwnNotes/blob/develop/src/mainwindow.ui). Suchen Sie einfach nach dem englischen Menütitel. Beachten Sie, dass sich diese Texte im Laufe der Zeit ändern können.
:::

Öffnen eines Eingabedialogs mit einem Auswahlfeld
-----------------------------------------

### Methodenaufruf und Parameter
```cpp
/ **
     * Öffnet einen Eingabedialog mit einem Auswahlfeld
     *
     * @param title {QString} Titel des Dialogfelds
     * @param label {QString} Beschriftungstext des Dialogfelds
     * @param items {QStringList} Liste der auszuwählenden Elemente
     * @param aktueller {int} Index des Elements, das ausgewählt werden soll (Standard: 0)
     * @param editable {bool} Wenn true, kann der Text im Dialogfeld bearbeitet werden (Standard: false).
     * @return {QString} Text des ausgewählten Elements
     */
QString ScriptingService :: inputDialogGetItem(
         const QString & title, const QString & label, const QStringList & items,
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
/**
     * Öffnet einen Eingabedialog mit einer Zeilenbearbeitung
     * *
     * @param title {QString} Titel des Dialogfelds
     * @param label {QString} Beschriftungstext des Dialogfelds
     * @param text {QString} Text im Dialog (optional)
     * @Rückkehr
     * /
QString ScriptingService::inputDialogGetText(
        const QString &title, const QString &label, const QString &text);
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
/**
     * Überprüfen Sie, ob eine Datei vorhanden ist
     * @param filePath
     * @return
     */
bool ScriptingService::fileExists(QString &filePath);
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
/**
 * Lesen Sie Text aus einer Datei
 *
 * @param filePath {QString} Pfad der zu ladenden Datei
 * @param codec {QString} Dateikodierung (Standard: UTF-8)
 * @return die Dateidaten oder null, wenn die Datei nicht existiert
 */
QString ScriptingService::readFromFile(const QString &filePath, const QString &codec)
```

### Beispiel
```js
if(script.fileExists(filePath)){
    var data = script.readFromFile(filePath);
    script.log(data);
}
```


Text in eine Datei schreiben
----------------------

### Methodenaufruf und Parameter
```cpp
/**
     * Schreibt einen Text in eine Datei
     *
     * @param filePath {QString}
     * @param data {QString}
     * @param createParentDirs {bool} optional (Standard: false)
     * @Rückkehr
     */
bool ScriptingService::writeToFile (const QString & filePath, const QString & data, bool createParentDirs);
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

Bitte schauen Sie sich das Beispiel an: [websocket-server.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/websocket-server.qml). Du kannst die Socket Server Verbindung testen indem du dich mit [Websocket test](https://www.websocket.org/echo.html?location=ws://127.0.0.1:35345) zu ihr verbindest.

Sie können Sockets auch mit `WebSocket` abhören. Bitte schauen Sie sich das Beispiel an: [websocket-client.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/websocket-client.qml).

Beachten Sie, dass Sie die QML `Websocket`-Bibliothek von Qt installiert haben müssen, um dies zu verwenden. Unter Ubuntu Linux können Sie beispielsweise `qml-module-qtwebsockets` installieren.

Hinzufügen einer Hervorhebungsregel für den Editor
-----------------------------------------

Sie können Hervorhebungsregeln direkt in den Editor einfügen, indem Sie reguläre Ausdrücke definieren und ihnen einen Hervorhebungszustand zuweisen.

### Methodenaufruf und Parameter
```cpp
/**
  * Fügt dem Syntax-Highlighter des Editors eine Hervorhebungsregel hinzu
  *
  * @param pattern {QString} das hervorzuhebende reguläre Ausdrucksmuster
  * @param shouldContain {QString} eine Zeichenfolge, die im hervorgehobenen Text enthalten sein muss, damit das Muster analysiert werden kann
  * @param state {int} der Zustand des zu verwendenden Syntax-Highlighters
  * @param capturingGroup {int} die Erfassungsgruppe für das Muster, das zum Hervorheben verwendet werden soll (Standard: 0)
  * @param maskedGroup {int} die Erfassungsgruppe für das Muster, das zum Maskieren verwendet werden soll (Standard: 0)
  */
void ScriptingService::addHighlightingRule(const QString &pattern,
                                            const QString &shouldContain,
                                            int-Zustand,
                                            int Erfassungsgruppe,
                                            int maskierteGruppe);
```

### Hervorhebungsarten

| Name                       | Nr. |
| -------------------------- | --- |
| NoState                    | -1  |
| Link                       | 0   |
| Bild                       | 3   |
| CodeBlock                  | 4   |
| CodeBlockComment           | 5   |
| Kursiv                     | 7   |
| Fett                       | 8   |
| Liste                      | 9   |
| Kommentar                  | 11  |
| H1                         | 12  |
| H2                         | 13  |
| H3                         | 14  |
| H4                         | 15  |
| H5                         | 16  |
| H6                         | 17  |
| BlockQuote                 | 18  |
| HorizontalRuler            | 21  |
| Tabelle                    | 22  |
| InlineCodeBlock            | 23  |
| MaskedSyntax               | 24  |
| CurrentLineBackgroundColor | 25  |
| BrokenLink                 | 26  |
| FrontmatterBlock           | 27  |
| TrailingSpace              | 28  |
| CheckBoxUnChecked          | 29  |
| CheckBoxChecked            | 30  |
| StUnderline                | 31  |

### Beispiel
```js
// Hervorheben einer Textzeile wie "BLOCK: irgendein Text" als Blockquote (Zustand 18)
script.addHighlightingRule("^BLOCK: (.+)", "BLOCK:", 18);

// Maskiere (Zustand 24) alle Zeichen nach 32 Zeichen in einer Zeile
// capturingGroup 1 bedeutet, dass der Ausdruck aus dem ersten eingeklammerten Teil des Musters hervorgehoben wird
// maskedGroup -1 bedeutet, dass keine Maskierung erfolgen soll
script.addHighlightingRule("^.{32}(.+)", "", 24, 1, -1);
```

Sie können sich auch die Beispiele in ansehen [highlighting.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/highlighting.qml).
