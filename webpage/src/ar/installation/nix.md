# التثبيت عبر Nix

يمكنك تثبيت QOwnNotes عبر [مدير الحزم Nix](https://wiki.nixos.org/wiki/Nix_package_manager) على [NixOS](https://nixos.org/)، وعلى توزيعات **لينكس** الأخرى، وعلى **ماك&nbsp;أو&nbsp;إس**، وعلى **ويندوز**. اسم الحزمة هو [qownnotes](https://search.nixos.org/packages?channel=unstable&show=qownnotes).

::: tip
Best use the NixOS Unstable channel to get the latest version of QOwnNotes!
:::

The package is built with Qt6, uses the system botan3 library and libgit2 and provides shell integration for fish and bash.

انظر [QOwnNotes على nix](https://search.nixos.org/packages?channel=unstable&show=qownnotes) للمزيد من المعلومات.

بعدئذٍ يمكنك تشغيل QOwnNotes بالأمر `qownnotes` أو الأمر `QOwnNotes`.

::: tip
يمكن أيضا تثبيت مدير قصاصات QOwnNotes لسطر الأوامر ([qc](https://github.com/qownnotes/qc)) عبر [متجر Nix](https://search.nixos.org/packages?channel=unstable&show=qc).

يمكنك تجربته بالأمر `nix-shell -p qc --run "qc exec"`.
:::

::: tip
Info
On **macOS** the package works natively on **x86 and Apple Silicon**!
:::
