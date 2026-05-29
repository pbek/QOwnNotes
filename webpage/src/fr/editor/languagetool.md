# LanguageTool

QOwnNotes peut utiliser un service [LanguageTool](https://languagetool.org/) local ou distant pour
une **vérification optionnelle de la grammaire et du style** dans l’éditeur de notes.

Il vient en complément du support actuel de [vérification orthographique](spellchecking.md) au lieu de le remplacer.

## Fonctionnalités

- Soulignage coloré des erreurs de grammaire, de style, de ponctuation et de typographie détectées
- Suggestions de remplacements dans un menu contextuel
- Lien `Plus d’informations…` pour chaque règle, permettant d’accéder à de la documentation supplémentaire
- Option `Ignorer cette règle` pour les règles que vous ne souhaitez plus voir
- Un simple bouton d’activation/désactivation `Vérifier la grammaire avec LanguageTool` dans le menu **Édition**

## Configuration

Allez dans `Préférences -> Éditeur` et configurez les éléments de la section **LanguageTool**.

- Cochez la case `Activer les vérifications grammaticale et stylistique avec LanguageTool`
- Configurez l’`URL du serveur`
  - Les serveurs locaux utilisent une URL telle que `http://localhost:8081`
  - QOwnNotes utilise automatiquement l’endpoint `/v2/check` sur ce serveur
- Choisissez la `Langue`
  - La sélection automatique (`Auto-détecter`) est disponible
  - Vous pouvez également entrer un code de langue personnalisé si besoin
- Ajoutez une `clé d’API` optionnelle si votre service LanguageTool en a besoin
- Ajustez le `Délai de vérification` pour contrôler combien de temps QOwnNotes attend entre le dernier caractère tapé et l’envoi de la requête
- Sélectionnez les catégories que vous souhaitez activer
  - `Orthographe`
  - `Grammaire`
  - `Style`
  - `Ponctuation`
  - `Typographie`

Utilisez `Tester la connexion` pour vérifier que le service configuré est joignable.

## Comment ça marche

- QOwnNotes ne vérifie que les blocs d’éditeur visibles au lieu du document entier
- Les lignes vides, les titres de sections et les blocs de code sont sautés
- Les requêtes sont délayées afin que le service ne soit pas interrogé à chaque touche frappée
- Les résultats sont affichés en-ligne dans l’éditeur Markdown

## Notes

- Le support LanguageTool est optionnel et dépend de si la fonctionnalité est activée dans votre version
- Si le service n’est pas joignable, QOwnNotes affiche un avertissement et arrête la vérification jusqu’à ce qu’il soit joignable à nouveau
- Vous pouvez l’utiliser avec un serveur LanguageTool auto-hébergé local ou un autre endpoint compatible avec LanguageTool

## Voir aussi

- [Vérification orthographique](spellchecking.md)
- [Concept](../getting-started/concept.md)
