# Contributions sur le code source

Merci de contribuer au code de QOwnNotes.

## Requêtes de fusion

### Ouvrez d’abord un ticket

Merci d’ouvrir un ticket (issue) avant d’ouvrir une requête de fusion (pull request).

Cela permet de discuter du problème, de la solution proposée, de sa portée et de ses possibles effets de bord avant que le code soit relu.

L’unique exception porte sur les changements triviaux, comme les fautes d’orthographes, les petites corrections de la documentation,
ou les autres changements ne nécessitant pas de discussion au préalable.

S’il y a déjà un ticket ouvert, référencez-le dans votre requête de fusion.

### Branches et requêtes de fusion

Merci d’ouvrir des requêtes de fusion dans la branche `main`.

Avant de commencer à travailler, créez une nouvelle branche pour votre changement et ouvrez la requête de fusion depuis cette branche.

Merci de ne pas ouvrir de requêtes de fusion depuis votre propre branche `main` : cela pourrait poser des problèmes pour
garder votre bifurcation (fork) à jour et pourrait accidentellement inclure des changements sans rapport dans votre requête de fusion.

### Contributions aux traductions

Merci de ne pas faire de contributions aux traductions en changeant directement les fichiers de traduction.
Les fichiers de traduction sont générés automatiquement, il vaut donc mieux aller voir la
page [Traduction](translation.md).

## Messages de validations (commits)

Merci de suivre le style de messages de validations utilisé dans ce dépôt.

Exemples récents :

- `#1765 misc: optionally anonymize personal information in debug output`
- `#1789 misc: add remembered file manager open prompts`
- `#3568 mainwindow: allow dragging text file to import as note`
- `ci: fix step name`
- `release: bump version`

De manière générale, les messages de validations doivent :

- être courts et descriptifs ;
- préciser d’abord la portée suivie de deux-points, comme `misc:`, `mainwindow:`, `tests:` ou `ci:` ;
- utiliser une description rédigée en anglais et à l’impératif après les deux-points, comme `fix`, `add`, `allow`, `update` ou `refactor` ;
- inclure le numéro de ticket au début quand il y en a un, par exemple `#3568 mainwindow: allow dragging text file to import as note`.

Merci de faire des petites validations représentant chacune un petit changement élémentaire.
