# Hooks

## onNoteStored

### Methodenaufruf und Parameter

```js
/**
 * Diese Funktion wird aufgerufen, wenn eine Notiz auf der Festplatte gespeichert wird
 * Sie können gespeicherte Notizen nicht modifizieren, da es sonst Chaos gäbe, weil
 * Sie sie wahrscheinlich gleichzeitig per Hand bearbeiten
 *
 * @param {NoteApi} note - das Notizobjekt der gespeicherten Notiz
 */
function onNoteStored (note);
```

Vielleicht schauen Sie sich mal das Beispiel auf [on-note-opened.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/on-note-opened.qml) an.

## noteOpenedHook

### Methodenaufruf und Parameter

```js
/**
 * Diese Funktion wird aufgerufen, nachdem eine Notiz geöffnet wurde
 *
 * @param {NoteApi} note - das geöffnete Notizobjekt
 */
function noteOpenedHook(note);
```

Schauen Sie sich dazu das Beispiel auf [on-note-opened.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/on-note-opened.qml) an.

## noteDoubleClickedHook

### Methodenaufruf und Parameter

```js
/**
 * Diese Funktion wird aufgerufen, nachdem auf eine Notiz doppelklickt wurde
 *
 * @param {NoteApi} note - das Notizobjekt, auf das geklickt wurde
 */
function noteDoubleClickedHook(note);
```

Schauen Sie sich dazu das Beispiel auf [external-note-open.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/external-note-open.qml) an.

## insertMediaHook

Diese Funktion wird aufgerufen, wenn eine Mediendatei in die aktuelle Notiz eingefügt wird.

Wenn diese Funktion in mehreren Skripten definiert ist, gewinnt das erste Skript, das eine nicht leere Zeichenfolge zurückgibt.

### Methodenaufruf und Parameter

```js
/**
 * @param fileName string Der Dateipfad der Quellmediendatei, bevor sie in den Medienordner kopiert wurde
 * @param markdownText string Der Markdown-Text der Mediendatei, z.B. ![my-image](media/my-image-4101461585.jpg)
 * @return string Der neue Markdown-Text der Mediendatei
 */
function insertMediaHook(fileName, markdownText);
```

Schauen Sie sich das Beispiel auf [example.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/example.qml) an.

## insertAttachmentHook

Diese Funktion wird aufgerufen, wenn eine Anhangsdatei in die aktuelle Notiz eingefügt wird.

Wenn diese Funktion in mehreren Skripten definiert ist, gewinnt das erste Skript, das eine nicht leere Zeichenfolge zurückgibt.

### Methodenaufruf und Parameter

```js
/**
 * @param fileName string Der Dateipfad der Quellanhangsdatei, bevor sie in den Anhangsordner kopiert wurde
 * @param markdownText string Der Markdown-Text der Anhangsdatei, z.B. [my-file.txt](attachments/my-file-4245650967.txt)
 * @return string der neue Markdown-Text der Anhangsdatei
 */
function insertAttachmentHook(fileName, markdownText);
```

Schauen Sie sich dazu ein Beispiel auf [example.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/example.qml) an.

## insertingFromMimeDataHook

### Methodenaufruf und Parameter

```js
/**
 * Diese Funktion wird aufgerufen, wenn HTML oder eine Mediendatei mit "Strg + Umschalt + V" in eine Notiz eingefügt wird
 *
 * @param text Text des QMimeData-Objekts
 * @param html HTML des QMimeData-Objekts
 * @gibt die Zeichenfolge zurück, die anstelle des Texts aus dem QMimeData-Objekt eingefügt werden soll
 */
function insertingFromMimeDataHook(text, html);
```

Schauen Sie sich dazu das Beispiel auf [example.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/example.qml), [insert-headline-with-link-from-github-url.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/insert-headline-with-link-from-github-url.qml) oder [note-text-from-5pm-mail.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/note-text-from-5pm-mail.qml) an.

## handleNoteTextFileNameHook

### Methodenaufruf und Parameter

```js
/**
 * Diese Funktion wird aufgerufen, wenn eine Notiz auf der Festplatte gespeichert wird, wenn
 * "Zulassen, dass der Dateiname der Notiz sich von der Überschrift unterscheidet" in den Einstellungen
 * aktiviert ist
 *
 * Hier können Sie den Namen der Notizdatei ändern
 * Denken Sie daran, dass Sie sich selbst um doppelte Namen kümmern müssen!
 *
 * Geben Sie eine leere Zeichenfolge zurück, wenn der Dateiname der Notiz
 * nicht geändert werden soll
 *
 * @param {NoteApi} note - das Notizobjekt der gespeicherten Notiz
 * @return {string} der Dateiname der Notiz
 */
function handleNoteTextFileNameHook(note);
```

Schauen Sie sich die Beispiele auf [example.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/example.qml) oder [use-tag-names-in-filename.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/use-tag-names-in-filename.qml) an.

## handleNoteNameHook

### Methodenaufruf und Parameter

```js
/**
 * Diese Funktion wird aufgerufen, wenn der Notizname für eine Notiz bestimmt wird
 *
 * Hier können Sie den Namen der angezeigten Notiz ändern
 *
 * Geben Sie eine leere Zeichenfolge zurück, wenn der Name der Notiz nicht geändert werden soll
 *
 * @param {NoteApi} note - das Notizobjekt der gespeicherten Notiz
 * @return {string} der Name der Notiz
 */
function handleNoteNameHook(note);
```

The note name is mainly used when showing the note in the lists.

Schauen Sie sich dazu das Beispiel auf [example.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/example.qml) an.

Es ist möglicherweise keine gute Idee, diesen Hook zu verwenden, wenn die Einstellung zur Verwendung des Dateinamens als Notizname aktiv ist.

## handleNewNoteHeadlineHook

### Methodenaufruf und Parameter

```js
/**
 * Diese Funktion wird aufgerufen, bevor eine Notiz erstellt wird
 *
 * Sie können damit die Überschrift der Notiz ändern, bevor sie erstellt wird
 * Beachten Sie, dass Sie auf einen eindeutigen Notennamen achten müssen, da
 * sondst die neue Notiz nicht erstellt wird, sonder nur eine vorhandene in der Notizliste gefunden wird
 *
 * Sie können diese Funktion zum Erstellen von Notizvorlagen verwenden
 *
 * @param headline Text, der zum Erstellen der Überschrift verwendet wird
 * @return {string} Die Überschrift der Notiz
 */
function handleNewNoteHeadlineHook(headline);
```

Schauen Sie sich dazu das Beispiel auf [custom-new-note-headline.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-new-note-headline.qml) an.

## preNoteToMarkdownHtmlHook

### Methodenaufruf und Parameter

```js
/**
 * Diese Funktion wird aufgerufen, bevor das Markdown-HTML einer Notiz generiert wird
 *
 * Sie können ändern, was an den Markdown-zu-HTML-Konverter übergeben wird
 *
 * Die Funktion kann zum Beispiel in mehreren Skripten verwendet werden, um Code (wie LaTeX math oder mermaid)
 * in seine grafische Darstellung für die Vorschau zu rendern
 *
 * Die Notiz wird dabei nicht verändert
 *
 * @param {NoteApi} note - das Notizobjekt
 * @param {string} markdown - der Markdown, der gerade in HTML konvertiert wird
 * @param {bool} forExport - true, wenn das HTML für einen Export verwendet wird, false für die Vorschau
 * @return {string} der geänderte Markdown oder ein leerer String, wenn nichts geändert werden soll
 */
function preNoteToMarkdownHtmlHook(note, markdown, forExport);
```

Schauen Sie sich dazu das Beispiel auf [preview-styling.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/preview-styling.qml) an.

## noteToMarkdownHtmlHook

### Methodenaufruf und Parameter

```js
/**
 * Diese Funktion wird aufgerufen, wenn das Markdown-HTML einer Notiz generiert wird
 *
 * Damit können Sie den HTML-Code ändern
 * Dies wird beispielsweise zuvor von der Notenvorschau aufgerufen
 *
 * Die Funktion kann in mehreren Skripten verwendet werden, um das HTML der Vorschau zu ändern
 *
 * @param {NoteApi} note - das Notizobjekt
 * @param {string} html - das HTML, das gerendert werden soll
 * @param {bool} forExport - true, wenn das HTML für einen Export verwendet wird, false für die Vorschau
 * @return {string} der geänderte HTML-Code oder ein leerer String, wenn nichts geändert werden soll
 */
function noteToMarkdownHtmlHook(note, html, forExport);
```

Schauen Sie sich dazu Beispiele auf [example.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/example.qml) oder [preview-styling.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/preview-styling.qml) an.

Eine Liste aller unterstützten CSS-Stile finden Sie in der Dokumentation [Supported HTML Subset](http://doc.qt.io/qt-5/richtext-html-subset.html).

## encryptionHook

### Methodenaufruf und Parameter

```js
/**
 * Diese Funktion wird aufgerufen, wenn Text ver- oder entschlüsselt werden soll
 *
 * @param text string Der Text zum ent-/verschlüsseln
 * @param password string Das Passwort
 * @param decrypt bool false, wenn verschlüsselt werden soll, true wenn Entschlüsseln verlangt wird
 * @return der ver-/entschlüsselte Text
 */
function encryptionHook(text, password, decrypt);
```

Schauen Sie sich dazu die Beispiele auf [encryption-keybase.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/encryption-keybase.qml), [encryption-pgp.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/encryption-pgp.qml) oder [encryption-rot13.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/encryption-rot13.qml) an.

## noteTaggingHook

Sie können Ihren eigenen Notiz-Schlagwortmechanismus beispielsweise mit speziellem Text in Ihrer Notiz implementieren, z.B. `@tag1`, `@tag2`, `@tag3`.

### Methodenaufruf und Parameter

```js
/ **
 * Kümmert sich um Notiz-Schlagworte für eine Notiz
 *
 * Diese Funktion wird aufgerufen, wenn Schlagworte zu einer Notiz hinzugefügt, entfernt
 * oder umbenannt werden, oder die Schlagworte einer Notiz aufgelistet werden sollen
 *
 * @param note
 * @param action kann "add", "remove", "rename" oder "list" sein.
 * @param tagName Schlagwortname, der hinzugefügt, entfernt oder umbenannt werden soll
 * @param newTagName Schlagwort-Name, in den umbenannt werden soll, wenn action = "rename"
 * @return note Text-String oder String-Liste von Schlagwortnamen (wenn action = "list")
 */
function noteTaggingHook(note, action, tagName, newTagName);
```

- as soon as a script is activated that implements the new function `noteTaggingHook` note tagging will be handled by that function
- die folgenden Funktionen sollten über die QOwnNotes-Benutzeroberfläche funktionieren
  - initially importing tags like `@tag` from your notes and overwriting your current tag assignment
    - you will not lose your tags tree, just the former assignment to notes
    - Sie können Schlagwörter weiterhin in andere Schlagwörter verschieben
    - if more than one tag has the same name in your tag tree the first hit will be assigned
  - durch Hinzufügen eines Schlagwortes zu einer Notiz wird das Schlagwort zum Notiztext hinzugefügt
  - durch Entfernen eines Schlagwortes aus einer Notiz wird das Schlagwort aus dem Notiztext entfernt
  - removing of tags in the tag list will remove those tags from your notes
  - renaming of tags in the tag list will rename those tags in your notes
  - bulk tagging of notes in the note list will add those tags to your notes
  - bulk removing of tags from notes in the note list will remove those tags from your notes
  - the application will trigger a series of `add` and `remove` actions for all selected tags and their children on all notes if tags are moved in the tag panel

Schauen Sie sich dazu das Beispiel auf [note-tagging.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/note-tagging.qml) an, um eigene Schlagwortmechanismen zu implementieren.

::: warning
Make sure your `list` action is really fast, because it will be executed for every note every time the note folder or the tag tree is reloaded!

That is every time you find `reloadTagTree` in [mainwindow.cpp](https://github.com/pbek/QOwnNotes/blob/main/src/mainwindow.cpp).
:::

## noteTaggingByObjectHook

Ähnlich wie bei [noteTaggingHook](#notetagginghook) können Sie Ihren eigenen Mechanismus zum Markieren von Notizen implementieren, sind jedoch nicht an Schlagwortnamen in der Schlagwortbaumwurzel gebunden. Auf diese Weise können Sie den gesamten Schlagwortbaum anstelle von nur einer Schlagwortliste verwenden.

Mit `noteTaggingByObjectHook` erhalten Sie anstelle eines Schlagwortnamens ein`TagApi `-Objekt als Parameter. Als Ergebnis für die Aktion `list` müssen Sie eine Liste der Schlagwort-IDs bereitstellen.

Dies bedeutet auch, dass Sie fehlende Schlagworte selbst erstellen müssen, um eine Liste bereits vorhandener Schlagwort-IDs für die Aktion `list` bereitstellen zu können.

### Methodenaufruf und Parameter

```js
/**
 * Kümmert sich um das das Markieren von Notizen für eine Notiz
 *
 * Diese Funktion wird aufgerufen, wenn Schlagworte in einer Notiz hinzugefügt, entfernt
 * oder umbenannt werden oder die Schlagworte einer Notiz aufgelistet werden sollen
 *
 * @param note
 * @param action kann "add", "remove", "rename" oder "list" sein.
 * @param Schlagwort, das hinzugefügt, entfernt oder umbenannt werden soll
 * @param newTagName Schlagwort-Name, in den umbenannt werden soll, wenn action = "rename"
 * @return note Text-string oder string-Liste von Schlagwort-IDs (wenn action = "list")
 */
function noteTaggingByObjectHook(note, action, tag, newTagName);
```

Schauen Sie sich dazu das Beispiel auf [note-tagging-by-object.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/note-tagging-by-object.qml) an, wie Sie Ihren eigenen Schlagwort-Mechanismus implementieren können.

## autocompletionHook

Sie können eine Liste von Zeichenfolgen zurückgeben, die der Autovervollständigungsliste hinzugefügt werden sollen, wenn Autovervollständigung aufgerufen wird (z.B. durch Drücken von <kbd>Strg + Leertaste</kbd>).

### Methodenaufruf und Parameter

```js
/**
 * Ruft die AutocompletionHook-Funktion für alle Skriptkomponenten auf
 * Diese Funktion wird aufgerufen, wenn die automatische Vervollständigung in einer Notiz aufgerufen wird
 *
 * @return QStringListe des Textes für die Autovervollständigungsliste
 */
function callAutocompletionHook ();
```

Schauen Sie sich dazu das Beispiel auf [autocompletion.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/autocompletion.qml) an.

## websocketRawDataHook

Dieser Hook wird aufgerufen, wenn Daten von der QOwnNotes Web Companion-Browsererweiterung über das Kontextmenü des Webbrowsers gesendet werden.

### Methodenaufruf und Parameter

```js
/**
 * @param requestType kann "page" oder "selection" sein
 * @param pageUrl die URL der Webseite, wo die Anfrage gestellt wurde
 * @param pageTitle der Titel der Webseite, wo die Anfrage gestellt wurde
 * @param rawData die übertragenen Daten, HTML für requestType "page" oder Klartext für requestType "selection"
 * @param screenshotDataUrl die Daten-URL des Screenshots der Webseite, wo die Anfrage gestellt wurde
 * @return true wenn Daten durch einen Hook gelaufen sind
 */
function callHandleWebsocketRawDataHook(requestType, pageUrl, pageTitle, rawData, screenshotDataUrl);
```

Schauen Sie sich dazu die Beispiele auf [websocket-raw-data-new-note.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/websocket-raw-data-new-note.qml) und [websocket-raw-data-selection-in-note.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/websocket-raw-data-selection-in-note.qml) an.

## onDetachedProcessCallback

Dieser Hook wird aufgerufen, wenn ein Skript-Thread von [startDetachedProcess](methods-and-objects.html#starting-an-external-program-in-the-background) ausgeführt wird.

### Methodenaufruf und Parameter

```js
/**
 * Diese Funktion wird aufgerufen, wenn ein Skript-Thread fertig ausgeführt wurde.
 * Hinweis: thread[1]==0 hilft festzustellen, ob ein Großteil der gestarteten Prozesse für einen bestimmten Identifier fertig ausgeführt wurde.
 *
 * @param {QString} callbackIdentifier - die beim Aufruf von startDetachedProcess() angegebene ID
 * @param {QString} resultSet - das Ergebnis des Prozesses
 * @param {QVariantList} cmd - das gesamte Befehlsarray [0-ausführbarer Pfad, 1-Parameter, 2-exitCode]
 * @param {QVariantList} thread - das Thread-Informationsarray [0-übergebener CallbackParameter, 1-verbleibende Threads für diesen Bezeichner]
 */
function onDetachedProcessCallback(callbackIdentifier, resultSet, cmd, thread);
```

Schauen Sie sich dazu das Beispiel auf [callback-example.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/callback.qml) an.

## windowStateChangedHook

### Methodenaufruf und Parameter

```js
/**
 * Diese Funktion wird aufgerufen, nachdem ein WindowStateChange-Ereignis ausgelöst wurde
 *
 * @param {QString} windowState - Der neue Fensterstatus; Parameterwert kann "minimized", "maximized", "fullscreen", "active" oder "nostate" sein.
 */
function windowStateChangedHook(windowState);
```

Schauen Sie sich dazu das Beispiel auf [window-state-changed.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/window-state-changed.qml) an.

## workspaceSwitchedHook

Dieser Hook wird aufgerufen, wenn Arbeitsbereiche gewechselt werden.

### Methodenaufruf und Parameter

```js
/**
 * Diese Funktion wird aufgerufen, wenn Arbeitsbereiche gewechselt werden
 *
 * @param oldUuid Alte Arbeitsbereich-UUID
 * @param newUuid Neue Arbeitsbereich-UUID
 */
function workspaceSwitchedHook(oldUuid, newUuid);
```

Schauen Sie sich dazu das Beispiel auf [websocket-raw-data-new-note.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/workspaces.qml) an.

## openAiBackendsHook

This hook is called, when the OpenAI service config is reloaded. For example, this is also done when the script engine is reloaded.

You can use it to provide config for custom OpenAI backends, like your own OpenAI API compatible LLMs, for example ChatGPT, Claude or systems like Ollama.

### Method call and parameters

```js
/**
 * This function is called when the OpenAI service config is reloaded
 * It returns a list of objects with config parameters for new OpenAI backends
 */
function openAiBackendsHook() {
  return [
    {
      id: "my-custom-ai",
      name: "My Custom AI",
      baseUrl: "http://localhost:5000",
      apiKey: "kDFJkjk3asdm",
      models: ["gpt-3.5-turbo", "gpt-4.0-turbo"],
    },
    {
      id: "my-custom-ai2",
      name: "My Custom AI 2",
      baseUrl: "http://localhost:5001",
      apiKey: "lOikf7eNdb9",
      models: ["gpt-3.5-turbo2", "gpt-4.0-turbo2"],
    },
  ];
}
```

::: tip
You can use [Registering script settings variables](./methods-and-objects.md#registering-script-settings-variables) to store the OpenAI backend settings in the script settings.
:::

You may want to take a look at the example [custom-openai-backends.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-openai-backends.qml).
