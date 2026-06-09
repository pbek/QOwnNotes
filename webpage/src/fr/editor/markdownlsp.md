# LSP Markdown

QOwnNotes supporte l’intégration avec un serveur implémentant un **Protocole de Serveur de Langage (LSP) Markdown** afin d’avoir des fonctionnalités d’édition supplémentaires telles que les diagnostics (« linting »), les complétions, les actions de code et le formatage de document directement dans l’éditeur de notes.

## Fonctionnalités

- Soulignage coloré pour les problèmes et non-respects des règles Markdown détectés
- Menu contextuel **actions sur le code** pour les diagnostics (par exemple des corrections rapides suggérées par le serveur)
- Action **Ignorer la règle** dans le menu contextuel pour supprimer certaines règles de diagnostic de façon permanente
- Pop-up **complétions** pour les liens de notes et d’autres suggestions fournies par le serveur (par exemple des complétions de lien-wiki avec [Marksman](https://github.com/artempyanykh/marksman))
- Actions **Formater le document** et **Formater la sélection** via le menu contextuel
- Synchronisation incrémentale du document pour une communication efficace avec le serveur
- Mode verbeux de journalisation pour le débogage des communications avec le serveur

## Serveurs de langage supportés

Deux serveurs de LSP Markdown sont connus et pris en charge par défaut :

| Serveur                                              | Commande       | Notes                                                                           |
| ---------------------------------------------------- | -------------- | ------------------------------------------------------------------------------- |
| [Marksman](https://github.com/artempyanykh/marksman) | `marksman`     | Par défaut ; fournit des complétions de liens-wiki et de navigation inter-notes |
| [Rumdl](https://github.com/rvben/rumdl)              | `rumdl server` | Règles de lintage Markdown (similaire à `markdownlint`)      |

N’importe quel serveur Markdown compatible-LSP qui communique via `stdio` devrait aussi fonctionner
en configurant la commande manuellement.

## Configuration

Ouvrez les `Préférences` et allez dans `Éditeur -> LSP Markdown`.

- Activez **Activer le serveur de langage (LSP) Markdown**
- Configurez le champ **Commande** en y insérant le nom de l’exécutable ou le chemin entier de votre serveur LSP
  - La commande par défaut est `marksman`
  - Utilisez **Détecter automatiquement** pour chercher automatiquement `marksman` ou `rumdl` dans votre `PATH` et remplir le champ « Commande »
- Fournissez optionnellement des **Arguments** à donner au processus du serveur
  - Pour `rumdl`, l’argument `server` est requis
- Activez **Afficher tous les logs du serveur LSP dans le journal de débogage** pour des diagnostics verbeux
- Utilisez **Tester la connexion** pour vérifier que QOwnNotes peut démarrer le serveur et compléter
  la poignée de main (« handshake ») d’initialisation LSP dans votre dossier de notes courant
- Utilisez **Réinitialisez les règles ignorées** pour effacer la liste des identifiants de règles de diagnostics précédemment ignorées

## Comment ça marche

- Quand vous ouvrez une note, QOwnNotes envoie une notification `textDocument/didOpen` au serveur
- Pendant que vous tapez, les changements sont envoyés avec `textDocument/didChange` utilisant de la synchronisation incrémentale quand le serveur la supporte, ou une synchronisation du document entier sinon
- Les diagnostics renvoyés par le serveur avec `textDocument/publishDiagnostics` sont affichés via
  du **soulignage coloré** dans l’éditeur Markdown
- Faire un clic droit sur une zone soulignée affiche des **actions de code** et l’option
  d’**ignorer la règle** pour ce diagnostic
- Les complétions sont affichées dans un popup quand le serveur fournit des éléments de complétion
- Le formatage du document est envoyé via `textDocument/formatting` ou `textDocument/rangeFormatting`

## Installer un serveur LSP Markdown

### Marksman

Marksman fournit des complétions de liens-wiki, de la navigation inter-notes et des diagnostics
pour les projets Markdown. Il fonctionne particulièrement bien avec les liens de note style-wik de QOwnNotes.

- **Linux/macOS** : téléchargez Marksman depuis [GitHub Releases](https://github.com/artempyanykh/marksman/releases)
  ou installez-le via votre gestionnaire de paquets (par exemple `brew install marksman` sur macOS,
  `nix-env -iA nixpkgs.marksman` sur NixOS)
- **Windows** : téléchargez le `.exe` depuis la page GitHub Releases et placez-le quelque part dans votre `PATH`

Après l’installation, utilisez **Détecter automatiquement** dans les préférences pour le localiser automatiquement.

### Rumdl

[Rumdl](https://github.com/rvben/rumdl) est un linter Markdown rapide qui implémente
des règles de style configurables et qui peut se lancer comme serveur LSP.

**Installation via Cargo (Rust) :**

```bash
cargo install rumdl
```

**Installation via pip :**

```bash
pip install rumdl
```

**Télécargement d’un binaire pré-compilé :**

Des binaires pré-compilés pour Linux, macOS et Windows sont disponibles sur
la page [Rumdl GitHub Releases](https://github.com/rvben/rumdl/releases).

Après l’installation, mettez `rumdl` dans le champ **Commande** et `server` dans le champ **Arguments**.

## Notes

- Le client LSP communique avec le serveur via le `stdio` (entrée/sortie standard)
- Le processus du serveur est démarré avec comme racine de travail le dossier de notes courant
- Les règles ignorées sont stockées de façon persistante et peuvent être réinitialisées avec **Réinitialiser les règles ignorées**
- Si le serveur ne peut pas être démarré, QOwnNotes affiche un avertissement dans la zone « Statut »

## Voir aussi

- [Vérification orthographique](spellchecking.md)
- [LanguageTool](languagetool.md)
- [Harper](harper.md)
- [Concept](../getting-started/concept.md)
