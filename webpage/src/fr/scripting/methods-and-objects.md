# Méthodes et objets fournis par QOwnNotes

Lancer un programme externe en arrière-plan
----------------------------------------------


### Appel de méthode et paramètres
```cpp
/ **
  * Wrapper QML pour démarrer un processus détaché
  *
  * @param executablePath le chemin de l'exécutable
  * @param parameters une liste de chaînes de paramètres
  * @param callbackIdentifier un identifiant à utiliser dans la fonction onDetachedProcessCallback () (optionnel)
  * @param callbackParameter un paramètre supplémentaire pour les boucles ou autres (facultatif)
  * @param processData données écrites dans le processus si le rappel est utilisé (facultatif)
  * @return true en cas de succès, false sinon
  * /
bool startDetachedProcess (QString executablePath, paramètres QStringList,
                             QString callbackIdentifier, QVariant callbackParameter,
                             QByteArray processData);
```

### Exemple

Exemple simple:

```js
script.startDetachedProcess("/path/to/my/program", ["my parameter"]);
```

Exemple simple:

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

Vous voudrez peut-être jeter un coup d'œil à l'exemple [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml), [callback.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/callback.qml) ou [exécution-commande-après-note-update.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/execute-command-after-note-update.qml).

Vous pouvez également consulter le hook [onDetachedProcessCallback](hooks.html#ondetachedprocesscallback).

Lancer un programme externe et attendre la sortie
----------------------------------------------------

### Appel de méthode et paramètres
```cpp
/**
  * Wrapper QML pour démarrer un processus synchrone
  *
  * @param executablePath le chemin de l'exécutable
  * @param parameters une liste de chaînes de paramètres
  * @param data les données qui seront écrites dans le processus (facultatif)
  * @return le texte qui a été renvoyé par le processus
QByteArray startSynchronousProcess(QString executablePath, QStringList parameters, QByteArray data);
```

### Exemple
```js
var result = script.startSynchronousProcess("/path/to/my/program", ["my parameter"], "data");
```

You may want to take a look at the example [encryption-keybase.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/encryption-keybase.qml).

Obtenir le chemin du dossier de notes actuel
-------------------------------------------

### Appel de méthode et paramètres
```cpp
/**
  * Wrapper QML pour obtenir le chemin du dossier de notes actuel
  *
  * @return le chemin du dossier de notes actuel
  */
QString currentNoteFolderPath();
```

### Exemple
```js
var path = script.currentNoteFolderPath();
```

You may want to take a look at the example [absolute-media-links.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/absolute-media-links.qml).

Obtenir la note actuelle
------------------------

### Appel de méthode et paramètres
```cpp
/**
  * Wrapper QML pour obtenir la note actuelle
  *
  * @returns {NoteApi} l'objet de note actuel
  */
NoteApi currentNote ();
```

### Exemple
```js
var note = script.currentNote();
```

You may want to take a look at the example [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml).

Connexion au widget de journal
-------------------------

### Appel de méthode et paramètres
```cpp
/**
 * Wrapper QML pour se connecter au widget de journal
 *
 * @param text
 */
void log(QString text);
```

### Exemple
```js
script.log("my text");
```

Téléchargement d'une URL dans une chaîne
------------------------------

### Appel de méthode et paramètres
```cpp
/**
  * Wrapper QML pour télécharger une URL et la renvoyer sous forme de texte
  *
  * @param url
  * @return {QString} le contenu de l'url téléchargée
  */
QString downloadUrlToString(QUrl url);
```

### Exemple
```js
var html = script.downloadUrlToString("https://www.qownnotes.org");
```

You may want to take a look at the example [insert-headline-with-link-from-github-url.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/insert-headline-with-link-from-github-url.qml).

Téléchargement d'une URL dans le dossier multimédia
--------------------------------------

### Appel de méthode et paramètres
```cpp
/**
  * Wrapper QML pour télécharger une URL dans le dossier multimédia et renvoyer le support
  * url ou le texte de l'image de démarque du média par rapport à la note actuelle
  *
  * @param {QString} url
  * @param {bool} returnUrlOnly si true, seule l'url du média sera renvoyée (par défaut false)
  * @return {QString} le markdown ou l'URL du média
  */
QString downloadUrlToMedia (QUrl url, booléen returnUrlOnly);
```

### Exemple
```js
var markdown = script.downloadUrlToMedia("http://latex.codecogs.com/gif.latex?\frac{1}{1+sin(x)}");
```

You may want to take a look at the example [paste-latex-image.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/paste-latex-image.qml).

Insertion d'un fichier multimédia dans le dossier multimédia
--------------------------------------------

### Appel de méthode et paramètres
```cpp
/**
  * Wrapper QML pour insérer un fichier multimédia dans le dossier multimédia et retourner
  * l'url du média ou le texte de l'image de démarque du média par rapport à la note actuelle
  *
  * @param {QString} mediaFilePath
  * @param {bool} returnUrlOnly si true, seule l'url du média sera retournée (par défaut false)
  * @return {QString} le markdown ou l'url du média
  */
QString ScriptingService::insertMediaFile (QString mediaFilePath,
                                         booléen returnUrlOnly);
```

### Exemple
```js
var markdown = script.insertMediaFile("/path/to/your/image.png");
```

You may want to take a look at the example [scribble.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/scribble.qml).

Inserting an attachment file into the attachments folder
--------------------------------------------------------

### Appel de méthode et paramètres
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

### Exemple
```js
var markdown = script.insertAttachmentFile("/path/to/your/file.png");
```

Regenerating the note preview
-----------------------------

Refreshes the note preview.

### Appel de méthode et paramètres
```cpp
/**
 * Regenerates the note preview
 */
QString ScriptingService::regenerateNotePreview();
```

### Exemple
```js
script.regenerateNotePreview();
```

You may want to take a look at the example [scribble.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/scribble.qml).

Registering a custom action
---------------------------

### Appel de méthode et paramètres
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

::: tip
You can also assign local and global shortcuts to your custom actions in the *Shortcuts settings*.
:::

### Exemple

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

For some more examples please see [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml).

::: tip
Vous pouvez également déclencher une action personnalisée après le démarrage de l'application avec le paramètre `--action customAction_<identifier>`. Pour plus d'informations, veuillez consulter [Déclencher des actions de menu après le démarrage](../getting-started/cli-parameters.md#trigger-menu-actions-after-startup).
:::

Registering a label
-------------------

### Appel de méthode et paramètres
```cpp
/**
 * Registers a label to write to
 *
 * @param identifier the identifier of the label
 * @param text the text shown in the label (optional)
 */
void ScriptingService::registerLabel(QString identifier, QString text);
```

### Exemple
```js
script.registerLabel("html-label", "<strong>Strong</strong> HTML text<br />with three lines<br />and a <a href='https://www.qownnotes.org'>link to a website</a>.");

script.registerLabel("long-label", "another very long text, another very long text, another very long text, another very long text, another very long text, another very long text, another very long text, another very long text, another very long text, another very long text, another very long text that will wrap");

script.registerLabel("counter-label");
```

Les étiquettes seront visibles dans le widget du dock de script.

Vous pouvez utiliser à la fois du texte brut ou du html dans les étiquettes. Le texte sera sélectionnable et les liens pourront être cliqués.

Vous pouvez ensuite jeter un œil à l'exemple de script [scripting-label-demo.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/scripting-label-demo.qml).

Setting the text of a registered label
--------------------------------------

### Appel de méthode et paramètres
```cpp
/**
 * Sets the text of a registered label
 *
 * @param identifier the identifier of the label
 * @param text the text shown in the label
 */
void ScriptingService::setLabelText(QString identifier, QString text);
```

### Exemple
```js
script.setLabelText("counter-label", "counter text");
```

Vous pouvez utiliser à la fois du texte brut ou du html dans les étiquettes. Le texte sera sélectionnable et les liens pourront être cliqués.

Vous pouvez ensuite jeter un œil à l'exemple de script [scripting-label-demo.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/scripting-label-demo.qml).

Creating a new note
-------------------

### Appel de méthode et paramètres
```cpp
/**
 * Creates a new note
 *
 * @param text the note text
 */
void ScriptingService::createNote(QString text);
```

### Exemple
```js
script.createNote("My note headline\n===\n\nMy text");
```

Vous voudrez peut-être jeter un coup d'œil à l'exemple [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml).

::: tip
Si vous avez désactivé que le titre de votre note détermine le nom de fichier de la note, vous devez renommer votre fichier de note vous-même par la suite, comme ceci:

```js
var note = script.currentNote();
note.renameNoteFile('your-filename');
```
:::

Accessing the clipboard
-----------------------

### Appel de méthode et paramètres
```cpp
/**
 * Returns the content of the clipboard as text or html
 *
 * @param asHtml returns the clipboard content as html instead of text
 */
QString ScriptingService::clipboard(bool asHtml);
```

### Exemple
```js
var clipboardText = script.clipboard();
var clipboardHtml = script.clipboard(true);
```

Vous pouvez jeter un œil à l'exemple [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml).

Write text to the note text edit
--------------------------------

### Appel de méthode et paramètres
```cpp
/**
 * Writes text to the current cursor position in the note text edit
 *
 * @param text
 */
void ScriptingService::noteTextEditWrite(QString text);
```

### Exemple
```js
// write text to the note text edit
script.noteTextEditWrite("My custom text");
```

Vous voudrez peut-être regarder l'action personnalisée `transformTextRot13` dans le exemple [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml).

Vous pouvez l'utiliser avec `noteTextEditSelectAll` pour écraser tout le texte de la note actuelle.

Read the selected text in the note text edit
--------------------------------------------

### Appel de méthode et paramètres
```cpp
/**
 * Reads the selected text in the note text edit
 *
 * @return
 */
QString ScriptingService::noteTextEditSelectedText();
```

### Exemple
```js
// read the selected text from the note text edit
var text = script.noteTextEditSelectedText();
```

Vous voudrez peut-être regarder l'action personnalisée `transformTextRot13` dans le exemple [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml).

Select all text in the note text edit
-------------------------------------

### Appel de méthode et paramètres
```cpp
/**
 * Selects all text in the note text edit
 */
void ScriptingService::noteTextEditSelectAll();
```

### Exemple
```js
script.noteTextEditSelectAll();
```

Vous pouvez l'utiliser avec `noteTextEditWrite` pour écraser tout le texte de la note actuelle.

Select the current line in the note text edit
---------------------------------------------

### Appel de méthode et paramètres
```cpp
/**
 * Selects the current line in the note text edit
 */
void ScriptingService::noteTextEditSelectCurrentLine();
```

### Exemple
```js
script.noteTextEditSelectCurrentLine();
```

Select the current word in the note text edit
---------------------------------------------

### Appel de méthode et paramètres
```cpp
/**
 * Selects the current line in the note text edit
 */
void ScriptingService::noteTextEditSelectCurrentWord();
```

### Exemple
```js
script.noteTextEditSelectCurrentWord();
```

Set the currently selected text in the note text edit
-----------------------------------------------------

### Appel de méthode et paramètres
```cpp
/**
 * Sets the currently selected text in the note text edit
 *
 * @param start
 * @param end
 */
void ScriptingService::noteTextEditSetSelection(int start, int end);
```

### Exemple
```js
// expands the current selection by one character
script.noteTextEditSetSelection(
    script.noteTextEditSelectionStart() - 1,
    script.noteTextEditSelectionEnd() + 1);
```

Get the start position of the current selection in the note text edit
---------------------------------------------------------------------

### Appel de méthode et paramètres
```cpp
/**
 * Returns the start position of the current selection in the note text edit
 */
int ScriptingService::noteTextEditSelectionStart();
```

### Exemple
```js
script.log(script.noteTextEditSelectionStart());
```

Get the end position of the current selection in the note text edit
-------------------------------------------------------------------

### Appel de méthode et paramètres
```cpp
/**
 * Returns the end position of the current selection in the note text edit
 */
int ScriptingService::noteTextEditSelectionEnd();
```

### Exemple
```js
script.log(script.noteTextEditSelectionEnd());
```

Set the text cursor in the note text edit to a certain position
---------------------------------------------------------------

### Appel de méthode et paramètres
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

### Exemple
```js
// jump to the 11th character in the note
script.noteTextEditSetCursorPosition(10);

// jump to the end of the note
script.noteTextEditSetCursorPosition(-1);
```

Get the current position of the text cursor in the note text edit
-----------------------------------------------------------------

### Appel de méthode et paramètres
```cpp
/**
 * Returns the current position of the text cursor in the note text edit
 * 0 would be the beginning of the note
 */
int ScriptingService::noteTextEditCursorPosition();
```

### Exemple
```js
script.log(script.noteTextEditCursorPosition());
```

Read the current word from the note text edit
---------------------------------------------

### Appel de méthode et paramètres
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

### Exemple
```js
// read the current word in the note text edit
var text = script.noteTextEditCurrentWord();
```

Vous pouvez jeter un œil à l'exemple [autocompletion.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/autocompletion.qml).

Check whether platform is Linux, OS X or Windows
------------------------------------------------

### Appel de méthode et paramètres
```cpp
bool ScriptingService::platformIsLinux();
bool ScriptingService::platformIsOSX();
bool ScriptingService::platformIsWindows();
```

### Exemple
```js
if (script.platformIsLinux()) {
    // only will be executed if under Linux
}
```

Marquer la note actuelle
--------------------

### Appel de méthode et paramètres
```cpp
/**
 * Tags the current note with a tag named tagName
 *
 * @param tagName
 */
void ScriptingService::tagCurrentNote(QString tagName);
```

### Exemple
```js
// add a "favorite" tag to the current note
script.tagCurrentNote("favorite");
```

Vous souhaiterez peut-être consulter l'action personnalisée `favoriteNote` dans l'exemple [favorite-note.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/favorite-note.qml).

Créer ou récupérer une balise par son nom liste de fil d'Ariane
-------------------------------------------------

### Appel de méthode et paramètres
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

### Exemple
```js
// creates all tags until the 3rd level and returns the tag object for
// tag "level3", which would look like that in the tag tree:
// level1 > level2 > level3
var tag = script.getTagByNameBreadcrumbList(["level1", "level2", "level3"]);
```

Rechercher des tags par nom
-----------------------

### Appel de méthode et paramètres
```cpp
/**
 * Fetches all tags by doing a substring search on the name field
 *
 * @param name {QString} name to search for
 * @return {QStringList} list of tag names
 */
QStringList ScriptingService::searchTagsByName(QString name);
```

### Exemple
```js
// searches for all tags with the word game in it
var tags = script.searchTagsByName("game");
```

Vous pouvez jeter un œil à l'exemple [autocompletion.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/autocompletion.qml).

Rechercher des notes par texte de note
-----------------------------

### Appel de méthode et paramètres
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

### Exemple
```js
var noteIds = script.fetchNoteIdsByNoteTextPart("mytext");

noteIds.forEach(function (noteId){
    var note = script.fetchNoteById(noteId);

    // do something with the note
});
```

Vous pouvez jeter un œil à l'exemple [unique-note-id.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/unique-note-id.qml).

Ajouter une feuille de style personnalisée
-----------------------

### Appel de méthode et paramètres
```cpp
/**
 * Adds a custom stylesheet to the application
 *
 * @param stylesheet
 */
void ScriptingService::addStyleSheet(QString stylesheet);
```

### Exemple
```js
// make the text in the note list bigger
script.addStyleSheet("QTreeWidget#noteTreeWidget {font-size: 30px;}");
```

Vous voudrez peut-être jeter un coup d'œil à l'exemple [custom-stylesheet.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-stylesheet.qml).

You can get the widget names from the `*.ui` files, for example the main window is [mainwindow.ui](https://github.com/pbek/QOwnNotes/blob/develop/src/mainwindow.ui).

La documentation Qt (par exemple [QMainWindow](https://doc.qt.io/qt-5/qmainwindow.html)) peut vous aider à voir comment les widgets sont liés les uns aux autres (recherchez `Inherits` sur les pages).

Le widget de base pour presque tout est [QWidget](https://doc.qt.io/qt-5/qwidget.html). Donc juste styliser `QWidget` avec par exemple `QWidget {background-color: black; color: white;}` signifierait que tout a une couleur de fond noire et une couleur de premier plan blanche.

::: tip
The [style.qss](https://github.com/pbek/QOwnNotes/blob/develop/src/libraries/qdarkstyle/style.qss) of [qdarkstyle](https://github.com/pbek/QOwnNotes/blob/develop/src/libraries/qdarkstyle) might also be a good reference for styles you can change.
:::

Take a look at [Style Sheet Reference](http://doc.qt.io/qt-5/stylesheet-reference.html) for a reference of what styles are available.

If you want to inject styles into html preview to alter the way notes are previewed please look at [notetomarkdownhtmlhook](hooks.html#notetomarkdownhtmlhook).

::: tip
If you actually want to see how the dialogs look and what the names are you could download [Qt Creator](https://www.qt.io/product/development-tools) and open the `*.ui` files in it.
:::

Recharger le moteur de script
------------------------------

### Appel de méthode et paramètres
```cpp
/**
  * Recharge le moteur de script
  */
void ScriptingService :: reloadScriptingEngine ();
```

### Exemple
```js
// recharger le moteur de script
script.reloadScriptingEngine ();
```

Récupérer une note par son nom de fichier
--------------------------------

### Appel de méthode et paramètres
```cpp
/**
  * Récupère une note par son nom de fichier
  *
  * @param fileName string le nom de fichier de la note (obligatoire)
  * @param noteSubFolderId ID entier du sous-dossier de notes
  * @return NoteApi *
  */
NoteApi * ScriptingService :: fetchNoteByFileName (QString fileName,
                                                 int noteSubFolderId);
```

### Exemple
```js
// récupère la note par nom de fichier
script.fetchNoteByFileName ("ma note.md");
```

Récupérer une note par son identifiant
-------------------------

### Appel de méthode et paramètres
```cpp
/**
  * Récupère une note par son identifiant
  *
  * @param id int l'identifiant de la note
  * @return NoteApi *
  */
NoteApi* ScriptingService::fetchNoteById(int id);
```

### Exemple
```js
// récupère la note par identifiant
script.fetchNoteById (243);
```

Vous voudrez peut-être jeter un coup d'œil à l'exemple [export-notes-as-one-html.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/export-notes-as-one-html.qml).

Vérifier si une note existe par son nom de fichier
------------------------------------------

### Appel de méthode et paramètres
```cpp
/**
  * Vérifie si un fichier de note existe par son nom de fichier
  *
  * @param fileName string le nom de fichier de la note (obligatoire)
  * @param ignoreNoteId identifiant entier d'une note à ignorer lors de la vérification
  * @param noteSubFolderId ID entier du sous-dossier de notes
  * @return booléen
  */
booléen ScriptingService :: noteExistsByFileName (QString fileName,
                                             int ignoreNoteId,
                                             int noteSubFolderId);
```

### Exemple
```js
// vérifie si la note existe, mais ignore l'id de "note"
script.noteExistsByFileName ("ma note.md", note.id);
```

Vous voudrez peut-être jeter un coup d'œil à l'exemple [use-tag-names-in-filename.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/use-tag-names-in-filename.qml).

Copier du texte dans le presse-papiers
-------------------------------

### Appel de méthode et paramètres
```cpp
/**
  * Copie le texte dans le presse-papiers sous forme de texte brut ou de données mime html
  *
  * @param text string texte à mettre dans le presse-papiers
  * @param asHtml bool si true, le texte sera défini en tant que données mime html
  */
void ScriptingService::setClipboardText(QString text, bool asHtml);
```

### Exemple
```js
// copie du texte dans le presse-papiers
script.setClipboardText ("texte à copier");
```

Vous voudrez peut-être jeter un coup d'œil à l'exemple [selected-markdown-to-bbcode.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/selected-markdown-to-bbcode.qml).

Passer à une note
-----------------

### Appel de méthode et paramètres
```cpp
/**
  * Définit la note actuelle si la note est visible dans la liste des notes
  *
  * @param note NoteApi note pour passer à
  */
void ScriptingService::setCurrentNote(NoteApi *note);
```

### Exemple
```js
// sauter à la note
script.setCurrentNote (note);
```

Vous voudrez peut-être jeter un coup d'œil à l'exemple [journal-entry.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/journal-entry.qml).

Accéder à un sous-dossier de notes
---------------------------

### Appel de méthode et paramètres
```cpp
/**
  * Saute dans un sous-dossier de notes
  *
  * @param noteSubFolderPath {QString} chemin du sous-dossier, relatif au dossier de notes
  * @param separator {QString} séparateur entre les parties du chemin, par défaut "/"
  * @return true si le saut a réussi
  */
bool ScriptingService::jumpToNoteSubFolder(const QString &noteSubFolderPath,
                                            QString separator);
```

### Exemple
```js
// passe au sous-dossier de notes "un sous-dossier"
script.jumpToNoteSubFolder("a sub folder");

// passe au sous-dossier de notes "sub" à l'intérieur de "un sous-dossier"
script.jumpToNoteSubFolder("a sub folder/sub");
```

::: tip
Vous pouvez créer un nouveau sous-dossier de notes dans le sous-dossier actuel en appelant [`mainWindow.createNewNoteSubFolder`](classes.html#example-2).
:::

Affichage d'une boîte de message d'information
----------------------------------

### Appel de méthode et paramètres
```cpp
/**
  * Affiche une boîte de message d'information
  *
  * @param text
  * @param title (optional)
  */
void ScriptingService::informationMessageBox(QString text, QString title);
```

### Exemple
```js
// affiche une boîte de message d'information
script.informationMessageBox ("Le texte que je veux afficher", "Un titre facultatif");
```

Affichage d'une boîte de message de question
------------------------------

### Appel de méthode et paramètres
```cpp
/**
  * Affiche une boîte de message de question
  *
  * Pour plus d'informations sur les boutons, voir:
  * https://doc.qt.io/qt-5/qmessagebox.html#StandardButton-enum
  *
  * @param text
  * @param title (facultatif)
  * Boutons @param boutons qui doivent être affichés (facultatif)
  * @param defaultButton bouton par défaut qui sera sélectionné (facultatif)
  * @return id du bouton enfoncé
  */
int ScriptingService :: questionMessageBox (
         Texte QString, titre QString, boutons int, int defaultButton);
```

### Exemple
```js
// affiche une boîte de message de question avec un bouton d'application et un bouton d'aide
// voir: https://doc.qt.io/qt-5/qmessagebox.html#StandardButton-enum
var result = script.questionMessageBox (
     "Le texte que je veux afficher", "Un titre facultatif", 0x01000000 | 0x02000000, 0x02000000);
script.log (résultat);
```

Pour plus d'informations sur les boutons, voir [StandardButton](https://doc.qt.io/qt-5/qmessagebox.html#StandardButton-enum).

Vous pouvez également jeter un œil à l'exemple [input-dialogs.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/input-dialogs.qml).

Affichage d'une boîte de dialogue de fichier ouvert
---------------------------

### Appel de méthode et paramètres
```cpp
/**
  * Affiche une boîte de dialogue de fichier ouvert
  *
  * @param caption (facultatif)
  * @param dir (facultatif)
  * @param filter (facultatif)
  * @return QString
  */
QString ScriptingService :: getOpenFileName (légende QString, répertoire QString,
                                             Filtre QString);
```

### Exemple
```js
// affiche une boîte de dialogue de fichier ouvert
var fileName = script.getOpenFileName("Please select an image", "/home/user/images", "Images (*.png *.xpm *.jpg)");
```

Affichage d'une boîte de dialogue d'enregistrement de fichier
--------------------------

### Appel de méthode et paramètres
```cpp
/**
  * Affiche une boîte de dialogue d'enregistrement de fichier
  *
  * @param caption (facultatif)
  * @param dir (facultatif)
  * @param filter (facultatif)
  * @return QString
  */
QString ScriptingService::getSaveFileName (légende QString, répertoire QString,
                                             Filtre QString);
```

### Exemple
```js
// affiche une boîte de dialogue de sauvegarde de fichier
var fileName = script.getSaveFileName ("Veuillez sélectionner le fichier HTML à enregistrer", "output.html", "HTML (*.html)");
```

You may want to take a look at the example [export-notes-as-one-html.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/export-notes-as-one-html.qml).

Registering script settings variables
-------------------------------------

You need to define your settings variables as properties in your script and register them in a property named `settingsVariables`.

The user can then set these properties in the script settings.

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

In addition you can override the `settingsVariables` with a special function `registerSettingsVariables()` like this:

### Exemple
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

Storing and loading persistent variables
----------------------------------------

### Appel de méthode et paramètres
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

### Exemple
```js
// store persistent variable
script.setPersistentVariable("PersistentVariablesTest/myVar", result);

// load and log persistent variable
script.log(script.getPersistentVariable("PersistentVariablesTest/myVar", "nothing here yet"));
```

Please make sure to use a meaningful prefix in your key like `PersistentVariablesTest/myVar` because the variables are accessible from all scripts.

You may also want to take a look at the example [persistent-variables.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/persistent-variables.qml).

Loading application settings variables
--------------------------------------

### Appel de méthode et paramètres
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

### Exemple
```js
// load and log an application settings variable
script.log(script.getApplicationSettingsVariable("gitExecutablePath"));
```

Keep in mind that settings actually can be empty, you have to take care about that yourself. `defaultValue` is only used if the setting doesn't exist at all.

Creating a cache directory
--------------------------

You can cache files at the default cache location of your system.

### Appel de méthode et paramètres
```cpp
/**
 * Returns a cache directory for a script
 *
 * @param {QString} subDir the subfolder to create and use
 * @return {QString} the cache dir path
 */
QString ScriptingService::cacheDir(const QString &subDir) const;
```

### Exemple
```js
// create the cache directory for my-script-id
var cacheDirForScript = script.cacheDir("my-script-id");
```

Clearing a cache directory
--------------------------

You can clear the cache files of your script by passing its name to clearCacheDir().

### Method call and parameters
```cpp
/**
 * Clears the cache directory for a script
 *
 * @param {QString} subDir the subfolder to clear
 * @return {bool} true on success
 */
bool ScriptingService::clearCacheDir(const QString &subDir) const;
```

### Example
```js
// clear cache directory of my-script-id 
script.clearCacheDir("my-script-id");
```

Reading the path to the directory of your script
------------------------------------------------

If you need to get the path to the directory where your script is placed to for example load other files you have to register a `property string scriptDirPath;`. This property will be set with the path to the script's directory.

### Exemple
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

Conversion des séparateurs de chemin en séparateurs natifs
-----------------------------------------

### Appel de méthode et paramètres
```cpp
/**
  * Renvoie le chemin avec les séparateurs '/' convertis en séparateurs
  * approprié pour le système d'exploitation sous-jacent.
 *
 * On Windows, toNativeDirSeparators("c:/winnt/system32") returns
 * "c:\winnt\system32".
 *
 * @param path
 * @return
 */
QString ScriptingService::toNativeDirSeparators(QString path);
```

### Exemple
```js
// will return "c:\winnt\system32" on Windows
script.log(script.toNativeDirSeparators("c:/winnt/system32"));
```

Conversion des séparateurs de chemin des séparateurs natifs
-------------------------------------------

### Appel de méthode et paramètres
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

### Exemple
```js
// will return "c:/winnt/system32" on Windows
script.log(script.fromNativeDirSeparators("c:\\winnt\\system32"));
```

Obtenir le séparateur de répertoire natif
--------------------------------------

### Appel de méthode et paramètres
```cpp
/**
 * Returns the native directory separator "/" or "\" on Windows
 *
 * @return
 */
QString ScriptingService::dirSeparator();
```

### Exemple
```js
// will return "\" on Windows
script.log(script.dirSeparator());
```

Obtenir une liste des chemins de toutes les notes sélectionnées
-------------------------------------------------

### Appel de méthode et paramètres
```cpp
/**
 * Returns a list of the paths of all selected notes
 *
 * @return {QStringList} list of selected note paths
 */
QStringList ScriptingService::selectedNotesPaths();
```

### Exemple
```js
// returns a list of the paths of all selected notes
script.log(script.selectedNotesPaths());
```

Vous voudrez peut-être jeter un coup d'œil à l'exemple [note-externe-diff.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/external-note-diff.qml).

Obtenir une liste des identifiants de toutes les notes sélectionnées
-----------------------------------------------

### Appel de méthode et paramètres
```cpp
/**
 * Returns a list of the ids of all selected notes
 *
 * @return {QList<int>} list of selected note ids
 */
QList<int> ScriptingService::selectedNotesIds();
```

### Exemple
```js
// returns a list of the ids of all selected notes
script.log(script.selectedNotesIds());
```

Vous voudrez peut-être jeter un coup d'œil à l'exemple [export-notes-as-one-html.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/export-notes-as-one-html.qml).

Déclencher une action de menu
------------------------

### Appel de méthode et paramètres
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

### Exemple
```js
// toggle the read-only mode
script.triggerMenuAction("actionAllow_note_editing");

// disable the read-only mode
script.triggerMenuAction("actionAllow_note_editing", 1);
```

Vous voudrez peut-être jeter un coup d'œil à l'exemple [disable-readonly-mode.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/disable-readonly-mode.qml).

::: tip
Vous pouvez obtenir les noms d'objet de l'action de menu à partir de [mainwindow.ui](https://github.com/pbek/QOwnNotes/blob/develop/src/mainwindow.ui). Recherchez simplement le titre du menu en anglais. Notez que ces textes peuvent changer avec le temps.
:::

Ouverture d'une boîte de dialogue de saisie avec une boîte de sélection
-----------------------------------------

### Appel de méthode et paramètres
```cpp
/**
  * Ouvre une boîte de dialogue de saisie avec une boîte de sélection
  *
  * @param title {QString} titre de la boîte de dialogue
  * @param label {QString} texte de l'étiquette de la boîte de dialogue
  * @param items {QStringList} liste des éléments à sélectionner
  * @param current {int} index de l'élément à sélectionner (par défaut: 0)
  * @param editable {bool} si true, le texte de la boîte de dialogue peut être édité (par défaut: false)
  * @return {QString} texte de l'élément sélectionné
  */
QString ScriptingService :: inputDialogGetItem (
         const QString & amp; title, const QString & amp; label, const QStringList & amp; items,
         int courant, booléen modifiable);
```

### Exemple
```js
var result = script.inputDialogGetItem(
    "combo box", "Please select an item", ["Item 1", "Item 2", "Item 3"]);
script.log(result);
```

Vous voudrez peut-être jeter un coup d'œil à l'exemple [input-dialogs.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/input-dialogs.qml).

Ouverture d'une boîte de dialogue de saisie avec une modification de ligne
----------------------------------------

### Appel de méthode et paramètres
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

### Exemple
```js
var result = script.inputDialogGetText(
    "line edit", "Please enter a name", "current text");
script.log(result);
```

Vérifier si un fichier existe
-------------------------

### Appel de méthode et paramètres
```cpp
/**
 * Check if a file exists
 * @param filePath
 * @return
 */
bool ScriptingService::fileExists(QString &filePath);
```

### Exemple
```js
var result = script.fileExists(filePath);
script.log(result);
```

Lire du texte à partir d'un fichier
------------------------

### Appel de méthode et paramètres
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

### Exemple
```js
if(script.fileExists(filePath)){
    var data = script.readFromFile(filePath);
    script.log(data);
}
```


Écrire du texte dans un fichier
----------------------

### Appel de méthode et paramètres
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

### Exemple
```js
var result = script.writeToFile(filePath, html);
script.log(result);
```

Vous voudrez peut-être jeter un coup d'œil à l'exemple [export-notes-as-one-html.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/export-notes-as-one-html.qml).

Travailler avec des Websockets
-----------------------

Vous pouvez contrôler à distance QOwnNotes en utilisant `WebSocketServer`.

Veuillez jeter un œil à l'exemple [websocket-server.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/websocket-server.qml). Vous pouvez tester le serveur socket en vous y connectant sur [Websocket test](https://www.websocket.org/echo.html?location=ws://127.0.0.1:35345).

Vous pouvez également écouter les sockets avec `WebSocket`. Please take look at the example [websocket-client.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/websocket-client.qml).

Keep in mind that you need to have Qt's QML `websocket` library installed to use this. For example under Ubuntu Linux you can install `qml-module-qtwebsockets`.
