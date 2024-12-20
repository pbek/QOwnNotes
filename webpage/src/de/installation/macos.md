# Installieren unter macOS™

Laden Sie das neueste **macOS-Disk-Image** von [QOwnNotes Versionen auf GitHub](https://github.com/pbek/QOwnNotes/releases) herunter. Öffnen Sie es und ziehen Sie die App **QOwnNotes** in Ihren **Anwendungsordner**.

The first time you have to right-click the application and click **Open** to open QOwnNotes.

The GitHub release is currently only built for the x86 platform.

::: warning
Wenn Sie die Fehlermeldung erhalten `QOwnNotes.app kann nicht geöffnet werden, da der Entwickler nicht überprüft werden kann` erhalten, müssen Sie Ihre macOS **Sicherheitseinstellungen** besuchen und auf `Trotzdem öffnen` klicken.
:::

::: tip
The application uses its **internal updater** to update itself. You can see the latest version on the bottom right of the application.
:::

## Homebrew

Unter [Homebrew](https://formulae.brew.sh/cask/qownnotes) befindet sich ein von der Community gepflegtes Cask mit QOwnNotes.

Sie können es installieren mit:

```bash
brew install qownnotes
```

## Nix

You can also install QOwnNotes with the [Nix package manager](https://wiki.nixos.org/wiki/Nix_package_manager) under macOS natively on **x86 and Apple Silicon**, see [Install via Nix](./nix.md).
