# Instalar por medio de Nix

Puede instalar QOwnNotes con el [administrador de paquetes de NIx](https://wiki.nixos.org/wiki/Nix_package_manager) en [NixOS](https://nixos.org/) y otras plataformas. El nombre del paquete es [qownnotes](https://search.nixos.org/packages?channel=unstable&show=qownnotes).

Véase [QOwnNotes en Nix](https://search.nixos.org/packages?channel=unstable&show=qownnotes) para mayor información.

Posteriormente, será capaz de ejecutar QOwnNotes con la orden `qownnotes`.

::: tip
El gestor de snippets de la línea de comandos de QOwnNotes [qc](https://github.com/qownnotes/qc) también puede instalarse a través de [Nix Store](https://search.nixos.org/packages?channel=unstable&show=qc).

Puede usar `nix-shell -p qc --run qc` para probarlo.
:::
