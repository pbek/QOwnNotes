# پارامترهای رابط خط فرمان

به منظور کنترل رفتار اپلیکیشن، می توانید این متغیرها را در رابط کاربری خط فرمان (CLI) استفاده کنید.

| پارامتر                      | شرح                                                                                                                 |
| ---------------------------- | ------------------------------------------------------------------------------------------------------------------- |
| `--راهنمایی`                 | صفحه راهنمایی را نشان می دهد                                                                                        |
| `--نسخه`                     | شماره نسخه نرم‌افزار را نمایش می‌دهد                                                                                |
| `--قابل حمل`                 | برنامه را در حالت قابل حمل اجرا می کند                                                                              |
| `--پاک کردن-تنظیمات `        | تنظیمات را پاک کرده و برنامه را اجرا می کند                                                                         |
| `--نسخه برداری-از-تنظیمات`   | روگرفتی از تنظیمات و سایر اطلاعات مربوط به برنامه و محیط را در مارک داون گیت هاب چاپ می کند و از برنامه خارج می شود |
| `--دوره <name>`        | برنامه را در یک زمینه متفاوت برای تنظیمات و پرونده های داخلی اجرا می کند                                            |
| `--پذیرفتن-نمونه های-متعدد`  | به شروع نمونه های متعددی از QOwnNotes اجازه می دهد، حتی اگر در تنظیمات مجاز نباشند.                                 |
| `--عملیات <name>`      | پس از اینکه برنامه شروع به کار کرد، یک منو اکشن را راه اندازی می کند (قسمت زیر را ملاحظه کنید).                     |
| `--completion <shell>` | Generate shell completion code. Supports `fish`, `bash`.                                                            |

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

::: tip
To directly copy the settings dump to the clipboard to post in a [GitHub issue](https://github.com/pbek/QOwnNotes/issues) you can use `xclip` or `xsel` on Linux, `pbcopy` on macOS and `clip` on Windows:

```bash
# Copy the settings dump to the clipboard with xclip on Linux
QOwnNotes --dump-settings | xclip -selection clipboard

# Copy the settings dump to the clipboard with xsel on Linux
QOwnNotes --dump-settings | xsel --clipboard

# Copy the settings dump to the clipboard with pbcopy on macOS
QOwnNotes --dump-settings | pbcopy

# Copy the settings dump to the clipboard with clip on Windows
QOwnNotes --dump-settings | clip
```

:::

## پس از شروع به کار، منو اکشن ها را راه اندازی کنید

با پارامتر `--action <name>` می توانید پس از شروع به کار برنامه منو اکشن ها را راه اندازی کنید.

برای مثال، برای باز کردن کادر گفتگوی لیست انجام دادنی پس از شروع به کار از این کد استفاده کنید:

```bash
QOwnNotes --action actionShow_Todo_List
```

::: tip
You can get the object names of the menu action from [mainwindow.ui](https://github.com/pbek/QOwnNotes/blob/main/src/mainwindow.ui). تنها عنوان منوی انگلیسی را جستجو کنید. توجه داشته باشید که این متن ها با گذشت زمان می توانند تغییر کنند.
:::

به منظور راه اندازی [عملیات اسکریپت نویسی](../scripting/methods-and-objects.md#registering-a-custom-action)، از `customAction_` و پس از آن از شناساگر عملیات سفارشی استفاده کنید. شناساگر عملیات سفارشی نخستین پارامتر درخواست `script.registerCustomAction` در اسکریپت است.

برای مثال، به منظور اجرای عملیات سفارشی `myAction`، QOwnNotes را به طریقی نظیر این راه اندازی کنید:

```bash
QOwnNotes --action customAction_myAction
```

::: tip
درصورتی که QOwnNotes را برای بار دوم با پارامتر عملیاتی اجرا کرده و تنها یک طرح برنامه را مجاز نموده باشید، عملیات منو در طرح نخست اعمال خواهد شد.
:::
