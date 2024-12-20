# Instalar por medio de Nix

You can install QOwnNotes with the [Nix package manager](https://nixos.org/download/) on [NixOS](https://nixos.org/), other **Linux** distributions, **macOS** and **Windows**. El nombre del paquete es [qownnotes](https://search.nixos.org/packages?channel=unstable&show=qownnotes).

El paquete está construido con Qt6 y provee integración de la consola en fish y bash.

Véase [QOwnNotes en Nix](https://search.nixos.org/packages?channel=unstable&show=qownnotes) para mayor información.

Posteriormente, debería ser capaz de abrir QOwnNotes con la orden `qownnotes` o `QOwnNotes`.

::: tip
El gestor de fragmentos de órdenes de terminal de QOwnNotes [qc](https://github.com/qownnotes/qc) también puede instalarse a través de [Nix Store](https://search.nixos.org/packages?channel=unstable&show=qc).

Puede usar `nix-shell -p qc --run "qc exec"` para probarlo.
:::

::: tip
Info
On **macOS** the package works natively on **x86 and Apple Silicon**!
:::
