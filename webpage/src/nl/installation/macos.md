# Installatie op macOS™

Download de nieuwste **macOS disk image** van [QOwnNotes Releases on GitHub](https://github.com/pbek/QOwnNotes/releases), open deze en sleep de **QOwnNotes** -app naar je **application folder**.

Om te beginnen moet je met de rechtermuisknop op de applicatie klikken en op **Open** klikken om QOwnNotes te openen.

De GitHub-release is momenteel alleen gebouwd voor het x86-platform.

::: warning
If you get an error message `QOwnNotes.app cannot be opened because the developer cannot be verified` you need to visit your macOS **Security settings** and click on `Open anyway`.

To take the application out of the quarantine you can also run the following command in the terminal:

```bash
xattr -r -d com.apple.quarantine /Applications/QOwnNotes.app
```
:::

::: tip
De applicatie gebruikt een **interne updater** om zichzelf bij te werken. Rechtsonder in de applicatie wordt de nieuwste versie getoond.
:::

## Homebrew

Er is een door de gemeenschap onderhouden vat QOwnNotes op [Homebrew](https://formulae.brew.sh/cask/qownnotes).

U kunt het installeren met:

```bash
brew install qownnotes
```

## Nix

Je kunt QOwnNotes ook installeren met de [Nix pakketbeheerder](https://wiki.nixos.org/wiki/Nix_package_manager) onder macOS op **x86 en Apple Silicon**, zie [Installatie via Nix](./nix.md).
