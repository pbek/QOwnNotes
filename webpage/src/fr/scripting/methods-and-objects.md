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
  "/chemin/vers/mon/programme",
  ["mon paramètre"],
  "données",
  "/chemin/où/s’exécuter",
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
Vous pouvez également attribuer des raccourcis locaux ou globaux à vos actions personnalisées dans les _Préférences des raccourcis_.
:::

::: warning
Soyez attentif au fait que les [icônes du thème freedesktop](https://specifications.freedesktop.org/icon-naming-spec/icon-naming-spec-latest.html) ne sont généralement disponibles que sous Linux. À cause de cela, si vous souhaitez vraiment utiliser un icône sous macOS ou Windows vous devrez la fournir avec votre script. Pour obtenir le chemin vers votre script afin de définir un chemin correct vers votre icône, utilisez la [propriété scriptDirPath](methods-and-objects.md#reading-the-path-to-the-directory-of-your-script).
:::

### Exemple

```js
import QtQml 2.0
import QOwnNotesTypes 1.0

Script {
    /**
     * Initialise les actions personnalisées
     */
    function init() {
        // ajoute une action personnalisée sans bouton
        script.registerCustomAction("monaction1", "Texte de menu");

        // ajoute une action personnalisée avec un bouton
        script.registerCustomAction("monaction2", "Texte de menu", "Texte de bouton");

        // ajoute une action personnalisée avec un bouton et une icône de thème freedesktop
        script.registerCustomAction("monaction3", "Texte de menu", "Texte de bouton", "task-new");

        // ajoute une action personnalisée avec un bouton et une icône stockée dans un fichier
        script.registerCustomAction("monaction4", "Texte de menu", "Texte de bouton", "/usr/share/icons/breeze/actions/24/view-calendar-tasks.svg");
    }

    /**
     * Cette fonction est invoquée lorsqu’une action personnalisée
     * est déclenchée dans le menu ou via un bouton
     *
     * @param identifier string l’identifiant défini dans registerCustomAction
     */
    function customActionInvoked(identifier) {
        switch (identifier) {
            case "monaction1":
                script.log("Action 1");
            break;
            case "monaction2":
                script.log("Action 2");
            break;
            case "monaction3":
                script.log("Action 3");
            break;
            case "monaction4":
                script.log("Action 4");
            break;
        }
    }
}
```

Pour d'autres exemples, voir [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-actions.qml).

::: tip
Vous pouvez également déclencher une action personnalisée après le démarrage de l'application avec le paramètre `--action customAction_<identifier>`. Pour plus d'informations, veuillez consulter [Déclencher des actions de menu après le démarrage](../getting-started/cli-parameters.md#trigger-menu-actions-after-startup).
:::

## Enregistrer une étiquette

### Appel de méthode et paramètres

```cpp
/**
 * Enregistre une étiquette sur laquelle écrire
 *
 * @param identifier l’identifiant de l’étiquette
 * @param text le texte affiché dans l’étiquette (optionnel)
 */
void ScriptingService::registerLabel(QString identifier, QString text);
```

### Exemple

```js
script.registerLabel(
  "html-label",
  "Du texte HTML <strong>en gras</strong><br />avec trois lignes<br />et un <a href='https://www.qownnotes.org'>lien vers un site web</a>.",
);

script.registerLabel(
  "long-label",
  "un long texte, un autre long texte, un autre long texte, un autre long texte, un autre long texte, un autre long texte, un autre long texte, un autre long texte, un autre long texte, un autre long texte, un autre long texte, un autre long texte, un autre long texte qui se fera couper par des retour-lignes",
);

script.registerLabel("counter-label");
```

Les étiquettes seront visibles dans le panneau _Écriture de scripts_, activable depuis le menu _Fenêtre / Panneaux_.

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
Les étiquettes de scriptage supportent aussi les **liens externes** ansi que les **liens internes** vers les notes, tâches, decks et plus. Vous pouvez vous faire une idée de ce qui est supporté en jetant un œil à la [prise en charge des URL](https://github.com/pbek/QOwnNotes/blob/964acf69b6382f8ee8252c640c5048f8f4644622/src/utils/urlhandler.cpp#L16-L75).
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
Si vous avez désactivé le fait que le titre de votre note détermine le nom de fichier de la note, vous devrez par la suite renommer vous-même votre fichier de note comme ceci :

```js
var note = script.currentNote();
note.renameNoteFile("votre-nom-de-fichier");
```

:::

## Accéder au presse-papiers

### Appel de méthode et paramètres

```cpp
/**
 * Renvoie le contenu du presse-papier sous forme de texte ou de HTML
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
script.noteTextEditWrite("Mon texte personnalisé");
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
// étend la sélection courante d’un caractère
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
 *                               des caractères de mots
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

## Lire le bloc actuel dans le texte de la note

### Appel de méthode et paramètres

```cpp
/**
 * Lit le bloc actuel dans le texte de la note
 *
 * @return
 */
QString ScriptingService::noteTextEditCurrentBlock();
```

### Exemple

```js
// Lit le bloc actuel dans le texte de la note
var text = script.noteTextEditCurrentBlock();
```

Vous voudrez peut-être jeter un coup d’œil à l’exemple [ai-autocompletion.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/ai-autocompletion.qml).

## Utiliser un prompt de complétion sur le modèle d’IA sélectionné

Le prompt de complétion d’IA est une fonctionnalité permettant d’utiliser un prompt de complétion sur le modèle d’IA sélectionné, comme ChatGPT, Claude ou des systèmes comme Ollama.

Le système d’IA nécessite d’être activé dans la barre d’outils d’IA ou le menu principal pour que cette méthode fonctionne.

### Appel de méthode et paramètres

```cpp
/**
 * Wrapper QML pour utiliser le compléteur AI
 *
 * @param prompt
 * @return {QString} le résultat du compléteur
 */
QString ScriptingService::aiComplete(const QString& prompt);
```

### Exemple

```js
// Demander au modèle d’IA sélectionné de compléter le prompt
var text = script.aiComplete("Dis-moi, comment te sens-tu aujourd’hui ?");
```

Vous voudrez peut-être jeter un coup d’œil à l’exemple [ai-autocompletion.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/ai-autocompletion.qml).

## Déterminer si la plate-forme est Linux, macOS ou Windows

### Appel de méthode et paramètres

```cpp
bool ScriptingService::platformIsLinux();
bool ScriptingService::platformIsOSX();
bool ScriptingService::platformIsWindows();
```

### Exemple

```js
if (script.platformIsLinux()) {
  // ne sera exécuté que sous Linux
}
```

## Taguer la note actuelle

### Appel de méthode et paramètres

```cpp
/**
 * Tague la note courante avec un tag nommé tagName
 *
 * @param tagName
 */
void ScriptingService::tagCurrentNote(QString tagName);
```

### Exemple

```js
// ajouter un tag "favori" à la note courante
script.tagCurrentNote("favori");
```

Vous voudrez peut-être jeter un coup d'œil à l'action personnalisée `favoriteNote` dans l'exemple [favorite-note.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/favorite-note.qml).

## Créer ou récupérer un tag par son nom de liste de fil d'Ariane

### Appel de méthode et paramètres

```cpp
/**
 * Créé ou récupère un tag par son nom de liste de fil d'Ariane
 * Element nameList[0] serait le plus élevé dans l'arborescence (avec parentId: 0)
 *
 * @param nameList
 * @param createMissing {bool} si true (défaut), tous les tags manquants seront créés
 * @return TagApi l’objet du tag le plus profond du nom de liste de fil d'Ariane
 */
TagApi *ScriptingService::getTagByNameBreadcrumbList(
    const QStringList &nameList, bool createMissing);
```

### Exemple

```js
// crée tous les tags jusqu'au 3ème niveau et renvoie l'objet tag pour le
// tag « level3 », qui ressemblerait à ceci dans l'arborescence des tags :
// level1 > level2 > level3
var tag = script.getTagByNameBreadcrumbList(["level1", "level2", "level3"]);
```

## Rechercher des tags par nom

### Appel de méthode et paramètres

```cpp
/**
 * Récupère tous les tags par le biais d'une recherche de sous-chaîne sur le champ « nom ».
 *
 * @param name {QString} nom à rechercher
 * @return {QStringList} liste des noms des tags
 */
QStringList ScriptingService::searchTagsByName(QString name);
```

### Exemple

```js
// recherche tous les tags contenant le mot « jeu »
var tags = script.searchTagsByName("jeu");
```

Vous voudrez peut-être jeter un coup d'œil à l'exemple [autocompletion.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/autocompletion.qml).

## Rechercher des notes à partir de texte contenu dans le corps d'une note

### Appel de méthode et paramètres

```cpp
/**
 * Renvoie une liste des identifiants de toutes les notes contenant un certain texte dans leur corps.
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

  // faire quelque chose de la note
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

Jetez un œil à [Style Sheet Reference](http://doc.qt.io/qt-5/stylesheet-reference.html) pour une référence des styles disponibles.

Si vous souhaitez injecter des styles dans l'aperçu HTML pour modifier la façon dont les notes sont prévisualisées, veuillez consulter [notetomarkdownhtmlhook](hooks.html#notetomarkdownhtmlhook).

::: tip
Si vous souhaitez voir l'aspect des dialogues et quels sont leurs noms, téléchargez [Qt Creator](https://www.qt.io/product/development-tools) et ouvrez les fichiers `*.ui` qu'il contient.
:::

## Recharger le moteur de scripts

### Appel de méthode et paramètres

```cpp
/**
 * Recharge le moteur de scripts
 */
void ScriptingService :: reloadScriptingEngine ();
```

### Exemple

```js
// recharger le moteur de scripts
script.reloadScriptingEngine();
```

## Récupérer une note par son nom de fichier

### Appel de méthode et paramètres

```cpp
/**
 * Récupère une note par son nom de fichier
 *
 * @param fileName string le nom de fichier de la note (obligatoire)
 * @param noteSubFolderId integer ID entier du sous-dossier de notes
 * @return NoteApi *
 */
NoteApi * ScriptingService :: fetchNoteByFileName (QString fileName,
                                                 int noteSubFolderId);
```

### Exemple

```js
// récupère la note par nom de fichier
script.fetchNoteByFileName("ma note.md");
```

## Récupérer une note par son identifiant

### Appel de méthode et paramètres

```cpp
/**
 * Récupère une note par son identifiant
 *
 * @param id int l'identifiant de la note
 * @return NoteApi*
 */
NoteApi* ScriptingService::fetchNoteById(int id);
```

### Exemple

```js
// récupère la note par identifiant
script.fetchNoteById(243);
```

Vous voudrez peut-être jeter un coup d'œil à l'exemple [export-notes-as-one-html.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/export-notes-as-one-html.qml).

## Vérifier si une note existe par son nom de fichier

### Appel de méthode et paramètres

```cpp
/**
 * Vérifie si un fichier de note existe par son nom de fichier
 *
 * @param fileName string le nom de fichier de la note (obligatoire)
 * @param ignoreNoteId integer identifiant entier d'une note à ignorer lors de la vérification
 * @param noteSubFolderId integer ID entier du sous-dossier de notes
 * @return bool
 */
booléen ScriptingService :: noteExistsByFileName (QString fileName,
                                             int ignoreNoteId,
                                             int noteSubFolderId);
```

### Exemple

```js
// vérifie si la note existe, mais ignore l'id de « note »
script.noteExistsByFileName("ma note.md", note.id);
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
script.setClipboardText("texte à copier");
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
// saute vers le sous-dossier de notes « un sous-dossier »
script.jumpToNoteSubFolder("un sous-dossier");

// saute vers le sous-dossier de notes « sub » à l'intérieur de « un sous-dossier »
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
// affiche une boîte de message de question avec un boton « appliquer » et un bouton « aide »
// voir : https://doc.qt.io/qt-5/qmessagebox.html#StandardButton-enum
var result = script.questionMessageBox(
  "Le texte à afficher",
  "Un titre optionnel",
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

### Exemple

```js
// affiche une boîte de dialogue d’ouverture du fichier
var fileName = script.getOpenFileName(
  "Veuillez sélectionner une image",
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
// affiche une boîte de dialogue d’enregistrement de fichier
var fileName = script.getSaveFileName(
  "Sélectionnez un fichier HTML dans lequel sauvegarder",
  "output.html",
  "HTML (*.html)",
);
```

Vous voudrez peut-être jeter un coup d'œil à l'exemple [export-notes-as-one-html.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/export-notes-as-one-html.qml).

## Enregistrement des variables de préférences de script

Vous devez définir vos variables de préférences en tant que propriétés dans votre script et les enregistrer dans une propriété nommée `settingsVariables`.

L’utilisateur peut ensuite définir ces propriétés dans les préférences du script.

### Exemple

```js
// vous devez définir vos variables afin d’y accéder plus tard
property string myString;
property string myStringSecret;
property bool myBoolean;
property string myText;
property int myInt;
property string myFile;
property string myDirectory;
property string mySelection;

// enregistrez vos variables de préférences afin que l’utilisateur puisse les modifiers dans les préférences de scripts
//
// malheureusement, il n’y a pas de QVariantHash dans Qt, il est uniquement possible d’utiliser
// QVariantMap (qui n’a pas d’ordre arbitraire) or QVariantList (qui au moins peut être arbitrairement ordonné)
property variant settingsVariables: [
    {
        "identifier": "myString",
        "name": "Je suis un champ d’édition de ligne",
        "description": "Merci d’entrer une chaîne valide :",
        "type": "string",
        "default": "Ma valeur par défaut",
    },
    {
        "identifier": "myStringSecret",
        "name": "Je suis un champ de mot de passe",
        "description": "Merci d’entrer une chaîne valide :",
        "type": "string-secret",
    },
    {
        "identifier": "myBoolean",
        "name": "Je suis une case à cocher",
        "description": "Une description",
        "text": "Cochez cette case",
        "type": "boolean",
        "default": true,
    },
    {
        "identifier": "myText",
        "name": "Je suis une boîte de texte",
        "description": "Veulliez entrer votre texte :",
        "type": "text",
        "default": "Ça peut être un texte très long\nsur plusieurs lignes.",
    },
    {
        "identifier": "myInt",
        "name": "Je suis un sélecteur de nombres",
        "description": "Veuillez entrer un nombre :",
        "type": "integer",
        "default": 42,
    },
    {
        "identifier": "myFile",
        "name": "Je suis un sélecteur de fichier",
        "description": "Veuillez sélectionner le fichier :",
        "type": "file",
        "default": "pandoc",
    },
    {
        "identifier": "myDirectory",
        "name": "Je suis un sélecteur de dossier",
        "description": "Veuillez sélectionner le dossier :",
        "type": "directory",
        "default": "/home",
    },
    {
        "identifier": "mySelection",
        "name": "Je suis un sélecteur d’élément",
        "description": "Veuillez sélectionner un élément :",
        "type": "selection",
        "default": "option2",
        "items": {"option1": "Texte de l’option 1", "option2": "Texte de l’option 2", "option3": "Texte de l’option 3"},
    }
];
```

De plus, vous pouvez outrepasser les `settingsVariables` avec une fonction spéciale `registerSettingsVariables ()` comme ceci :

### Exemple

```js
/**
 * Enregistre à nouveau les variables de préférences
 *
 * Utilisez cette méthode si vous souhaitez utiliser du code pour outrepasser vos variables,
 * telles que le réglage des valeurs par défaut dépendant du système d'exploitation.
 */
function registerSettingsVariables() {
  if (script.platformIsWindows()) {
    // outrepasse la valeur par défaut de myFile
    settingsVariables[3].default = "pandoc.exe";
  }
}
```

Vous voudrez peut-être jeter un coup d'œil à l'exemple [variables.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/variables.qml).

## Stockage et chargement de variables persistantes

### Appel de méthode et paramètres

```cpp
/**
 * Stocke une variable persistante
 * Ces variables sont accessibles globalement dans tous les scripts
 * Utilisez un préfixe utile dans votre clé tel que « PersistentVariablesTest/myVar »
 *
 * @param key {QString}
 * @param value {QVariant}
 */
void ScriptingService::setPersistentVariable(const QString &key,
                                             const QVariant &value);

/**
 * Charge une variable persistante
 * Ces variables sont accessibles globalement dans tous les scripts
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
// stocker une variable persistante
script.setPersistentVariable("PersistentVariablesTest/myVar", result);

// charge et affiche dans le journal la variable persistante
script.log(
  script.getPersistentVariable(
    "PersistentVariablesTest/myVar",
    "rien encore ici",
  ),
);
```

Veuillez vous assurer d'utiliser un préfixe explicite dans votre clé, tel que `PersistentVariablesTest / myVar` car les variables sont accessibles depuis tous les scripts.

Vous voudrez peut-être également jeter un coup d'œil à l'exemple [persistent-variables.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/persistent-variables.qml).

## Chargement des variables des préférences de l’application

### Appel de méthode et paramètres

```cpp
/**
 * Charge une variable des préférences de l’application
 *
 * @param key {QString}
 * @param defaultValue {QVariant} valeur de retour si la préférence n’existe pas (optionnelle)
 * @return
 */
QVariant ScriptingService::getApplicationSettingsVariable(const QString &key,
                                                          const QVariant &defaultValue);
```

### Exemple

```js
// charger et enregistrer une variable des préférences de l’application
script.log(script.getApplicationSettingsVariable("gitExecutablePath"));
```

Gardez à l'esprit que les préférences peuvent être vides, vous devez vous en occuper vous-même. `defaultValue` n'est utilisé que si le paramètre n'existe nulle part.

## Créer un répertoire de cache

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
// vider le répertoire cache de mon-id-de-script
script.clearCacheDir("mon-id-de-script");
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
// renverra « c:\winnt\system32 » sous Windows
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
// renverra « c:/winnt/system32 » sous Windows
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
// renverra « \ » sous Windows
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

Une chaîne vide sera renvoyée si `Annuler` a été cliqué ou si la touche `Échap` a été pressée.

### Exemple

```js
var result = script.inputDialogGetItem("combo box", "Veuillez sélectionner un élément", [
  "Élément 1",
  "Élément 2",
  "Élément 3",
]);
script.log(result);
```

Vous voudrez peut-être jeter un coup d'œil à l'exemple [input-dialogs.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/input-dialogs.qml).

## Ouverture d'une boîte de dialogue de saisie de texte sur une ligne

### Appel de méthode et paramètres

```cpp
/ **
  * Ouvre une boîte de dialogue de saisie de texte sur une ligne
  *
  * @param title {QString} titre de la boîte de dialogue
  * @param label {QString} texte de l'étiquette de la boîte de dialogue
  * @param text {QString} texte dans la boîte de dialogue (facultatif)
  * @ return
 */
QString ScriptingService::inputDialogGetText(
        const QString &title, const QString &label, const QString &text);
```

Une chaîne vide sera renvoyée si `Annuler` a été cliqué ou si la touche `Échap` a été pressée.

### Exemple

```js
var result = script.inputDialogGetText(
  "édition de ligne",
  "Veuillez entrer un nom",
  "texte actuel",
);
script.log(result);
```

## Ouverture d'une boîte de dialogue de saisie de texte sur plusieurs lignes

### Appel de méthode et paramètres

```cpp
/**
 * Ouvre une boîte de dialogue de saisie de texte sur plusieurs lignes
 *
 * @param title {QString} titre de la boîte de dialogue
 * @param label {QString} texte d’étiquette du dialogue
 * @param text {QString} texte dans le dialogue (optionnel)
 * @return
 */
QString ScriptingService::inputDialogGetMultiLineText(
        const QString &title, const QString &label, const QString &text);
```

Une chaîne vide sera renvoyée si `Annuler` a été cliqué ou si la touche `Échap` a été pressée.

### Exemple

```js
var result = script.inputDialogGetMultiLineText(
  "édition multi-ligne",
  "Veuillez entrer un texte",
  "texte courant",
);
script.log(result);
```

## Ouverture d’une boîte de dialogue montrant les différences entre deux textes

### Appel de méthode et paramètres

```cpp
/**
* Ouvre une boîte de dialogue montrant la différence entre deux textes et laisse l’utilisateur modifier le résultat
*
* @param title {QString} titre de la boîte de dialogue
* @param label {QString} étiquette du dialogue
* @param text1 {QString} premier texte
* @param text2 {QString} second texte
* @return
  */
  QString ScriptingService::textDiffDialog(const QString &title, const QString &label,
                                           const QString &text1, const QString &text2);
```

`text2` est le texte qu’il sera possible de modifier dans la boîte de dialogue. Une chaîne vide sera renvoyée si `Annuler` a été cliqué ou si la touche `Échap` a été pressée.

### Exemple

```js
const text = script.noteTextEditSelectedText();
const aiPrompt = "Translate the text to English";
const aiResult = script.aiComplete(aiPrompt + ":\n\n" + text);

var result = script.textDiffDialog(
  "Outil de texte d’IA",
  "Texte résultant",
  text,
  aiResult,
);
script.log(result);
```

## Vérifier si un fichier existe

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

## Lire du texte à partir d'un fichier

### Appel de méthode et paramètres

```cpp
/**
 * Lire du texte à partir d'un fichier
 *
 * @param filePath {QString} chemin d'accès du fichier à charger
 * @param codec {QString} encodage du fichier (par défaut : UTF-8)
 * @return les données contenues dans le fichier ou null si le fichier n'existe pas
 */
QString ScriptingService::readFromFile(const QString &filePath, const QString &codec)
```

### Exemple

```js
if (script.fileExists(filePath)) {
  var data = script.readFromFile(filePath);
  script.log(data);
}
```

## Écrire du texte dans un fichier

### Appel de méthode et paramètres

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

### Exemple

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

### Appel de méthode et paramètres

```cpp
/**
 * Ajoute une règle de mise en évidence au coloriseur syntaxique de l’éditeur
 *
 * @param pattern {QString} l’expression régulière représentant le motif à mettre en évidence
 * @param shouldContain {QString} une chaîne qui doit être contenue dans le texte mis en évidence pour que le motif soit analysé
 * @param state {int} l’état du coloriseur syntaxique à utiliser
 * @param capturingGroup {int} le groupe de capture du motif à utiliser pour la mise en évidence (défaut : 0)
 * @param maskedGroup {int} le groupe de capture du motif à utiliser pour le masquage (défaut : 0)
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
| Link                       | 0      |
| Image                      | 3      |
| CodeBlock                  | 4      |
| CodeBlockComment           | 5      |
| Italic                     | 7      |
| Gras                       | 8      |
| List                       | 9      |
| Comment                    | 11     |
| H1                         | 12     |
| H2                         | 13     |
| H3                         | 14     |
| H4                         | 15     |
| H5                         | 16     |
| H6                         | 17     |
| BlockQuote                 | 18     |
| HorizontalRule             | 21     |
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

### Exemple

```js
// Met en évidence une ligne de texte telle que « BLOCK: du texte » comme bloc de citation (état 18 : blockquote)
script.addHighlightingRule("^BLOCK: (.+)", "BLOCK:", 18);

// Masque (état 24) tous les caractères après 32 caractères dans une ligne
// capturingGroup 1 signifie que l’expression à partir de la première partie entre crochets sera mise en évidence
// maskedGroup -1 signifie qu’aucun masquage ne doit être fait
script.addHighlightingRule("^.{32}(.+)", "", 24, 1, -1);
```

Vous pouvez également jeter un œil aux exemples dans [highlighting.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/highlighting.qml).

## Ajouter une règle de mise en évidence avec des couleurs et styles personnalisés

Vous pouvez également ajouter des règles de mise en évidence avec des couleurs de premier/arrière plan et des styles de polices personnalisés, au lieu d’être limité par les états de mise en évidence prédéfinis. Cela vous permet de définir vos propres schémas de couleur pour des motifs syntaxiques personnalisés.

### Appel de méthode et paramètres

```cpp
/**
 * Ajoute une règle de mise en évidence personnalisée au coloriseur syntaxique
 *
 * @param pattern {QString} l’expression régulière du motif à mettre en évidence
 * @param shouldContain {QString} une chaîne devant être contenue dans le texte mis en évidence pour que le motif soit analysé
 * @param state {int} l’état de mise en évidence à utiliser (utilisez -1 / NoState pour les formats personnalisés uniquement)
 * @param capturingGroup {int} le groupe de capture du motif à utiliser pour la mise en évidence
 * @param maskedGroup {int} le groupe de capture du motif à utiliser pour le masquage
 * @param formatStyle {QVariantMap} une association (map) avec des propriétés de formatage personnalisées :
 *   - foregroundColor {QString} nom ou valeur hexadécimale de couleur de premier-plan (par ex. "#ff0000" ou "red")
 *   - backgroundColor {QString} nom ou valeur hexadécimale de couleur d’arrière-plan
 *   - bold {bool} utiliser le gras ou non
 *   - italic {bool} utiliser l’italique ou non
 *   - underline {bool} souligner ou non
 *   - fontSize {int} la taille le la police (pt.)
 */
void ScriptingService::addHighlightingRule(const QString &pattern,
                                            const QString &shouldContain,
                                            int state,
                                            int capturingGroup,
                                            int maskedGroup,
                                            const QVariantMap &formatStyle);
```

::: tip
Vous pouvez combiner un `state` prédéfini avec des propriétés de formatage personnalisées. Les propriétés personnalisées écraseront les valeurs par défaut de l’état. Utilisez l’état `-1` (`NoState`) si vous ne voulez utiliser que du formattage personnalisé.
:::

### Exemple

```js
function init() {
  // Met en évidence « IMPORTANT » en gras avec un texte en rouge sur un fond jaune
  script.addHighlightingRule("IMPORTANT", "IMPORTANT", -1, 0, 0, {
    foregroundColor: "#ff0000",
    backgroundColor: "#ffff00",
    bold: true,
  });

  // Met en évidence les mentions « @nomdutilisateur » avec un texte bleu souligné
  script.addHighlightingRule("@\\w+", "@", -1, 0, 0, {
    foregroundColor: "#3366cc",
    underline: true,
  });

  // Met en évidence « NOTE: » avec un texte vert en italique
  script.addHighlightingRule("NOTE:", "NOTE:", -1, 0, 0, {
    foregroundColor: "#00aa00",
    italic: true,
  });
}
```

Vous pouvez également jeter un œil aux examples dans [highlighting.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/highlighting.qml) et [custom-highlighting.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-highlighting.qml).
