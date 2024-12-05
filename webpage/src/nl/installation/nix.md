# Installatie via Nix

Je kunt QOwnNotes installeren met de [Nix pakketbeheerder](https://nixos.org/download/) op [NixOS](https://nixos.org/), andere Linuxdistributies, macOS en Windows. De pakketnaam is [qownnotes](https://search.nixos.org/packages?channel=unstable&show=qownnotes).

Het pakket is gebouwd met Qt6 en biedt shell-integratie voor fish en bash.

Zie [QOwnNotes op nix](https://search.nixos.org/packages?channel=unstable&show=qownnotes) voor meer informatie.

Vervolgens zou je QOwnNotes moeten kunnen uitvoeren met de opdracht `qownnotes` of `QOwnNotes`.

::: tip
<0 /> De QOwnNotes command-line snippet manager [qc](https://github.com/qownnotes/qc) kan ook worden geïnstalleerd via de [Nix Store](https://search.nixos.org/packages?channel=unstable&show=qc).

Je kunt `nix-shell -p qc --run "qc exec"` gebruiken om het uit te proberen.
:::
