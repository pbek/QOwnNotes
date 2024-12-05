# التثبيت عبر Nix

You can install QOwnNotes with the [Nix package manager](https://nixos.org/download/) on [NixOS](https://nixos.org/), other Linux distributions, macOS and Windows. The package name is [qownnotes](https://search.nixos.org/packages?channel=unstable&show=qownnotes).

The package is built with Qt6 and provides shell integration for fish and bash.

انظر [QOwnNotes على nix](https://search.nixos.org/packages?channel=unstable&show=qownnotes) للمزيد من المعلومات.

Afterward you should be able to run QOwnNotes with the command `qownnotes` or `QOwnNotes`.

::: tip
يمكن أيضا تثبيت مدير قصاصات QOwnNotes لسطر الأوامر ([qc](https://github.com/qownnotes/qc)) عبر [متجر Nix](https://search.nixos.org/packages?channel=unstable&show=qc).

You can use `nix-shell -p qc --run "qc exec"` to try it out.
:::
