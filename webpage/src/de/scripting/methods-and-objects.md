# Methoden und Objekte, die QOwnNotes bereitstellt

## Starten eines externen Programms im Hintergrund

### Methodenaufruf und Parameter

```cpp
/**
 * QML-Wrapper um einen unabhängigen Prozess zu starten
 *
 * @param executablePath der Pfad der Programmdatei
 * @param parameters eine Liste von Parameterstrings
 * @param callbackIdentifier ein Identifier, der in der Funktion onDetachedProcessCallback() verwendet werden soll (optional)
 * @param callbackParameter ein zusätzlicher Parameter für Schleifen oder ähnliches (optional)
 * @param processData in den Prozess geschriebene Daten, wenn der Callback verwendet wird (optional)
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

Schauen Sie sich dazu die Beispiele auf [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-actions.qml), [callback.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/callback.qml) oder [execute-command-after-note-update.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/execute-command-after-note-update.qml) an.

Vielleicht möchten Sie auch einen Blick auf den Hook [onDetachedProcessCallback](hooks.html#ondetachedprocesscallback) werfen.

## Ein externes Programm starten und auf eine Ausgabe warten

### Methodenaufruf und Parameter

```cpp
/**
 * QML-Wrapper, um einen gleichlaufenden Prozess zu starten
 *
 * @param executablePath der Pfad der Programmdatei
 * @param parameters eine Liste von Parameter-strings
 * @param data die Daten, die an den Prozess geschrieben werden (optional)
 * @param workingDirectory das Arbeitsverzeichnis, in dem der Prozess ausgeführt werden soll (optional)
 * @return der Text, der durch den Prozess zurückgegeben wurde
QByteArray startSynchronousProcess(QString executablePath, QStringList parameters, QByteArray data, QString workingDirectory);
```

### Beispiel

```js
var result = script.startSynchronousProcess(
  "/path/to/my/program",
  ["my parameter"],
  "data",
  "/path/to/execute/in",
);
```

Schauen Sie sich dazu das Beispiel auf [encryption-keybase.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/encryption-keybase.qml) an.

## Abrufen des Pfads des aktuellen Notizordners

### Methodenaufruf und Parameter

```cpp
/**
 * QML-Wrapper, um den aktuellen Pfad des Notizordners abzurufen
 *
 * @return der Pfad des aktuellen Notizordners
 */
QString currentNoteFolderPath();
```

### Beispiel

```js
var path = script.currentNoteFolderPath();
```

Schauen Sie sich dazu das Beispiel auf [absolute-media-links.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/absolute-media-links.qml) an.

## Abrufen der aktuellen Notiz

### Methodenaufruf und Parameter

```cpp
/**
 * QML-Wrapper, um die aktuelle Notiz zu erhalten
 *
 * @returns {NoteApi} das aktuelle Notizobjekt
 */
NoteApi currentNote();
```

### Beispiel

```js
var note = script.currentNote();
```

Schauen Sie sich dazu das Beispiel auf [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-actions.qml) an.

## Protokollierung auf das Protokoll-Widget

### Methodenaufruf und Parameter

```cpp
/**
 * QML-Wrapper, um auf das Protokoll-Widget zu protokollieren
 *
 * @param text
 */
void log(QString text);
```

### Beispiel

```js
script.log("my text");
```

## Herunterladen einer URL in eine Zeichenfolge

### Methodenaufruf und Parameter

```cpp
/**
 * QML-Wrapper, um eine URL herunterzuladen und als Text zurückzugeben
 *
 * @param url
 * @return {QString} der Inhalt der heruntergeladenen URL
 */
QString downloadUrlToString(QUrl url);
```

### Beispiel

```js
var html = script.downloadUrlToString("https://www.qownnotes.org");
```

Schauen Sie sich dazu das Beispiel auf [insert-headline-with-link-from-github-url.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/insert-headline-with-link-from-github-url.qml) an.

## Herunterladen einer URL in den Medienordner

### Methodenaufruf und Parameter

```cpp
/**
 * QML-Wrapper, um eine URL in den Medienordner herunterzuladen und die Medien-URL oder
 * den Markdown-Bildtext der Medien relativ zur aktuellen Notiz zurückzugeben
 *
 * @param {QString} url
 * @param {bool} returnUrlOnly falls true, wird nur die Medien-URL zurückgeben (standardmäßig false)
 * @return {QString} Medien-Markdown oder -URL
 */
QString downloadUrlToMedia(QUrl url, bool returnUrlOnly);
```

### Beispiel

```js
var markdown = script.downloadUrlToMedia(
  "http://latex.codecogs.com/gif.latex?\frac{1}{1+sin(x)}",
);
```

Schauen Sie sich das Beispiel auf [paste-latex-image.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/paste-latex-image.qml) an.

## Einfügen einer Mediendatei in den Medienordner

### Methodenaufruf und Parameter

```cpp
/**
 * QML-Wrapper, um eine Mediendatei in den Medienordner einzufügen und die Medien-URL
 * oder den Markdown-Bildtext der Medien relativ zur aktuellen Notiz zurückzugeben
 *
 * @param {QString} mediaFilePath
 * @param {bool} returnUrlOnly falls true, wird nur die Medien-URL zurückgegeben (standardmäßig false)
 * @return {QString} Medien-Markdown oder -URL
 */
QString ScriptingService::insertMediaFile(QString mediaFilePath,
                                        bool returnUrlOnly);
```

### Beispiel

```js
var markdown = script.insertMediaFile("/path/to/your/image.png");
```

Schauen Sie sich dazu das Beispiel auf [scribble.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/scribble.qml) an.

## Einfügen einer Anhangsdatei in den Anhangsordner

### Methodenaufruf und Parameter

```cpp
 * QML-Wrapper zum Einfügen einer Anhangsdatei in den Ordner `attachments` und
 * Rückgabe der Anhangs-URL oder des Markdown-Textes des Anhangs
 * bezogen auf die aktuelle Notiz
 *
 * @param {QString} attachmentFilePath
 * @param {QString} fileName für den Markdown
 * @param {bool} returnUrlOnly falss true, wird nur die Anhangs-URL zurückgegeben
 * (standardmäßig false)
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

## Die Notizvorschau erneuern

Aktualisiert die Notizvorschau.

### Methodenaufruf und Parameter

```cpp
/**
 * Erzeugt die Notizvorschau neu
 */
QString ScriptingService::regenerateNotePreview();
```

### Beispiel

```js
script.regenerateNotePreview();
```

Schauen Sie sich dazu das Beispiel auf [scribble.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/scribble.qml) an.

## Registrieren einer benutzerdefinierten Aktion

### Methodenaufruf und Parameter

```cpp
/**
 * Registriert eine benutzerdefinierte Aktion
 *
 * @param identifier Der Identifier der Aktion
 * @param menuText Der Text, der im Menü angezeigt wird
 * @param buttonText Der Text, der auf dem Button angezeigt wird
 *                   (falls leer, wird kein Button angezeigt)
 * @param icon Dateipfad des Icons, oder der Name eines freedesktop theme-Icons
 *             hier finden Sie eine Liste von Icons:
 *             https://specifications.freedesktop.org/icon-naming-spec/icon-naming-spec-latest.html
 * @param useInNoteEditContextMenu falls true, nutze die Aktion in der Notizbearbeitungs-
 *                                 Konextmenü (standard: false)
 * @param hideButtonInToolbar falls true, wird der Button nicht in der Toolbar für
 *                            benutzerdefinierte Aktionen angezeigt (standard: false)
 * @param useInNoteListContextMenu falls true, wird die Aktion im Notizlisten-
 *                                 Kontextmenü benutzt (standard: false)
 */
void ScriptingService::registerCustomAction(QString identifier,
                                            QString menuText,
                                            QString buttonText,
                                            QString icon,
                                            bool useInNoteEditContextMenu,
                                            bool hideButtonInToolbar,
                                            bool useInNoteListContextMenu);
```

::: tip
You can also assign local and global shortcuts to your custom actions in the _Shortcuts settings_.
:::

::: warning
Denken Sie daran, dass [freedesktop theme Icons](https://specifications.freedesktop.org/icon-naming-spec/icon-naming-spec-latest.html) meist nur unter Linux verfügbar sind. Wenn Sie also wirklich ein Icon unter macOS oder Windows verwenden möchten, müssen Sie eines mit Ihrem Skript bereitstellen. Um den Pfad Ihres Skripts zu erhalten, um einen richtigen Pfad für Ihr Icon festzulegen, können Sie die [scriptDirPath property](methods-and-objects.md#reading-the-path-to-the-directory-of-your-script) verwenden.
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

Weitere Beispiele finden Sie unter [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-actions.qml).

::: tip
Sie können eine benutzerdefinierte Aktion auch nach dem Start der Anwendung mit dem Parameter `--action customAction_<identifier>` auslösen. Weitere Informationen finden Sie unter [Menüaktionen nach dem Start auslösen](../getting-started/cli-parameters.md#trigger-menu-actions-after-startup).
:::

## Ein Label registrieren

### Methodenaufruf und Parameter

```cpp
/**
 * Registriert ein Label, um etwas darauf zu schreiben
 *
 * @param identifier Der Identifier des Labels
 * @param text Der Text, der auf dem Label angezeigt wird (optional)
 */
void ScriptingService::registerLabel(QString identifier, QString text);
```

### Beispiel

```js
script.registerLabel(
  "html-label",
  "<strong>Strong</strong> HTML text<br />with three lines<br />and a <a href='https://www.qownnotes.org'>link to a website</a>.",
);

script.registerLabel(
  "long-label",
  "another very long text, another very long text, another very long text, another very long text, another very long text, another very long text, another very long text, another very long text, another very long text, another very long text, another very long text that will wrap",
);

script.registerLabel("counter-label");
```

The labels will be visible in the _Scripting panel_, which you need to enable in the _Window / Panels_ menu.

Sie können sowohl einfachen Text als auch HTML in den Labels verwenden. Der Text kann ausgewählt werden und Links können angeklickt werden.

Schauen Sie sich dazu das Beispielskript auf [scripting-label-demo.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/scripting-label-demo.qml) an.

## Festlegen des Textes eines registrierten Labels

### Methodenaufruf und Parameter

```cpp
/**
 * Legt den Text eines registrierten Labels fest
 *
 * @param identifier Der Identifier des Labels
 * @param text Der Text, der auf dem Label angezeigt wird
 */
void ScriptingService::setLabelText(QString identifier, QString text);
```

### Beispiel

```js
script.setLabelText("counter-label", "counter text");
```

Sie können sowohl einfachen Text als auch HTML in den Labels verwenden. Der Text kann ausgewählt werden und Links können angeklickt werden.

Schauen Sie sich das Beispielskript auf [scripting-label-demo.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/scripting-label-demo.qml) an.

::: tip
The scripting label also supports **external links**, as well as **internal links** to notes, tasks, deck cards and more. Lesen Sie hier weiter, was alles unterstützt wird: [URL-Unterstützung](https://github.com/pbek/QOwnNotes/blob/964acf69b6382f8ee8252c640c5048f8f4644622/src/utils/urlhandler.cpp#L16-L75).
:::

## Neue Notiz erstellen

### Methodenaufruf und Parameter

```cpp
/**
 * Erstellt eine neue Notiz
 **
 * @param text Der Notiztext
 */
void ScriptingService::createNote(QString-Text);
```

### Beispiel

```js
script.createNote("My note headline\n===\n\nMy text");
```

Schauen Sie sich dazu das Beispiel auf [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-actions.qml) an.

::: tip
Falls Sie abgeschaltet haben, dass Ihre Notizkopfzeile den Notiz-Dateinamen festlegt, müssen Sie die Notizdatei nachher selbst so umbenennen:

```js
var note = script.currentNote();
note.renameNoteFile("your-filename");
```

:::

## Zugriff auf die Zwischenablage

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

Schauen Sie sich dazu das Beispiel auf [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-actions.qml) an.

## Text in das Notizbearbeitungsfeld schreiben

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

Schauen Sie sich die benutzerdefinierte Aktion `transformTextRot13` in dem Beispiel auf [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-actions.qml) an.

Sie können dies zusammen mit `noteTextEditSelectAll` verwenden, um den gesamten Text der aktuellen Notiz zu überschreiben.

## Den ausgewählten Text in der Notizbearbeitung lesen

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
var text = script.noteTextEditSelectedText();
```

Schauen Sie sich die benutzerdefinierte Aktion `transformTextRot13` in dem Beispiel auf [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-actions.qml) an.

## Den gesamten Text in der Notiztextbearbeitung auswählen

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

## Wählen Sie die aktuelle Zeile in der Notiztextbearbeitung aus

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

## Das aktuelle Wort in der Notiztextbearbeitung auswählen

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

## Den aktuell ausgewählten Text in der Notiztextbearbeitung festlegen

### Methodenaufruf und Parameter

```cpp
/**
 * Legt den aktuell markierten Text in der Notiztextbearbeitung fest
 *
 * @param start
 * @param end
 */
void ScriptingService::noteTextEditSetSelection(int start, int end);
```

### Beispiel

```js
// expands the current selection by one character
script.noteTextEditSetSelection(
  script.noteTextEditSelectionStart() - 1,
  script.noteTextEditSelectionEnd() + 1,
);
```

## Die Startposition der aktuellen Auswahl in der Notiztextbearbeitung erhalten

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

## Die Endposition der aktuellen Auswahl in der Notiztextbearbeitung erhalten

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

## Den Textcursor in der Notiztextbearbeitung auf eine bestimmte Position setzen

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
script.noteTextEditSetCursorPosition(10);

// zum Ende der Notiz springen
script.noteTextEditSetCursorPosition(-1);
```

## Die aktuelle Position des Textcursors in der Notiztextbearbeitung abrufen

### Methodenaufruf und Parameter

```cpp
/**
 * Gibt die aktuelle Position des Textcursors in der Notiztextbearbeitung zurück
 * 0 wäre der Anfang der Notiz
 */
int ScriptingService::noteTextEditCursorPosition();
```

### Beispiel

```js
script.log(script.noteTextEditCursorPosition());
```

## Das aktuelle Wort aus der Notiztextbearbeitung lesen

### Methodenaufruf und Parameter

```cpp
/**
 * Liest das aktuelle Wort in der Notiztextbearbeitung
 *
 * @param withPreviousCharacters erhält am Anfang auch mehrere Zeichen
 *                                    um Zeichen wie "@" zu erhalten, die nicht
 *                                    Wortzeichen sind
 * @return
 */
QString ScriptingService::noteTextEditCurrentWord(bool withPreviousCharacters);
```

### Beispiel

```js
// Liest das aktuelle Wort in der Notiztextbearbeitung
var text = script.noteTextEditCurrentWord();
```

Schauen Sie sich dazu das Beispiel auf [autocompletion.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/autocompletion.qml) an.

## Read the current block from the note text edit

### Methodenaufruf und Parameter

```cpp
/**
 * Reads the current block in the note text edit
 *
 * @return
 */
QString ScriptingService::noteTextEditCurrentBlock();
```

### Beispiel

```js
// Read the current block in the note text edit
var text = script.noteTextEditCurrentBlock();
```

You may want to take a look at the example [ai-autocompletion.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/ai-autocompletion.qml).

## Use a completion prompt on the currently selected AI model

The AI completion prompt is a feature that allows you to use a completion prompt on the currently selected AI model, like ChatGPT, Claude or systems like Ollama.

The AI system needs to be enabled in the AI toolbar or main menu for this to work.

### Methodenaufruf und Parameter

```cpp
/**
 * QML wrapper to use the AI Completer
 *
 * @param prompt
 * @return {QString} the result of the completer
 */
QString ScriptingService::aiComplete(const QString& prompt);
```

### Beispiel

```js
// Ask the currently selected AI model to complete the prompt
var text = script.aiComplete("Tell me how do you feel today?");
```

You may want to take a look at the example [ai-autocompletion.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/ai-autocompletion.qml).

## Prüfen, ob die Plattform Linux, OS X oder Windows ist

### Methodenaufruf und Parameter

```cpp
bool ScriptingService::platformIsLinux();
bool ScriptingService::platformIsOSX();
bool ScriptingService::platformIsWindows();
```

### Beispiel

```js
if (script.platformIsLinux()) {
  // Will be executed only if under Linux
}
```

## Der aktuellen Notiz ein Schlagwort hinzufügen

### Methodenaufruf und Parameter

```cpp
/**
 * Markiert die aktuelle Notiz mit einem Schlagwort namens tagName
 *
 * @param tagName
 */
void ScriptingService::tagCurrentNote(QString tagName);
```

### Beispiel

```js
// fügt der aktuellen Notiz ein "favorite"-Schlagwort hinzu
script.tagCurrentNote("favorite");
```

Schauen Sie sich dazu die benutzerdefinierte Aktion `favoriteNote` in dem Beispiel auf [favorite-note.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/favorite-note.qml) an.

## Ein Schlagwort anhand seiner Namen-Breadcrumb-Liste erstellen oder abrufen

### Methodenaufruf und Parameter

```cpp
/**
 * Erstellt oder ruft ein Schlagwort anhand seiner "Breadcrumb-Liste" von Schlagwortnamen auf
 * Element nameList[0] wäre das höchste im Baum (mit parentId: 0)
 *
 * @param nameList
 * @param createMissing {bool} falls true (standard), werden alle fehlenden Schlagworte erstellt
 * @return TagApi Objekt des tiefsten Schlagworts in der Namen-Breadcrumb-Liste
 */
TagApi *ScriptingService::getTagByNameBreadcrumbList(
    const QStringList &nameList, bool createMissing);
```

### Beispiel

```js
// erstellt alle Schlagworte bis zur 3. Ebene und gibt das Schlagwort-Objekt für
// Schlagwort "level3" zurück, das im Tag-Baum so aussehen würde:
// level1 > level2 > level3
var tag = script.getTagByNameBreadcrumbList(["level1", "level2", "level3"]);
```

## Schlagworte nach Namen suchen

### Methodenaufruf und Parameter

```cpp
/**
 * Ruft alle Schlagworte ab, indem eine Teilstringsuche im Namensfeld durchgeführt wird
 *
 * @param name {QString} Name, nach dem gesucht werden soll
 * @return {QStringList} Liste der Schlagwort-Namen
 */
QStringList ScriptingService::searchTagsByName(QString name);
```

### Beispiel

```js
// sucht nach allen Schlagworten mit dem Wort Spiel darin
var tags = script.searchTagsByName("Spiel");
```

Schauen Sie sich dazu das Beispiel auf [autocompletion.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/autocompletion.qml) an.

## Notizen nach Notiztext suchen

### Methodenaufruf und Parameter

```cpp
/**
 * Gibt eine Liste der Notiz-IDs aller Notizen mit einem bestimmten Text im Notiztext zurück
 *
 * Leider gibt es keine einfache Möglichkeit, eine QList<NoteApi*> in QML zu verwenden,
 * weshalb wir nur die Notiz-IDs übertragen können
 *
 * @return {QList<int>} Liste der Notiz-IDs
 */
QList<int> ScriptingService::fetchNoteIdsByNoteTextPart(QString text);
```

### Beispiel

```js
var noteIds = script.fetchNoteIdsByNoteTextPart("mytext");

noteIds.forEach(function (noteId) {
  var note = script.fetchNoteById(noteId);

  // do something with the note
});
```

Schauen Sie sich das Beispiel auf [unique-note-id.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/unique-note-id.qml) an.

## Ein benutzerdefiniertes Stylesheet hinzufügen

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
// vergrößere den Text in der Notizliste
script.addStyleSheet("QTreeWidget#noteTreeWidget {font-size: 30px;}");
```

Schauen Sie sich das Beispiel auf [custom-stylesheet.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-stylesheet.qml) an.

Sie können die Widget-Namen aus den `*.ui`-Dateien erhalten; das Hauptfenster ist beispielsweise [mainwindow.ui](https://github.com/pbek/QOwnNotes/blob/main/src/mainwindow.ui).

Die Qt-Dokumentation (zum Beispiel [QMainWindow](https://doc.qt.io/qt-5/qmainwindow.html)) kann Ihnen helfen zu sehen, wie die Widgets miteinander verbunden sind (suchen Sie auf den Seiten nach `Inherits`).

Das Basis-Widget für fast alles ist [QWidget](https://doc.qt.io/qt-5/qwidget.html). Also einfach `QWidget` mit zum Beispiel `QWidget {background-color: black; color: white;}` würde bedeuten, dass alles eine schwarze Hintergrundfarbe und eine weiße Vordergrundfarbe hat.

::: tip
[style.qss](https://github.com/pbek/QOwnNotes/blob/main/src/libraries/qdarkstyle/style.qss) von [qdarkstyle](https://github.com/pbek/QOwnNotes/blob/main/src/libraries/qdarkstyle) ist möglicherweise auch eine gute Referenz für Styles, die Sie ändern können.
:::

Sehen Sie sich die [Stylesheet-Referenz](http://doc.qt.io/qt-5/stylesheet-reference.html) an, um eine Referenz zu den verfügbaren Stilen zu erhalten.

Wenn Sie Stile in die HTML-Vorschau einfügen möchten, um die Vorschau von Notizen zu ändern, sehen Sie sich bitte [notetomarkdownhtmlhook](hooks.html#notetomarkdownhtmlhook) an.

::: tip
Wenn Sie tatsächlich sehen möchten, wie die Dialoge aussehen und wie sie heißen, können Sie [Qt Creator](https://www.qt.io/product/development-tools) herunterladen und die darin enthaltenen `*.ui`-Dateien öffnen.
:::

## Neuladen der Scripting-Engine

### Methodenaufruf und Parameter

```cpp
/**
 * Lädt die Scripting-Engine neu
 */
void ScriptingService::reloadScriptingEngine();
```

### Beispiel

```js
// lädt die Scripting-Engine neu
script.reloadScriptingEngine();
```

## Abrufen einer Notiz anhand ihres Dateinamens

### Methodenaufruf und Parameter

```cpp
/**
 * Ruft eine Notiz anhand ihres Dateinamens ab
 *
 * @param fileName string Der Dateinamen der Notiz (notwendig)
 * @param noteSubFolderId integer ID des Notizunterordners
 * @return NoteApi*
 */
NoteApi* ScriptingService::fetchNoteByFileName(QString fileName,
                                                int noteSubFolderId);
```

### Beispiel

```js
// Notiz nach Dateiname abrufen
script.fetchNoteByFileName("my note.md");
```

## Abrufen einer Notiz anhand ihrer ID

### Methodenaufruf und Parameter

```cpp
/**
 * Ruft eine Notiz anhand ihrer ID ab
 *
 * @param id int Die ID der Notiz
 * @return NoteApi*
 */
NoteApi* ScriptingService::fetchNoteById(int id);
```

### Beispiel

```js
// Notiz nach ID abrufen
script.fetchNoteById(243);
```

Schauen Sie sich das Beispiel auf [export-notes-as-one-html.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/export-notes-as-one-html.qml) an.

## Anhand des Dateinamens überprüfen, ob eine Notiz vorhanden ist

### Methodenaufruf und Parameter

```cpp
/**
 * Überprüft anhand des Dateinamens, ob eine Notizdatei vorhanden ist
 *
 * @param fileName string Der Dateiname der Notiz (obligatorisch)
 * @param ignoreNoteId integer ID einer Notiz, die bei der Prüfung ignoriert werden soll
 * @param noteSubFolderId integer ID des Notiz-Unterordners
 * @return bool
 */
bool ScriptingService::noteExistsByFileName(QString fileName,
                                            int ignoreNoteId,
                                            int noteSubFolderId);
```

### Beispiel

```js
// prüfe ob eine Notiz existiert, aber ignoriere die ID von "note"
script.noteExistsByFileName("my note.md", note.id);
```

Schauen Sie sich das Beispiel auf [use-tag-names-in-filename.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/use-tag-names-in-filename.qml) an.

## Kopieren von Text in die Zwischenablage

### Methodenaufruf und Parameter

```cpp
/**
 * Kopiert Text als Klartext- oder HTML-MIME-Daten in die Zwischenablage
 *
 * @param text string Text, der in die Zwischenablage eingefügt werden soll
 * @param asHtml bool Wenn true, wird der Text als HTML-MIME-Daten festgelegt
 */
void ScriptingService::setClipboardText(QString text, bool asHtml);
```

### Beispiel

```js
// Text in die Zwischenablage kopieren
script.setClipboardText("zu kopierender Text");
```

Schauen Sie sich das Beispiel auf [selected-markdown-to-bbcode.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/selected-markdown-to-bbcode.qml) an.

## Zu einer Notiz springen

### Methodenaufruf und Parameter

```cpp
/**
 * Legt die aktuelle Notiz fest, wenn die Notiz in der Notizliste sichtbar ist
 *
 * @param note NoteApi Notiz, in die gesprungen werden soll
 * @param asTab bool falls true, wird die Notiz in einem neuen Tab geöffnet (falls nicht schon geöffnet)
 */
void ScriptingService::setCurrentNote(NoteApi *note, bool asTab = false);
```

### Beispiel

```js
// springt zur Notiz
script.setCurrentNote(note);

// öffnet Notiz in neuem Tab (falls nicht schon geöffnet)
script.setCurrentNote(note, true);
```

Schauen Sie sich das Beispiel auf [journal-entry.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/journal-entry.qml) an.

## Zu einem Notiz-Unterordner springen

### Methodenaufruf und Parameter

```cpp
/**
 * Springt zu einem Notizunterordner
 *
 * @param noteSubFolderPath {QString} Pfad des Unterordners relativ zum Notizordner
 * @param separator {QString} Trennzeichen zwischen Teilen des Pfads, Standard "/"
 * @return true, wenn der Sprung erfolgreich war
 */
bool ScriptingService::jumpToNoteSubFolder(const QString &noteSubFolderPath,
                                            QString separator);
```

### Beispiel

```js
// springe zum Notiz-Unterordner "ein Unterordner"
script.jumpToNoteSubFolder("ein Unterordner");

// springe zum Notiz-Unterordner "Sub" innerhalb von "einem Unterordner"
script.jumpToNoteSubFolder("ein Unterordner/Sub");
```

::: tip
Sie können einen neuen Notizunterordner im aktuellen Unterordner erstellen, indem Sie [`mainWindow.createNewNoteSubFolder`](classes.html#example-2) aufrufen.
:::

## Anzeigen eines Informationsmeldungsfelds

### Methodenaufruf und Parameter

```cpp
/ **
 * Zeigt ein Informationsmeldungsfeld an
 *
 * @param text
 * @param title (optional)
 */
void ScriptingService::informationMessageBox(QString text, QString title);
```

### Beispiel

```js
// ein Informationsmeldungsfeld anzeigen
script.informationMessageBox("Der Text, den ich anzeigen möchte", "Ein optionaler Titel");
```

## Anzeigen eines Fragenmeldungsfelds

### Methodenaufruf und Parameter

```cpp
/ **
 * Zeigt ein Fragenmeldungsfeld an
 *
 * Informationen zu Schaltflächen finden Sie unter:
 * https://doc.qt.io/qt-5/qmessagebox.html#StandardButton-enum
 *
 * @param text
 * @param title (optional)
 * @param buttons Schaltflächen, die angezeigt werden sollen (optional)
 * @param defaultButton Standardschaltfläche, die ausgewählt wird (optional)
 * @return ID der gedrückten Schaltfläche
 */
int ScriptingService::questionMessageBox(
        QString text, QString title, int buttons, int defaultButton);
```

### Beispiel

```js
// show a question message box with an apply and a help button
// see: https://doc.qt.io/qt-5/qmessagebox.html#StandardButton-enum
var result = script.questionMessageBox(
  "The text I want to show",
  "Some optional title",
  0x01000000 | 0x02000000,
  0x02000000,
);
script.log(result);
```

Informationen zu Schaltflächen finden Sie unter [StandardButton](https://doc.qt.io/qt-5/qmessagebox.html#StandardButton-enum).

Schauen Sie sich dazu das Beispiel auf [input-dialogs.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/input-dialogs.qml) an.

## Anzeigen eines Dialogs zum Öffnen von Dateien

### Methodenaufruf und Parameter

```cpp
/**
 * Zeigt einen Dialog zum Öffnen von Dateien an
 *
 * @param caption (optional)
 * @param dir (optional)
 * @param filter (optional)
 * @return QString
 */
QString ScriptingService::getSaveFileName(QString caption, QString dir,
                                             QString filter);
```

### Example

```js
// show an open file dialog
var fileName = script.getOpenFileName(
  "Please select an image",
  "/home/user/images",
  "Images (*.png *.xpm *.jpg)",
);
```

## Anzeigen eines Dialogfelds zum Speichern von Dateien

### Methodenaufruf und Parameter

```cpp
/**
 * Zeigt einen Dialog zum Speichern von Dateien an
 *
 * @param caption (optional)
 * @param dir (optional)
 * @param filter (optional)
 * @return QString
 */
QString ScriptingService::getSaveFileName(QString caption, QString dir,
                                             QString filter);
```

### Beispiel

```js
// show a save file dialog
var fileName = script.getSaveFileName(
  "Please select HTML file to save",
  "output.html",
  "HTML (*.html)",
);
```

Schauen Sie sich dazu das Beispiel auf [export-notes-as-one-html.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/export-notes-as-one-html.qml) an.

## Registrieren von Skripteinstellungsvariablen

Sie müssen Ihre Einstellungsvariablen als Eigenschaften in Ihrem Skript definieren und in einer weiteren Eigenschaft mit dem Namen `settingsVariables` registrieren.

Der Benutzer kann diese Eigenschaften dann in den Skripteinstellungen festlegen.

### Beispiel

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

### Beispiel

```js
/**
 * Registriert die Einstellungsvariablen erneut
 *
 * Verwenden Sie diese Methode, wenn Sie Code verwenden möchten, um Ihre Variablen zu überschreiben, z.B.
 * um Standardwerte abhängig vom Betriebssystem festzulegen.
 */
function registerSettingsVariables() {
  if (script.platformIsWindows()) {
    // override the myFile default value
    settingsVariables[3].default = "pandoc.exe";
  }
}
```

Schauen Sie sich dazu das Beispiel auf [variables.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/variables.qml) an.

## Speichern und Laden persistenter Variablen

### Methodenaufruf und Parameter

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

### Beispiel

```js
// store persistent variable
script.setPersistentVariable("PersistentVariablesTest/myVar", result);

// load and log persistent variable
script.log(
  script.getPersistentVariable(
    "PersistentVariablesTest/myVar",
    "nothing here yet",
  ),
);
```

Stellen Sie sicher, dass Sie in Ihrem Schlüssel ein aussagekräftiges Präfix wie `PersistentVariablesTest/myVar` verwenden, da auf die Variablen von allen Skripten aus zugegriffen werden kann.

Schauen Sie sich dazu das Beispiel auf [persistent-variables.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/persistent-variables.qml) an.

## Laden von Anwendungseinstellungsvariablen

### Methodenaufruf und Parameter

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

### Beispiel

```js
// Anwendungseinstellungsvariable laden und protokollieren script.log(script.getApplicationSettingsVariable("gitExecutablePath"));
```

Denken Sie daran, dass die Einstellungen tatsächlich leer sein können und Sie sich selbst darum kümmern müssen. `defaultValue` wird nur verwendet, wenn die Einstellung überhaupt nicht vorhanden ist.

## Erstellen eines Cache-Verzeichnisses

Sie können Dateien am Standard-Cache-Speicherort Ihres Systems zwischenspeichern.

### Methodenaufruf und Parameter

```cpp
/**
 * Gibt ein Cache-Verzeichnis für ein Skript zurück
 *
 * @param {QString} subDir Der zu erstellende und zu verwendende Unterordner
 * @return {QString} Der Cache-Verzeichnispfad
 */
QString ScriptingService::cacheDir(const QString &subDir) const;
```

### Beispiel

```js
// erstellt das Cache-Verzeichnis für my-script-id
var cacheDirForScript = script.cacheDir("my-script-id");
```

## Löschen eines Cache-Verzeichnisses

Sie können das Cache-Verzeichnis Ihres Skripts löschen, indem Sie seinen Namen an clearCacheDir() übergeben.

### Methodenaufruf und Parameter

```cpp
/**
 * Löscht das Cache-Verzeichnis für ein Skript
 *
 * @param {QString} subDir Der zu löschende Unterordner
 * @return {bool} true bei Erfolg
 */
bool ScriptingService::clearCacheDir(const QString &subDir) const;
```

### Beispiel

```js
// clear cache directory of my-script-id
script.clearCacheDir("my-script-id");
```

## Den Pfad zum Verzeichnis Ihres Skripts lesen

Wenn Sie den Pfad zu dem Verzeichnis abrufen müssen, in dem sich Ihr Skript befindet, um beispielsweise andere Dateien zu laden, müssen Sie eine `property string scriptDirPath;` registrieren. Diese Eigenschaft wird mit dem Pfad zum Verzeichnis des Skripts festgelegt.

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

## Pfadtrennzeichen zu nativen konvertieren

### Methodenaufruf und Parameter

```cpp
/**
 * Gibt den Pfad zurück, und konvertiert die "/" Trennzeichen zu Trennzeichen,
 * die für das zugrundeliegende Betriebssystem angemessen sind.
 *
 * Unter Windows, toNativeDirSeparators("c:/winnt/system32") gibt
 * "c:\winnt\system32" zurück.
 *
 * @param path
 * @return
 */
QString ScriptingService::toNativeDirSeparators(QString path);
```

### Beispiel

```js
// gibt "c:\winnt\system32" unter Windows zurück
script.log(script.toNativeDirSeparators("c:/winnt/system32"));
```

## Pfadtrennzeichen von nativen konvertieren

### Methodenaufruf und Parameter

```cpp
/**
 * Gibt den Pfad mit '/' als Dateitrennzeichen zurück.
 * Unter Windows gibt fromNativeDirSeparators("c:\\winnt\\system32")
 * beispielsweise "c:/winnt/system32" zurück.
 *
 * @param path
 * @return
 */
QString ScriptingService::toNativeDirSeparators(QString path);
```

### Beispiel

```js
// gibt unter Windows "c:/winnt/system32" zurück
script.log(script.fromNativeDirSeparators("c:\\winnt\\system32"));
```

## Abrufen des nativen Verzeichnis-Trennzeichens

### Methodenaufruf und Parameter

```cpp
/**
 * Gibt das native Verzeichnis-Trennzeichen "/" oder "\" unter Windows zurück
 *
 * @return
 */
QString ScriptingService::dirSeparator();
```

### Beispiel

```js
// gibt unter Windows "\" zurück
script.log(script.dirSeparator());
```

## Abrufen einer Liste der Pfade aller ausgewählten Notizen

### Methodenaufruf und Parameter

```cpp
/**
 * Gibt eine Liste der Pfade aller ausgewählten Notizen zurück
 *
 * @return {QStringList} Liste der ausgewählten Notizpfade
 */
QStringList ScriptingService::selectedNotesPaths();
```

### Beispiel

```js
// gibt eine Liste der Pfade aller ausgewählten Notizen zurück
script.log(script.selectedNotesPaths());
```

Schauen Sie sich dazu das Beispiel auf [external-note-diff.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/external-note-diff.qml) an.

## Abrufen einer Liste der IDs aller ausgewählten Notizen

### Methodenaufruf und Parameter

```cpp
/**
 * Gibt eine Liste der IDs aller ausgewählten Notizen zurück
 *
 * @return {QList<int>} Liste der ausgewählten Notiz-IDs
 */
QList<int> ScriptingService::selectedNotesIds();
```

### Beispiel

```js
// gibt eine Liste der IDs aller ausgewählten Notizen zurück
script.log(script.selectedNotesIds());
```

Schauen Sie sich dazu das Beispiel auf [export-notes-as-one-html.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/export-notes-as-one-html.qml) an.

## Auslösen einer Menüaktion

### Methodenaufruf und Parameter

```cpp
/**
 * Löst eine Menüaktion aus
 *
 * @param objectName {QString} Objektname der auszulösenden Aktion
 * @param checked {QString} löst die Aktion nur aus, wenn der check-state anders
 *                          als dieser Parameter ist (optional, kann 0 oder 1 sein)
 */
void ScriptingService::triggerMenuAction(QString objectName, QString checked);
```

### Beispiel

```js
// wechsle in den Nur-Lesen-Modus
script.triggerMenuAction("actionAllow_note_editing");

// schalte den Nur-Lesen-Modus ab
script.triggerMenuAction("actionAllow_note_editing", 1);
```

Schauen Sie sich dazu das Beispiel auf [disable-readonly-mode.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/disable-readonly-mode.qml) an.

::: tip
Sie können die Objektnamen der Menüaktion aus [mainwindow.ui](https://github.com/pbek/QOwnNotes/blob/main/src/mainwindow.ui) erhalten. Suchen Sie einfach nach dem englischen Menütitel. Beachten Sie, dass sich diese Texte im Laufe der Zeit ändern können.
:::

## Öffnen eines Eingabedialogs mit einem Auswahlfeld

### Methodenaufruf und Parameter

```cpp
/ **
 * Öffnet einen Eingabedialog mit einem Auswahlfeld
 *
 * @param title {QString} Titel des Dialogfelds
 * @param label {QString} Beschriftungstext des Dialogfelds
 * @param items {QStringList} Liste der auszuwählenden Elemente
 * @param current {int} Index des Elements, das ausgewählt werden soll (standard: 0)
 * @param editable {bool} Wenn true, kann der Text im Dialogfeld bearbeitet werden (standard: false).
 * @return {QString} Text des ausgewählten Elements
 */
QString ScriptingService::inputDialogGetItem(
        const QString &title, const QString &label, const QStringList &items,
        int current, bool editable);
```

An empty string will be returned, if `Cancel` was clicked or `Escape` was pressed.

### Beispiel

```js
var result = script.inputDialogGetItem("combo box", "Please select an item", [
  "Item 1",
  "Item 2",
  "Item 3",
]);
script.log(result);
```

Schauen Sie sich dazu das Beispiel auf [input-dialogs.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/input-dialogs.qml) an.

## Öffnen eines Eingabedialogs mit einer Zeilenbearbeitung

### Methodenaufruf und Parameter

```cpp
/**
 * Öffnet einen Eingabedialog mit einer Zeilenbearbeitung
 *
 * @param title {QString} Titel des Dialogfelds
 * @param label {QString} Beschriftungstext des Dialogfelds
 * @param text {QString} Text im Dialog (optional)
 * @return
 * /
QString ScriptingService::inputDialogGetText(
        const QString &title, const QString &label, const QString &text);
```

An empty string will be returned, if `Cancel` was clicked or `Escape` was pressed.

### Beispiel

```js
var result = script.inputDialogGetText(
  "line edit",
  "Please enter a name",
  "current text",
);
script.log(result);
```

## Opening an input dialog with a multi-line text edit

### Methodenaufruf und Parameter

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

An empty string will be returned, if `Cancel` was clicked or `Escape` was pressed.

### Beispiel

```js
var result = script.inputDialogGetMultiLineText(
  "multi-line edit",
  "Please enter a text",
  "current text",
);
script.log(result);
```

## Opening a dialog to show the differences between two texts

### Methodenaufruf und Parameter

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

### Beispiel

```js
const text = script.noteTextEditSelectedText();
const aiPrompt = "Translate the text to English";
const aiResult = script.aiComplete(aiPrompt + ":\n\n" + text);

var result = script.textDiffDialog(
  "AI Text Tool",
  "Resulting text",
  text,
  aiResult,
);
script.log(result);
```

## Überprüfen, ob eine Datei vorhanden ist

### Method call and parameters

```cpp
/**
 * Überprüft, ob eine Datei vorhanden ist
 * @param filePath
 * @return
 */
bool ScriptingService::fileExists(QString &filePath);
```

### Example

```js
var result = script.fileExists(filePath);
script.log(result);
```

## Text aus einer Datei lesen

### Method call and parameters

```cpp
/**
 * Liest Text aus einer Datei
 *
 * @param filePath {QString} Pfad der zu ladenden Datei
 * @param codec {QString} Dateikodierung (Standard: UTF-8)
 * @return die Dateidaten oder null, wenn die Datei nicht existiert
 */
QString ScriptingService::readFromFile(const QString &filePath, const QString &codec)
```

### Example

```js
if (script.fileExists(filePath)) {
  var data = script.readFromFile(filePath);
  script.log(data);
}
```

## Text in eine Datei schreiben

### Method call and parameters

```cpp
/**
 * Schreibt einen Text in eine Datei
 *
 * @param filePath {QString}
 * @param data {QString}
 * @param createParentDirs {bool} optional (Standard: false)
 * @return
 */
bool ScriptingService::writeToFile(const QString &filePath, const QString &data, bool createParentDirs);
```

### Example

```js
var result = script.writeToFile(filePath, html);
script.log(return);
```

Schauen Sie sich dazu das Beispiel auf [export-notes-as-one-html.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/export-notes-as-one-html.qml) an.

## Arbeiten mit Websockets

Sie können QOwnNotes mithilfe von `WebSocketServer` fernsteuern.

Schauen Sie sich dazu das Beispiel auf [websocket-server.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/websocket-server.qml) an. Du kannst den Socket Server testen, indem du dich mit [Websocket test](https://www.websocket.org/echo.html?location=ws://127.0.0.1:35345) mit ihm verbindest.

Sie können Sockets auch mit `WebSocket` abhören. Bitte schauen Sie sich das Beispiel auf [websocket-client.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/websocket-client.qml) an.

Beachten Sie, dass Sie die QML `Websocket`-Bibliothek von Qt installiert haben müssen, um dies zu verwenden. Unter Ubuntu Linux können Sie beispielsweise `qml-module-qtwebsockets` installieren.

## Hinzufügen einer Hervorhebungsregel für den Editor

Sie können Hervorhebungsregeln direkt in den Editor einfügen, indem Sie reguläre Ausdrücke definieren und ihnen einen Hervorhebungszustand zuweisen.

### Method call and parameters

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
                                            int state,
                                            int capturingGroup,
                                            int maskedGroup);
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

### Example

```js
// Hervorheben einer Textzeile wie "BLOCK: irgendein Text" als Blockzitat (Zustand 18)
script.addHighlightingRule("^BLOCK: (.+)", "BLOCK:", 18);

// Maskiere (Zustand 24) alle Zeichen nach 32 Zeichen in einer Zeile
// capturingGroup 1 bedeutet, dass der Ausdruck aus dem ersten eingeklammerten Teil des Musters hervorgehoben wird
// maskedGroup -1 bedeutet, dass keine Maskierung erfolgen soll
script.addHighlightingRule("^.{32}(.+)", "", 24, 1, -1);
```

Schauen Sie sich dazu auch die Beispiele auf [highlighting.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/highlighting.qml) an.
