# Installation mit Nix

Sie können QOwnNotes mit dem [Nix package manager](https://nixos.wiki/wiki/Nix_package_manager) auf [NixOS](https://nixos.org/) und anderen Plattformen installieren. Das Paket heißt [qownnotes](https://search.nixos.org/packages?channel=unstable&show=qownnotes).

Mehr Informationen auf [QOwnNotes auf Nix](https://search.nixos.org/packages?channel=unstable&show=qownnotes).

Danach sollten Sie QOwnNotes mit dem Befehl `qownnotes` ausführen können.

::: tip
Der QOwnNotes Befehlszeilen-Snippet-Manager [qc](https://github.com/qownnotes/qc) kann auch über den [Nix Store](https://search.nixos.org/packages?channel=unstable&show=qc) installiert werden.

Sie können es mit den Aufruf `nix-shell -p qc --run qc` ausprobieren.
:::
