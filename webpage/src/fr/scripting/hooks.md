# Hooks

## onNoteStored

### Appel de méthode et paramètres

```js
/**
 * Cette fonction est appelée quand une note est stockée dans le disque
 * Vous ne pouvez pas modifier les notes stockées, ce serait le bazar puisque
 * vous les éditeriez sûrement à la main en même temps
 *
 * @param {NoteApi} note - l’objet de note de la note stockée
 */
function onNoteStored(note);
```

Vous voudrez peut-être jeter un coup d'œil à l'exemple [on-note-opened.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/on-note-opened.qml).

## noteOpenedHook

### Appel de méthode et paramètres

```js
/**
 * Cette fonction est appelée après l'ouverture d'une note
 *
 * @param {NoteApi} note - l'objet note qui a été ouvert
 */
function noteOpenedHook(note);
```

Vous voudrez peut-être jeter un coup d'œil à l'exemple [on-note-opened.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/on-note-opened.qml).

## noteDoubleClickedHook

### Appel de méthode et paramètres

```js
/**
 * Cette fonction est appelée après un double clic sur une note
 *
 * @param {NoteApi} note - l'objet de note sur lequel l'utilisateur a cliqué
 */
function noteDoubleClickedHook(note);
```

Vous voudrez peut-être jeter un coup d'œil à l'exemple [external-note-open.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/external-note-open.qml).

## insertMediaHook

Cette fonction est appelée lorsqu'un fichier multimédia est inséré dans la note actuelle.

Si cette fonction est définie dans plusieurs scripts, le premier script qui renvoie une chaîne non vide l'emporte.

### Appel de méthode et paramètres

```js
/**
 * @param fileName string le chemin du fichier du fichier multimédia source avant sa copie dans le dossier multimédia
 * @param markdownText string le texte en markdown du fichier multimédia, par exemple ![mon-image](média / mon-image-4101461585.jpg)
 * @return string le nouveau texte Markdown du fichier multimédia
 */
function insertMediaHook(fileName, markdownText);
```

Vous voudrez peut-être jeter un coup d'œil à l'exemple [example.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/example.qml).

## insertAttachmentHook

Cette fonction est appelée lorsqu'un fichier de pièce jointe est inséré dans la note actuelle.

Si cette fonction est définie dans plusieurs scripts, le premier script qui renvoie une chaîne non vide l'emporte.

### Appel de méthode et paramètres

```js
/**
 * @param fileName string le chemin du fichier de la pièce jointe source avant sa copie dans le dossier de la pièce jointe
 * @param markdownText string le texte en Markdown du fichier joint, par exemple [my-file.txt](pièces jointes/mon-fichier-4245650967.txt)
 * @return string le nouveau texte en Markdown du fichier joint
 */
function insertAttachmentHook(fileName, markdownText);
```

Vous voudrez peut-être jeter un coup d'œil à l'exemple [example.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/example.qml).

## insertingFromMimeDataHook

### Appel de méthode et paramètres

```js
/**
 * Cette fonction est appelée lorsque du HTML ou un fichier multimédia est collé dans une note avec `Ctrl + Maj + V`
 *
 * @param text texte de l'objet QMimeData
 * @param html HTML de l'objet QMimeData
 * @returns la chaîne à insérer à la place du texte de l'objet QMimeData
 */
function insertingFromMimeDataHook(text, html);
```

Vous voudrez peut-être jeter un coup d'œil aux exemples [example.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/example.qml), [insert-headline-with-link-from-github-url.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/insert-headline-with-link-from-github-url.qml) ou [note-text-from-5pm-mail.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/note-text-from-5pm-mail.qml).

## handleNoteTextFileNameHook

### Appel de méthode et paramètres

```js
/**
 * Cette fonction est appelée lorsqu'une note est stockée sur le disque si
 * « Autoriser le nom du fichier de note à être différent du titre » est activé
 * dans les préférences
 *
 * Elle vous permet de modifier le nom du fichier de note
 * Gardez à l'esprit que vous devez vous soucier des noms en double !
 *
 * Renvoie une chaîne vide si le nom de fichier de la note ne doit
 * pas être modifié
 *
 * @param {NoteApi} note - l'objet note de la note stockée
 * @return {string} le nom de fichier de la note
 */
function handleNoteTextFileNameHook(note);
```

Vous voudrez peut-être jeter un coup d'œil aux exemples [example.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/example.qml) ou [use-tag-names-in-filename.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/use-tag-names-in-filename.qml).

## handleNoteNameHook

### Appel de méthode et paramètres

```js
/**
 * Cette fonction est appelée lorsque le nom de la note est déterminé pour une note
 *
 * Elle vous permet de modifier le nom de la note qui est visualisée
 *
 * Renvoie une chaîne vide si le nom de la note ne doit pas être modifié
 *
 * @param {NoteApi} note - l'objet note de la note stockée
 * @return {string} le nom de la note
 */
function handleNoteNameHook(note);
```

Le nom de la note est surtout utilisé lorsque la note est affichée dans les listes.

Vous voudrez peut-être jeter un coup d'œil à l'exemple [example.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/example.qml).

Ce n'est peut-être pas une bonne idée d'utiliser ce hook si le paramètre permettant d'utiliser le nom de fichier comme nom de note est actif.

## handleNewNoteHeadlineHook

### Appel de méthode et paramètres

```js
/**
 * Cette fonction est appelée avant la création d'une note
 *
 * Elle vous permet de modifier le titre de la note avant sa création
 * Notez que vous devez faire attention à un nom de note unique, sinon
 * la nouvelle note ne sera pas créée, elle sera seulement présente dans la liste des notes
 *
 * Vous pouvez utiliser cette fonction pour créer des modèles de notes
 *
 * @param texte du titre qui serait utilisé pour créer le titre
 * @return {string} le titre de la note
 */
function handleNewNoteHeadlineHook(headline);
```

Vous pouvez jeter un œil à l'exemple [custom-new-note-headline.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-new-note-headline.qml).

## preNoteToMarkdownHtmlHook

### Appel de méthode et paramètres

```js
/**
 * Cette fonction est appelée avant la génération du code HTML Markdown d'une note
 *
 * Elle vous permet de modifier ce qui est passé au convertisseur Markdown vers HTML
 *
 * La fonction peut par exemple être utilisée dans de multiples scripts pour rendre du code (comme des maths LaTeX ou mermaid)
 * dans sa représentation graphique pour l'aperçu
 *
 * La note ne sera pas modifiée dans ce processus
 *
 * @param {NoteApi} note - l'objet note
 * @param {string} markdown - le Markdown qui doit être converti en HTML
 * @param {bool} forExport - true si le HTML est utilisé pour une exportation, false pour l'aperçu
 * @return {string} le Markdown modifié ou une chaîne vide si rien ne doit être modifié
 */
function preNoteToMarkdownHtmlHook(note, markdown, forExport);
```

Vous voudrez peut-être jeter un coup d'œil à l'exemple [preview-styling.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/preview-styling.qml).

## noteToMarkdownHtmlHook

### Appel de méthode et paramètres

```js
/**
 * Cette fonction est appelée lorsque le code HTML Markdown d'une note est généré
 *
 * Elle vous permet de modifier ce HTML
 * Elle est par exemple appelée avant par l'aperçu de la note
 *
 * Cette fonction peut être utilisée dans plusieurs scripts pour modifier le HTML de l'aperçu
 *
 * @param {NoteApi} note - l'objet note
 * @param {string} html - le HTML qui est doit être rendu
 * @param {bool} forExport - true si le HTML est utilisé pour une exportation, false pour l'aperçu
 * @return {string} le code HTML modifié ou une chaîne vide si rien ne doit être modifié
 */
function noteToMarkdownHtmlHook(note, html, forExport);
```

Vous voudrez peut-être jeter un coup d'œil aux exemples [example.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/example.qml) ou [preview-styling.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/preview-styling.qml).

Veuillez vous référer à la documentation du [sous-ensemble HTML pris en charge](http://doc.qt.io/qt-5/richtext-html-subset.html) pour une liste de tous les styles CSS pris en charge.

## encryptionHook

### Appel de méthode et paramètres

```js
/**
 * Cette fonction est appelée lorsque le texte doit être chiffré ou déchiffré
 *
 * @param text string le texte à crypter ou décrypter
 * @param password string le mot de passe
 * @param decrypt bool si un faux chiffrement est demandé, si un vrai déchiffrement est demandé
 * @return le texte déchiffré chiffré
 */
fonction encryptionHook(text, password, decrypt);
```

Vous voudrez peut-être jeter un coup d'œil aux exemples [encryption-keybase.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/encryption-keybase.qml), [encryption-pgp.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/encryption-pgp.qml) ou [encryption-rot13.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/encryption-rot13.qml).

## noteTaggingHook

Vous pouvez implémenter votre propre mécanisme de tagage de note, par exemple avec un texte spécial dans votre note comme `@tag1`, `@tag2`, `@tag3`.

### Appel de méthode et paramètres

```js
/**
 * Gère le tagage pour une note
 *
 * Cette fonction est appelée lorsque des tags sont ajoutés, supprimés ou renommés dans
 * une note ou que les tags d'une note doivent être listés
 *
 * @param note
 * L'action @param peut être « add », « remove », « rename » ou « list »
 * @param tagName nom du tag à ajouter, supprimer ou renommer
 * @param newTagName nom du tag à renommer si action = "rename"
 * @return note chaîne de texte ou chaîne-liste de noms de tags (si action = "list")
 */
function noteTaggingHook(note, action, tagName, newTagName);
```

- dès qu'un script qui implémente la nouvelle fonction `noteTaggingHook` est activé, le tagage des notes sera géré par cette fonction
- les fonctionnalités suivantes doivent fonctionner via l'interface utilisateur QOwnNotes
  - importation initiale de tags tels que `@tag` à partir de vos notes et écrasement de votre attribution de tag actuelle
    - vous ne perdrez pas votre arbre de tags, juste l'ancienne affectation aux notes
    - vous pouvez toujours déplacer des tags dans d'autres tags
    - si plus d'un tag porte le même nom dans votre arborescence de tags, le premier résultat sera attribué
  - l'ajout d'un tag à une note ajoutera le tag au texte de la note
  - la suppression d'un tag d'une note supprimera le tag du texte de la note
  - la suppression des tags dans la liste des tags supprimera ces tags de vos notes
  - renommer les tags dans la liste des tags renommera ces tags dans vos notes
  - le tagage en masse des notes dans la liste de notes ajoutera ces tags à vos notes
  - la suppression en masse des tags des notes de la liste de notes supprimera ces tags de vos notes
  - l'application déclenchera une série d'actions `ajouter` et `supprimer` pour tous les tags sélectionnés et leurs enfants sur toutes les notes si les tags sont déplacés dans le panneau des tags

Vous voudrez peut-être jeter un coup d'œil à l'exemple [note-tagging.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/note-tagging.qml) pour implémenter votre propre mécanisme de tagage.

::: warning
Assurez-vous que votre action `list` est vraiment rapide car elle sera exécutée pour chaque note à chaque rechargement du dossier de notes ou de l’arbre des tags !

Cela correspond à chaque vois que vous trouverez `reloadTagTree` dans [mainwindow.cpp](https://github.com/pbek/QOwnNotes/blob/main/src/mainwindow.cpp).
:::

## noteTaggingByObjectHook

De la même manière que [noteTaggingHook](#notetagginghook), vous pouvez implémenter votre propre mécanisme de tagage de note, mais vous n'êtes pas lié aux noms de tags dans la racine de l'arborescence des tags. De cette façon, vous pouvez utiliser l'intégralité de l'arborescence des tags au lieu d'une seule liste de tags.

Avec `noteTaggingByObjectHook`, vous obtenez un objet `TagApi` comme paramètre, au lieu d'un nom de tag. Et comme résultat pour l'action `list`, vous devez fournir une liste d'identifiants de tags.

Cela signifie également que vous devez créer vous-même les tags manquants pour pouvoir fournir une liste des identifiants de tags déjà existants pour l'action `list`.

### Appel de méthode et paramètres

```js
/ **
 * Gère le tagage des notes pour une note
 *
 * Cette fonction est appelée lorsque des tags sont ajoutés, supprimés ou renommés dans
 * une note ou que les tags d'une note doivent être listés
 *
 * @param note
 * @param action peut être « add », « remove », « rename » ou « list »
 * @param tag à ajouter, supprimer ou renommer
 * @param newTagName nom de la balise à renommer si action = "rename"
 * @return note chaîne de texte ou liste de chaînes d'identifiants de balises (si action = "list")
 * /
function noteTaggingByObjectHook(note, action, tag, newTagName);
```

Vous voudrez peut-être jeter un coup d'œil à l'exemple [note-tagging-by-object.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/note-tagging-by-object.qml) pour implémenter votre propre mécanisme de tagage.

## autocompletionHook

Vous pouvez renvoyer une liste de chaînes à ajouter à la liste de saisie semi-automatique lorsque la saisie semi-automatique est invoquée (par exemple en appuyant sur <kbd>Ctrl + Espace</kbd>).

### Appel de méthode et paramètres

```js
/**
 * Appelle la fonction autocompletionHook pour tous les composants de script
 * Cette fonction est appelée lorsque l'auto-complétion est invoquée dans une note
 *
 * @return QStringListe de texte pour la liste de saisie semi-automatique
 */
function callAutocompletionHook();
```

Vous voudrez peut-être jeter un coup d'œil à l'exemple [autocompletion.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/autocompletion.qml).

## websocketRawDataHook

Ce hook est appelé lorsque des données sont envoyées depuis l'extension de navigateur QOwnNotes Web Companion via le menu contextuel du navigateur Web.

### Appel de méthode et paramètres

```js
/**
 * @param requestType peut être "page" ou "selection"
 * @param pageUrl l’URL de la page web où la requête a été faite
 * @param pageTitle le titre de la page web où la requete a été faite
 * @param rawData les données qui ont été transmises : du HTML pour requestType="page" ou du texte brut pour requestType="selection"
 * @param screenshotDataUrl l’URL de données de la capture d’écran de la page web où la requête a été faite
 * @return true si les données ont été traitées par un hook
 */
function callHandleWebsocketRawDataHook(requestType, pageUrl, pageTitle, rawData, screenshotDataUrl);
```

Vous voudrez peut-être jeter un coup d’œil aux exemples [layouts.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/layouts.qml) et [websocket-raw-data-selection-in-note.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/websocket-raw-data-selection-in-note.qml).

## onDetachedProcessCallback

Ce hook est appelé lorsqu'un fil de script de [startDetachedProcess](methods-and-objects.html#starting-an-external-program-in-the-background) a fini de s’exécuter.

### Appel de méthode et paramètres

```js
/**
 * Cette fonction est appelée lorsqu'un fil de script a fini de s’exécuter.
 * Astuce : thread[1]==0 aide à déterminer si un lot de processus démarrés pour un certain identifiant est terminé.
 *
 * @param {QString} callbackIdentifier - l'identifiant fourni lors de l'appel de startDetachedProcess()
 * @param {QString} resultSet - le résultat du processus
 * @param {QVariantList} cmd - l'ensemble du tableau de commandes [0-executablePath, 1-parameters, 2-exitCode]
 * @param {QVariantList} thread - le tableau d'information sur les fils [0-callbackParameter passé, 1-fils restants pour cet identifiant]
 */
function onDetachedProcessCallback(callbackIdentifier, resultSet, cmd, thread);
```

Vous voudrez peut-être jeter un coup d'œil à l'exemple [callback-example.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/callback.qml).

## windowStateChangedHook

### Appel de méthode et paramètres

```js
/**
 * Cette fonction est appelée après le déclenchement d'un événement WindowStateChange
 *
 * @param {QString} windowState - le nouvel état de la fenêtre, la valeur du paramètre peut être "minimized", "maximized", "fullscreen", "active" ou "nostate"
 */
function windowStateChangedHook(windowState);
```

Vous voudrez peut-être jeter un coup d'œil à l'exemple [window-state-changed.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/window-state-changed.qml).

## layoutSwitchedHook

Ce hook est appelé quand la disposition est changée.

### Appel de méthode et paramètres

```js
/**
 * Cette fonction est appelée quand la disposition est changée
 *
 * @param oldUuid l’ancien UUID de disposition
 * @param newUuid le nouvel UUID de disposition
 */
function layoutSwitchedHook(oldUuid, newUuid);
```

Vous voudrez peut-être jeter un œil à l’exemple [layouts.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/layouts.qml).

## openAiBackendsHook

Ce hook est appelé quand la configuration du service OpenAI est rechargée. Par exemple, c’est également le cas lorsque le moteur de scripts est rechargé.

Vous pouvez l’utiliser pour fournir la configuration à des back-ends OpenAI personnalisés, comme vos propres LLM compatibles avec l’API d’OpenAI, tels que ChatGPT, Claude ou des systèmes comme Ollama.

### Appel de méthode et paramètres

```js
/**
 * Cette fonction est appelée quand la configuration du service OpenAI est rechargée
 * Elle renvoie une liste d’objets avec des préférences de configurations pour les nouveaux back-ends d’OpenAI
 */
function openAiBackendsHook() {
  return [
    {
      id: "mon-ia-personnalisee",
      name: "Mon IA personnalisée",
      baseUrl: "http://localhost:5000",
      apiKey: "kDFJkjk3asdm",
      models: ["gpt-3.5-turbo", "gpt-4.0-turbo"],
    },
    {
      id: "mon-ia-personnalisee2",
      name: "Mon IA personnalisée 2",
      baseUrl: "http://localhost:5001",
      apiKey: "lOikf7eNdb9",
      models: ["gpt-3.5-turbo2", "gpt-4.0-turbo2"],
    },
  ];
}
```

::: tip
Vous pouvez utiliser [Enregistrement des variables de préférences de script](./methods-and-objects.md#registering-script-settings-variables) pour stocker les préférences de back-end OpenAI dans les préférences de script.
:::

Vous voudrez peut-être jeter un coup d’œil à l’exemple [custom-openai-backends.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-openai-backends.qml).

## highlightingHook

Ce hook est appelé pour chaque bloc de texte dans l’éditeur lors de la coloration syntaxique. Il vous permet d’ajouter de la mise en évidence dynamique et contextuelle qui va plus loin que des simples expressions régulières statiques avec `addHighlightingRule`.

::: warning
Ce hook est appelé très fréquemment (pour chaque bloc de texte visible à chaque appui de touche), gardez donc votre implémentation rapide. Si vous n’avez besoin que de mise en évidence statique fondée sur des expressions régulières, préférez plutôt l’utilisation de [`addHighlightingRule`](methods-and-objects.md#adding-a-highlighting-rule-for-the-editor) ou [`addHighlightingRule` avec des couleurs personnalisées](methods-and-objects.md#adding-a-highlighting-rule-with-custom-colors-and-styles).
:::

### Appel de méthode et paramètres

```js
/**
 * Cette fonction est appelée pour chaque bloc de texte lors de la coloration syntaxique.
 * Elle permet de la mise en évidence contextuelle et dynamique.
 *
 * @param text {QString} le texte du bloc courant en cours de mise en évidence
 * @param previousBlockState {int} l’état de mise en évidence du bloc précédent
 *     (-1 si c’est le premier bloc)
 * @return {Array} un tableau d’objets highlight range, chacun avec :
 *     start {int} - position de départ dans le texte
 *     length {int} - nombre de caractères à mettre en évidence
 *     state {int} - le HighlighterState à utiliser (optionnel, -1 pour « personnalisé uniquement »)
 *     foregroundColor {string} - nom ou hexadécimal de la couleur de premier plan (optionnel)
 *     backgroundColor {string} - nom ou hexadécimal de la couleur d’arrière-plan (optionnel)
 *     bold {bool} - utiliser le gras (optionnel)
 *     italic {bool} - utiliser l’italique (optionnel)
 *     underline {bool} - souligner le texte (optionnel)
 *     fontSize {int} - taille de la police (optionnel, en pt.)
 */
function highlightingHook(text, previousBlockState) {
  var highlights = [];

  // Example: surligne toutes les occurences de « FIXME » avec un soulignage rouge et en gras
  var re = /FIXME/g;
  var match;
  while ((match = re.exec(text)) !== null) {
    highlights.push({
      start: match.index,
      length: match[0].length,
      foregroundColor: "#ff0000",
      underline: true,
      bold: true,
    });
  }

  return highlights;
}
```

Vous voudrez peut-être jeter un œil aux exemples [highlighting.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/highlighting.qml) et [custom-highlighting.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-highlighting.qml).
