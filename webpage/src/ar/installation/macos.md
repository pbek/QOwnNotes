# التثبيت على ماك أو إس

قم بتنزيل **صورة قرص macOS** الأخيرة من [إصدارات QOwnNotes على GitHub](https://github.com/pbek/QOwnNotes/releases)، وافتحها واسحب تطبيق **QOwnNotes** إلى **مجلد تطبيقاتك**.

في المرة الأولى، ستحتاج إلى النقر بزر الفأرة الأيمن على التطبيق والنقر على **فتح (Open)** لفتح QOwnNotes.

الإصدارة المتاحة حاليًّا على GitHub ليست متاحة إلا لمعمارية x86.

::: warning
إذا واجهتك رسالة الخطأ `تعذر فتح QOwnNotes.app لعدم القدرة على التحقق من المطور` (أو `QOwnNotes.app cannot be opened because the developer cannot be verified`)، فعليك زيارة **إعدادات الأمان** في نظامك (أو **Security settings**) والضغط على `فتح على أي حال` (أو `Open anyway`).
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
