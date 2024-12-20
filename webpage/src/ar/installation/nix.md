# التثبيت عبر Nix

يمكنك تثبيت QOwnNotes عبر [مدير الحزم Nix](https://wiki.nixos.org/wiki/Nix_package_manager) على [NixOS](https://nixos.org/)، وعلى توزيعات **لينكس** الأخرى، وعلى **ماك&nbsp;أو&nbsp;إس**، وعلى **ويندوز**. اسم الحزمة هو [qownnotes](https://search.nixos.org/packages?channel=unstable&show=qownnotes).

هذه الحزمة مبنية بـ&nbsp;Qt6 وتشمل التكامل مع صدفة fish وصدفة bash.

انظر [QOwnNotes على nix](https://search.nixos.org/packages?channel=unstable&show=qownnotes) للمزيد من المعلومات.

بعدئذٍ يمكنك تشغيل QOwnNotes بالأمر `qownnotes` أو الأمر `QOwnNotes`.

::: tip
يمكن أيضا تثبيت مدير قصاصات QOwnNotes لسطر الأوامر ([qc](https://github.com/qownnotes/qc)) عبر [متجر Nix](https://search.nixos.org/packages?channel=unstable&show=qc).

يمكنك تجربته بالأمر <code dir="ltr">nix-shell -p qc --run "qc exec"</code>.
:::

::: tip
Info
On **macOS** the package works natively on **x86 and Apple Silicon**!
:::
