# نصب در سیستم عامل مکینتاش

آخرین **دیسک ایمیج سیستم عامل مکینتاش** را از [انتشارهای QOwnNotes در گیت هاب](https://github.com/pbek/QOwnNotes/releases) بارگیری کرده، آن را باز کنید و اپ **QOwnNotes** را به **پوشه برنامه** خود بکشید.

The first time you have to right-click the application and click **Open** to open QOwnNotes.

The GitHub release is currently only built for the x86 platform.

::: warning
در صورت دریافت پیام خطای `QOwnNotes.app قادر به باز شدن نیست چون توسعه دهنده قابل تایید نمی باشد`، شما باید **تنظیمات امنیتی** سیستم عامل مکینتاش خود را بازبینی کرده و بر روی `باز کردن به هر صورت` کلیک کنید.
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
