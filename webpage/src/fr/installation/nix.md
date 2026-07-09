# Installer via Nix

Vous pouvez installer QOwnNotes avec le [gestionnaire de paquets Nix](https://nixos.org/download/) sur [NixOS](https://nixos.org/), les autres distributions **Linux**, **macOS** et **Windows**. Le nom du paquet est [qownnotes](https://search.nixos.org/packages?channel=unstable&show=qownnotes).

::: tip
Mieux vaut utiliser le canal NixOS Unstable pour avoir la dernière version de QOwnNotes !
:::

Le paquet est compilé avec Qt6, utilise les bibliothèques botan3 et libgit2 et fournit l’intégration shell pour fish et bash.

Voir [QOwnNotes sur nix](https://search.nixos.org/packages?channel=unstable&show=qownnotes) pour plus d’informations.

Vous devriez ensuite pouvoir exécuter QOwnNotes avec la commande `qownnotes` ou `QOwnNotes`.

::: tip
Le gestionnaire de snippets en ligne de commande de QOwnNotes [qc](https://github.com/qownnotes/qc) peut aussi être installé via le [Nix Store](https://search.nixos.org/packages?channel=unstable&show=qc).

Vous pouvez utiliser `nix-shell -p qc --run "qc exec"` pour l’essayer.
:::

::: tip
Info
Sur **macOS**, le paquet fonctionne sur **x86 et Apple Silicon** !
:::

::: tip
Si QOwnNotes affiche `Could not write secret to keychain` sous Linux, installez une implémentation Secret Service et redémarrez votre session de bureau.

Pour GNOME et les autres environnements reposant sur Secret Service, ajoutez des paquets comme `gnome-keyring`, `libsecret` et `seahorse` à votre environnement.

Pour KDE Plasma, ajoutez un support de KWallet tel que `kdePackages.kwalletmanager` et `kdePackages.kwallet`.

QOwnNotes repassera sur le chiffrement obsolète si le porte-clés de l’environnement n’est pas disponible.
:::
