# Installer sur macOS™

Téléchargez la dernière **image disque macOS** à partir de [la page des Releases de QOwnNotes sur GitHub](https://github.com/pbek/QOwnNotes/releases), ouvrez-la et faites glisser l'application **QOwnNotes** vers votre **dossier d'application**.

La première fois, vous devez faire un clic droit sur l'application et cliquer sur **Ouvrir** pour ouvrir QOwnNotes.

Le fichier `QOwnNotes.dmg` sur les releases GitHub est un **binaire universel**, compilé avec **Qt6**. Vous devez avoir au moins macOS 13 (Ventura) pour exécuter le binaire universel.

L’ancienne version avec Qt5 est disponible sous le nom `QOwnNotesQt5.dmg`. Vous ne pouvez pas utiliser l’outil interne de mise à jour avec cette version, vous devrez donc télécharger les nouvelles versions manuellement.

::: warning
Si vous obtenez un message d'erreur `QOwnNotes.app ne peut pas être ouvert car le développeur ne peut pas être vérifié` vous devez visiter vos **Préférences de sécurité** de macOS et cliquer sur `Ouvrir quand même`.

Pour sortir l’application de quarantaine, vous pouvez aussi exécuter la commande suivante dans le terminal :

```bash
xattr -r -d com.apple.quarantine /Applications/QOwnNotes.app
```

:::

::: tip
L’application utilise son **outil de mise à jour interne** pour se mettre à jour. Vous pouvez voir la dernière version en bas à droite de l’application.
:::

## Homebrew

Il y a un cask de QOwnNotes maintenu par la communauté à [Homebrew](https://formulae.brew.sh/cask/qownnotes).

Vous pouvez l'installer avec :

```bash
brew install qownnotes
```

## Nix

Vous pouvez aussi installer QOwnNotes avec le [gestionnaire de paquets Nix](https://wiki.nixos.org/wiki/Nix_package_manager) nativement sous macOS sur **x86 et Apple Silicon**, voir [Installer via Nix](./nix.md).

::: tip
Mieux vaut utiliser le canal NixOS Unstable pour avoir la dernière version de QOwnNotes !
:::
