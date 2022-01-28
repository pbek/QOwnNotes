# نصب به صورت AppImage

شما می توانید آخرین نسخه AppImage را از [صفحه انتشار های QOwnNotes](https://github.com/pbek/QOwnNotes/releases) بارگیری کنید. آن باید در صفحه مورد نظر به شکلی نظیر `QOwnNotes-x86_64.AppImage` نامگذاری شود.

::: tip
در صورتی که [jq](https://stedolan.github.io/jq/) را نصب کرده اید، آخرین نسخه AppImage را نیز می توانید از این طریق دانلود کنید:

```bash
# query the latest Linux release from the QOwnNotes API, parse the JSON for the URL and download it
curl -L https://api.qownnotes.org/latest_releases/linux | jq .url | xargs curl -Lo QOwnNotes-x86_64.AppImage
```
:::

سپس می توانید مجوزهای اجرا را در پرونده تغییر دهید:

```bash
chmod a+x QOwnNotes-*.AppImage
```

پس از آن حتماً قادر به اجرای AppImage برای استفاده از QOwnNotes خواهید بود.

::: warning
در صورت تمایل به استفاده از **بروز رسان خودکار**، لطفاً مطمئن شوید که AppImage خود را در جایی که حساب کاربری شما به آن دسترسی نوشتاری دارد (مثلاً در فهرست شخصی)، قرار دهید.
:::

::: tip
If you have troubles running the AppImage, because your glibc version is too old you can try the [AppImage built on OBS](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/AppImage/QOwnNotes-latest-x86_64.AppImage), it should be built with glibc 2.16.
:::
