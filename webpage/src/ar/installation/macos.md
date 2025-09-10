# التثبيت على ماك أو إس

قم بتنزيل **صورة قرص macOS** الأخيرة من [إصدارات QOwnNotes على GitHub](https://github.com/pbek/QOwnNotes/releases)، وافتحها واسحب تطبيق **QOwnNotes** إلى **مجلد تطبيقاتك**.

في المرة الأولى، ستحتاج إلى النقر بزر الفأرة الأيمن على التطبيق والنقر على **فتح (Open)** لفتح QOwnNotes.

The GitHub release `QOwnNotes.dmg` is a **universal binary**, built with **Qt6**. You need at least macOS 13 (Ventura) to run the universal binary.

The Qt5 legacy version is available as `QOwnNotesQt5.dmg`. You can't use the internal updater with this version, so you have to download new versions manually.

::: warning
If you get an error message `QOwnNotes.app cannot be opened because the developer cannot be verified` you need to visit your macOS **Security settings** and click on `Open anyway`.

To take the application out of the quarantine you can also run the following command in the terminal:

```bash
xattr -r -d com.apple.quarantine /Applications/QOwnNotes.app
```

:::

::: tip
The application uses its **internal updater** to update itself. You can see the latest version on the bottom right of the application.
:::

## Homebrew

يتعهد المجتمع حزمة مبنية (cask) لـ&nbsp;QOwnNotes في [Homebrew](https://formulae.brew.sh/cask/qownnotes).

يمكنك تثبيته بالأمر:

```bash
brew install qownnotes
```

## Nix

كذلك يمكنك تثبيت QOwnNotes عبر [مدير الحزم Nix](https://wiki.nixos.org/wiki/Nix_package_manager) على ماك&nbsp;أو&nbsp;إس **على x86 و&nbsp;Apple&nbsp;Silicon**؛ انظر [التثبيت عبر Nix](./nix.md).

::: tip
Best use the NixOS Unstable channel to get the latest version of QOwnNotes!
:::
