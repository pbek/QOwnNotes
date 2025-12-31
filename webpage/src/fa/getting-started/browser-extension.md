---
image: /img/bookmarks.png
---

# افزونه مرورگر همراه وب QOwnNotes

امکان برش زدن از صفحه مرورگر و مدیریت نشانک های مرورگر را در مرورگرها و سیستم های عامل فراهم می کند.

::: tip Info
**QOwnNotes must be running** for the Web Companion browser extension to work. But no internet connection is needed, the browser extensions works **offline**.
:::

## نصب

1. دریافت افزونه
   - [کروم وب استور](https://chrome.google.com/webstore/detail/qownnotes-web-companion/pkgkfnampapjbopomdpnkckbjdnpkbkp)
   - [صفحه افزونه های فایرفاکس](https://addons.mozilla.org/firefox/addon/qownnotes-web-companion)
   - شما می توانید افزونه را در [گیت هاب](https://github.com/qownnotes/web-companion/) نیز پیدا کنید.
2. برای پیکربندی افزونه توکن امنیتی را اضافه کنید.
   - نخستین باری که روی آیکون افزونه مرورگر QOwnNotes کلیک کنید، کادر محاوره ای دارای توکن امنیتی دریافت خواهید کرد. رونوشت برداری توکن.
   - به مکان مدیریت افزونه مرورگر خود بروید. روی جزئیات افزونه QOwnNotes کلیک کنید.
   - توکن را در قسمت توکن امنیتی بچسبانید.
3. To use the private mode, you need to allow the extension to run in private windows in the extension settings.

## وب کلیپر

![وب کلیپر](/img/web-clipper.png)

به منظور استفاده کارآمد از **وب کلیپر** بر روی یک صفحه وب یا متن منتخب کلیک راست کنید.  علاوه بر این، می توانید یک یادداشت جدید را با **نماگرفتی** از صفحه وب کنونی ایجاد نمایید.

::: warning
Info If the web clipper doesn't work, please check if the QOwnNotes desktop application is running, and the Web Companion browser extension is configured with the security token.

Also, make sure you accept the consent dialog when you first click the extension icon in the browser toolbar.

If you are using Firefox, you also need to make sure that you have turned on the optional permission `Access your data for all websites` in the _Permission_ extension settings, otherwise you might get an error `TypeError: chrome.tabs.captureVisibleTab is not a function`.
:::

::: tip
وب کلیپر هم قابل اسکریپت نویسی می باشد! در صورت تمایل به کنترل آنچه که از صفحات وب برش می زنید، نگاهی به [websocketRawDataHook](../scripting/hooks.md#websocketrawdatahook) بیندازید.
:::

## نشانک ها

![نشانک ها](/img/bookmarks.png)

در صورت تمایل به کنترل کامل نشانک های مرورگر خود و **استفاده از آنها در مرورگرها و سیستم عامل های مختلف** می‌توانید از افزونه مرورگر QOwnNotes بهره گیرید.

By default, the browser extension will show all **links of the current note** in a popup when you click the QOwnNotes icon in your browser. این پیوندها یک برچسب `کنونی` می گیرند.

همچنین می توانید با `نشانک های` برچسب یادداشت (قابل تغییر در تنظیمات) **نشانک های خود را در یادداشت ها مدیریت کنید**. این پیوند ها همچنین برچسب ها و توضیحی دارند که در افزونه مرورگر نشان داده می شود.

نشانک های جدید در یادداشتی با نام `نشانک ها` ذخیره شده‌اند (همچنین در بخش تنظیمات قابل تغییر هستند).

::: tip
علاوه بر این می توانید نشانک های مرورگر خود را با افزونه مرورگر همراه وب در QOwnNotes وارد نمایید!
:::

### چیدمان پیوندهای نشانک

```markdown
- [Webpage name](https://www.example.com)
- [Webpage name](https://www.example.com) #tag1 #tag2
- [Webpage name](https://www.example.com) some description only
- [Webpage name](https://www.example.com) #tag1 #tag2 some description and tags

* [Webpage name](https://www.example.com) the alternative list character also works
```

You are able to search for name, url tags or description in the browser extension. In the current note all other links inside of text are parsed, but without tags or description.
