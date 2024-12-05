# Instalar por medio de Nix

You can install QOwnNotes with the [Nix package manager](https://nixos.org/download/) on [NixOS](https://nixos.org/), other Linux distributions, macOS and Windows. The package name is [qownnotes](https://search.nixos.org/packages?channel=unstable&show=qownnotes).

The package is built with Qt6 and provides shell integration for fish and bash.

Véase [QOwnNotes en Nix](https://search.nixos.org/packages?channel=unstable&show=qownnotes) para mayor información.

Afterward you should be able to run QOwnNotes with the command `qownnotes` or `QOwnNotes`.

::: tip
El gestor de fragmentos de órdenes de terminal de QOwnNotes [qc](https://github.com/qownnotes/qc) también puede instalarse a través de [Nix Store](https://search.nixos.org/packages?channel=unstable&show=qc).

You can use `nix-shell -p qc --run "qc exec"` to try it out.
:::
