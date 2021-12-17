# پارامترهای رابط خط فرمان

شما می توانید از این پارامترها در رابط خط فرمان استفاده کنید:

| پارامتر                      | شرح                                                                                                                                        |
| ---------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------ |
| `--راهنمایی`                 | صفحه راهنمایی را نشان می دهد                                                                                                               |
| `--version`                  | شماره نسخه نرم‌افزار را نمایش می‌دهد                                                                                                       |
| `--portable`                 | Runs the application in portable mode                                                                                                      |
| `--clear-settings`           | Clears the settings and runs the application                                                                                               |
| `--dump-settings`            | Prints out a dump of the settings and other information about the application and environment in GitHub Markdown and exits the application |
| `--session <name>`     | Runs the application in a different context for settings and internal files                                                                |
| `--allow-multiple-instances` | Allows multiple instances of QOwnNotes to be started even if disallowed in the settings                                                    |
| `--action <name>`      | Triggers a menu action after the application was started (see below)                                                                       |

::: tip
اگر با نصب QOwnNotes به مشکل برخورد کردید، شاید بخواهید با استفاده از پارامتر `--دوره` برنامه را با تنظیمات جدید و بدون از دست دادن تنظیمات فعلی خود آغاز کنید.

```bash
QOwnNotes --تست دوره
```
:::

شما می توانید برنامه را به صورتی متفاوت با رابط خط فرمان در سیستم عامل های مختلف اجرا کنید:

| سیستم عامل         | فرمان                                                      |
| ------------------ | ---------------------------------------------------------- |
| لینوکس             | `QOwnNotes` (یا `qownnotes` اگر به صورت اسنپ نصب شده باشد) |
| سیستم عامل مکینتاش | `/Applications/QOwnNotes.app/Contents/MacOS/QOwnNotes`     |
| ویندوز             | `QOwnNotes.exe`                                            |

## پس از شروع به کار، منو اکشن ها را راه اندازی کنید

با پارامتر `--action <name>` می توانید پس از شروع به کار برنامه منو اکشن ها را راه اندازی کنید.

برای مثال، برای باز کردن کادر گفتگوی لیست انجام دادنی پس از شروع به کار از این کد استفاده کنید:

```bash
QOwnNotes --action actionShow_Todo_List
```

::: tip
شما می توانید اسامی هدف منو اکشن را از [mainwindow.ui](https://github.com/pbek/QOwnNotes/blob/develop/src/mainwindow.ui)دریافت کنید. تنها عنوان منوی انگلیسی را جستجو کنید. توجه داشته باشید که این متن ها با گذشت زمان می توانند تغییر کنند.
:::

به منظور راه اندازی [عملیات اسکریپت نویسی](../scripting/methods-and-objects.md#registering-a-custom-action)، از `customAction_` و پس از آن از شناساگر عملیات سفارشی استفاده کنید. شناساگر عملیات سفارشی نخستین پارامتر درخواست `script.registerCustomAction` در اسکریپت است.

برای مثال، به منظور اجرای عملیات سفارشی `myAction`، QOwnNotes را به طریقی نظیر این راه اندازی کنید:

```bash
QOwnNotes --action customAction_myAction
```

::: tip
درصورتی که QOwnNotes را برای بار دوم با پارامتر عملیاتی اجرا کرده و تنها یک طرح برنامه را مجاز نموده باشید، عملیات منو در طرح نخست اعمال خواهد شد.
:::
