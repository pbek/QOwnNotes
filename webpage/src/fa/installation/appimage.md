# نصب به صورت AppImage

شما می توانید آخرین نسخه AppImage را از [صفحه انتشار های QOwnNotes](https://github.com/pbek/QOwnNotes/releases) بارگیری کنید. آن باید در صفحه مورد نظر به شکلی نظیر `QOwnNotes-x86_64.AppImage` نامگذاری شود.

::: tip
If you have [jq](https://stedolan.github.io/jq/) installed, you can also download the latest AppImage directly:

```bash
# query the latest Linux release from the QOwnNotes API, parse the JSON for the URL and download it
curl -L https://api.qownnotes.org/latest_releases/linux | jq .url | xargs curl -Lo QOwnNotes-x86_64.AppImage
```

:::

سپس می توانید مجوزهای اجرا را در پرونده تغییر دهید:

```bash
chmod a+x QOwnNotes-*.AppImage
```

::: tip
AppImages need `libfuse2` to run! For example under Ubuntu you can install the library with `sudo apt install libfuse2`.
:::

Afterward you should be able to execute the AppImage to run QOwnNotes.

::: warning
در صورت تمایل به استفاده از **بروز رسان خودکار**، لطفاً مطمئن شوید که AppImage خود را در جایی که حساب کاربری شما به آن دسترسی نوشتاری دارد (مثلاً در دایرکتوری شخصی)، قرار دهید.
:::

::: tip
در صورت بروز مشکل در اجرای AppImage، به دلیل بسیار قدیمی بودن نسخه glibc شما، [AppImage built on OBS](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/AppImage/QOwnNotes-latest-x86_64.AppImage) که بایستی با glibc 2.16 ایجاد شود را می‌توانید امتحان کنید.
:::
