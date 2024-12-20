# Telepítse a macOS™ -ra

Töltse le a legújabb **macOS lemezképet** a [QOwnNotes Releases on GitHub webhelyről](https://github.com/pbek/QOwnNotes/releases), nyissa meg, és húzza a **QOwnNotes** alkalmazást az **alkalmazás mappájába**.

The first time you have to right-click the application and click **Open** to open QOwnNotes.

The GitHub release is currently only built for the x86 platform.

::: warning
Ha hibaüzenetet kap, a `QOwnNotes.app nem nyitható meg, mert a fejlesztőt nem lehet ellenőrizni` meg kell látogatnia a macOS **biztonsági beállításait** és kattintson a `Megnyitás mindenképp`.
:::

::: tip
The application uses its **internal updater** to update itself. You can see the latest version on the bottom right of the application.
:::

## Homebrew

Van egy közösség által fenntartott QOwnNotes hordó a [Homebrew](https://formulae.brew.sh/cask/qownnotes) -nál.

Telepítheti:

```bash
brew install qownnotes
```

## Nix

You can also install QOwnNotes with the [Nix package manager](https://wiki.nixos.org/wiki/Nix_package_manager) under macOS natively on **x86 and Apple Silicon**, see [Install via Nix](./nix.md).
