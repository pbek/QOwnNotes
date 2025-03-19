# نصب سیستم عامل ویندوز مایکروسافت

Download the latest **Windows archive** from [QOwnNotes Releases on GitHub](https://github.com/pbek/QOwnNotes/releases) (look for a file called `QOwnNotes.zip`) and unzip it to anywhere you like. هیچ نیازی به نصب نیست!

::: warning
در صورت تمایل به استفاده از **بروز رسان خودکار**، لطفاً مطمئن شوید که آن هر جایی که حساب کاربری شما دسترسی نوشتاری دارد، از حالت فشرده خارج شده باشد. By default, your user account most likely **doesn't have write access** to places like `C:\Program Files (x86)` or `C:\Program Files`.
:::

در این صورت می ‌توانید `QOwnNotes.exe` را از پوشه `QOwnNotes` خود به طور مستقیم و بدون نیاز به نصب اجرا کنید.

### حالت قابل حمل

برای اجرای QOwnNotes در **حالت قابل حمل** که در آن هر چیزی (از جمله یادداشت های شما) فقط در پوشه `QOwnNotes` شما ذخیره می شوند از `QOwnNotesPortable.bat` استفاده کنید.

::: tip
در صورتی که که اجازه مدیریت رایانه خود را ندارید، نیازی هم به حالت قابل حمل نخواهید داشت. QOwnNotes نیازی به نصب شدن ندارد!
:::

## ویندوز XP

Qt با نسخه 5.8 از ویندوز XP پشتیبانی نمی کند ولی QOwnNotes در حال حاضر نیز با Qt 5.7 ایجاد شده است تا به کاربران ویندوز XP همچنان امکان استفاده از آن را بدهد.

شما باید فایل فشرده را از [AppVeyor](https://ci.appveyor.com/project/pbek/qownnotes/build/artifacts) دانلود کرده و به داخل پوشه مورد نظر از حالت فشرده خارج کنید.

در این صورت می ‌توانید `QOwnNotes.exe` را از آن پوشه به طور مستقیم و بدون نیاز به نصب اجرا کنید.

::: tip
Info
مکانیسم بروزرسانی خودکار با سازه AppVeyor برای ویندوز XP کار نمی کند!
شما باید خودتان انتشارهای جدید را بارگیری کنید.
:::

## چاکلتلی

یک بسته نگهداری شده جمعی از QOwnNotes در [Chocolatey](https://chocolatey.org/packages/qownnotes/) موجود می باشد.

می توانید آن را با عبارت زیر نصب کنید:

```shell
choco install qownnotes
```

## اسکوپ

یک [بسته نگهداری شده جمعی از QOwnNotes](https://github.com/ScoopInstaller/Extras/blob/master/bucket/qownnotes.json) در [اسکوپ](https://scoop.sh/) در دسترس قرار دارد. در صورت افزودن باکت افزونه ها (Extras)، برای نصب QOwnNotes به شکل قابل حمل می توانید از آن استفاده کنید.

```shell
scoop bucket add extras
scoop update
scoop install qownnotes
```
