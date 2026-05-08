---
image: /img/bookmarks.png
---

# Extension de navigateur QOwnNotes Web Companion

Permet de capturer du contenu depuis une page affichée dans un navigateur et de gérer les signets entre différents navigateurs et systèmes d'exploitation.

::: tip Info
**QOwnNotes doit être en cours d'exécution** pour que l'extension de navigateur Web Companion fonctionne. Aucune connexion Internet n’est requise, l’extension de navigateur fonctionne **hors-ligne**.
:::

## Installation

1. Téléchargez l'extension
   - [Chrome Web Store](https://chrome.google.com/webstore/detail/qownnotes-web-companion/pkgkfnampapjbopomdpnkckbjdnpkbkp)
   - [Firefox Add-ons page](https://addons.mozilla.org/firefox/addon/qownnotes-web-companion)
   - Vous pouvez également trouver l'extension sur [GitHub](https://github.com/qownnotes/web-companion/).
2. Ajoutez le jeton de sécurité pour configurer l'extension.
   - Une boîte de dialogue contenant un jeton de sécurité sera affichée la première fois que vous cliquerez sur l'extension de navigateur QOwnNotes. Copiez le jeton.
   - Ouvrez l'emplacement de gestion des extensions de votre navigateur. Cliquez sur les détails de l'extension QOwnNotes.
   - Copiez le jeton dans le champ « Jeton de sécurité ».
3. Pour utiliser l’extension en mode navigation privée, vous devez autoriser l’extension à s’exécuter dans les fenêtres privées dans ses préférences.

## Clipper Web

![web-clipper](/img/web-clipper.png)

Cliquez avec le bouton droit sur une page Web ou sur du texte sélectionné pour utiliser la fonctionnalité **web-clipper**. Vous pouvez ici également créer une nouvelle note avec une **capture d'écran** de la page Web actuelle.

::: warning
Info Si le clipper Web ne fonctionne pas, vérifiez que l’application de bureau QOwnNotes fonctionne, et que l’extension de navigateur est configurée avec le jeton de sécurité.

Vérifiez aussi que vous avez accepté la boîte de consentement lors de votre premier clic sur l’icône de l’extension dans la barre d’outils du navigateur.

Si vous utilisez Firefox, vous devez aussi vérifier que vous avez coché la permission optionnelle `Accéder à vos données pour tous les sites web` dans l’onglet _Autorisations et données_ des préférences de l’extension, sans quoi vous seriez susceptible d’obtenir l’erreur `TypeError: chrome.tabs.captureVisibleTab is not a function`.
:::

::: tip
Le Web Clipper est également scriptable ! Jetez un œil à [websocketRawDataHook](../scripting/hooks.md#websocketrawdatahook) si vous souhaitez contrôler ce que vous récupérez dans les pages Web.
:::

## Signets

![signets](/img/bookmarks.png)

Si vous souhaitez un contrôle total sur les signets de votre navigateur et **les utiliser sur différents navigateurs et systèmes d'exploitation**, l'extension de navigateur QOwnNotes est faite pour vous.

Par défaut, l'extension de navigateur affichera tous les **liens de la note actuelle** dans une fenêtre contextuelle lorsque vous cliquez sur l'icône QOwnNotes dans votre navigateur. Ces liens seront étiquettés `current`.

Vous pouvez également **gérer vos signets dans des notes** avec le tag de note `bookmarks` (personnalisable dans les préférences). Ces liens peuvent également avoir des tags et une description qui seront affichés dans l'extension de navigateur.

Les nouveaux signets sont stockés dans une note appelée `Bookmarks` (également personnalisable dans les préférences).

::: tip
Vous pouvez également importer les signets de votre navigateur dans QOwnNotes avec l’extension Web Companion !
:::

### Syntaxe des liens de signets

```markdown
- [Nom de page Web](https://www.example.com)
- [Nom de page Web](https://www.example.com) #tag1 #tag2
- [Nom de page Web](https://www.example.com) uniqement une description
- [Nom de page Web](https://www.example.com) #tag1 #tag2 une description et des tags

* [Nom de page Web](https://www.example.com) l’autre caractère de liste fonctionne aussi
```

Il est possible de rechercher un nom, des tags d'URL ou une description dans l'extension de navigateur. Dans la note courante, tous les autres liens dans le corps du texte sont analysés, mais sans tags ni description.

## API de suggestion Homepage

Pour l’intégration de [Homepage](https://github.com/gethomepage/homepage) avec les suggestions de signets QOwnNotes, voir [API de suggestion Homepage](./homepage-suggestion-api.md).
