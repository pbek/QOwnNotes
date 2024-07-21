# Installa usando Nix

Puoi installare QOwnNotes con il [Gestore dei pacchetti Nix](https://wiki.nixos.org/wiki/Nix_package_manager) su [NixOS](https://nixos.org/) e altre piattaforme. Il nome del pacchetto è [qownnotes](https://search.nixos.org/packages?channel=unstable&show=qownnotes).

Guarda [QOwnNotes su nix](https://search.nixos.org/packages?channel=unstable&show=qownnotes) per ulteriori informazioni.

Dopo di che dovresti essere in grado di eseguire QOwnNotes con il comando `qownnotes`.

::: tip
Anche il gestore degli snippet della linea di comandi [qc](https://github.com/qownnotes/qc) di QOwnNotes può essere installato con lo [Store di Nix](https://search.nixos.org/packages?channel=unstable&show=qc).

Puoi usare `nix-shell -p qc --run qc` per provarlo.
:::
