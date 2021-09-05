---
image: /img/bookmarks.png
---

# افزونه مرورگر همراه وب QOwnNotes

به منظور نصب [ ** افزونه مرورگر همراه وب QOwnNotes ** ](https://github.com/qownnotes/web-companion/) نگاهی به [ کروم وب استور ](https://chrome.google.com/webstore/detail/qownnotes-web-companion/pkgkfnampapjbopomdpnkckbjdnpkbkp) یا [ صفحه افزونه های فایرفاکس ](https://addons.mozilla.org/firefox/addon/qownnotes-web-companion) بیندازید.

شما می توانید افزونه را در [گیت هاب](https://github.com/qownnotes/web-companion/) نیز پیدا کنید.

::: tip Info
QOwnNote برای کار با افزونه مرورگر همراه وب باید اجرا شود. افزونه های مرورگر به صورت **آفلاین** و بدون نیاز به اتصال اینترنت اجرا می شوند.
:::

## وب کلیپر

![وب کلیپر](/img/web-clipper.png)

به منظور استفاده کارآمد از **وب کلیپر** بر روی یک صفحه وب یا متن منتخب کلیک راست کنید.  علاوه بر این، می توانید یک یادداشت جدید را با **نماگرفتی** از صفحه وب کنونی ایجاد نمایید.

::: tip
وب کلیپر هم قابل اسکریپت نویسی می باشد! در صورت تمایل به کنترل آنچه که از صفحات وب برش می زنید، نگاهی به [websocketRawDataHook](../scripting/hooks.md#websocketrawdatahook) بیندازید.
:::

## نشانک ها

![نشانک ها](/img/bookmarks.png)

If you want full control over your browser bookmarks and **use them across different browsers and operating systems** then the QOwnNotes browser extension is for you.

By default the browser extension will show all **links of the current note** in a popup when you click the QOwnNotes icon in your browser. These links will get a tag `current`.

You can also **manage your bookmarks in notes** with the note tag `bookmarks` (changeable in the settings). These links can also have tags and a description that will be shown in the browser extension.

New bookmarks are stored in a note called `Bookmarks` (also changeable in the settings).

::: tip
You can also import your browser bookmarks into QOwnNotes with the web companion browser extension!
:::

### چیدمان پیوندهای نشانک

```markdown
- [نام صفحه وب](https://www.example.com)
- [نام صفحه وب](https://www.example.com) #برچسب1 #برچسب2
- [نام صفحه وب](https://www.example.com) فقط مقداری توضیح
- [نام صفحه وب](https://www.example.com) #برچسب1 #برچسب2 مقداری توضیح و برچسب ها
* [نام صفحه وب](https://www.example.com) نویسه لیست جایگزین هم کار می کند
```

You are able to search for name, url tags or description in the browser extension.
