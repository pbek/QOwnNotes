# نصب در سیستم عامل مکینتاش

آخرین **دیسک ایمیج سیستم عامل مکینتاش** را از [انتشارهای QOwnNotes در گیت هاب](https://github.com/pbek/QOwnNotes/releases) بارگیری کرده، آن را باز کنید و اپ **QOwnNotes** را به **پوشه برنامه** خود بکشید.

The first time you have to right-click the application and click **Open** to open QOwnNotes.

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

## هوم برو

یک بسته نگهداری شده جمعی از QOwnNotes در [هوم برو](https://formulae.brew.sh/cask/qownnotes) موجود می باشد.

می توانید آن را با عبارت زیر نصب کنید:

```bash
brew install qownnotes
```

## Nix

You can also install QOwnNotes with the [Nix package manager](https://wiki.nixos.org/wiki/Nix_package_manager) under macOS natively on **x86 and Apple Silicon**, see [Install via Nix](./nix.md).

::: tip
Best use the NixOS Unstable channel to get the latest version of QOwnNotes!
:::
