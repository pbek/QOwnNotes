# Installation mit Nix

You can install QOwnNotes with the [Nix package manager](https://nixos.org/download/) on [NixOS](https://nixos.org/), other **Linux** distributions, **macOS** and **Windows**. The package name is [qownnotes](https://search.nixos.org/packages?channel=unstable&show=qownnotes).

::: tip
Best use the NixOS Unstable channel to get the latest version of QOwnNotes!
:::

The package is built with Qt6, uses the system botan3 library and libgit2 and provides shell integration for fish and bash.

Mehr Informationen auf [QOwnNotes auf Nix](https://search.nixos.org/packages?channel=unstable&show=qownnotes).

Afterward you should be able to run QOwnNotes with the command `qownnotes` or `QOwnNotes`.

::: tip
Der QOwnNotes Befehlszeilen-Snippet-Manager [qc](https://github.com/qownnotes/qc) kann auch über den [Nix Store](https://search.nixos.org/packages?channel=unstable&show=qc) installiert werden.

You can use `nix-shell -p qc --run "qc exec"` to try it out.
:::

::: tip
Info
On **macOS** the package works natively on **x86 and Apple Silicon**!
:::
