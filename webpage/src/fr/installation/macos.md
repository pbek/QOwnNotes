# Installer sur macOS™

Téléchargez la dernière **image disque macOS** à partir de [QOwnNotes Releases sur GitHub](https://github.com/pbek/QOwnNotes/releases), ouvrez-la et faites glisser l'application **QOwnNotes** vers votre **dossier d'application**.

The first time you have to right-click the application and click **Open** to open QOwnNotes.

The GitHub release is currently only built for the x86 platform.

::: warning
Si vous obtenez un message d'erreur `QOwnNotes.app ne peut pas être ouvert car le développeur ne peut pas être vérifié` vous devez visiter vos **Paramètres de sécurité** de macOS et cliquer sur ` Ouvrir quand même`.
:::

::: tip
The application uses its **internal updater** to update itself. You can see the latest version on the bottom right of the application.
:::

## Homebrew

Il y a un fût de QOwnNotes maintenu par la communauté à [ Homebrew](https://formulae.brew.sh/cask/qownnotes).

Vous pouvez l'installer avec :

```bash
brew install qownnotes
```

## Nix

You can also install QOwnNotes with the [Nix package manager](https://wiki.nixos.org/wiki/Nix_package_manager) under macOS natively on **x86 and Apple Silicon**, see [Install via Nix](./nix.md).
