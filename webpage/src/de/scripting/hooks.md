# Hooks

onNoteStored
------------

### Methodenaufruf und Parameter
```js
/**
  * Diese Funktion wird aufgerufen, wenn eine Notiz auf der Festplatte gespeichert wird
  * Sie können gespeicherte Notizen nicht ändern, da dies seitdem ein Chaos wäre
  * Sie bearbeiten sie höchstwahrscheinlich gleichzeitig von Hand
  *
  * @param {NoteApi} note - das Notizobjekt der gespeicherten Notiz
  */
Funktion onNoteStored (Hinweis);
```

Vielleicht möchten Sie sich das Beispiel [on-note-opened.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/on-note-opened.qml) ansehen.

noteOpenedHook
--------------

### Methodenaufruf und Parameter
```js
/ **
  * Diese Funktion wird aufgerufen, nachdem eine Notiz geöffnet wurde
  *
  * @param {NoteApi} note - das geöffnete Notizobjekt
  */
function noteOpenedHook(note);
```

Vielleicht möchten Sie sich das Beispiel [on-note-opened.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/on-note-opened.qml) ansehen.

noteDoubleClickedHook
---------------------

### Methodenaufruf und Parameter
```js
/**
  * Diese Funktion wird aufgerufen, nachdem auf eine Notiz doppelklickt wurde
  *
  * @param {NoteApi} note - das Notizobjekt, auf das geklickt wurde
  */
function noteDoubleClickedHook(note);
```

Vielleicht möchten Sie sich das Beispiel [external-note-open.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/external-note-open.qml) ansehen.

insertMediaHook
---------------

Diese Funktion wird aufgerufen, wenn eine Mediendatei in die aktuelle Notiz eingefügt wird.

Wenn diese Funktion in mehreren Skripten definiert ist, gewinnt das erste Skript, das eine nicht leere Zeichenfolge zurückgibt.

### Methodenaufruf und Parameter
```js
/**
  * @param fileName string Der Dateipfad der Quellmediendatei, bevor sie in den Medienordner kopiert wurde
  * @param markdownText string Der Markdown-Text der Mediendatei, z.B. ![my-image](media/my-image-4101461585.jpg)
  * @return string der neue Markdown-Text der Mediendatei
  */
function insertMediaHook(fileName, markdownText);
```

Vielleicht möchten Sie sich das Beispiel [example.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/example.qml) ansehen.

insertAttachmentHook
--------------------

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

Vielleicht möchten Sie sich das Beispiel [example.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/example.qml) ansehen.

insertingFromMimeDataHook
-------------------------

### Methodenaufruf und Parameter
```js
/**
  * Diese Funktion wird aufgerufen, wenn HTML oder eine Mediendatei mit "Strg + Umschalt + V" in eine Notiz eingefügt wird
  *
  * @param text Text des QMimeData-Objekts
  * @param html Html des QMimeData-Objekts
  * @ gibt die Zeichenfolge zurück, die anstelle des Texts aus dem QMimeData-Objekt eingefügt werden soll
  */
function insertingFromMimeDataHook(text, html);
```

Vielleicht möchten Sie sich ein Beispiel ansehen: [example.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/example.qml), [insert-headline-with-link-from-github-url.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/insert-headline-with-link-from-github-url.qml) oder [note-text-from-5pm-mail.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/note-text-from-5pm-mail.qml).

handleNoteTextFileNameHook
--------------------------

### Methodenaufruf und Parameter
```js
/**
  * Diese Funktion wird aufgerufen, wenn eine Notiz auf der Festplatte gespeichert wird, wenn
  * "Der Name der Notizdatei darf sich von der Überschrift unterscheiden" ist aktiviert
  * in den Einstellungen
  *
  * Hier können Sie den Namen der Notizdatei ändern
  * Denken Sie daran, dass Sie sich selbst um doppelte Namen kümmern müssen!
 *
  * Geben Sie eine leere Zeichenfolge zurück, wenn der Dateiname der Notiz angezeigt werden soll
  * nicht geändert werden
  *
  * @param {NoteApi} note - das Notizobjekt der gespeicherten Notiz
  * @return {string} der Dateiname der Notiz
  */
function handleNoteTextFileNameHook(note);
```

Vielleicht möchten Sie sich das Beispiel ansehen

 example.qml </ 0> oder [ use-tag-names-in-filename.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/use-tag-names-in-filename.qml).</p> 





handleNoteNameHook
------------------



### Methodenaufruf und Parameter


```js
/**
  * Diese Funktion wird aufgerufen, wenn der Notenname für eine Note bestimmt wird
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


Vielleicht möchten Sie sich das Beispiel [example.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/example.qml) ansehen.

Es ist möglicherweise keine gute Idee, diesen Hook zu verwenden, wenn die Einstellung zur Verwendung des Dateinamens als Notizenname aktiv ist.





handleNewNoteHeadlineHook
-------------------------



### Methodenaufruf und Parameter


```js
/**
  * Diese Funktion wird aufgerufen, bevor eine Notiz erstellt wird
  *
  * Sie können die Überschrift der Notiz ändern, bevor sie erstellt wird
  * Beachten Sie, dass Sie ansonsten auf einen eindeutigen Notennamen achten müssen
  * Die neue Notiz wird nicht erstellt, sondern nur in der Notizliste gefunden
  *
  * Sie können diese Funktion zum Erstellen von Notizvorlagen verwenden
  *
  * @param Überschriftentext, der zum Erstellen der Überschrift verwendet wird
 */
function handleNewNoteHeadlineHook(headline);
```


Vielleicht möchten Sie sich das Beispiel ansehen [custom-new-note-headline.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-new-note-headline.qml).





preNoteToMarkdownHtmlHook
-------------------------



### Methodenaufruf und Parameter


```js
/**
  * Diese Funktion wird aufgerufen, bevor das Markdown-HTML einer Notiz generiert wird
  *
  * Sie können ändern, was an den Markdown-zu-HTML-Konverter übergeben wird
  *
  * Die Funktion kann zum Beispiel in mehreren Skripten verwendet werden, um Code zu rendern (wie LaTeX math oder meermaid)
  * zu seiner grafischen Darstellung für die Vorschau
  *
  * Die Notiz wird dabei nicht verändert
  *
  * @param {NoteApi} note - das Notizobjekt
  * @param {string} Markdown - der Markdown, der gerade in HTML konvertiert wird
  * @param {bool} forExport - true, wenn das HTML für einen Export verwendet wird, false für die Vorschau
  * @return {string} der geänderte Markdown oder ein leerer String, wenn nichts geändert werden soll
  */
Funktion preNoteToMarkdownHtmlHook(note, markdown, forExport);
```


Vielleicht möchten Sie sich das Beispiel ansehen: [preview-styling.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/preview-styling.qml).





noteToMarkdownHtmlHook
----------------------



### Methodenaufruf und Parameter


```js
/**
  * Diese Funktion wird aufgerufen, wenn das Markdown-HTML einer Notiz generiert wird
  *
  * Sie können diesen HTML-Code ändern
  * Dies wird beispielsweise zuvor von der Notenvorschau aufgerufen
  *
  * Die Funktion kann in mehreren Skripten verwendet werden, um das HTML der Vorschau zu ändern
  *
  * @param {NoteApi} note - das Notizobjekt
  * @param {string} html - das HTML, das gerendert werden soll
  * @param {bool} forExport - true, wenn das HTML für einen Export verwendet wird, false für die Vorschau
  * @return {string} der geänderte HTML-Code oder ein leerer String, wenn nichts geändert werden soll
  */
Funktion noteToMarkdownHtmlHook(note, html, forExport);
```


Vielleicht möchten Sie sich das Beispiel ansehen [example.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/example.qml) oder [preview-styling.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/preview-styling.qml).

Eine Liste aller unterstützten CSS-Stile finden Sie in der Dokumentation [Unterstützte HTML-Teilmenge](http://doc.qt.io/qt-5/richtext-html-subset.html).





encryptionHook
--------------



### Methodenaufruf und Parameter


```js
/**
  * Diese Funktion wird aufgerufen, wenn das Markdown-HTML einer Notiz generiert wird
  *
  * Sie können dieses HTML ändern
  * Dies wird zum Beispiel zuvor von der Notenvorschau aufgerufen
  *
  * Die Methode kann in mehreren Skripten verwendet werden, um das HTML der Vorschau zu ändern
  *
  * @param {NoteApi} note - das Notizobjekt
  * @param {string} html - das HTML, das gerendert werden soll
  * @param {string} forExport - true, wenn der HTML-Code für einen Export verwendet wird, false für die Vorschau
  * @return {string} das geänderte HTML oder eine leere Zeichenfolge, wenn nichts geändert werden soll
  */
function encryptionHook(text, password, decrypt);
```


Vielleicht möchten Sie sich ein Beispiel ansehen: [encryption-keybase.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/encryption-keybase.qml), [encryption-pgp.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/encryption-pgp.qml) oder [encryption-rot13.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/encryption-rot13.qml).





noteTaggingHook
---------------

Sie können Ihren eigenen Notizkennzeichnungsmechanismus beispielsweise mit speziellem Text in Ihrer Notiz implementieren, z. B. `@ tag1`, `@tag2`, `@tag3`.



### Methodenaufruf und Parameter


```js
/ **
  * Behandelt das Markieren von Notizen für eine Notiz
  *
  * Diese Funktion wird aufgerufen, wenn Tags hinzugefügt, entfernt oder umbenannt werden
  * Eine Notiz oder die Tags einer Notiz sollten aufgelistet sein
  *
  * @param note
  * @param Aktion kann "Hinzufügen", "Entfernen", "Umbenennen" oder "Liste" sein.
  * @param tagName Tagname, der hinzugefügt, entfernt oder umbenannt werden soll
  * @param newTagName Tag-Name, in den umbenannt werden soll, wenn action = "umbenennen"
  * @return note text string oder string-list of tag names (wenn action = "list")
  */
function noteTaggingHook (note, action, tagName, newTagName);
```


-   Sobald ein Skript aktiviert ist, das die neue Funktion `noteTaggingHook` implementiert, wird das Note-Tagging von dieser Funktion verarbeitet
-   die folgenden Funktionen sollten über die QOwnNotes-Benutzeroberfläche funktionieren
  
      -   importieren Sie zunächst Tags wie `@tag` aus Ihren Notizen und überschreiben Sie Ihre aktuelle Tag-Zuweisung
      
              -   Sie werden Ihren Schlagworte-Baum nicht verlieren, sondern nur die frühere Zuordnung zu Notizen

        -   Sie können Tags weiterhin in andere Tags verschieben

        -   wenn mehr als ein Tag denselben Namen in Ihrem Tag-Baum hat, wird der erste Treffer zugewiesen
    -   durch Hinzufügen eines Tags zu einer Notiz wird das Tag zum Notiztext hinzugefügt

    -   durch Entfernen eines Tags aus einer Notiz wird das Tag aus dem Notiztext entfernt
    -   durch das Entfernen von Tags in der Tag-Liste werden diese Tags aus Ihren Notizen entfernt

    -   durch das Umbenennen von Tags in der Tag-Liste werden diese Tags in Ihren Notizen umbenannt

    -   durch Massen-Tagging von Notizen in der Notizenliste werden diese Tags zu Ihren Notizen hinzugefügt

    -   durch das Entfernen von Tags aus Notizen in der Notizenliste werden diese Tags aus Ihren Notizen entfernt

    -   die Anwendung, die eine Reihe von Aktionen zum `add` und `remove` aus für alle Rechte Tags und ihre unter Elemente Elemente in allen Notizen, wenn Tags werden im Tag-Panel

Vielleicht möchten Sie sich das Beispiel ansehen: [note-tagging.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/note-tagging.qml), um Ihren eigenen Tagging-Mechanismus zu implementieren.

::: warning
Stellen Sie sicher, dass Ihre Aktion `list` sehr schnell ist, da sie bei jedem erneuten Laden des Notizordners für jede Notiz ausgeführt wird!
:::





noteTaggingByObjectHook
----------------------

Ähnlich wie bei [noteTaggingHook](#notetagginghook) können Sie Ihren eigenen Mechanismus zum Markieren von Notizen implementieren, sind jedoch nicht an Tag-Namen im Tag-Stammbaum gebunden. Auf diese Weise können Sie den gesamten Tag-Baum anstelle von nur einer Tag-Liste verwenden.

Mit `noteTaggingByObjectHook ` erhalten Sie anstelle eines Tag-Namens ein` TagApi ` -Objekt als Parameter. Als Ergebnis für die Aktion `Liste` müssen Sie eine Liste der Tag-IDs bereitstellen.

Dies bedeutet auch, dass Sie fehlende Tags selbst erstellen müssen, um eine Liste bereits vorhandener Tag-IDs für die Aktion `Liste` bereitstellen zu können.  



### Methodenaufruf und Parameter


```js
/**
  * Behandelt das Markieren von Notizen für eine Notiz
  *
  * Diese Funktion wird aufgerufen, wenn Tags hinzugefügt, entfernt oder umbenannt werden
  * Eine Notiz oder die Tags einer Notiz sollten aufgelistet sein
  *
  * @param note
  * @param Aktion kann "Hinzufügen", "Entfernen", "Umbenennen" oder "Liste" sein.
  * @ param-Tag, das hinzugefügt, entfernt oder umbenannt werden soll
  * @param newTagName Tag-Name, in den umbenannt werden soll, wenn action = "umbenennen"
  * @return note text string oder string-list of tag ids (wenn action = "list")
  */
function noteTaggingByObjectHook(note, action, tag, newTagName);
```


Sie können sich das Beispiel [note-tagging-by-object.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/note-tagging-by-object.qml) ansehen, um Ihren eigenen Tagging-Mechanismus zu implementieren.





autocompletionHook
------------------

Sie können eine Liste von Zeichenfolgen zurückgeben, die der Autovervollständigungsliste hinzugefügt werden sollen, wenn diese automatisch vervollständigt wird (z.B. durch Drücken von <kbd>Ctrl + Space</kbd>).



### Methodenaufruf und Parameter


```js
/**
  * Ruft die AutocompletionHook-Funktion für alle Skriptkomponenten auf
  * Diese Funktion wird aufgerufen, wenn die automatische Vervollständigung in einer Notiz aufgerufen wird
  *
  * @return QStringListe des Textes für die Autovervollständigungsliste
  */
Funktion callAutocompletionHook ();
```


Vielleicht möchten Sie sich das Beispiel ansehen [autocompletion.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/autocompletion.qml).





websocketRawDataHook
--------------------

Dieser Hook wird aufgerufen, wenn Daten von der QOwnNotes Web Companion-Browsererweiterung über das Kontextmenü des Webbrowsers gesendet werden.



### Methodenaufruf und Parameter


```js
/**
 * @param requestType kann "Seite" oder "Auswahl" sein
 * @param pageUrl die Url der Webseite, auf der die Anfrage gestellt wurde
 * @param pageTitle der Seitentitel der Webseite, auf der die Anfrage gestellt wurde
 * @param rawData die übertragenen Daten, html für requestType „page“ oder Klartext für requestType „selection“
 * @param screenshotDataUrl die Daten-Url des Screenshots, wenn die Webseite, auf der die Anfrage gestellt wurde
 * @return true, wenn Daten von einem Hook verarbeitet wurden
 */
function callHandleWebsocketRawDataHook(requestType, pageUrl, pageTitle, rawData, screenshotDataUrl);
```


Vielleicht möchten Sie sich die Beispiele ansehen [websocket-raw-data-new-note.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/websocket-raw-data-new-note.qml) und [Websocket-Rohdatenauswahl-in-Note.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/websocket-raw-data-selection-in-note.qml).





onDetachedProcessCallback
-------------------------

Dieser Hook wird aufgerufen, wenn ein Skript-Thread von [startDetachedProcess](methods-and-objects.html#starting-an-external-program-in-the-background) ausgeführt wird.



### Methodenaufruf und Parameter


```js
/**
 * Diese Funktion wird aufgerufen, wenn ein Skript-Thread ausgeführt wurde.
 * Hinweis: Thread [1]==0 hilft festzustellen, ob ein Großteil der gestarteten Prozesse für einen bestimmten Bezeichner ausgeführt wird.
 *
  * @param {QString} callbackIdentifier - die beim Aufruf von startDetachedProcess () angegebene ID
  * @param {QString} resultSet - das Ergebnis des Prozesses
  * @param {QVariantList} cmd - das gesamte Befehlsarray [0-ausführbarer Pfad, 1-Parameter, 2-exitCode]
  * @param {QVariantList} Thread - das Thread-Informationsarray [0-übergebener CallbackParameter, 1 verbleibende Threads für diesen Bezeichner]
  */
Funktion onDetachedProcessCallback (callbackIdentifier, resultSet, cmd, thread);
```


Vielleicht möchten Sie sich das Beispiel ansehen [ callback-example.qml ](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/callback.qml).





windowStateChangedHook
--------------



### Methodenaufruf und Parameter


```js
/**
  * Diese Funktion wird aufgerufen, nachdem ein WindowStateChange-Ereignis ausgelöst wurde
  *
  * @param {QString} windowState - Der neue Fensterstatus, Parameterwert kann "minimiert", "maximiert", "Vollbild", "aktiv" oder "nostate" sein.
  */
Funktion windowStateChangedHook (windowState);
```


Vielleicht möchten Sie sich das Beispiel ansehen [window-state-changed.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/window-state-changed.qml).





workspaceSwitchedHook
----------------------

Dieser Hook wird aufgerufen, wenn Workspaces gewechselt werden.



### Methodenaufruf und Parameter


```js
/**
 * Diese Funktion wird aufgerufen, wenn Workspaces umgeschaltet werden
 *
 * @param oldUuid old uuid of workspace
 * @param newUuid new uuid of workspace
 */
function workspaceSwitchedHook(oldUuid, newUuid);
```


Vielleicht möchten Sie sich das Beispiel ansehen [websocket-raw-data-new-note.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/workspaces.qml).
