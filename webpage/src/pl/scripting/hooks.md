# Haki

## onNoteStored

### Wywołanie metody i parametry

```js
/**
  * Ta funkcja jest wywoływana, gdy notatka zostanie zapisana na dysku
  * Nie możesz modyfikować zapisanych notatek, ponieważ byłby to bałagan, ponieważ
  * najprawdopodobniej edytujesz je ręcznie w tym samym czasie
 *
 * @param {NoteApi} note - the note object of the stored note
 */
function onNoteStored(note);
```

Może zechcesz rzucić okiem na przykład [on-note-opened.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/on-note-opened.qml).

## noteOpenedHook

### Wywołanie metody i parametry

```js
/**
 * Ta funkcja jest wywoływana po otwarciu notatki
 *
 * @param {NoteApi} note - the note object that was opened
 */
function noteOpenedHook(note);
```

Może zechcesz rzucić okiem na przykład [on-note-opened.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/on-note-opened.qml).

## noteDoubleClickedHook

### Wywołanie metody i parametry

```js
/**
 * Ta funkcja jest wywoływana po dwukrotnym kliknięciu notatki
 *
 * @param {NoteApi} note - kliknięty obiekt notatki
 */
function noteDoubleClickedHook(note);
```

Może zechcesz rzucić okiem na przykład [external-note-open.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/external-note-open.qml).

## insertMediaHook

Ta funkcja jest wywoływana, gdy plik multimedialny jest wstawiany do bieżącej notatki.

Jeśli ta funkcja jest zdefiniowana w wielu skryptach, wygrywa pierwszy skrypt, który zwróci niepusty ciąg.

### Wywołanie metody i parametry

```js
/**
 * @param fileName string the file path of the source media file before it was copied to the media folder
 * @param markdownText string the markdown text of the media file, e.g. ![my-image](media/my-image-4101461585.jpg)
 * @return string the new markdown text of the media file
 */
function insertMediaHook(fileName, markdownText);
```

Może zechcesz rzucić okiem na przykład [example.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/example.qml).

## insertAttachmentHook

Ta funkcja jest wywoływana, gdy plik załącznika jest wstawiany do bieżącej notatki.

Jeśli ta funkcja jest zdefiniowana w wielu skryptach, wygrywa pierwszy skrypt, który zwróci niepusty ciąg.

### Wywołanie metody i parametry

```js
/**
 * @param fileName string the file path of the source attachment file before it was copied to the attachment folder
 * @param markdownText string the markdown text of the attachment file, e.g. [my-file.txt](attachments/my-file-4245650967.txt)
 * @return string the new markdown text of the attachment file
 */
function insertAttachmentHook(fileName, markdownText);
```

Może zechcesz rzucić okiem na przykład [example.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/example.qml).

## insertingFromMimeDataHook

### Wywołanie metody i parametry

```js
/**
 * Ta funkcja jest wywoływana, gdy html lub plik multimedialny jest wklejany do notatki za pomocą `Ctrl + Shift + V`
 *
 * Tekst @param text obiektu QMimeData
 * @param html html obiektu QMimeData
 * @zwraca ciąg, który należy wstawić zamiast tekstu z obiektu QMimeData
 */
funkcja insertingFromMimeDataHook(text, html);
```

Może zechcesz rzucić okiem na przykład [example.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/example.qml), [insert-headline-with-link-from-github-url.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/insert-headline-with-link-from-github-url.qml) or [note-text-from-5pm-mail.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/note-text-from-5pm-mail.qml).

## handleNoteTextFileNameHook

### Wywołanie metody i parametry

```js
/**
  * Ta funkcja jest wywoływana, gdy notatka zostanie zapisana na dysku, jeśli
  * "Zezwalaj, aby nazwa pliku notatki różniła się od nagłówka" jest włączona
  * w ustawieniach
  *
  * Pozwala zmienić nazwę pliku notatki
  * Pamiętaj, że sam musisz zadbać o zduplikowane nazwy!
 *
  * Zwróć pusty ciąg, jeśli nazwa pliku notatki powinna
  * nie można modyfikować
  *
  * @param {NoteApi} note - obiekt notatki przechowywanej notatki
  * @return {string} nazwa pliku notatki
  */
funkcja handleNoteTextFileNameHook(note);
```

Może zechcesz rzucić okiem na przykład [example.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/example.qml) or [use-tag-names-in-filename.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/use-tag-names-in-filename.qml).

## handleNoteNameHook

### Wywołanie metody i parametry

```js
/**
  * Ta funkcja jest wywoływana, gdy nazwa nuty jest określona dla nuty
  *
  * Pozwala zmienić nazwę wyświetlanej notatki
  *
  * Zwróć pusty ciąg, jeśli nazwa notatki nie powinna być modyfikowana
  *
  * @param {NoteApi} note - obiekt notatki przechowywanej notatki
  * @return {string} nazwa notatki
  */
funkcja handleNoteNameHook(uwaga);
```

The note name is mainly used when showing the note in the lists.

Może zechcesz rzucić okiem na przykład [example.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/example.qml).

Używanie tego haka może nie być dobrym pomysłem, jeśli ustawienie używania nazwy pliku jako nazwy notatki jest aktywne.

## handleNewNoteHeadlineHook

### Wywołanie metody i parametry

```js
/**
  * Ta funkcja jest wywoływana przed utworzeniem notatki
  *
  * Pozwala modyfikować nagłówek notatki przed jej utworzeniem
  * Pamiętaj, że musisz zadbać o unikalną nazwę notatki, w przeciwnym razie
  * nowa notatka nie zostanie utworzona, zostanie po prostu znaleziona na liście notatek
  *
  * Możesz użyć tej funkcji do tworzenia szablonów notatek
  *
  * @param tekst nagłówka, który zostałby użyty do utworzenia nagłówka
  * @return {string} nagłówek notatki
  */
uchwyt funkcjiNowaUwagaNagłówekHook(nagłówek);
```

Może zechcesz rzucić okiem na przykład [custom-new-note-headline.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-new-note-headline.qml).

## preNoteToMarkdownHtmlHook

### Wywołanie metody i parametry

```js
/**
  * Ta funkcja jest wywoływana przed wygenerowaniem kodu html notatki
  *
  * Pozwala modyfikować to, co jest przekazywane do konwertera przecen do html
  *
  * Funkcja może być używana na przykład w wielu skryptach do renderowania kodu (takich jak matematyka LaTeX lub syrenka)
  * do jego graficznej reprezentacji do podglądu
  *
  * Notatka nie zostanie zmieniona w tym procesie
  *
  * Notatka @param {NoteApi} – obiekt notatki
  * @param {string} przecena - przecena, która ma zostać przekonwertowana na html
  * @param {bool} forExport - true jeśli html jest używany do eksportu, false dla podglądu
  * @return {string} zmodyfikowany przecena lub pusty ciąg, jeśli nic nie powinno być modyfikowane
  */
funkcja preNoteToMarkdownHtmlHook(note, markdown, forExport);
```

Może zechcesz rzucić okiem na przykład [preview-styling.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/preview-styling.qml).

## noteToMarkdownHtmlHook

### Wywołanie metody i parametry

```js
/**
  * Ta funkcja jest wywoływana, gdy generowany jest znacznik html notatki
  *
  * Pozwala na modyfikację tego html
  * Jest to na przykład nazywane wcześniej w podglądzie notatki
  *
  * Funkcja może być używana w wielu skryptach do modyfikowania html podglądu
  *
  * Notatka @param {NoteApi} – obiekt notatki
  * @param {string} html - html, który ma zostać wyrenderowany
  * @param {bool} forExport - true jeśli html jest używany do eksportu, false dla podglądu
  * @return {string} zmodyfikowany html lub pusty ciąg, jeśli nic nie powinno być modyfikowane
  */
funkcja noteToMarkdownHtmlHook(note, html, forExport);
```

Może zechcesz rzucić okiem na przykład [example.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/example.qml) or [preview-styling.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/preview-styling.qml).

Proszę zapoznać się z [Obsługiwanym kodem HTML Dokumentacja podzbioru](http://doc.qt.io/qt-5/richtext-html-subset.html) aby zobaczyć listę wszystkich obsługiwanych stylów CSS.

## encryptionHook

### Wywołanie metody i parametry

```js
/**
  * Ta funkcja jest wywoływana, gdy tekst ma być zaszyfrowany lub odszyfrowany
  *
  * @param ciąg tekstowy tekst do zaszyfrowania lub odszyfrowania
  * @param hasło ciąg hasła
  * @param decrypt bool, jeśli wymagane jest fałszywe szyfrowanie, jeśli wymagane jest prawdziwe odszyfrowanie
  * @zwróć zaszyfrowany odszyfrowany tekst
  */
funkcja szyfrowaniaHook (tekst, hasło, odszyfrowanie);
```

Może zechcesz rzucić okiem na przykład [encryption-keybase.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/encryption-keybase.qml), [encryption-pgp.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/encryption-pgp.qml) lub [encryption-rot13.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/encryption-rot13.qml).

## noteTaggingHook

Możesz zaimplementować własny mechanizm tagowania notatek, na przykład za pomocą specjalnego tekstu w swojej notatce, takiego jak `@tag1`, `@tag2`, `@tag3`.

### Wywołanie metody i parametry

```js
/**
  * Obsługuje tagowanie notatek dla notatki
  *
  * Ta funkcja jest wywoływana, gdy tagi są dodawane, usuwane lub zmieniane w
  * notatka lub tagi notatki powinny być wymienione
  *
  * @param uwaga
  * Akcja @param może być „dodaj”, „usuń”, „zmień nazwę” lub „lista”
  * @param tagName nazwa tagu do dodania, usunięcia lub zmiany nazwy
  * @param newTagName nazwa tagu do zmiany nazwy, jeśli akcja = „zmień nazwę”
  * @return note ciąg tekstowy lub lista ciągów nazw tagów (jeśli akcja = „lista”)
  */
funkcja noteTaggingHook(notatka, akcja, nazwa_znacznika, nazwa_nowa_znacznika);
```

- as soon as a script is activated that implements the new function `noteTaggingHook` note tagging will be handled by that function
- następujące funkcje powinny działać w interfejsie użytkownika QOwnNotes
  - initially importing tags like `@tag` from your notes and overwriting your current tag assignment
    - you will not lose your tags tree, just the former assignment to notes
    - nadal możesz przenosić tagi do innych tagów
    - if more than one tag has the same name in your tag tree the first hit will be assigned
  - dodanie tagu do notatki spowoduje dodanie tagu do tekstu notatki
  - usunięcie znacznika z notatki spowoduje usunięcie znacznika z tekstu notatki
  - removing of tags in the tag list will remove those tags from your notes
  - renaming of tags in the tag list will rename those tags in your notes
  - bulk tagging of notes in the note list will add those tags to your notes
  - bulk removing of tags from notes in the note list will remove those tags from your notes
  - the application will trigger a series of `add` and `remove` actions for all selected tags and their children on all notes if tags are moved in the tag panel

Może zechcesz rzucić okiem na przykład [note-tagging.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/note-tagging.qml) zaimplementuj własny mechanizm tagowania.

::: warning
Make sure your `list` action is really fast, because it will be executed for every note every time the note folder or the tag tree is reloaded!

That is every time you find `reloadTagTree` in [mainwindow.cpp](https://github.com/pbek/QOwnNotes/blob/main/src/mainwindow.cpp).
:::

## noteTaggingByObjectHook

Podobnie jak w przypadku [noteTaggingHook](#notetagginghook), możesz zaimplementować własny mechanizm tagowania notatek, ale nie jesteś związany z nazwami tagów w korzeniu drzewa tagów. W ten sposób możesz wykorzystać całe drzewo tagów zamiast tylko listy tagów.

Dzięki `noteTaggingByObjectHook` otrzymujesz jako parametr obiekt `TagApi` zamiast nazwy tagu. W rezultacie dla akcji `lista` musisz podać listę identyfikatorów tagów.

Oznacza to również, że musisz samodzielnie utworzyć brakujące tagi, aby móc dostarczyć listę już istniejących identyfikatorów tagów dla akcji `lista`.

### Wywołanie metody i parametry

```js
/**
  * Obsługuje tagowanie notatek dla notatki
  *
  * Ta funkcja jest wywoływana, gdy tagi są dodawane, usuwane lub zmieniane w
  * notatka lub tagi notatki powinny być wymienione
  *
  * @param uwaga
  * Akcja @param może być „dodaj”, „usuń”, „zmień nazwę” lub „lista”
  * @param tag do dodania, usunięcia lub zmiany nazwy
  * @param newTagName nazwa tagu do zmiany nazwy, jeśli akcja = „zmień nazwę”
  * @return note ciąg tekstowy lub ciąg-lista identyfikatorów tagów (jeśli action = „list”)
  */
funkcja noteTaggingByObjectHook(notatka, akcja, tag, nowaNazwaTag);
```

Może zechcesz rzucić okiem na przykład [note-tagging-by-object.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/note-tagging-by-object.qml) zaimplementuj własny mechanizm tagowania.

## autocompletionHook

Możesz zwrócić listę ciągów, które mają zostać dodane do listy autouzupełniania po wywołaniu autouzupełniania (na przykład naciskając <kbd>Ctrl + spacja</kbd>).

### Wywołanie metody i parametry

```js
/**
  * Wywołuje funkcję autocompletionHook dla wszystkich komponentów skryptu
  * Ta funkcja jest wywoływana, gdy w notatce wywoływane jest autouzupełnianie
  *
  * @return QStringList tekstu dla listy autouzupełniania
  */
wywołanie funkcjiAutouzupełnianieHook();
```

Może zechcesz rzucić okiem na przykład [autocompletion.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/autocompletion.qml).

## websocketRawDataHook

Ten zaczep jest wywoływany, gdy dane są wysyłane z przeglądarki QOwnNotes Web Companion rozszerzenie poprzez menu kontekstowe przeglądarki internetowej.

### Wywołanie metody i parametry

```js
/**
 * @param requestType can be "page" or "selection"
 * @param pageUrl the url of the webpage where the request was made
 * @param pageTitle the page title of the webpage where the request was made
 * @param rawData the data that was transmitted, html for requestType "page" or plain text for requestType "selection"
 * @param screenshotDataUrl the data url of the screenshot of the webpage where the request was made
 * @return true if data was handled by a hook
 */
function callHandleWebsocketRawDataHook(requestType, pageUrl, pageTitle, rawData, screenshotDataUrl);
```

Może zechcesz rzucić okiem na przykład [websocket-raw-data-new-note.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/websocket-raw-data-new-note.qml) i [websocket-raw-data-selection-in-note.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/websocket-raw-data-selection-in-note.qml).

## onDetachedProcessCallback

Ten zaczep jest wywoływany, gdy wątek skryptu [startDetachedProcess](methods-and-objects.html#starting-an-external-program-in-the-background) jest wykonywany.

### Wywołanie metody i parametry

```js
/**
  * Ta funkcja jest wywoływana po zakończeniu wykonywania wątku skryptu.
 * Podpowiedź: thread[1]==0 pomaga określić, czy większość uruchomionych procesów dla określonego identyfikatora została wykonana.
 *
 * @param {QString} callbackIdentifier - the provided id when calling startDetachedProcess()
 * @param {QString} resultSet - the result of the process
 * @param {QVariantList} cmd - the entire command array [0-executablePath, 1-parameters, 2-exitCode]
 * @param {QVariantList} thread - the thread information array [0-passed callbackParameter, 1-remaining threads for this identifier]
 */
function onDetachedProcessCallback(callbackIdentifier, resultSet, cmd, thread);
```

Może zechcesz rzucić okiem na przykład [callback-example.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/callback.qml).

## windowStateChangedHook

### Wywołanie metody i parametry

```js
/**
  * Ta funkcja jest wywoływana po wyzwoleniu zdarzenia WindowStateChange
 *
 * @param {QString} windowState - the new window state, parameter value can be "minimized", "maximized", "fullscreen", "active" or "nostate"
 */
function windowStateChangedHook(windowState);
```

Może zechcesz rzucić okiem na przykład [window-state-changed.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/window-state-changed.qml).

## workspaceSwitchedHook

This hook is called when workspaces are switched.

### Wywołanie metody i parametry

```js
/**
 * This function is called when workspaces are switched
 *
 * @param oldUuid old uuid of workspace
 * @param newUuid new uuid of workspace
 */
function workspaceSwitchedHook(oldUuid, newUuid);
```

Może zechcesz rzucić okiem na przykład [websocket-raw-data-new-note.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/workspaces.qml).

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
