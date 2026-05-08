# Harper

QOwnNotes peut utiliser [Harper](https://writewithharper.com/) pour une **vérification hors-ligne et
optionnelle de la grammaire et du style** dans l’éditeur de notes (en anglais uniquement).

Il vient en complément du support actuel de [vérification orthographique](spellchecking.md) au lieu de le remplacer.

![harper-demo](/img/editor/harper.webp)

## Fonctionnalités

- Soulignage coloré des erreurs de grammaire, de style, de ponctuation et de formulation détectées
- Suggestion de remplacements dans le menu contextuel
- Actions `Ignorer cette règle` et `Ignorer ce mot` dans le menu contextuel
- Un simple bouton d’activation/désactivation `Vérifier la grammaire avec Harper` dans le menu **Édition**
- Vérification locale avec le serveur de langue `harper-ls` via `Stdio` ou `TCP`

## Configuration

Ouvrez les `Préférences` et allez dans `Éditeur -> Harper`.

- Cochez `Activer les vérifications grammaticale et de style avec Harper (anglais uniquement)`
- Choisissez le `Protocole de transport`
  - `Stdio (recommendé)` démarre directement un processus local `harper-ls`
  - `TCP` se connecte à un serveur `harper-ls` déjà en place
- Pour `Stdio`, configurez `Commande / chemin vers le binaire`
  - La commande par défaut est `harper-ls`
  - `Détecter automatique` cherche la commande dans votre `PATH`
- Pour `TCP`, configurez l’`Adresse du serveur` et le `Port`
  - Les valeurs par défaut sont `127.0.0.1` et `4000`
- Choisissez le `Dialecte`
  - `États-unien`
  - `Britannique`
  - `Australien`
  - `Canadien`
  - `Indien`
- Ajustez le `Délai de vérification` pour contrôler combien de temps QOwnNotes attend entre le dernier caractère tapé et l’envoi de la requête
- Sélectionnez les règles que vous souhaitez activer
  - `Orthographe`
  - `Capitalisation des phrases`
  - `Répétitions de mots`
  - `Longues phrases`
  - `An vs A`
  - `Guillemets non fermés`
  - `Suffixes de nombres corrects`
  - `Espaces`
  - `Espacement des guillemets`
  - `Pas d’espaces françaises`
  - `Apostrophes incorrectes`
  - `Nombres en toutes lettres`

Utilisez `Tester la connexion` pour vérifier que QOwnNotes peut soit démarrer `harper-ls` (dans le mode `Stdio`)
soit atteindre le serveur configuré (dans le mode `TCP`).

## Comment ça marche

- QOwnNotes ne vérifie que les blocs d’éditeur visibles au lieu du document entier
- Les lignes vides, les titres de sections et les blocs de code sont sautés
- Les requêtes sont délayées afin que Harper ne soit pas interrogé à chaque touche frappée
- Les résultats sont affichés en-ligne dans l’éditeur Markdown
- Harper et LanguageTool peuvent être activés en même temps

## Notes

- Le support Harper est optionnel et dépend de si la fonctionnalité est activée dans votre version
- Le mode `Stdio` est la configuration la plus simple pour de la vérification hors ligne
- Si Harper n’est pas joignable, QOwnNotes affiche un avertissement et arrête la vérification jusqu’à ce qu’il soit joignable à nouveau

## Voir aussi

- [Vérification orthographique](spellchecking.md)
- [LanguageTool](languagetool.md)
- [Concept](../getting-started/concept.md)
