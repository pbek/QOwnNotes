# Installa su macOS™

Scarica l'ultima **immagine del disco macOS** da [QOwnNotes Releases su GitHub](https://github.com/pbek/QOwnNotes/releases), aprila e trascina l'app **QOwnNotes** nella tua **cartella delle applicazioni**.

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

C'è un pacchetto di QOwnNotes manutenuto dalla comunità su [Homebrew](https://formulae.brew.sh/cask/qownnotes).

Puoi installarlo con:

```bash
brew install qownnotes
```

## Nix

You can also install QOwnNotes with the [Nix package manager](https://wiki.nixos.org/wiki/Nix_package_manager) under macOS natively on **x86 and Apple Silicon**, see [Install via Nix](./nix.md).

::: tip
Best use the NixOS Unstable channel to get the latest version of QOwnNotes!
:::
