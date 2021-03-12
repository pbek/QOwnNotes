---
image: /img/bookmarks.png
---

# Extension de navigateur QOwnNotes Web Companion

![bookmarks](/img/bookmarks.png)

Accédez au [Chrome Web Store](https://chrome.google.com/webstore/detail/qownnotes-web-companion/pkgkfnampapjbopomdpnkckbjdnpkbkp) ou à la [page des modules complémentaires Firefox](https://addons.mozilla.org/firefox/addon/qownnotes-web-companion) pour installer l '[**extension de navigateur QOwnNotes Web Companion**](https://github.com/qownnotes/web-companion/).

Vous pouvez également trouver l'extension sur [GitHub](https://github.com/qownnotes/web-companion/).

## Clipper Web

Cliquez avec le bouton droit sur une page Web ou le texte sélectionné pour utiliser la fonctionnalité **web-clipper**. Là, vous pouvez également créer une nouvelle note avec une **capture d'écran** de la page Web actuelle.

::: tip
Le clipper Web est également scriptable! Jetez un œil au [websocketRawDataHook](../scripting/hooks.md#websocketrawdatahook) si vous souhaitez contrôler ce que vous découpez à partir de pages Web.
:::

## Favoris

Par défaut, l'extension de navigateur affichera tous les liens ** de la note actuelle** dans une fenêtre contextuelle lorsque vous cliquez sur l'icône QOwnNotes dans votre navigateur. Ces liens recevront une balise `current`.

Vous pouvez également **gérer vos signets dans les notes** avec la balise de note `signets` (modifiable dans les paramètres). Ces liens peuvent également avoir des balises et une description qui seront affichées dans l'extension du navigateur.

Les nouveaux signets sont stockés dans une note appelée `Signets` (également modifiable dans les paramètres).

::: tip
Vous pouvez également importer les signets de votre navigateur dans QOwnNotes avec l'extension de navigateur Web compagnon!
:::

### Syntaxe des liens de signets

```markdown
- [Webpage name](https://www.example.com)
- [Webpage name](https://www.example.com) #tag1 #tag2
- [Webpage name](https://www.example.com) some description only
- [Webpage name](https://www.example.com) #tag1 #tag2 some description and tags
* [Webpage name](https://www.example.com) the alternative list character also works
```

Vous pouvez rechercher un nom, des balises URL ou une description dans l'extension de navigateur.
