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

::: tip
Vous pouvez également attribuer des raccourcis locaux et globaux à vos actions personnalisées dans les *Paramètres des raccourcis*.
:::

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

Vous pouvez jeter un œil à l'exemple [encryption-keybase.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/encryption-keybase.qml).

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

Vous voudrez peut-être jeter un œil à l'exemple [absolute-media-links.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/absolute-media-links.qml).

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

Vous pouvez jeter un œil à l'exemple [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml).

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

Vous pouvez jeter un œil à l'exemple [insert-headline-with-link-from-github-url.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/insert-headline-with-link-from-github-url.qml).

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

Vous pouvez jeter un œil à l'exemple [paste-latex-image.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/paste-latex-image.qml).

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

Vous pouvez jeter un œil à l'exemple [scribble.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/scribble.qml).

Régénérer l'aperçu de la note
-----------------------------

Actualise l'aperçu de la note.

### Appel de méthode et paramètres
```cpp
/**
  * Régénère l'aperçu de la note
  */
QString ScriptingService::regenerateNotePreview();
```

### Exemple
```js
script.regenerateNotePreview();
```

Vous pouvez jeter un œil à l'exemple [scribble.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/scribble.qml).

Enregistrer une action personnalisée
---------------------------

### Appel de méthode et paramètres
```cpp
/**
 * Enregistre une action personnalisée
 *
 * @ identifiant du paramètre l'identifiant de l'action
 * @ menu des paramètres Texte le texte affiché dans le menu
 * Bouton de paramètre @ Texte le texte affiché dans le bouton
 * (aucun bouton ne sera affiché s'il est vide)
 * @param icon le chemin du fichier d'icône ou le nom d'une icône de thème freedesktop
 * vous trouverez une liste d'icônes ici:
 * https://specifications.freedesktop.org/icon-naming-spec/icon-naming-spec-latest.html
 * @param useInNoteEditContextMenu si true, utilisez l'action dans la note edit
 * menu contextuel (par défaut: faux)
 * @param hideButtonInToolbar si true, le bouton ne sera pas affiché dans le
 * barre d'outils d'action personnalisée (par défaut: faux)
 * @param useInNoteListContextMenu si true, utilisez l'action dans la liste de notes
 * menu contextuel (par défaut: faux)
 */
void ScriptingService :: registerCustomAction (identifiant QString,
                                            QString menuText,
                                            QString buttonText,
                                            Icône QString,
                                            booléen useInNoteEditContextMenu,
                                            booléen hideButtonInToolbar,
                                            booléen useInNoteListContextMenu);
```

### Exemple
```js
// ajoute une action personnalisée sans bouton
script.registerCustomAction ("mycustomaction1", "Texte du menu");

// ajoute une action personnalisée avec un bouton
script.registerCustomAction ("mycustomaction1", "Menu text", "Button text");

// ajouter une action personnalisée avec un bouton et une icône de thème freedesktop
script.registerCustomAction ("mycustomaction1", "Menu text", "Button text", "task-new");

// ajoute une action personnalisée avec un bouton et une icône à partir d'un fichier
script.registerCustomAction ("mycustomaction1", "Menu text", "Button text", "/usr/share/icons/breeze/actions/24/view-calendar-tasks.svg");
```

Vous pouvez ensuite utiliser l'identifiant avec la fonction `customActionInvoked` dans un script comme [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml).

Enregistrer une étiquette
-------------------

### Appel de méthode et paramètres
```cpp
/**
  * Enregistre une étiquette pour écrire
  *
  * @param identifiant l'identifiant du label
  * @param text le texte affiché dans l'étiquette (facultatif)
  */
void ScriptingService :: registerLabel (identifiant QString, texte QString);
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

Définition du texte d'une étiquette enregistrée
--------------------------------------

### Appel de méthode et paramètres
```cpp
/**
  * Définit le texte d'une étiquette enregistrée
  *
  * @param identifiant l'identifiant du label
  * @param text le texte affiché dans l'étiquette
  */
void ScriptingService :: setLabelText (identificateur QString, texte QString);
```

### Exemple
```js
script.setLabelText("counter-label", "counter text");
```

Vous pouvez utiliser à la fois du texte brut ou du html dans les étiquettes. Le texte sera sélectionnable et les liens pourront être cliqués.

Vous pouvez ensuite jeter un œil à l'exemple de script [scripting-label-demo.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/scripting-label-demo.qml).

Créer une nouvelle note
-------------------

### Appel de méthode et paramètres
```cpp
/**
  * Crée une nouvelle note
  *
  * @param text le texte de la note
  */
void ScriptingService::createNote(QString text);
```

### Example
```js
script.createNote("My note headline\n===\n\nMy text");
```

Vous pouvez jeter un œil à l'exemple [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml).

::: tip
Si vous avez désactivé que le titre de votre note détermine le nom de fichier de la note, vous devez renommer votre fichier de note vous-même par la suite, comme ceci:

```js
var note = script.currentNote();
note.renameNoteFile('your-filename');
```
:::

Accéder au presse-papiers
-----------------------

### Appel de méthode et paramètres
```cpp
/**
  * Renvoie le contenu du presse-papiers sous forme de texte ou html
  *
  * @param asHtml renvoie le contenu du presse-papiers au format HTML au lieu de texte
  */
QString ScriptingService :: clipboard (booléen asHtml);
```

### Exemple
```js
var clipboardText = script.clipboard();
var clipboardHtml = script.clipboard(true);
```

Vous pouvez jeter un œil à l'exemple [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml).

Écrire du texte dans le texte de la note Modifier
--------------------------------

### Appel de méthode et paramètres
```cpp
/**
  * Écrit le texte à la position actuelle du curseur dans l'édition du texte de la note
  *
  * @param text
  */
void ScriptingService :: noteTextEditWrite (texte QString);
```

### Exemple
```js
// écrire du texte dans le texte de la note modifier
script.noteTextEditWrite ("Mon texte personnalisé");
```

Vous voudrez peut-être regarder l'action personnalisée `transformTextRot13` dans le exemple [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml).

Vous pouvez l'utiliser avec `noteTextEditSelectAll` pour écraser tout le texte de la note actuelle.

Lire le texte sélectionné dans la modification du texte de la note
--------------------------------------------

### Appel de méthode et paramètres
```cpp
/**
  * Lit le texte sélectionné dans l'édition du texte de la note
  *
  * @revenir
  */
QString ScriptingService :: noteTextEditSelectedText();
```

### Exemple
```js
// lit le texte sélectionné dans l'édition du texte de la note
var text = script.noteTextEditSelectedText();
```

Vous voudrez peut-être regarder l'action personnalisée `transformTextRot13` dans le exemple [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml).

Sélectionnez tout le texte dans la modification du texte de la note
-------------------------------------

### Appel de méthode et paramètres
```cpp
/**
  * Sélectionne tout le texte dans l'édition du texte de la note
  */
void ScriptingService::noteTextEditSelectAll();
```

### Exemple
```js
script.noteTextEditSelectAll();
```

Vous pouvez l'utiliser avec `noteTextEditWrite` pour écraser tout le texte de la note actuelle.

Sélectionnez la ligne actuelle dans l'édition du texte de la note
---------------------------------------------

### Appel de méthode et paramètres
```cpp
/**
 * Sélectionne la ligne actuelle dans l'édition du texte de la note
 */
void ScriptingService::noteTextEditSelectCurrentLine();
```

### Exemple
```js
script.noteTextEditSelectCurrentLine();
```

Sélectionnez le mot actuel dans la modification du texte de la note
---------------------------------------------

### Appel de méthode et paramètres
```cpp
/**
 * Sélectionne la ligne actuelle dans l'édition du texte de la note
 */
void ScriptingService::noteTextEditSelectCurrentWord();
CONTEXTREQUEST

```

### Exemple
```js
script.noteTextEditSelectCurrentWord();
```

Définir le texte actuellement sélectionné dans la modification du texte de la note
-----------------------------------------------------

### Appel de méthode et paramètres
```cpp
/**
  * Définit le texte actuellement sélectionné dans l'édition du texte de la note
  *
  * @param start
  * @param end
  */
void ScriptingService :: noteTextEditSetSelection (int start, int end);
```

### Exemple
```js
// étend la sélection actuelle d'un caractère
script.noteTextEditSetSelection(
     script.noteTextEditSelectionStart() - 1,
     script.noteTextEditSelectionEnd() + 1);
```

Obtenir la position de départ de la sélection actuelle dans l'édition du texte de la note
---------------------------------------------------------------------

### Appel de méthode et paramètres
```cpp
/**
  * Renvoie la position de départ de la sélection actuelle dans l'édition du texte de la note
  */
int ScriptingService::noteTextEditSelectionStart();
```

### Exemple
```js
script.log(script.noteTextEditSelectionStart());
```

Obtenir la position de fin de la sélection actuelle dans la modification du texte de la note
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

Placez le curseur de texte dans l'édition de texte de la note à une certaine position
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

Obtenir la position actuelle du curseur de texte dans la modification du texte de la note
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

Lire le mot actuel à partir de la modification du texte de la note
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

Vérifiez si la plate-forme est Linux, OS X ou Windows
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
  * Récupère ou crée une balise par sa "liste de fil d'Ariane" de noms de balises
  * L'élément nameList [0] serait le plus élevé dans l'arborescence (avec parentId: 0)
  *
  * @param nameList
  * @param createMissing {bool} si true (par défaut) toutes les balises manquantes seront créées
  * @return TagApi objet de la balise la plus profonde de la liste de fil d'Ariane de nom
  */
TagApi * ScriptingService::getTagByNameBreadcrumbList(
     const QStringList & nameList, booléen createMissing);
```

### Exemple
```js
// crée toutes les balises jusqu'au 3ème niveau et renvoie l'objet balise pour
// tag "level3", qui ressemblerait à cela dans l'arborescence des tags:
// niveau1 > niveau2 > niveau3
var tag = script.getTagByNameBreadcrumbList (["level1", "level2", "level3"]);
```

Rechercher des tags par nom
-----------------------

### Appel de méthode et paramètres
```cpp
/**
  * Récupère toutes les balises en effectuant une recherche de sous-chaîne sur le champ de nom
  *
  * @param name {QString} nom à rechercher
  * @return {QStringList} liste des noms de balises
  */
QStringList ScriptingService::searchTagsByName(QString name);
```

### Exemple
```js
// recherche toutes les balises contenant le jeu de mots
var tags = script.searchTagsByName("jeu");
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
//agrandit le texte de la liste de notes
script.addStyleSheet("QTreeWidget#noteTreeWidget {font-size: 30px;}");
```

Vous voudrez peut-être jeter un coup d'œil à l'exemple [custom-stylesheet.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-stylesheet.qml).

Vous pouvez obtenir les noms d'objets à partir des fichiers `*.ui`, par exemple [mainwindow.ui](https://github.com/pbek/QOwnNotes/blob/develop/src/mainwindow.ui).

Jetez un œil à la feuille de style  [Style Sheet Reference](http://doc.qt.io/qt-5/stylesheet-reference.html) pour un référence des styles disponibles.

Si vous souhaitez injecter des styles dans l'aperçu html pour modifier la façon dont les notes sont prévisualisées, veuillez consulter [notetomarkdownhtmlhook](hooks.html#notetomarkdownhtmlhook).

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
script.fetchNoteById(243);
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
script.setClipboardText("text to copy");
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
 * Jumps to a note subfolder
 *
 * @param noteSubFolderPath {QString} path of the subfolder, relative to the note folder
 * @param separator {QString} separator between parts of the path, default "/"
 * @return true if jump was successful
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

Vous voudrez peut-être jeter un coup d'œil à l'exemple [export-notes-as-one-html.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/export-notes-as-one-html.qml).

Enregistrement des variables de paramètres de script
-------------------------------------

Vous devez définir vos variables de paramètres en tant que propriétés dans votre script et les enregistrer dans une propriété nommée `settingsVariables`.

L'utilisateur peut ensuite définir ces propriétés dans les paramètres du script.

### Exemple
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

De plus, vous pouvez remplacer les `settingsVariables` avec une fonction spéciale `registerSettingsVariables ()` comme ceci:

### Exemple
```js
/**
  *Enregistre à nouveau les variables de paramètres
  *
  *Utilisez cette méthode si vous souhaitez utiliser du code pour remplacer vos variables, comme le réglage
  *les valeurs par défaut dépendaient du système d'exploitation.
 */
function registerSettingsVariables() {
    if (script.platformIsWindows()) {
        // override the myFile default value
        settingsVariables[3].default = "pandoc.exe"
    }
}
```

Vous pouvez également jeter un œil à l'exemple [variables.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/variables.qml).

Stockage et chargement de variables persistantes
----------------------------------------

### Appel de méthode et paramètres
```cpp
/**
  * Stocke une variable persistante
  * Ces variables sont accessibles globalement sur tous les scripts
  * Veuillez utiliser un préfixe significatif dans votre clé comme "PersistentVariablesTest / myVar"
  *
  * @param key {QString}
  * @param value {QVariant}
  */
void ScriptingService :: setPersistentVariable (const QString & amp; key,
                                                 const QVariant & amp; valeur);

/**
  * Charge une variable persistante
  * Ces variables sont accessibles globalement sur tous les scripts
  *
  * @param key {QString}
  * @param defaultValue {QVariant} valeur de retour si le paramètre n'existe pas (facultatif)
  * @revenir
  */
QVariant ScriptingService :: getPersistentVariable (const QString & amp; key,
                                                     const QVariant & amp; defaultValue);
```

### Exemple
```js
// stocker la variable persistante
script.setPersistentVariable ("PersistentVariablesTest/myVar", résultat);

// charge et consigne la variable persistante
script.log (script.getPersistentVariable ("PersistentVariablesTest/myVar", "rien ici encore"));
```

Veuillez vous assurer d'utiliser un préfixe significatif dans votre clé comme `PersistentVariablesTest / myVar` car les variables sont accessibles depuis tous les scripts.

Vous voudrez peut-être également jeter un coup d'œil à l'exemple [variables-persistantes.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/persistent-variables.qml).

Chargement des variables de paramètres d'application
--------------------------------------

### Appel de méthode et paramètres
```cpp
/**
  * Charge une variable de paramètres d'application
  *
  * @param key {QString}
  * @param defaultValue {QVariant} valeur de retour si le paramètre n'existe pas (facultatif)
  * @revenir
  */
QVariant ScriptingService :: getApplicationSettingsVariable (const QString & amp; key,
                                                             const QVariant & amp; defaultValue);
```

### Exemple
```js
// charger et enregistrer une variable de paramètres d'application
script.log (script.getApplicationSettingsVariable ("gitExecutablePath"));
```

Gardez à l'esprit que les paramètres peuvent être vides, vous devez vous en occuper vous-même. `defaultValue` n'est utilisé que si le paramètre n'existe pas du tout.

Créer un répertoire de cache
--------------------------

Vous pouvez mettre en cache les fichiers à l'emplacement de cache par défaut de votre système.

### Appel de méthode et paramètres
```cpp
/**
  * Renvoie un répertoire de cache pour un script
  *
  * @param {QString} subDir le sous-dossier à créer et à utiliser
  * @return {QString} le chemin du répertoire du cache
  */
QString ScriptingService :: cacheDir (const QString & amp; subDir) const;
```

### Exemple
```js
// crée le répertoire cache pour my-script-id
var cacheDirForScript = script.cacheDir ("mon-id-script");
```

Effacer un répertoire de cache
--------------------------

Vous pouvez effacer les fichiers cache de votre script en passant son nom à clearCacheDir().

### Appel de méthode et paramètres
```cpp
/**
  * Efface le répertoire de cache pour un script
  *
  * @param {QString} subDir le sous-dossier à effacer
  * @return {bool} true en cas de succès
  */
bool ScriptingService :: clearCacheDir (const QString & amp; subDir) const;
```

### Exemple
```js
// clear cache directory of my-script-id 
script.clearCacheDir("my-script-id");
```

Lecture du chemin vers le répertoire de votre script
------------------------------------------------

Si vous avez besoin d'obtenir le chemin d'accès au répertoire où votre script est placé pour, par exemple, charger d'autres fichiers, vous devez enregistrer une chaîne de propriété `scriptDirPath;`. Cette propriété sera définie avec le chemin d'accès au répertoire du script.

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
  * Renvoie le chemin en utilisant '/' comme séparateur de fichier.
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
  * Renvoie une liste des chemins de toutes les notes sélectionnées
  *
  * @return {QStringList} liste des chemins de notes sélectionnés
  */
QStringList ScriptingService::selectedNotesPaths();
```

### Exemple
```js
// renvoie une liste des chemins de toutes les notes sélectionnées
script.log(script.selectedNotesPaths());
```

Vous voudrez peut-être jeter un coup d'œil à l'exemple [note-externe-diff.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/external-note-diff.qml).

Obtenir une liste des identifiants de toutes les notes sélectionnées
-----------------------------------------------

### Appel de méthode et paramètres
```cpp
/**
  * Renvoie une liste des identifiants de toutes les notes sélectionnées
  *
  * @return {QList <int>} liste des identifiants de note sélectionnés
  */
QList<int> ScriptingService::selectedNotesIds();
```

### Exemple
```js
// renvoie une liste des identifiants de toutes les notes sélectionnées
script.log(script.selectedNotesIds());
```

Vous voudrez peut-être jeter un coup d'œil à l'exemple [export-notes-as-one-html.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/export-notes-as-one-html.qml).

Déclencher une action de menu
------------------------

### Appel de méthode et paramètres
```cpp
/**
  * Déclenche une action de menu
  *
  * @param objectName {QString} nom d'objet de l'action à déclencher
  * @param vérifié {QString} ne déclenche l'action que si l'état vérifié est
  * différent de ce paramètre (facultatif, peut être 0 ou 1)
  */
void ScriptingService::triggerMenuAction(QString objectName, QString checked);
```

### Exemple
```js
// bascule en mode lecture seule
script.triggerMenuAction ("actionAllow_note_editing");

// désactiver le mode lecture seule
script.triggerMenuAction ("actionAllow_note_editing", 1);
```

Vous voudrez peut-être jeter un coup d'œil à l'exemple [disable-readonly-mode.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/disable-readonly-mode.qml).

Vous pouvez obtenir les noms d'objets de l'action de menu à partir de [mainwindow.ui](https://github.com/pbek/QOwnNotes/blob/develop/src/mainwindow.ui).

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
/ **
  * Ouvre une boîte de dialogue de saisie avec une modification de ligne
  *
  * @param title {QString} titre de la boîte de dialogue
  * @param label {QString} texte de l'étiquette de la boîte de dialogue
  * @param text {QString} texte dans la boîte de dialogue (facultatif)
  * @ return
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
 * Vérifier si un fichier existe
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
  * Lire le texte d'un fichier
  *
  * @param filePath {QString} chemin du fichier à charger
  * Codec @param codec {QString} (par défaut: UTF-8)
  * @return les données du fichier ou nul si le fichier n'existe pas
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

Vous pouvez également écouter les sockets avec `WebSocket`. Veuillez regarder l'exemple [websocket-client.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/websocket-client.qml).

Gardez à l'esprit que vous devez avoir la bibliothèque QML `websocket` de Qt installée pour l'utiliser. Par exemple sous Ubuntu Linux, vous pouvez installer `module-qml-qtwebsockets`.
