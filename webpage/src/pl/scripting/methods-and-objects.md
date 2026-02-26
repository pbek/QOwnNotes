# Methods and objects QOwnNotes provides

## Uruchamianie zewnętrznego programu w tle

### Wywołanie metody i parametry

```cpp
/**
  * Nakładka QML, aby rozpocząć odłączony proces
  *
  * @param executablePath ścieżka do pliku wykonywalnego
  * @param parameters lista ciągów parametrów
  * @param callbackIdentifier identyfikator do użycia w funkcji onDetachedProcessCallback() (opcjonalnie)
  * @param callbackParameter dodatkowy parametr dla pętli itp. (opcjonalnie)
  * @param processData dane zapisywane do procesu w przypadku użycia wywołania zwrotnego (opcjonalnie)
  * @param workingDirectory katalog roboczy do wykonania procesu (opcjonalnie, działa tylko bez wywołania zwrotnego)
  * @zwróć prawdę w przypadku sukcesu, w przeciwnym razie fałsz
  */
bool startDetachedProcess(QString executablePath, parametry QStringList,
                             QString callbackIdentifier, QVariant callbackParameter,
                             QByteArray processData, QString workingDirectory);
```

### Przykład

Prosty przykład:

```js
script.startDetachedProcess("/path/to/my/program", ["my parameter"]);
```

Uruchamianie wielu procesów:

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

Może zechcesz rzucić okiem na przykład [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-actions.qml), [callback.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/callback.qml) lub [execute-command-after-note-update.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/execute-command-after-note-update.qml).

Możesz również przyjrzeć się haczykowi [onDetachedProcessCallback](hooks.html#ondetachedprocesscallback).

## Uruchamiam zewnętrzny program i czekam na wyjście

### Wywołanie metody i parametry

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

### Przykład

```js
var result = script.startSynchronousProcess(
  "/path/to/my/program",
  ["my parameter"],
  "data",
  "/path/to/execute/in",
);
```

Może zechcesz rzucić okiem na przykład [encryption-keybase.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/encryption-keybase.qml).

## Pobieranie ścieżki bieżącego folderu notatek

### Wywołanie metody i parametry

```cpp
/**
 * QML wrapper to get the current note folder path
 *
 * @return the path of the current note folder
 */
QString currentNoteFolderPath();
```

### Przykład

```js
var path = script.currentNoteFolderPath();
```

Może zechcesz rzucić okiem na przykład [absolute-media-links.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/absolute-media-links.qml).

## Pobieranie aktualnej notatki

### Wywołanie metody i parametry

```cpp
/**
 * QML wrapper, aby uzyskać bieżącą notatkę
 *
 * @returns {NoteApi} bieżący obiekt notatki
 */
NoteApi currentNote();
```

### Przykład

```js
var note = script.currentNote();
```

Może zechcesz rzucić okiem na przykład [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-actions.qml).

## Logowanie do widżetu dziennika

### Wywołanie metody i parametry

```cpp
/**
 * QML wrapper to log to the log widget
 *
 * @param text
 */
void log(QString text);
```

### Przykład

```js
script.log("my text");
```

## Pobieranie adresu URL do ciągu znaków

### Wywołanie metody i parametry

```cpp
/**
 * QML wrapper to download an url and returning it as text
 *
 * @param url
 * @return {QString} the content of the downloaded url
 */
QString downloadUrlToString(QUrl url);
```

### Przykład

```js
var html = script.downloadUrlToString("https://www.qownnotes.org");
```

Może zechcesz rzucić okiem na przykład [insert-headline-with-link-from-github-url.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/insert-headline-with-link-from-github-url.qml).

## Wywołanie metody i pobieranie adresu URL do folderu multimediów

### Wywołanie metody i parametry

```cpp
/**
 * QML wrapper to download an url to the media folder and returning the media
 * url or the markdown image text of the media relative to the current note
 *
 * @param {QString} url
 * @param {bool} returnUrlOnly if true only the media url will be returned (default false)
 * @return {QString} the media markdown or url
 */
QString downloadUrlToMedia(QUrl url, bool returnUrlOnly);
```

### Przykład

```js
var markdown = script.downloadUrlToMedia(
  "http://latex.codecogs.com/gif.latex?\frac{1}{1+sin(x)}",
);
```

Może zechcesz rzucić okiem na przykład [paste-latex-image.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/paste-latex-image.qml).

## Wstawianie pliku multimedialnego do folderu multimediów

### Wywołanie metody i parametry

```cpp
/**
  * Opakowanie QML do wstawienia pliku multimedialnego do folderu multimediów i powrotu
  * adres URL multimediów lub tekst obrazu przeceny multimediów w stosunku do bieżącej notatki
  *
  * @param {QString} mediaFilePath
  * @param {bool} returnUrlTylko jeśli true zostanie zwrócony tylko adres URL mediów (domyślnie false)
  * @return {QString} znacznik lub adres URL mediów
  */
QString ScriptingService::insertMediaFile(QString mediaFilePath,
                                         bool returnUrlOnly);
```

### Przykład

```js
var markdown = script.insertMediaFile("/path/to/your/image.png");
```

Może zechcesz rzucić okiem na przykład [scribble.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/scribble.qml).

## Wstawianie pliku załącznika do folderu załączników

### Wywołanie metody i parametry

```cpp
 * QML wrapper to insert an attachment file into the `attachments` folder and
 * returning the attachment url or the markdown text of the attachment
 * relative to the current note
 *
 * @param {QString} attachmentFilePath
 * @param {QString} fileName to use in the markdown
 * @param {bool} returnUrlOnly if true only the attachment url will be returned
 * (default false)
 * @return {QString} the attachment markdown or url
 */
QString ScriptingService::insertAttachmentFile(const QString &attachmentFilePath,
                                               const QString &fileName,
                                               bool returnUrlOnly);
```

### Przykład

```js
var markdown = script.insertAttachmentFile("/path/to/your/file.png");
```

## Ponowne generowanie podglądu notatki

Odświeża podgląd notatki.

### Wywołanie metody i parametry

```cpp
/**
 * Regenerates the note preview
 */
QString ScriptingService::regenerateNotePreview();
```

### Przykład

```js
script.regenerateNotePreview();
```

Może zechcesz rzucić okiem na przykład [scribble.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/scribble.qml).

## Rejestrowanie akcji niestandardowej

### Wywołanie metody i parametry

```cpp
/**
 * Rejestruje akcję niestandardową
 *
 * @param identyfikator identyfikator akcji
 * @param menuText tekst widoczny w menu
 * @param buttonText tekst widoczny na przycisku
 * (żaden przycisk nie będzie widoczny, jeśli jest pusty)
 * @param icon ścieżka pliku ikony lub nazwa ikony motywu freedesktop
 * listę ikon znajdziesz tutaj:
 * https://specyfikacje.freedesktop.org/icon-naming-spec/icon-naming-spec-latest.html
 * @param useInNoteEditContextMenu jeśli true użyj akcji w edycji notatki
 * menu kontekstowe (domyślnie: false)
 * @param hideButtonInToolbar jeśli prawda, przycisk nie będzie pokazywany w
 * niestandardowy pasek narzędzi akcji (domyślnie: fałsz)
 * @param useInNoteListContextMenu jeśli true użyj akcji na liście notatek
 * menu kontekstowe (domyślnie: false)
 */
void ScriptingService::registerCustomAction (identyfikator QString,
                                            Menu QStringTekst,
                                            Przycisk QStringTekst,
                                            ikona QString,
                                            bool useInNoteEditContextMenu,
                                            bool hideButtonInToolbar,
                                            bool useInNoteListContextMenu);
```

::: tip
You can also assign local and global shortcuts to your custom actions in the _Shortcuts settings_.
:::

::: warning
Pamiętaj, że [ikony motywów freedesktop](https://specifications.freedesktop.org/icon-naming-spec/icon-naming-spec-latest.html) są w większości dostępne tylko pod Linuksem. Więc jeśli naprawdę chcesz użyć ikony pod macOS lub Windows musisz dostarczyć go ze swoim skryptem. Aby uzyskać ścieżkę skryptu, aby ustawić właściwą ścieżkę dla swojej ikony, możesz użyć właściwości [scriptDirPath property](methods-and-objects.md#reading-the-path-to-the-directory-of-your-script).
:::

### Przykład

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

Aby uzyskać więcej przykładów, zobacz [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-actions.qml).

::: tip
Możesz również wyzwolić akcję niestandardową po uruchomieniu aplikacji z parametrem `--action customAction_<identifier>`. Aby uzyskać więcej informacji, spójrz na [Wyzwalaj działania menu po uruchomieniu](../getting-started/cli-parameters.md#trigger-menu-actions-after-startup).
:::

## Rejestracja etykiety

### Wywołanie metody i parametry

```cpp
/**
  * Rejestruje etykietę do pisania
  *
  * @param identyfikator identyfikator etykiety
  * @param text tekst widoczny na etykiecie (opcjonalnie)
  */
void ScriptingService::registerLabel(Identyfikator QString, tekst QString);
```

### Przykład

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

W etykietach można używać zarówno zwykłego tekstu, jak i html. W etykietach można używać zarówno zwykłego tekstu, jak i html.

Następnie możesz rzucić okiem na przykładowy skrypt [scripting-label-demo.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/scripting-label-demo.qml).

## Ustawianie tekstu zarejestrowanej etykiety

### Metoda wywołania i parametry

```cpp
/**
  * Ustawia tekst zarejestrowanej etykiety
  *
  * @param identyfikator identyfikator etykiety
  * @param text tekst pokazany na etykiecie
  */
void ScriptingService::setLabelText(Identyfikator QString, tekst QString);
```

### Przykład

```js
script.setLabelText("counter-label", "counter text");
```

W etykietach można używać zarówno zwykłego tekstu, jak i html. Tekst będzie do wyboru i linki można kliknąć.

Następnie możesz rzucić okiem na przykładowy skrypt [scripting-label-demo.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/scripting-label-demo.qml).

::: tip
The scripting label also supports **external links**, as well as **internal links** to notes, tasks, deck cards and more. You might get a grasp of what's supported by taking a look at [URL handling](https://github.com/pbek/QOwnNotes/blob/964acf69b6382f8ee8252c640c5048f8f4644622/src/utils/urlhandler.cpp#L16-L75).
:::

## Tworzenie nowej notatki

### Wywołanie metody i parametry

```cpp
/**
 * Tworzy nową notatkę
 *
 * @param text the note text
 */
void ScriptingService::createNote(QString text);
```

### Przykład

```js
script.createNote("My note headline\n===\n\nMy text");
```

Może zechcesz rzucić okiem na przykład [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-actions.qml).

::: tip
If you turned off that your note headline determines the note filename then you have to rename your note file yourself afterward, like this:

```js
var note = script.currentNote();
note.renameNoteFile("your-filename");
```

:::

## Dostęp do schowka

### Wywołanie metody i parametry

```cpp
/**
 * Zwraca zawartość schowka jako tekst lub html
 *
 * @param asHtml returns the clipboard content as html instead of text
 */
QString ScriptingService::clipboard(bool asHtml);
```

### Przykład

```js
var clipboardText = script.clipboard();
var clipboardHtml = script.clipboard(true);
```

Może zechcesz rzucić okiem na przykład [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-actions.qml).

## Napisz tekst do notatki edytuj tekst

### Wywołanie metody i parametry

```cpp
/**
 * Zapisuje tekst w bieżącej pozycji kursora w edycji tekstu notatki
 *
 * @param text
 */
void ScriptingService::noteTextEditWrite(QString text);
```

### Przykład

```js
// napisz tekst do notatki edytuj tekst
script.noteTextEditWrite("My custom text");
```

Możesz spojrzeć na akcję niestandardową `transformTextRot13` na przykładzie [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-actions.qml).

Możesz użyć tego razem z `noteTextEditSelectAll`, aby nadpisać cały tekst bieżącej notatki.

## Przeczytaj zaznaczony tekst w edycji tekstu notatki

### Wywołanie metody i parametry

```cpp
/**
 * Czyta zaznaczony tekst w edycji tekstu notatki
 *
 * @return
 */
QString ScriptingService::noteTextEditSelectedText();
```

### Przykład

```js
// read the selected text from the note text edit
var text = script.noteTextEditSelectedText();
```

Możesz spojrzeć na akcję niestandardową `transformTextRot13` na przykładzie [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-actions.qml).

## Zaznacz cały tekst w edycji tekstu notatki

### Wywołanie metody i parametry

```cpp
/**
 * Zaznacza cały tekst w edycji tekstu notatki
 */
void ScriptingService::noteTextEditSelectAll();
```

### Przykład

```js
script.noteTextEditSelectAll();
```

Możesz użyć tego razem z `noteTextEditWrite`, aby nadpisać cały tekst bieżącej notatki.

## Wybierz bieżący wiersz w edycji tekstu notatki

### Wywołanie metody i parametry

```cpp
/**
  * Wybiera bieżącą linię w edycji tekstu notatki
  */
void ScriptingService::noteTextEditSelectCurrentLine();
```

### Przykład

```js
script.noteTextEditSelectCurrentLine();
```

## Wybierz bieżące słowo w edycji tekstu notatki

### Wywołanie metody i parametry

```cpp
/**
 * Wybiera bieżące słowo w edycji tekstu notatki
 */
void ScriptingService::noteTextEditSelectCurrentWord();
```

### Przykład

```js
script.noteTextEditSelectCurrentWord();
```

## Ustaw aktualnie zaznaczony tekst w edycji tekstu notatki

### Wywołanie metody i parametry

```cpp
/**
 * Ustawia aktualnie zaznaczony tekst w edycji tekstu notatki
 *
 * @param start
 * @param end
 */
void ScriptingService::noteTextEditSetSelection(int start, int end);
```

### Przykład

```js
// expands the current selection by one character
script.noteTextEditSetSelection(
  script.noteTextEditSelectionStart() - 1,
  script.noteTextEditSelectionEnd() + 1,
);
```

## Uzyskaj pozycję początkową bieżącego zaznaczenia w edycji tekstu notatki

### Wywołanie metody i parametry

```cpp
/**
 * Zwraca pozycję początkową bieżącego zaznaczenia w edycji tekstu notatki
 */
int ScriptingService::noteTextEditSelectionStart();
```

### Przykład

```js
script.log(script.noteTextEditSelectionStart());
```

## Uzyskaj pozycję końcową bieżącego zaznaczenia w edycji tekstu notatki

### Wywołanie metody i parametry

```cpp
/**
 * Zwraca pozycję początkową bieżącego zaznaczenia w edycji tekstu notatki
 */
int ScriptingService::noteTextEditSelectionStart();
```

### Przykład

```js
script.log(script.noteTextEditSelectionEnd());
```

## Zwraca pozycję początkową bieżącego zaznaczenia w edycji tekstu notatki

### Wywołanie metody i parametry

```cpp
/**
 * Ustawia kursor tekstowy w edycji tekstu notatki w określonej pozycji
 * 0 would be the beginning of the note
 * special case: -1 would be the end of the note
 *
 * @param position
 */
void ScriptingService::noteTextEditSetCursorPosition(int position);
```

### Przykład

```js
// jump to the 11th character in the note
script.noteTextEditSetCursorPosition(10);

// jump to the end of the note
script.noteTextEditSetCursorPosition(-1);
```

## Uzyskaj aktualną pozycję kursora tekstowego w edycji tekstu notatki

### Metoda wywołania i parametry

```cpp
/**
 * Zwraca bieżącą pozycję kursora tekstowego w edycji tekstu notatki
 * 0 would be the beginning of the note
 */
int ScriptingService::noteTextEditCursorPosition();
```

### Przykład

```js
script.log(script.noteTextEditCursorPosition());
```

## Przeczytaj bieżące słowo z edycji tekstu notatki

### Wywołanie metody i parametry

```cpp
/**
  * Czyta bieżące słowo w edycji tekstu notatki
  *
 * @param withPreviousCharacters also get more characters at the beginning
 *                               to get characters like "@" that are not
 *                               word-characters
 * @return
 */
QString ScriptingService::noteTextEditCurrentWord(bool withPreviousCharacters);
```

### Przykład

```js
// odczytaj bieżące słowo w tekście notatki edytuj
var text = script.noteTextEditCurrentWord();
```

Może zechcesz rzucić okiem na przykład [autocompletion.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/autocompletion.qml).

## Read the current block from the note text edit

### Wywołanie metody i parametry

```cpp
/**
 * Reads the current block in the note text edit
 *
 * @return
 */
QString ScriptingService::noteTextEditCurrentBlock();
```

### Przykład

```js
// Read the current block in the note text edit
var text = script.noteTextEditCurrentBlock();
```

You may want to take a look at the example [ai-autocompletion.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/ai-autocompletion.qml).

## Use a completion prompt on the currently selected AI model

The AI completion prompt is a feature that allows you to use a completion prompt on the currently selected AI model, like ChatGPT, Claude or systems like Ollama.

The AI system needs to be enabled in the AI toolbar or main menu for this to work.

### Wywołanie metody i parametry

```cpp
/**
 * QML wrapper to use the AI Completer
 *
 * @param prompt
 * @return {QString} the result of the completer
 */
QString ScriptingService::aiComplete(const QString& prompt);
```

### Przykład

```js
// Ask the currently selected AI model to complete the prompt
var text = script.aiComplete("Tell me how do you feel today?");
```

You may want to take a look at the example [ai-autocompletion.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/ai-autocompletion.qml).

## Sprawdź, czy platforma to Linux, OS X czy Windows

### Wywołanie metody i parametry

```cpp
bool ScriptingService::platformIsLinux();
bool ScriptingService::platformIsOSX();
bool ScriptingService::platformIsWindows();
```

### Przykład

```js
if (script.platformIsLinux()) {
  // Will be executed only if under Linux
}
```

## Oznacz bieżącą notatkę

### Wywołanie metody i parametry

```cpp
/**
  * Oznacza bieżącą notatkę znacznikiem o nazwie tagName
 *
 * @param tagName
 */
void ScriptingService::tagCurrentNote(QString tagName);
CONTEXTREQUEST

```

### Przykład

```js
// dodaj znacznik "ulubionych" do bieżącej notatki
script.tagCurrentNote("favorite");
```

Możesz spojrzeć na akcję niestandardową `favoriteNote` na przykładzie [favorite-note.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/favorite-note.qml).

## Utwórz lub pobierz tag według nazwy listy nawigacyjnej

### Wywołanie metody i parametry

```cpp
/**
  * Pobiera lub tworzy tag na podstawie jego „listy okruszków” nazw tagów
  * Element nameList[0] byłby najwyżej w drzewie (z parentId: 0)
  *
  * @param nameList
  * @param createMissing {bool} jeśli true (domyślnie) wszystkie brakujące tagi zostaną utworzone
  * @return TagApi obiekt najgłębszego tagu na liście okruszków w nazwie
  */
TagApi *ScriptingService::getTagByNameBreadcrumbList(
     const QStringList &nameList, bool createMissing);
```

### Przykład

```js
// tworzy wszystkie tagi do 3 poziomu i zwraca obiekt tagu dla
// tag "level3", który będzie wyglądał tak w drzewie tagów:
// level1 > level2 > level3
var tag = script.getTagByNameBreadcrumbList(["level1", "level2", "level3"]);
```

## Wyszukaj tagi według nazwy

### Wywołanie metody i parametry

```cpp
/**
  * Pobiera wszystkie tagi, przeszukując podciągi w polu nazwy
  *
  * @param name {QString} nazwa do wyszukania
  * @return {QStringList} lista nazw znaczników
  */
QStringList ScriptingService::searchTagsByName(nazwa QString);
```

### Przykład

```js
// wyszukuje wszystkie tagi zawierające słowo gra
var tags = script.searchTagsByName("game");
```

Może zechcesz rzucić okiem na przykład [autocompletion.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/autocompletion.qml).

## Szukaj notatek według tekstu notatki

### Wywołanie metody i parametry

```cpp
/**
  * Zwraca listę identyfikatorów wszystkich notatek z określonym tekstem w tekście notatki
  *
  * Niestety nie ma łatwego sposobu na użycie QList<NoteApi*> w QML, więc my
  * może przenosić tylko identyfikatory banknotów
  *
* @return {QList<int>} list of note ids
 */
QList<int> ScriptingService::fetchNoteIdsByNoteTextPart(QString text);
```

### Przykład

```js
var noteIds = script.fetchNoteIdsByNoteTextPart("mytext");

noteIds.forEach(function (noteId) {
  var note = script.fetchNoteById(noteId);

  // do something with the note
});
```

Może zechcesz rzucić okiem na przykład [unique-note-id.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/unique-note-id.qml).

## Dodaj niestandardowy arkusz stylów

### Wywołanie metody i parametry

```cpp
/**
 * Dodaje niestandardowy arkusz stylów do aplikacji
 *
 * @param stylesheet
 */
void ScriptingService::addStyleSheet(QString stylesheet);
```

### Przykład

```js
// zwiększ tekst na liście notatek
script.addStyleSheet("QTreeWidget#noteTreeWidget {font-size: 30px;}");
```

Może zechcesz rzucić okiem na przykład [custom-stylesheet.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-stylesheet.qml).

Możesz uzyskać nazwy widżetów z `*.ui` to pliki, na przykład główne okno [mainwindow.ui](https://github.com/pbek/QOwnNotes/blob/main/src/mainwindow.ui).

Dokumentacja Qt (na przykład [QMainWindow](https://doc.qt.io/qt-5/qmainwindow.html)) może pomóc Ci zobaczyć, jak widżety są ze sobą powiązane (wyszukaj `Dziedziczy` na stronach).

Podstawowym widżetem dla prawie wszystkiego jest [QWidget](https://doc.qt.io/qt-5/qwidget.html). Więc po prostu stylizuj `QWidget` za pomocą na przykład `QWidget {background-color: black; kolor: biały;}` oznaczałoby, że wszystko ma czarne tło i biały kolor pierwszego planu.

::: Wskazówka To [style.qss](https://github.com/pbek/QOwnNotes/blob/main/src/libraries/qdarkstyle/style.qss) z [qdarkstyle](https://github.com/pbek/QOwnNotes/blob/main/src/libraries/qdarkstyle) może być również dobrym punktem odniesienia dla stylów, które można zmienić.
:::

Spójrz na [Informacje o arkuszu stylów](http://doc.qt.io/qt-5/stylesheet-reference.html), aby dowiedzieć się, jakie style są dostępne.

Jeśli chcesz wstawić style do podglądu HTML, aby zmienić sposób wyświetlania podglądu notatek, spójrz na [notetomarkdownhtmlhak](hooks.html#notetomarkdownhtmlhook).

::: tip
Jeśli rzeczywiście chcesz zobaczyć, jak wyglądają okna dialogowe i jakie są ich nazwy, możesz pobrać [Qt Creator](https://www.qt.io/product/development-tools) i otworzyć w nim pliki `*.ui`.
:::

## Ponowne ładowanie silnika skryptów

### Wywołanie metody i parametry

```cpp
/**
 * Reloads the scripting engine
 */
void ScriptingService::reloadScriptingEngine();
```

### Przykład

```js
// reload the scripting engine
script.reloadScriptingEngine();
```

## Pobieranie notatki według nazwy pliku

### Wywołanie metody i parametry

```cpp
/**
  * Pobiera notatkę według nazwy pliku
  *
  * @param fileName ciąg nazwy pliku notatki (obowiązkowe)
  * @param noteSubFolderId całkowity identyfikator podfolderu notatki
  * @return NoteApi*
  */
NoteApi* ScriptingService::fetchNoteByFileName(QString fileName,
                                                 int notePodFolderId);
```

### Przykład

```js
// fetch note by file name
script.fetchNoteByFileName("my note.md");
```

## Pobieranie notatki według jej identyfikatora

### Wywołanie metody i parametry

```cpp
/**
 * Pobiera notatkę według swojego identyfikatora
 *
 * @param id int the id of the note
 * @return NoteApi*
 */
NoteApi* ScriptingService::fetchNoteById(int id);
```

### Przykład

```js
// fetch note by id
script.fetchNoteById(243);
```

Może zechcesz rzucić okiem na przykład [export-notes-as-one-html.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/export-notes-as-one-html.qml).

## Sprawdzanie, czy notatka istnieje według nazwy pliku

### Wywołanie metody i parametry

```cpp
/**
 * Sprawdza, czy plik notatki istnieje według nazwy pliku
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

### Przykład

```js
// sprawdź, czy notatka istnieje, ale zignoruj identyfikator „note”
script.noteExistsByFileName("my note.md", note.id);
```

Może zechcesz rzucić okiem na przykład [use-tag-names-in-filename.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/use-tag-names-in-filename.qml).

## Copying text into the clipboard

### Wywołanie metody i parametry

```cpp
/**
 * Kopiuje tekst do schowka jako zwykły tekst lub dane mime html
 *
 * @param text string text to put into the clipboard
 * @param asHtml bool if true the text will be set as html mime data
 */
void ScriptingService::setClipboardText(QString text, bool asHtml);
```

### Przykład

```js
// skopiuj tekst do schowka
script.setClipboardText("text to copy");
```

Może zechcesz rzucić okiem na przykład [selected-markdown-to-bbcode.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/selected-markdown-to-bbcode.qml).

## Skoki do notatki

### Wywołanie metody i parametry

```cpp
/**
 * Sets the current note if the note is visible in the note list
 *
 * @param note NoteApi note to jump to
 * @param asTab bool if true the note will be opened in a new tab (if not already open)
 */
void ScriptingService::setCurrentNote(NoteApi *note, bool asTab = false);
```

### Przykład

```js
// jump to the note
script.setCurrentNote(note);

// open note in new tab (if not already open)
script.setCurrentNote(note, true);
```

You may want to take a look at the example [journal-entry.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/journal-entry.qml).

## Przeskakiwanie do podfolderu notatki

### Wywołanie metody i parametry

```cpp
/**
  * Przeskakuje do podfolderu notatki
  *
  * @param noteSubFolderPath {QString} ścieżka podfolderu względem folderu notatek
  * @param separator {QString} separator między częściami ścieżki, domyślnie "/"
  * @return true, jeśli skok się powiódł
  */
bool ScriptingService::jumpToNoteSubFolder(const QString &noteSubFolderPath,
                                             separator QString);
```

### Przykład

```js
// jump to the note subfolder "a sub folder"
script.jumpToNoteSubFolder("a sub folder");

// jump to the note subfolder "sub" inside of "a sub folder"
script.jumpToNoteSubFolder("a sub folder/sub");
```

::: tip
Możesz utworzyć nowy podfolder notatek w bieżącym podfolderze, dzwoniąc [`mainWindow.createNewNoteSubFolder`](classes.html#example-2).
:::

## Wyświetlam okno wiadomości informacyjnej

### Wywołanie metody i parametry

```cpp
/**
 * Wyświetla okno komunikatu informacyjnego
 *
 * @param text
 * @param title (optional)
 */
void ScriptingService::informationMessageBox(QString text, QString title);
```

### Przykład

```js
// show a information message box
script.informationMessageBox("The text I want to show", "Some optional title");
```

## Wyświetlanie okna wiadomości z pytaniem

### Wywołanie metody i parametry

```cpp
/**
 * Wyświetla okno wiadomości z pytaniem
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

### Przykład

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

Aby uzyskać informacje o przyciskach, zobacz [StandardButton](https://doc.qt.io/qt-5/qmessagebox.html#StandardButton-enum).

You may also want to take a look at the example [input-dialogs.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/input-dialogs.qml).

## Pokazywanie okna dialogowego z otwartym plikiem

### Wywołanie metody i parametry

```cpp
/**
 * Pokazuje okno dialogowe otwierania pliku
 *
 * @param caption (optional)
 * @param dir (optional)
 * @param filter (optional)
 * @return QString
 */
QString ScriptingService::getOpenFileName(QString caption, QString dir,
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

## Wyświetlam okno dialogowe zapisywania pliku

### Wywołanie metody i parametry

```cpp
/**
 * Pokazuje okno dialogowe zapisywania pliku
 *
 * @param caption (optional)
 * @param dir (optional)
 * @param filter (optional)
 * @return QString
 */
QString ScriptingService::getSaveFileName(QString caption, QString dir,
                                            QString filter);
```

### Przykład

```js
// show a save file dialog
var fileName = script.getSaveFileName(
  "Please select HTML file to save",
  "output.html",
  "HTML (*.html)",
);
```

You may want to take a look at the example [export-notes-as-one-html.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/export-notes-as-one-html.qml).

## Rejestrowanie zmiennych ustawień skryptu

Musisz zdefiniować zmienne ustawień jako właściwości w skrypcie i zarejestruj je we właściwości o nazwie `settingsVariables`.

Użytkownik może następnie ustawić te właściwości w ustawieniach skryptu.

### Przykład

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

### Przykład

```js
/**
 * Ponownie rejestruje zmienne ustawień
 *
 * Użyj tej metody, jeśli chcesz użyć kodu do zastąpienia zmiennych, takich jak ustawienie
 * wartości domyślne w zależności od systemu operacyjnego.
 */
function registerSettingsVariables() {
  if (script.platformIsWindows()) {
    // override the myFile default value
    settingsVariables[3].default = "pandoc.exe";
  }
}
```

You may also want to take a look at the example [variables.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/variables.qml).

## Przechowywanie i ładowanie trwałych zmiennych

### Wywołanie metody i parametry

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

### Przykład

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

Upewnij się, że używasz znaczącego prefiksu w swoim kluczu, takiego jak `PersistentVariablesTest/myVar`, ponieważ zmienne są dostępne ze wszystkich skryptów.

You may also want to take a look at the example [persistent-variables.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/persistent-variables.qml).

## Ładowanie zmiennych ustawień aplikacji

### Wywołanie metody i parametry

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

### Przykład

```js
// load and log an application settings variable
script.log(script.getApplicationSettingsVariable("gitExecutablePath"));
```

Pamiętaj, że ustawienia faktycznie mogą być puste, musisz uważać o tym sam. `defaultValue` jest używany tylko wtedy, gdy ustawienie w ogóle nie istnieje.

## Tworzenie katalogu pamięci podręcznej

Możesz buforować pliki w domyślnej lokalizacji pamięci podręcznej systemu.

### Wywołanie metody i parametry

```cpp
/**
 * Zwraca katalog pamięci podręcznej dla skryptu
 *
 * @param {QString} subDir the subfolder to create and use
 * @return {QString} the cache dir path
 */
QString ScriptingService::cacheDir(const QString &subDir) const;
```

### Przykład

```js
// create the cache directory for my-script-id
var cacheDirForScript = script.cacheDir("my-script-id");
```

## Czyszczenie katalogu pamięci podręcznej

Katalog pamięci podręcznej skryptu można wyczyścić, przekazując jego nazwę do metody clearCacheDir().

### Wywołanie metody i parametry

```cpp
/**
 * Clears the cache directory for a script
 *
 * @param {QString} subDir the subfolder to clear
 * @return {bool} true on success
 */
bool ScriptingService::clearCacheDir(const QString &subDir) const;
```

### Przykład

```js
// clear cache directory of my-script-id
script.clearCacheDir("my-script-id");
```

## Czytanie ścieżki do katalogu twojego skryptu

Jeśli chcesz uzyskać ścieżkę do katalogu, w którym znajduje się twój skrypt, aby na przykład załadować inne pliki, musisz się zarejestrować `property string scriptDirPath;`. Ta właściwość zostanie ustawiona ze ścieżką do katalogu skryptu.

### Przykład

```js
import QtQml 2.0
import QOwnNotesTypes 1.0

Script {
    // ścieżka do katalogu skryptu zostanie ustawiona tutaj
    property string scriptDirPath;

    function init() {
        script.log(scriptDirPath);
    }
}
```

## Konwersja separatorów ścieżek na natywne

### Wywołanie metody i parametry

```cpp
/**
  * Zwraca ścieżkę z separatorami „/” zamienionymi na separatory, które są
  * odpowiednie dla bazowego systemu operacyjnego.
 *
 * On Windows, toNativeDirSeparators("c:/winnt/system32") returns
 * "c:\winnt\system32".
 *
 * @param path
 * @return
 */
QString ScriptingService::toNativeDirSeparators(QString path);
```

### Przykład

```js
// will return "c:\winnt\system32" on Windows
script.log(script.toNativeDirSeparators("c:/winnt/system32"));
```

## Konwersja separatorów ścieżek z natywnych

### Wywołanie metody i parametry

```cpp
/**
  * Zwraca ścieżkę używając '/' jako separatora plików.
 * On Windows, for instance, fromNativeDirSeparators("c:\\winnt\\system32")
 * returns "c:/winnt/system32".
 *
 * @param path
 * @return
 */
QString ScriptingService::fromNativeDirSeparators(QString path);
```

### Przykład

```js
// will return "c:/winnt/system32" on Windows
script.log(script.fromNativeDirSeparators("c:\\winnt\\system32"));
```

## Pobieranie natywnego separatora katalogów

### Wywołanie metody i parametry

```cpp
/**
 * Zwraca natywny separator katalogu "/" lub "\" w systemie Windows
 *
 * @return
 */
QString ScriptingService::dirSeparator();
```

### Przykład

```js
// will return "\" on Windows
script.log(script.dirSeparator());
```

## Uzyskanie listy ścieżek wszystkich wybranych notatek

### Wywołanie metody i parametry

```cpp
/**
 * Zwraca listę ścieżek wszystkich wybranych notatek
 *
 * @return {QStringList} list of selected note paths
 */
QStringList ScriptingService::selectedNotesPaths();
```

### Przykład

```js
// returns a list of the paths of all selected notes
script.log(script.selectedNotesPaths());
```

You may want to take a look at the example [external-note-diff.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/external-note-diff.qml).

## Uzyskanie listy identyfikatorów wszystkich wybranych notatek

### Wywołanie metody i parametry

```cpp
/**
  * Zwraca listę identyfikatorów wszystkich wybranych notatek
  *
  * @return {QList<int>} lista wybranych identyfikatorów notatek
  */
QList<int> ScriptingService::selectedNotesIds();
```

### Przykład

```js
// zwraca listę identyfikatorów wszystkich wybranych notatek
script.log(script.selectedNotesIds());
```

You may want to take a look at the example [export-notes-as-one-html.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/export-notes-as-one-html.qml).

## Wywoływanie akcji menu

### Wywołanie metody i parametry

```cpp
/**
 * Uruchamia akcję menu
 *
 * @param objectName {QString} object name of the action to trigger
 * @param checked {QString} only trigger the action if checked-state is
 *                          different than this parameter (optional, can be 0 or 1)
 */
void ScriptingService::triggerMenuAction(QString objectName, QString checked);
```

### Przykład

```js
// przełącz tryb tylko do odczytu
script.triggerMenuAction("actionAllow_note_editing");

// wyłącz tryb tylko do odczytu
script.triggerMenuAction("actionAllow_note_editing", 1);
```

You may want to take a look at the example [disable-readonly-mode.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/disable-readonly-mode.qml).

::: tip
You can get the object names of the menu action from [mainwindow.ui](https://github.com/pbek/QOwnNotes/blob/main/src/mainwindow.ui). Po prostu wyszukaj tytuł menu w języku angielskim. Pamiętaj, że te teksty mogą się zmieniać w czasie.
:::

## Otwieranie okna dialogowego wprowadzania danych z polem wyboru

### Wywołanie metody i parametry

```cpp
/**
  * Otwiera okno dialogowe wprowadzania danych z polem wyboru
  *
  * @param title {QString} tytuł okna dialogowego
  * @param label {QString} tekst etykiety okna dialogowego
  * @param items {QStringList} lista elementów do wyboru
  * @param bieżący indeks {int} elementu, który należy wybrać (domyślnie: 0)
  * @param editable {bool} jeśli true tekst w oknie dialogowym można edytować (domyślnie: false)
  * @return {QString} tekst wybranego elementu
  */
QString ScriptingService::inputDialogGetItem(
         const QString &title, const QString &label, const QStringList &items,
         int bieżący, bool edytowalny);
```

An empty string will be returned, if `Cancel` was clicked or `Escape` was pressed.

### Przykład

```js
var result = script.inputDialogGetItem("combo box", "Please select an item", [
  "Item 1",
  "Item 2",
  "Item 3",
]);
script.log(result);
```

You may want to take a look at the example [input-dialogs.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/input-dialogs.qml).

## Otwieranie okna dialogowego wprowadzania danych z edycją linii

### Wywołanie metody i parametry

```cpp
/**
  * Otwiera okno dialogowe wprowadzania danych z edycją linii
  *
  * @param title {QString} tytuł okna dialogowego
  * @param label {QString} tekst etykiety okna dialogowego
  * @param text {QString} tekst w oknie dialogowym (opcjonalnie)
  * @zwrócić
  */
QString ScriptingService::inputDialogGetText(
         const QString i tytuł, const QString i etykieta, const QString i tekst);
```

An empty string will be returned, if `Cancel` was clicked or `Escape` was pressed.

### Przykład

```js
var result = script.inputDialogGetText(
  "line edit",
  "Please enter a name",
  "current text",
);
script.log(result);
```

## Opening an input dialog with a multi-line text edit

### Wywołanie metody i parametry

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

### Przykład

```js
var result = script.inputDialogGetMultiLineText(
  "multi-line edit",
  "Please enter a text",
  "current text",
);
script.log(result);
```

## Opening a dialog to show the differences between two texts

### Metoda wywołania i parametry

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

### Przykład

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

## Sprawdzanie, czy plik istnieje

### Method call and parameters

```cpp
/**
 * Sprawdź, czy plik istnieje
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

## Czytanie tekstu z pliku

### Method call and parameters

```cpp
/**
  * Czytaj tekst z pliku
  *
  * @param filePath {QString} ścieżka pliku do załadowania
  * Kodek @param codec {QString} (domyślnie: UTF-8)
  * @zwróć dane pliku lub null, jeśli plik nie istnieje
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

## Zapisywanie tekstu do pliku

### Method call and parameters

```cpp
/**
 * Zapisuje tekst do pliku
 *
 * @param filePath {QString}
 * @param data {QString}
 * @param createParentDirs {bool} optional (default: false)
 * @return
 */
bool ScriptingService::writeToFile(const QString &filePath, const QString &data, bool createParentDirs);
```

### Example

```js
var result = script.writeToFile(filePath, html);
script.log(result);
```

You may want to take a look at the example [export-notes-as-one-html.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/export-notes-as-one-html.qml).

## Zapisuje tekst do pliku

Możesz zdalnie sterować QOwnNotes za pomocą `WebSocketServer`.

Please take a look at the example [websocket-server.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/websocket-server.qml). Możesz przetestować serwer gniazd, łącząc się z nim w [Websocket test](https://www.websocket.org/echo.html?location=ws://127.0.0.1:35345).

Możesz także słuchać gniazd za pomocą `WebSocket`. Please take look at the example [websocket-client.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/websocket-client.qml).

Pamiętaj, że potrzebujesz biblioteki QML `websocket` Qt zainstalowany, aby z tego korzystać. Na przykład pod Ubuntu Linux możesz zainstalować `qml-module-qtwebsockets`.

## Dodawanie reguły wyróżniania dla edytora

Możesz bezpośrednio wstrzyknąć reguły podświetlania do edytora, definiując wyrażenia regularne i przypisując je do stanu podświetlania.

### Method call and parameters

```cpp
/**
 * Dodaje wyróżnioną regułę dla podświetlenia składni edytora
 *
 * @param pattern {QString} wyrażenie regularne do wyróżnienia
 * @param shouldContain {QString} ciąg, który musi być zawarty w podświetlonym tekście, aby wzorzec mógł być analizowany
 * @param state {int} stan podświetlania składni, który ma być używany
 * @param capturingGroup {int} grupa przechwytywania dla wzorca, która ma być używana do wyróżniania (domyślnie: 0)
 * @param maskedGroup {int} grupa przechwytująca dla wzorca do użycia maskowania (domyślnie: 0)
 */
void ScriptingService::addHighlightingRule(const QString &pattern,
                                           const QString &shouldContain, 
                                           int state, 
                                           int capturingGroup, 
                                           int maskedGroup);
```

### Wyróżnianie stanów

| Nazwa                      | Nr. |
| -------------------------- | --- |
| NoState                    | -1  |
| Łącze                      | 0   |
| Obraz                      | 3   |
| CodeBlock                  | 4   |
| CodeBlockComment           | 5   |
| Kursywa                    | 7   |
| Pogrubienie                | 8   |
| Lista                      | 9   |
| Komentarz                  | 11  |
| H1                         | 12  |
| H2                         | 13  |
| H3                         | 14  |
| H4                         | 15  |
| H5                         | 16  |
| H6                         | 17  |
| BlockQuote                 | 18  |
| HorizontalRule            | 21  |
| Tabela                     | 22  |
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
// Highlight a text line like "BLOCK: some text" as blockquote (state 18)
script.addHighlightingRule("^BLOCK: (.+)", "BLOCK:", 18);

// Mask out (state 24) all characters after 32 characters in a line
// capturingGroup 1 means the expression from the first bracketed part of the pattern will be highlighted
// maskedGroup -1 means that no masking should be done
script.addHighlightingRule("^.{32}(.+)", "", 24, 1, -1);
```

You can also take a look at the examples in [highlighting.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/highlighting.qml).
