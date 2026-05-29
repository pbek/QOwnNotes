# API de suggestion Homepage

QOwnNotes peut ouvrir un point d’accès local HTTP pour le support du `suggestionUrl` de [Homepage](https://github.com/gethomepage/homepage),
utilisant le même système d’analyse et d’indexage des signets utilié par le Web Companion.

## L’activer dans QOwnNotes

1. Ouvrez `Préférences -> Extension de navigateur / snippets de commandes` ;
2. Cochez `Activer le serveur de socket` ;
3. Dans `API de suggestion de signets`, cochez `Activer l’API de suggestion de signets compatible avec Homepage` et choisissez un port.

Le service est lié uniquement à `127.0.0.1`.

## Point d’accès

- `GET /suggest?q=home`
- Optionnel : `limit` (par défaut : `10`, maximum : `50`)
- Optionnel : `token` (si vous avez configuré un jeton de sécurité dans QOwnNotes)

Si vous utilisez les ressources personnalisées de Homepage de `docs/homepage/custom.js`, `QON_TOKEN` doit être le même jeton de sécurité que celui que vous avez configuré dans QOwnNotes. Le script ajoutera alors `&token=...` à chaque requête de suggestions.

Exemple de réponse :

```json
["home", ["Homepage", "https://example.com/home", "Un lien de page d’accueil"]]
```

## Exemple de configuration Homepage

```yaml
search:
  provider: custom
  url: https://example.com/search?q=
  suggestionUrl: http://127.0.0.1:22224/suggest?q=
  showSearchSuggestions: true
```

Si vous protégez le point d’accès avec un jeton de sécurité et que vous utilisez `custom.js`, laissez `suggestionUrl` inchangé
et configurez `QON_TOKEN` dans `custiom.js` à la place.

## Quel fichier Homepage modifier

Dans une installation Homepage standard, ajoutez le bloc `search` dans l’un de ces fichiers :

- `settings.yaml` (préférences Homepage globales)
- `settings.yml` (idem que le précédent, dépend de votre configuration)
- le fichier monté dans votre conteneur dans `/app/config/settings.yaml`

Si la configuration de votre installation Homepage est partagée entre plusieurs fichiers, modifiez le fichier qui contient déjà vos préférences `search`.

## Ressources Homepage personalisées

Le dépôt contient également une personnalisation Homepage prête à l’emploi dans :

- [docs/homepage sur GitHub](https://github.com/pbek/QOwnNotes/tree/main/docs/homepage)

Utilisez ces fichiers lorsque votre installation Homepage supporte de charger `custom.js` et `custom.css`.

Si votre Homepage tourne sur un hôte externe (une machine ou un conteneur différent que celle/celui de QOwnNotes),
il faudra placer ces fichiers sur votre instance hôte de Homepage pour permettre de faire des requêtes à votre API locale de suggestion QOwnNotes
et de fusionner/afficher ces suggestions dans la recherche Homepage.

Lorsque vous utilisez cette configuration avec un jeton de sécurité QOwnNotes, assurez-vous que le même jeton est configuré dans
`QON_TOKEN` à l’intérieur de `custom.js`.

Si vous avez besoin d’un accès au-delà de localhost, placez un proxy inversé devant votre point d’accès et restreignez l’accès
(par exemple par des règles de pare-feu ou une liste blanche), étant donné que les données des signets peuvent contenir des URL sensibles.
