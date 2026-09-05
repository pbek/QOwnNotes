# Installatie via Nix

Je kunt QOwnNotes met de [Nix pakketbeheerder](https://nixos.org/download/) installeren op [NixOS](https://nixos.org/), andere **Linux**-versies, **macOS** en **Windows**. De pakketnaam is [qownnotes](https://search.nixos.org/packages?channel=unstable&show=qownnotes).

::: tip
Best use the NixOS Unstable channel to get the latest version of QOwnNotes!
:::

The package is built with Qt6, uses the system botan3 library and libgit2 and provides shell integration for fish and bash.

Zie [QOwnNotes op nix](https://search.nixos.org/packages?channel=unstable&show=qownnotes) voor meer informatie.

Vervolgens zou je QOwnNotes moeten kunnen uitvoeren met de opdracht `qownnotes` of `QOwnNotes`.

::: tip
<0 /> De QOwnNotes command-line snippet manager [qc](https://github.com/qownnotes/qc) kan ook worden geïnstalleerd via de [Nix Store](https://search.nixos.org/packages?channel=unstable&show=qc).

Je kunt `nix-shell -p qc --run "qc exec"` gebruiken om het uit te proberen.
:::

::: tip
Info
Op **macOS** werkt het pakket op **x86 en Apple Silicon**!
:::

::: tip
If QOwnNotes logs `Could not write secret to keychain` on Linux, install a Secret Service implementation and restart your desktop session.

For GNOME and other Secret Service based desktops, add packages such as `gnome-keyring`, `libsecret` and `seahorse` to your environment.

For KDE Plasma, add KWallet support such as `kdePackages.kwalletmanager` and `kdePackages.kwallet`.

QOwnNotes will fall back to legacy encryption if the desktop keychain is unavailable.
:::
