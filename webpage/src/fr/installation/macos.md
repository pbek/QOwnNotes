# Installer sur macOS™

Téléchargez la dernière **image disque macOS** à partir de [QOwnNotes Releases sur GitHub](https://github.com/pbek/QOwnNotes/releases), ouvrez-la et faites glisser l'application **QOwnNotes** vers votre **dossier d'application**.

The first time you have to right-click the application and click **Open** to open QOwnNotes.

The GitHub release is currently only built for the x86 platform.

::: warning
If you get an error message `QOwnNotes.app cannot be opened because the developer cannot be verified` you need to visit your macOS **Security settings** and click on `Open anyway`.

To take the application out of the quarantine you can also run the following command in the terminal:

```bash
xattr -r -d com.apple.quarantine /Applications/QOwnNotes.app
```

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

::: tip
Best use the NixOS Unstable channel to get the latest version of QOwnNotes!
:::
