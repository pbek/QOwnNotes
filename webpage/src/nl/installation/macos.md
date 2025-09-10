# Installatie op macOS™

Download de nieuwste **macOS disk image** van [QOwnNotes Releases on GitHub](https://github.com/pbek/QOwnNotes/releases), open deze en sleep de **QOwnNotes** -app naar je **application folder**.

Om te beginnen moet je met de rechtermuisknop op de applicatie klikken en op **Open** klikken om QOwnNotes te openen.

The GitHub release `QOwnNotes.dmg` is a **universal binary**, built with **Qt6**. You need at least macOS 13 (Ventura) to run the universal binary.

The Qt5 legacy version is available as `QOwnNotesQt5.dmg`. You can't use the internal updater with this version, so you have to download new versions manually.

::: waarschuwing Bij de foutmelding `QOwnNotes.app niet kan worden geopend omdat de ontwikkelaar niet kan worden geverifieerd` kunt u de macOS **Beveiligingsinstellingen** openen en `Toch openen` selecteren.

Om de applicatie uit de quarantaine te halen, kunt u ook de volgende opdrachtregel uitvoeren:

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

::: tip
Best use the NixOS Unstable channel to get the latest version of QOwnNotes!
:::
