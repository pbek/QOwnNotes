# Instalar mediante Nix

Puede instalar QOwnNotes con el [gestor de paquetes de Nix](https://nixos.org/download/) en [NixOS](https://nixos.org/), otras distribuciones **Linux**, **macOS** y **Windows**. El nombre del paquete es [qownnotes](https://search.nixos.org/packages?channel=unstable&show=qownnotes).

::: tip
¡Lo mejor es utilizar el canal inestable de NixOS para obtener la última versión de QOwnNotes!
:::

El paquete está construido con Qt6, utiliza la biblioteca del sistema botan3 y libgit2, y proporciona integración de la consola para fish y bash.

Véase [QOwnNotes en Nix](https://search.nixos.org/packages?channel=unstable&show=qownnotes) para mayor información.

Posteriormente, debería ser capaz de abrir QOwnNotes con el comando `qownnotes` o `QOwnNotes`.

::: tip
El gestor de fragmentos de comandos de terminal de QOwnNotes [qc](https://github.com/qownnotes/qc) también puede instalarse a través de [Nix Store](https://search.nixos.org/packages?channel=unstable&show=qc).

Puede usar `nix-shell -p qc --run "qc exec"` para probarlo.
:::

::: tip
Info
¡En **macOS** el paquete funciona de forma nativa en **x86 y Apple Silicon**!
:::

::: tip
If QOwnNotes logs `Could not write secret to keychain` on Linux, install a Secret Service implementation and restart your desktop session.

For GNOME and other Secret Service based desktops, add packages such as `gnome-keyring`, `libsecret` and `seahorse` to your environment.

For KDE Plasma, add KWallet support such as `kdePackages.kwalletmanager` and `kdePackages.kwallet`.

QOwnNotes will fall back to legacy encryption if the desktop keychain is unavailable.
:::
