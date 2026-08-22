---
image: /img/qownnotes-tui.webp
---

# TUI QOwnNotes

La [TUI (interface terminal) de QOwnNotes](https://github.com/qownnotes/qownnotes-tui) est un
**navigateur et éditeur dans le terminal** de dossiers locaux de notes Markdown compatibles avec QOwnNotes.
Elle peut directement **utiliser les dossiers de notes configurés dans QOwnNotes**, vous permettant
de commencer à travailler directement avec vos notes existantes.

![TUI QOwnNotes](/img/qownnotes-tui.webp "Parcourez et modifiez vos notes dans le terminal")

- Elle utilise les **dossiers de notes configurés dans QOwnNotes** et suit ses préférences en terme de tri de notes
  (la base de données d’application de QOwnNotes est ouverte en lecture-seule)
- Il est possible de **créer, modifier, rechercher, coloriser syntaxiquement, renommer** et **supprimer en sécurité** des notes depuis le terminal
- Les changements sont **sauvegardés automatiquement** et les conflits de changements externes sont détectés
- Elle supporte des **thèmes configurables**, **le contrôle par la souris**, **Nix** et un **module Home Manager**
- Les autocomplétions dans la console peuvent être générées pour **Bash, Fish, Zsh** et plus

## Installation

Visitez la [page de la dernière version](https://github.com/qownnotes/qownnotes-tui/releases/latest)
et téléchargez la version qui vous convient.

::: tip
Si vous utilisez [Nix](https://nixos.org/), vous pouvez compiler et exécuter l’application directement :

```bash
nix run github:qownnotes/qownnotes-tui -- --notes-dir ~/Notes
```

Le paquet Nix installe les complétions pour Bash, Fish et Zsh automatiquement.
:::

## Utilisation

Ouvrir un dossier de notes depuis la ligne de commande :

```bash
qownnotes-tui --notes-dir ~/Notes
```

Si aucun dossier de note n’est donné, l’application lit tous les dossiers de notes configurés
dans QOwnNotes et ouvre initialement le dossier courant.

Quelques-unes des touches les plus importantes :

| Touche          | Action                                               |
| --------------- | ---------------------------------------------------- |
| `j`, `k`        | Déplacer la sélection ou déplacer la fenêtre visible |
| `h`, `l`, `Tab` | Changer de panneau                                   |
| `Entrée`        | Activer un dossier de notes ou ouvrir une note       |
| `n`, `Ctrl-n`   | Créer une note horodatée                             |
| `d`             | Supprimer une note après confirmation                |
| `e`             | Modifier la note sélectionnée                        |
| `/`             | Rechercher parmi les noms et textes de notes         |
| `Ctrl-s`        | Sauvegarder la note                                  |
| `Échap`         | Quitter l’éditeur ou revenir à la liste de notes     |
| `?`             | Afficher l’aide                                      |
| `q`, `Ctrl-c`   | Quitter                                              |

::: tip
Pour plus d’informations, visitez le
[dépôt de la TUI de QOwnNotes sur GitHub](https://github.com/qownnotes/qownnotes-tui).
:::
