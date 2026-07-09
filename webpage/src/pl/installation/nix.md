# Install via Nix

You can install QOwnNotes with the [Nix package manager](https://nixos.org/download/) on [NixOS](https://nixos.org/), other **Linux** distributions, **macOS** and **Windows**. The package name is [qownnotes](https://search.nixos.org/packages?channel=unstable&show=qownnotes).

::: tip
Best use the NixOS Unstable channel to get the latest version of QOwnNotes!
:::

The package is built with Qt6, uses the system botan3 library and libgit2 and provides shell integration for fish and bash.

See [QOwnNotes on nix](https://search.nixos.org/packages?channel=unstable&show=qownnotes) for more information.

Afterward you should be able to run QOwnNotes with the command `qownnotes` or `QOwnNotes`.

::: tip
The QOwnNotes command-line snippet manager [qc](https://github.com/qownnotes/qc) can also be installed via the [Nix Store](https://search.nixos.org/packages?channel=unstable&show=qc).

You can use `nix-shell -p qc --run "qc exec"` to try it out.
:::

::: tip
Info
On **macOS** the package works natively on **x86 and Apple Silicon**!
:::

::: tip
If QOwnNotes logs `Could not write secret to keychain` on Linux, install a Secret Service implementation and restart your desktop session.

For GNOME and other Secret Service based desktops, add packages such as `gnome-keyring`, `libsecret` and `seahorse` to your environment.

For KDE Plasma, add KWallet support such as `kdePackages.kwalletmanager` and `kdePackages.kwallet`.

QOwnNotes will fall back to legacy encryption if the desktop keychain is unavailable.
:::
