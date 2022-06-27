---
image: /img/bookmarks.png
---

# Extension de navigateur QOwnNotes Web Companion

Permet de capturer du contenu depuis une page affichée dans un navigateur et de gérer les favoris entre différents navigateurs et systèmes d'exploitation.

::: tip
Info
- ::: tip Info
QOwnNotes doit être en cours d'exécution pour que l'extension de navigateur Web Companion fonctionne.
- Une connexion Internet n'est pas nécessaire. L'extension de navigateur fonctionne **hors-ligne**.
:::

## Installation

1. Obtenir l'extension
    - [Chrome Web Store](https://chrome.google.com/webstore/detail/qownnotes-web-companion/pkgkfnampapjbopomdpnkckbjdnpkbkp)
    - [Firefox Add-ons page](https://addons.mozilla.org/firefox/addon/qownnotes-web-companion)
    - Vous pouvez également trouver l'extension sur [GitHub](https://github.com/qownnotes/web-companion/).
2. Ajouter le jeton de sécurité pour configurer l'extension.
    - Une boîte de dialogue contenant un jeton de sécurité sera affichée la première fois que vous cliquerez sur l'extension de navigateur QOwnNotes. Copiez le jeton.
    - Ouvrez l'emplacement de gestion des extensions de votre navigateur. Cliquez sur les détails de l'extension QOwnNotes.
    - Copiez le jeton dans le champ "Jeton de sécurité".

## Clipper Web

![web-clipper](/img/web-clipper.png)

Cliquez avec le bouton droit sur une page Web ou sur du texte sélectionné pour utiliser la fonctionnalité **web-clipper**. Vous pouvez ici également créer une nouvelle note avec une **capture d'écran** de la page Web actuelle.

::: tip
Le Web Clipper est également scriptable ! Jetez un œil à [websocketRawDataHook](../scripting/hooks.md#websocketrawdatahook) si vous souhaitez contrôler ce que vous récupérez dans les pages Web.
:::

## Favoris

![signets](/img/bookmarks.png)

Si vous souhaitez un contrôle total sur les favoris de votre navigateur et **les utiliser sur différents navigateurs et systèmes d'exploitation**, l'extension de navigateur QOwnNotes est faite pour vous.

Par défaut, l'extension de navigateur affichera tous les **liens de la note actuelle** dans une fenêtre contextuelle lorsque vous cliquez sur l'icône QOwnNotes dans votre navigateur. Ces liens seront étiquettés `current`.

Vous pouvez également **gérer vos favoris dans des notes** avec l'étiquette de note `bookmarks` (personnalisable dans les paramètres). Ces liens peuvent également avoir des étiquettes et une description qui seront affichées dans l'extension de navigateur.

Les nouveaux favoris sont stockés dans une note appelée `Bookmarks` (également personnalisable dans les paramètres).

::: tip
Vous pouvez également importer les favoris de votre navigateur dans QOwnNotes avec Web Compagnon !
:::

### Syntaxe des liens de favoris

```markdown
- [Webpage name](https://www.example.com)
- [Webpage name](https://www.example.com) #étiquette1 #étiquette2
- [Webpage name](https://www.example.com) juste une description
- [Webpage name](https://www.example.com) #étiquette1 #étiquette2 une description et des étiquettes
* [Webpage name](https://www.example.com) le caractère alternatif pour un élément de liste fonctionne également
```

Il est possible de rechercher un nom, des étiquettes d'URL ou une description dans l'extension de navigateur.
