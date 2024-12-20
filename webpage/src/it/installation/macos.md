# Installa su macOS™

Scarica l'ultima **immagine del disco macOS** da [QOwnNotes Releases su GitHub](https://github.com/pbek/QOwnNotes/releases), aprila e trascina l'app **QOwnNotes** nella tua **cartella delle applicazioni**.

The first time you have to right-click the application and click **Open** to open QOwnNotes.

The GitHub release is currently only built for the x86 platform.

::: warning
Se ricevi un messaggio di errore `Impossibile aprire QOwnNotes.app perché lo sviluppatore non può essere verificato` devi visitare le **Impostazioni di sicurezza** di macOS e fare clic su `Apri comunque`.
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
