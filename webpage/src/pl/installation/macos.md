# Zainstaluj na macOS™

Pobierz najnowszy **obraz dysku macOS** z [QOwnNotes Releases na GitHub](https://github.com/pbek/QOwnNotes/releases), otwórz go i przeciągnij aplikację **QOwnNotes** do **folderu aplikacji</0 >.</p>

The first time you have to right-click the application and click **Open** to open QOwnNotes.

The GitHub release is currently only built for the x86 platform.

::: warning
Jeśli pojawi się komunikat o błędzie, `QOwnNotes.app nie można otworzyć, ponieważ nie można zweryfikować programisty`, musisz odwiedzić **Ustawienia zabezpieczeń** w systemie MacOS i kliknąć `Otwórz mimo to`.
:::

::: tip
The application uses its **internal updater** to update itself. You can see the latest version on the bottom right of the application.
:::

## Homebrew

There is a community maintained cask of QOwnNotes at [Homebrew](https://formulae.brew.sh/cask/qownnotes).

Możesz go zainstalować za pomocą:

```bash
brew install qownnotes
```

## Nix

You can also install QOwnNotes with the [Nix package manager](https://wiki.nixos.org/wiki/Nix_package_manager) under macOS natively on **x86 and Apple Silicon**, see [Install via Nix](./nix.md).
