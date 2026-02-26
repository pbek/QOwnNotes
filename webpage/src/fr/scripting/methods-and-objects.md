# Méthodes et objets fournis par QOwnNotes

## Lancer un programme externe en arrière-plan

### Appel de méthode et paramètres

```cpp
/**
 * Wrapper QML pour démarrer un processus détaché
 *
 * @param executablePath le chemin d'accès de l'exécutable
 * @param parameters une liste de chaînes de paramètres
 * @param callbackIdentifier un indetifiant à utiliser dans la fonction onDetachedProcessCallback() (optionnel)
 * @param callbackParameter un paramètre additionel pour les boucles ou équivalents (optionnel)
 * @param processData données écrites vers le processus si la fonction de rappel est utilisée (optionnel)
 * @param workingDirectory le répertoire de travail dans lequel exécuter le processus (optionnel, fonctionne seulement avec une fonction de rappel)
 * @return true en cas de succès, false sinon
 */
bool startDetachedProcess(QString executablePath, QStringList parameters,
                            QString callbackIdentifier, QVariant callbackParameter,
                            QByteArray processData, QString workingDirectory);
```

### Exemple

Exemple simple :

```js
script.startDetachedProcess("/chemin/vers/mon/programme", ["mon paramètre"]);
```

Exemple simple :

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

Vous voudrez peut-être jeter un coup d'œil aux exemples [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-actions.qml), [callback.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/callback.qml) ou [execute-command-after-note-update.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/execute-command-after-note-update.qml).

Vous voudrez peut-être jeter un coup d'œil au hook [onDetachedProcessCallback](hooks.html#ondetachedprocesscallback).

## Lancer un programme externe et attendre la sortie

### Appel de méthode et paramètres

```cpp
/**
 * Wrapper QML pour démarrer un processus synchrone
 *
 * @param executablePath le chemin vers l'exécutable
 * @param parameters une liste de chaînes de paramètres
 * @param data les données qui seront écrites vers le processus (optionnel)
 * @param workingDirectory le répertoire de travail dans lequel exécuter le processus (optionnel)
 * @return le texte qui a été retourné par le processus
QByteArray startSynchronousProcess(QString executablePath, QStringList parameters, QByteArray data, QString workingDirectory);
```

### Exemple

```js
var result = script.startSynchronousProcess(
  "/path/to/my/program",
  ["my parameter"],
  "data",
  "/path/to/execute/in",
);
```

Vous voudrez peut-être jeter un œil à l'exemple [encryption-keybase.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/encryption-keybase.qml).

## Obtenir le chemin du dossier de notes actuel

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

Vous voudrez peut-être jeter un coup d'œil à l'exemple [absolute-media-links.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/absolute-media-links.qml).

## Obtenir la note actuelle

### Appel de méthode et paramètres

```cpp
/**
 * Wrapper QML pour obtenir la note courante
 *
 * @returns {NoteApi} l'objet note courante
 */
NoteApi currentNote ();
```

### Exemple

```js
var note = script.currentNote();
```

Vous voudrez peut-être jeter un coup d'œil à l'exemple [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-actions.qml).

## Connexion au widget de journal

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
script.log("mon texte");
```

## Téléchargement d'une URL dans une chaîne

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

Vous voudrez peut-être jeter un coup d'œil à l'exemple [insert-headline-with-link-from-github-url.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/insert-headline-with-link-from-github-url.qml).

## Téléchargement d'une URL dans le dossier multimédia

### Appel de méthode et paramètres

```cpp
/**
 * Wrapper QML pour télécharger une URL dans le dossier multimédia et renvoyer le média
 * url ou le texte Markdown de l'image du média relativement à la note actuelle
 *
 * @param {QString} url
 * @param {bool} returnUrlOnly si true, seule l'URLdu média sera renvoyée (par défaut false)
 * @return {QString} le Markdown ou l'URL du média
 */
QString downloadUrlToMedia (QUrl url, booléen returnUrlOnly);
```

### Exemple

```js
var markdown = script.downloadUrlToMedia(
  "http://latex.codecogs.com/gif.latex?\frac{1}{1+sin(x)}",
);
```

Vous voudrez peut-être jeter un coup d'œil à l'exemple [paste-latex-image.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/paste-latex-image.qml).

## Insertion d'un fichier multimédia dans le dossier multimédia

### Appel de méthode et paramètres

```cpp
/**
  * Wrapper QML pour insérer un fichier multimédia dans le dossier des médias et retourner
  * l'URL du média ou le texte Markdown de l'image du média relativement à la note actuelle
  *
  * @param {QString} mediaFilePath
  * @param {bool} returnUrlOnly si true, seule l'URL du média sera retournée (par défaut false)
  * @return {QString} le Markdown ou l'URL du média
  */
QString ScriptingService::insertMediaFile (QString mediaFilePath,
                                         booléen returnUrlOnly);
```

### Exemple

```js
var markdown = script.insertMediaFile("/chemin/vers/votre/image.png");
```

Vous voudrez peut-être jeter un coup d'œil à l'exemple [scribble.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/scribble.qml).

## Insérer un fichier joint dans le dossier des pièces jointes

### Appel de méthode et paramètres

```cpp
 * Wrapper QML pour insérer un fichier joint dans le dossier `attachments` et
  * retourner l'URL de la pièce jointe ou le texte en Markdown de la pièce jointe
* relativement à la note actuelle
*
* @param {QString} attachmentFilePath
 * @param {QString} fileName à utiliser dans le Markdown
 * @param {bool} returnUrlOnly si true seule l'url de la pièce jointes era retournée
 * (false par défaut)
 * @return {QString} le Markdown ou l'url de la pièce jointe
 */
QString ScriptingService::insertAttachmentFile(const QString &attachmentFilePath,
                                               const QString &fileName,
                                               bool returnUrlOnly);
```

### Exemple

```js
var markdown = script.insertAttachmentFile("/chemin/vers/votre/fichier.png");
```

## Régénérer l'aperçu de la note

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

Vous voudrez peut-être jeter un coup d'œil à l'exemple [scribble.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/scribble.qml).

## Déclarer une action personnalisée

### Appel de méthode et paramètres

```cpp
/**
 * Enregistre une action personnalisée
 *
 * @param identifier l'identifiant de l'action
 * @param menuText le texte affiché dans le menu
 * @param buttonText le texte affiché dans le bouton
 *                   (aucun bouton vide ne sera affiché)
 * @param icon le chemin d'accès vers l'icône ou le nom d'un icône du thème freedesktop
 *             vous trouverez ici une liste des icônes :
 *             https://specifications.freedesktop.org/icon-naming-spec/icon-naming-spec-latest.html
 * @param useInNoteEditContextMenu si true utiilser l'action dans l'éditeur de notes
 *                                 context menu (default: false)
 * @param hideButtonInToolbar si true le bouton ne sera pas affiché dans
 *                            la barre d'outil des actions personnalisées (par défaut : false)
 * @param useInNoteListContextMenu si true utiliser l'action dans le menu contextuel de
 *                                 la liste de notes (par défaut : false)
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
Soyez attentif au fait que les [icones du thème freedesktop](https://specifications.freedesktop.org/icon-naming-spec/icon-naming-spec-latest.html) ne sont généralement disponibles que sous Linux. À cause de cela, si vous souhaitez vraiment utiliser un icone sous macOS ou Windows vous devrez le fournir avec votre script. Pour obtenir le chemin vers votre script afin de définir un chemin correct vers votre icone, utilisez [scriptDirPath property](methods-and-objects.md#reading-the-path-to-the-directory-of-your-script).
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

Pour d'autres exemples allez voir [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-actions.qml).

::: tip
Vous pouvez également déclencher une action personnalisée après le démarrage de l'application avec le paramètre `--action customAction_<identifier>`. Pour plus d'information, veuillez consulter [Déclencher des actions de menu après le démarrage](../getting-started/cli-parameters.md#trigger-menu-actions-after-startup).
:::

## Enregistrer une étiquette

### Appel de méthode et paramètres

```cpp
/**
 * Enregistre une étiquette sur laquelle écrire
 *
 * @param identifier the identifier of the label
 * @param text the text shown in the label (optional)
 */
void ScriptingService::registerLabel(QString identifier, QString text);
```

### Exemple

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

Vous pouvez utiliser à la fois du texte brut ou du HTML dans les étiquettes. Le texte sera sélectionnable et les liens pourront être cliqués.

Vous aurez peut-être envie de jeter un œil à l'exemple de script [scripting-label-demo.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/scripting-label-demo.qml).

## Définition du texte d'une étiquette enregistrée

### Appel de méthode et paramètres

```cpp
/**
 * Définit le texte d'une étiquette enregistrée
 *
 * @param identifier l'identifiant du label
 * @param text le texte affiché dans l'étiquette
 */
void ScriptingService::setLabelText(QString identifier, QString text);
```

### Exemple

```js
script.setLabelText("étiquette-compteur", "texte compteur");
```

Vous pouvez utiliser à la fois du texte brut ou du HTML dans les étiquettes. Le texte sera sélectionnable et les liens pourront être cliqués.

Vous aurez peut-être envie de jeter un œil à l'exemple de script [scripting-label-demo.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/scripting-label-demo.qml).

::: tip
The scripting label also supports **external links**, as well as **internal links** to notes, tasks, deck cards and more. You might get a grasp of what's supported by taking a look at [URL handling](https://github.com/pbek/QOwnNotes/blob/964acf69b6382f8ee8252c640c5048f8f4644622/src/utils/urlhandler.cpp#L16-L75).
:::

## Créer une nouvelle note

### Appel de méthode et paramètres

```cpp
/**
 * Créé une nouvelle note
 *
 * @param text le texte de la note
 */
void ScriptingService::createNote(QString text);
```

### Exemple

```js
script.createNote("Le titre de ma note\n===\n\nMon texte");
```

Vous voudrez peut-être jeter un coup d'œil à l'exemple [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-actions.qml).

::: tip
If you turned off that your note headline determines the note filename then you have to rename your note file yourself afterward, like this:

```js
var note = script.currentNote();
note.renameNoteFile("your-filename");
```

:::

## Accéder au presse-papiers

### Appel de méthode et paramètres

```cpp
/**
 * Retourne le contenu du presse-papier sous forme de texte ou de HTML
 *
 * @param asHtml retourne le contenu du presse-papier sous forme de HTML au lieu de texte
 */
QString ScriptingService::clipboard(bool asHtml);
```

### Exemple

```js
var clipboardText = script.clipboard();
var clipboardHtml = script.clipboard(true);
```

Vous aurez peut-être envie de jeter un œil à l'exemple [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-actions.qml).

## Écrire du texte dans le corps de la note

### Appel de méthode et paramètres

```cpp
/**
 * Écrit du texte à la position courante du curseur dans le corps de la note
 *
 * @param text
 */
void ScriptingService::noteTextEditWrite(QString text);
```

### Exemple

```js
// écrire texte dans le corps de la note
script.noteTextEditWrite("Mon texte à moi");
```

Vous voudrez peut-être jeter un œil à l'action personnalisée `transformTextRot13` dans l'exemple [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-actions.qml).

Vous pouvez l'utiliser en conjonction avec `noteTextEditSelectAll` pour écraser tout le texte de la note actuelle.

## Lire le texte sélectionné dans le corps de la note

### Appel de méthode et paramètres

```cpp
/**
 * Lit le texte sélectionné dans le corps de la note
 *
 * @return
 */
QString ScriptingService::noteTextEditSelectedText();
```

### Exemple

```js
// lit le texte sélectionné dans le corps de la note
var text = script.noteTextEditSelectedText();
```

Vous voudrez peut-être jeter un œil à l'action personnalisée `transformTextRot13` dans l'exemple [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-actions.qml).

## Sélectionner l'intégralité du texte de la note

### Appel de méthode et paramètres

```cpp
/**
 * Sélectionne l'intégralité du texte de la note
 */
void ScriptingService::noteTextEditSelectAll();
```

### Exemple

```js
script.noteTextEditSelectAll();
```

Vous pouvez utiliser ceci en conjonction avec `noteTextEditWrite` pour écraser tout le texte de la note actuelle.

## Sélectionner la ligne actuelle dans le texte de la note

### Appel de méthode et paramètres

```cpp
/**
 * Sélectionne la ligne actuelle dans le texte de la note
 */
void ScriptingService::noteTextEditSelectCurrentLine();
```

### Exemple

```js
script.noteTextEditSelectCurrentLine();
```

## Sélectionner le mot actuel dans le texte de la note

### Appel de méthode et paramètres

```cpp
/**
 * Sélectionne la ligne actuelle dans l'édition du texte de la note
 */
void ScriptingService::noteTextEditSelectCurrentWord();
```

### Exemple

```js
script.noteTextEditSelectCurrentWord();
```

## Définir le texte actuellement sélectionné dans le corps de la note

### Appel de méthode et paramètres

```cpp
/**
 *Définit le texte actuellement sélectionné dans le corps de la note
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
  script.noteTextEditSelectionEnd() + 1,
);
```

## Obtenir la position de départ de la sélection courante dans le texte de la note

### Appel de méthode et paramètres

```cpp
/**
 * Renvoie la position de départ de la sélection actuelle dans l'éditeur de texte de la note
 */
int ScriptingService::noteTextEditSelectionStart();
```

### Exemple

```js
script.log(script.noteTextEditSelectionStart());
```

## Obtenir la position de fin de la sélection courante dans le texte de la note

### Appel de méthode et paramètres

```cpp
/**
 * Renvoie la position de fin de la sélection actuelle dans l'éditeur de texte de la note
 */
int ScriptingService :: noteTextEditSelectionEnd ();
```

### Exemple

```js
script.log(script.noteTextEditSelectionEnd());
```

## Placer le curseur à un endroit donné du texte de la note

### Appel de méthode et paramètres

```cpp
/**
 * Place le curseur à un endroit donné dans le texte de la note
 * 0 serait le début de la note
 * cas particulier: -1 serait la fin de la note
 *
 * @param position
 */
void ScriptingService::noteTextEditSetCursorPosition(int position);
```

### Exemple

```js
// sauter au 11ème caractère dans la note
script.noteTextEditSetCursorPosition(10);

// sauter à la fin de la note
script.noteTextEditSetCursorPosition(-1);
```

## Obtenir la position actuelle du curseur dans le texte de la note

### Appel de méthode et paramètres

```cpp
/**
 * Retourne la position actuelle du curseur dans le texte de la note
 * 0 serait le début de la note
 */
int ScriptingService::noteTextEditCursorPosition();
```

### Exemple

```js
script.log(script.noteTextEditCursorPosition());
```

## Lire le mot actuel dans le texte de la note

### Appel de méthode et paramètres

```cpp
/**
 * Lit le mot actuel dans le texte de la note
 *
 * @param withPreviousCharacters pour récupérer plus de caractères au début
 *                               pour récupérer des caractères tels que "@" qui ne sont pas
 *                              des caractères de mots
 * @return
 */
QString ScriptingService::noteTextEditCurrentWord(bool withPreviousCharacters);
```

### Exemple

```js
// Lit le mot actuel dans le texte de la note
var text = script.noteTextEditCurrentWord();
```

Vous voudrez peut-être jeter un coup d'œil à l'exemple [autocompletion.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/autocompletion.qml).

## Read the current block from the note text edit

### Appel de méthode et paramètres

```cpp
/**
 * Reads the current block in the note text edit
 *
 * @return
 */
QString ScriptingService::noteTextEditCurrentBlock();
```

### Exemple

```js
// Read the current block in the note text edit
var text = script.noteTextEditCurrentBlock();
```

You may want to take a look at the example [ai-autocompletion.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/ai-autocompletion.qml).

## Use a completion prompt on the currently selected AI model

The AI completion prompt is a feature that allows you to use a completion prompt on the currently selected AI model, like ChatGPT, Claude or systems like Ollama.

The AI system needs to be enabled in the AI toolbar or main menu for this to work.

### Appel de méthode et paramètres

```cpp
/**
 * QML wrapper to use the AI Completer
 *
 * @param prompt
 * @return {QString} the result of the completer
 */
QString ScriptingService::aiComplete(const QString& prompt);
```

### Exemple

```js
// Ask the currently selected AI model to complete the prompt
var text = script.aiComplete("Tell me how do you feel today?");
```

You may want to take a look at the example [ai-autocompletion.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/ai-autocompletion.qml).

## Déterminer si la plate-forme est Linux, OS X ou Windows

### Appel de méthode et paramètres

```cpp
bool ScriptingService::platformIsLinux();
bool ScriptingService::platformIsOSX();
bool ScriptingService::platformIsWindows();
```

### Exemple

```js
if (script.platformIsLinux()) {
  // Will be executed only if under Linux
}
```

## Étiquetter la note actuelle

### Appel de méthode et paramètres

```cpp
/**
 * Étiquette la note courante avec une étiquette nommée tagName
 *
 * @param tagName
 */
void ScriptingService::tagCurrentNote(QString tagName);
```

### Exemple

```js
// ajouter une étiquette "favorite" à la note courante
script.tagCurrentNote("favorite");
```

Vous voudrez peut-être jeter un coup d'œil à l'action personnalisée `favoriteNote` dans l'exemple [favorite-note.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/favorite-note.qml).

## Créer ou récupérer une étiquette par son nom liste de fil d'Ariane

### Appel de méthode et paramètres

```cpp
/**
 * Créé ou récupère une étiquette par son nom liste de fil d'Ariane
 * Element nameList[0] serait le plus élevé dans l'arborescence (with parentId: 0)
 *
 * @param nameList
 * @param createMissing {bool} si true (default) toutes les étiquettes manquantes seront créées
 * @return TagApi object de l'étiquette la plus profonde du nom liste de fil d'Ariane
 */
TagApi *ScriptingService::getTagByNameBreadcrumbList(
    const QStringList &nameList, bool createMissing);
```

### Exemple

```js
// crée toutes les étiquettes jusqu'au 3ème niveau et renvoie l'objet étiquette pour la
// balise "level3", qui ressemblerait à ceci dans l'arborescence des balises:
// level1 > level2 > level3
var tag = script.getTagByNameBreadcrumbList(["level1", "level2", "level3"]);
```

## Rechercher des étiquettes par nom

### Appel de méthode et paramètres

```cpp
/**
 * Récupère toutes les étiquettes par le biais d'une recherche de sous-chaîne sur le champ « nom ».
 *
 * @param name {QString} nom à rechercher
 * @return {QStringList} liste des noms des étiquettes
 */
QStringList ScriptingService::searchTagsByName(QString name);
```

### Exemple

```js
// recherche toutes les étiquettes contenant le mot "jeu"
var tags = script.searchTagsByName("jeu");
```

Vous voudrez peut-être jeter un coup d'œil à l'exemple [autocompletion.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/autocompletion.qml).

## Rechercher des notes à partir de texte contenu dans le corps d'une note

### Appel de méthode et paramètres

```cpp
/**
 * Retourne une liste des identifiants de toutes les notes contenant un certain texte dans leur corps.
 *
 * Malheureusement il n'y a pas de moyen facile pour utiliser un QList<NoteApi*> en QML, c'est pourquoi on
 * ne peut transférer que les identifiants de notes
 *
 * @return {QList<int>} liste des identifiants de notes
 */
QList<int> ScriptingService::fetchNoteIdsByNoteTextPart(QString text);
```

### Exemple

```js
var noteIds = script.fetchNoteIdsByNoteTextPart("mytext");

noteIds.forEach(function (noteId) {
  var note = script.fetchNoteById(noteId);

  // do something with the note
});
```

Vous voudrez peut-être jeter un coup d'œil à l'exemple [unique-note-id.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/unique-note-id.qml).

## Ajouter une feuille de style personnalisée

### Appel de méthode et paramètres

```cpp
/**
 * Ajoute une feuille de style personnalisée à l'application
 *
 * @param stylesheet
 */
void ScriptingService::addStyleSheet(QString stylesheet);
```

### Exemple

```js
// augmenter la taille du texte de la liste des notes
script.addStyleSheet("QTreeWidget#noteTreeWidget {font-size: 30px;}");
```

Vous voudrez peut-être jeter un coup d'œil à l'exemple [custom-stylesheet.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-stylesheet.qml).

Vous pouvez obtenir les noms des widgets à partir des fichiers `*.ui`. Par exemple, la fenêtre principale est [mainwindow.ui](https://github.com/pbek/QOwnNotes/blob/main/src/mainwindow.ui).

La documentation Qt (par exemple [QMainWindow](https://doc.qt.io/qt-5/qmainwindow.html)) peut vous aider à voir comment les widgets sont liés les uns aux autres (recherchez `Inherits`).

Le widget de base pour presque tout est [QWidget](https://doc.qt.io/qt-5/qwidget.html). De ce fait, le seul ajustement de style de `QWidget` avec par exemple `QWidget {background-color: black; color: white;}` signifierait que tout a une couleur d'arrière-plan noire et une couleur de premier plan blanche.

::: tip
Le [style.qss](https://github.com/pbek/QOwnNotes/blob/main/src/libraries/qdarkstyle/style.qss) de [qdarkstyle](https://github.com/pbek/QOwnNotes/blob/main/src/libraries/qdarkstyle) peut également constituer une bonne référence des styles que vous pouvez changer.
:::

Jetez un œil à [Style Sheet Reference](http://doc.qt.io/qt-5/stylesheet-reference.html) en tant que référence des styles disponibles.

Si vous souhaitez injecter des styles dans l'aperçu HTML pour modifier la façon dont les notes sont prévisualisées, veuillez consulter [notetomarkdownhtmlhook](hooks.html#notetomarkdownhtmlhook).

::: tip
Si vous souhaitez voir l'aspect des dialogues et quels sont leurs noms, téléchargez[Qt Creator](https://www.qt.io/product/development-tools) et ouvrez les fichiers `*.ui` qu'il contient.
:::

## Recharger le moteur de script

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

## Récupérer une note par son nom de fichier

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

## Récupérer une note par son identifiant

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

Vous voudrez peut-être jeter un coup d'œil à l'exemple [export-notes-as-one-html.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/export-notes-as-one-html.qml).

## Vérifier si une note existe par son nom de fichier

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

Vous voudrez peut-être jeter un coup d'œil à l'exemple [use-tag-names-in-filename.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/use-tag-names-in-filename.qml).

## Copier du texte dans le presse-papiers

### Appel de méthode et paramètres

```cpp
/**
 * Copie du texte dans le presse-papiers sous forme de texte brut ou de données MIME HTML
 *
 * @param text string texte à mettre dans le presse-papiers
 * @param asHtml bool si true, le texte sera défini en tant que données MIME HTML
 */
void ScriptingService::setClipboardText(QString text, bool asHtml);
```

### Exemple

```js
// copie du texte dans le presse-papiers
script.setClipboardText ("texte à copier");
```

Vous voudrez peut-être jeter un coup d'œil à l'exemple [selected-markdown-to-bbcode.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/selected-markdown-to-bbcode.qml).

## Sauter vers une note

### Appel de méthode et paramètres

```cpp
/**
 * Définit la note courante si la note est visible dans la liste des notes
 *
 * @param note NoteApi note vers laquelle sauter
 * @param asTab bool si vrai la note sera ouverte dans un nouvel onglet (si pas déjà ouverte)
 */
void ScriptingService::setCurrentNote(NoteApi *note, bool asTab = false);
```

### Exemple

```js
// sauter à la note
script.setCurrentNote(note);

// ouvrir la note dans un nouvel onglet (si pas déjà ouverte)
script.setCurrentNote(note, true);
```

Vous voudrez peut-être jeter un coup d'œil à l'exemple [journal-entry.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/journal-entry.qml).

## Sauter vers un sous-dossier de notes

### Appel de méthode et paramètres

```cpp
/**
  * Saute vers un sous-dossier de notes
  *
  * @param noteSubFolderPath {QString} chemin du sous-dossier, relativement au dossier de notes
  * @param separator {QString} séparateur entre les parties du chemin, par défaut "/"
  * @return true si le saut a réussi
  */
bool ScriptingService::jumpToNoteSubFolder(const QString &noteSubFolderPath,
                                            QString separator);
```

### Exemple

```js
// saute vers le sous-dossier de notes "un sous-dossier"
script.jumpToNoteSubFolder("un sous-dossier");

// saute vers le sous-dossier de notes "sub" à l'intérieur de "un sous-dossier"
script.jumpToNoteSubFolder("un sous-dossier/sub");
```

::: tip
Vous pouvez créer un nouveau sous-dossier de notes dans le sous-dossier courant en appelant [`mainWindow.createNewNoteSubFolder`](classes.html#example-2).
:::

## Affichage d'une boîte de message d'information

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

## Affichage d'une boîte de message de question

### Appel de méthode et paramètres

```cpp
/**
  * Affiche une boîte de message de question
  *
  * Pour plus d'informations sur les boutons, voir :
  * https://doc.qt.io/qt-5/qmessagebox.html#StandardButton-enum
  *
  * @param text
  * @param title (facultatif)
  * Boutons @param boutons qui doivent être affichés (facultatif)
  * @param defaultButton bouton qui sera sélectionné par défaut (facultatif)
  * @return id du bouton enfoncé
  */
int ScriptingService :: questionMessageBox (
         Texte QString, titre QString, boutons int, int defaultButton);
```

### Exemple

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

Pour plus d'informations sur les boutons voir [StandardButton](https://doc.qt.io/qt-5/qmessagebox.html#StandardButton-enum).

Vous voudrez peut-être jeter un coup d'œil à l'exemple [input-dialogs.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/input-dialogs.qml).

## Affichage d'une boîte de dialogue d'ouverture de fichier

### Appel de méthode et paramètres

```cpp
/**
  * Affiche une boîte de dialogue d'ouverture de fichier
  *
  * @param caption (facultatif)
  * @param dir (facultatif)
  * @param filter (facultatif)
  * @return QString
  */
QString ScriptingService :: getOpenFileName (légende QString, répertoire QString,
                                             Filtre QString);
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

## Affichage d'une boîte de dialogue d'enregistrement de fichier

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
// show a save file dialog
var fileName = script.getSaveFileName(
  "Please select HTML file to save",
  "output.html",
  "HTML (*.html)",
);
```

Vous voudrez peut-être jeter un coup d'œil à l'exemple [export-notes-as-one-html.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/export-notes-as-one-html.qml).

## Enregistrement des variables de paramètres de script

Vous devez définir vos variables de paramètres en tant que propriétés dans votre script et les enregistrer dans une propriété nommée `settingsVariables`.

L'utilisateur peut ensuite définir ces propriétés dans les paramètres du script.

### Exemple

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

### Exemple

```js
/**
 *Enregistre à nouveau les variables de paramètres
 *
 *Utilisez cette méthode si vous souhaitez utiliser du code pour outrepasser vos variables, telles que le réglage
 *des valeurs par défaut dépendant du système d'exploitation.
 */
function registerSettingsVariables() {
  if (script.platformIsWindows()) {
    // override the myFile default value
    settingsVariables[3].default = "pandoc.exe";
  }
}
```

Vous voudrez peut-être jeter un coup d'œil à l'exemple [variables.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/variables.qml).

## Stockage et chargement de variables persistantes

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
script.log(
  script.getPersistentVariable(
    "PersistentVariablesTest/myVar",
    "nothing here yet",
  ),
);
```

Veuillez vous assurer d'utiliser un préfixe explicite dans votre clé, tel que `PersistentVariablesTest / myVar` car les variables sont accessibles depuis tous les scripts.

Vous voudrez peut-être également jeter un coup d'œil à l'exemple [persistent-variables.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/persistent-variables.qml).

## Chargement des variables de paramètres d'application

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
// charger et enregistrer une variable de paramètres d'application
script.log (script.getApplicationSettingsVariable ("gitExecutablePath"));
```

Gardez à l'esprit que les paramètres peuvent être vides, vous devez vous en occuper vous-même. `defaultValue` n'est utilisé que si le paramètre n'existe nulle part.

## Créer un répertoire cache

Vous pouvez mettre en cache des fichiers à l'emplacement de cache par défaut de votre système.

### Appel de méthode et paramètres

```cpp
/**
 * Renvoie un répertoire de cache pour un script
 *
 * @param {QString} subDir le sous-dossier à créer et à utiliser
 * @return {QString} le chemin du répertoire cache
 */
QString ScriptingService :: cacheDir (const QString & subDir) const;
```

### Exemple

```js
// crée le répertoire cache pour mon-id-de-script
var cacheDirForScript = script.cacheDir ("mon-id-de-script");
```

## Vider un répertoire cache

Vous pouvez vider le répertoire cache de votre script en passant son nom à clearCacheDir().

### Appel de méthode et paramètres

```cpp
/**
 * Vide le répertoire cache pour un script
 *
 * @param {QString} subDir le sous-dossier à vider
 * @return {bool} true en cas de succès
 */
bool ScriptingService :: clearCacheDir (const QString & subDir) const;
```

### Exemple

```js
// clear cache directory of my-script-id
script.clearCacheDir("my-script-id");
```

## Lire le chemin d'accès au répertoire de votre script

Si vous avez besoin d'obtenir le chemin d'accès au répertoire où votre script est placé pour, par exemple, charger d'autres fichiers, vous devez enregistrer une chaîne de propriété `scriptDirPath;`. Cette propriété sera définie avec le chemin d'accès au répertoire du script.

### Exemple

```js
import QtQml 2.0
import QOwnNotesTypes 1.0

Script {
    // le chemin d'accès au répertoire du script est défini ici
    property string scriptDirPath;

    function init() {
        script.log(scriptDirPath);
    }
}
```

## Conversion des séparateurs de chemin en séparateurs natifs

### Appel de méthode et paramètres

```cpp
/**
 * Renvoie le chemin avec les séparateurs '/' convertis en séparateurs
 * appropriés au système d'exploitation sous-jacent.
 *
 * Sous Windows, toNativeDirSeparators("c:/winnt/system32") retourne
 * "c:\winnt\system32".
 *
 * @param path
 * @return
 */
QString ScriptingService::toNativeDirSeparators(QString path);
```

### Exemple

```js
// retournera "c:\winnt\system32" sous Windows
script.log(script.toNativeDirSeparators("c:/winnt/system32"));
```

## Conversion des séparateurs de chemin depuis des séparateurs natifs

### Appel de méthode et paramètres

```cpp
/**
 * Renvoie le chemin en utilisant '/' comme séparateur de fichier.
 * Sous Windows par exemple, fromNativeDirSeparators("c:\\winnt\\system32")
 * renverra "c:/winnt/system32".
 *
 * @param path
 * @return
 */
QString ScriptingService::fromNativeDirSeparators(QString path);
```

### Exemple

```js
// retournera "c:/winnt/system32" sous Windows
script.log(script.toNativeDirSeparators("c:\\winnt\\system32"));
```

## Obtenir le séparateur de répertoire natif

### Appel de méthode et paramètres

```cpp
/**
 * Renvoie le séparateur de répertoire natif "/" ou "\" sous Windows
 *
 * @return
 */
QString ScriptingService::dirSeparator();
```

### Exemple

```js
// renverra "\" sous Windows
script.log(script.dirSeparator());
```

## Obtenir une liste des chemins d'accès de toutes les notes sélectionnées

### Appel de méthode et paramètres

```cpp
/**
 * Renvoie une liste des chemins d'accès de toutes les notes sélectionnées
 *
 * @return {QStringList} liste des chemins d'accès de toutes les notes sélectionnées
 */
QStringList ScriptingService::selectedNotesPaths();
```

### Exemple

```js
// renvoie une liste des chemins d'accès de toutes les notes sélectionnées
script.log(script.selectedNotesPaths());
```

Vous voudrez peut-être jeter un coup d'œil à l'exemple [external-note-diff.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/external-note-diff.qml).

## Obtenir une liste des identifiants de toutes les notes sélectionnées

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

Vous voudrez peut-être jeter un coup d'œil à l'exemple [export-notes-as-one-html.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/export-notes-as-one-html.qml).

## Déclencher une action de menu

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
// basculer en mode lecture seule
script.triggerMenuAction ("actionAllow_note_editing");

// désactiver le mode lecture seule
script.triggerMenuAction ("actionAllow_note_editing", 1);
```

Vous voudrez peut-être jeter un coup d'œil à l'exemple [disable-readonly-mode.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/disable-readonly-mode.qml).

::: tip
Vous pouvez obtenir les noms d'objet de l'action de menu avec [mainwindow.ui](https://github.com/pbek/QOwnNotes/blob/main/src/mainwindow.ui). Recherchez simplement le titre du menu en anglais. Notez que ces textes peuvent changer avec le temps.
:::

## Ouverture d'une boîte de dialogue de saisie avec une boîte de sélection

### Appel de méthode et paramètres

```cpp
/**
  * Ouvre une boîte de dialogue de saisie avec une boîte de sélection
  *
  * @param title {QString} titre de la boîte de dialogue
  * @param label {QString} texte de l'étiquette de la boîte de dialogue
  * @param items {QStringList} liste des éléments à sélectionner
  * @param current {int} index de l'élément à sélectionner (par défaut : 0)
  * @param editable {bool} si true, le texte de la boîte de dialogue peut être édité (par défaut : false)
  * @return {QString} texte de l'élément sélectionné
  */
QString ScriptingService :: inputDialogGetItem (
         const QString & title, const QString & label, const QStringList & items,
         int courant, booléen modifiable);
```

An empty string will be returned, if `Cancel` was clicked or `Escape` was pressed.

### Exemple

```js
var result = script.inputDialogGetItem("combo box", "Please select an item", [
  "Item 1",
  "Item 2",
  "Item 3",
]);
script.log(result);
```

Vous voudrez peut-être jeter un coup d'œil à l'exemple [input-dialogs.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/input-dialogs.qml).

## Ouverture d'une boîte de dialogue de saisie avec une modification de ligne

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

An empty string will be returned, if `Cancel` was clicked or `Escape` was pressed.

### Exemple

```js
var result = script.inputDialogGetText(
  "line edit",
  "Please enter a name",
  "current text",
);
script.log(result);
```

## Opening an input dialog with a multi-line text edit

### Appel de méthode et paramètres

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

### Exemple

```js
var result = script.inputDialogGetMultiLineText(
  "multi-line edit",
  "Please enter a text",
  "current text",
);
script.log(result);
```

## Opening a dialog to show the differences between two texts

### Appel de méthode et paramètres

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

### Exemple

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

## Vérifier si un fichier existe

### Method call and parameters

```cpp
/**
 * Vérifier si un fichier existe
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

## Lire du texte à partir d'un fichier

### Method call and parameters

```cpp
/**
 * Lire du texte à partir d'un fichier
 *
 * @param filePath {QString} chemin d'accès du fichier à charger
 * @param codec {QString} encodage du fichier (par défaut : UTF-8)
 * @return les données contenues dans le fichier ou 'null' si le fichier n'existe pas
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

## Écrire du texte dans un fichier

### Method call and parameters

```cpp
/**
 * Écrire du texte dans un fichier
 *
 * @param filePath {QString}
 * @param data {QString}
 * @param createParentDirs {bool} otionnel (par défaut : false)
 * @return
 */
bool ScriptingService::writeToFile(const QString &filePath, const QString &data, bool createParentDirs);
```

### Example

```js
var result = script.writeToFile(filePath, html);
script.log(result);
```

Vous voudrez peut-être jeter un coup d'œil à l'exemple [export-notes-as-one-html.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/export-notes-as-one-html.qml).

## Travailler avec des Websockets

Vous pouvez contrôler QOwnNotes à distance en utilisant `WebSocketServer`.

Veuillez jeter un œil à l'exemple [websocket-server.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/websocket-server.qml). Vous pouvez tester le serveur de sockets en vous y connectant sur [Websocket test](https://www.websocket.org/echo.html?location=ws://127.0.0.1:35345).

Vous pouvez également écouter les sockets avec `WebSocket`. Veuillez jeter un œil à l'exemple [websocket-client.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/websocket-client.qml).

Gardez à l'esprit que vous devez avoir la bibliothèque QML `websocket` de Qt installée pour utiliser cette fonction. Par exemple, sous Ubuntu Linux vous pouvez installer `module-qml-qtwebsockets`.

## Ajout d'une règle de mise en évidence pour l'éditeur

Il est possible d'injecter des règles de mise en évidence directement dans l'éditeur en définissant et assignant des expressions régulières à un état de mise en évidence.

### Method call and parameters

```cpp
/**
 * Ajout d'une règle de mise en évidence au surligneur de syntaxe de l'éditeur
 *
 * @param pattern {QString} le motif d'expression régulière à mettre en évidence
 * @param shouldContain {QString} une chaîne qui doit être contenue dans le texte mis en évidence pour que le motif soit analysé syntaxiquement
 * @param state {int} l'état du surligneur de syntaxe à utiliser
 * @param capturingGroup {int} le groupe de capture du motif à utiliser pour la mise en évidence (par défaut : 0)
 * @param maskedGroup {int} le groupe de capture du motif à utiliser pour la mise en évidence (par défault : 0)
 */
void ScriptingService::addHighlightingRule(const QString &pattern,
                                           const QString &shouldContain,
                                           int state,
                                           int capturingGroup,
                                           int maskedGroup);
```

### États de mise en évidence

| Nom                        | Numéro |
| -------------------------- | ------ |
| NoState                    | -1     |
| Lien                       | 0      |
| Image                      | 3      |
| CodeBlock                  | 4      |
| CodeBlockComment           | 5      |
| Italic                     | 7      |
| Gras                       | 8      |
| List                       | 9      |
| Commentaire                | 11     |
| H1                         | 12     |
| H2                         | 13     |
| H3                         | 14     |
| H4                         | 15     |
| H5                         | 16     |
| H6                         | 17     |
| BlockQuote                 | 18     |
| HorizontalRule            | 21     |
| Table                      | 22     |
| InlineCodeBlock            | 23     |
| MaskedSyntax               | 24     |
| CurrentLineBackgroundColor | 25     |
| BrokenLink                 | 26     |
| FrontmatterBlock           | 27     |
| TrailingSpace              | 28     |
| CheckBoxUnChecked          | 29     |
| CheckBoxChecked            | 30     |
| StUnderline                | 31     |

### Example

```js
// Highlight a text line like "BLOCK: some text" as blockquote (state 18)
script.addHighlightingRule("^BLOCK: (.+)", "BLOCK:", 18);

// Mask out (state 24) all characters after 32 characters in a line
// capturingGroup 1 means the expression from the first bracketed part of the pattern will be highlighted
// maskedGroup -1 means that no masking should be done
script.addHighlightingRule("^.{32}(.+)", "", 24, 1, -1);
```

Vous pouvez également jeter un œil aux exemples dans [highlighting.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/highlighting.qml).
