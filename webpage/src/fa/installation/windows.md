# نصب سیستم عامل ویندوز مایکروسافت

آخرین نسخه **بایگانی ویندوز** را از [انتشار های QOwnNotes در گیت هاب](https://github.com/pbek/QOwnNotes/releases) بارگیری نموده و هر جایی که دوست دارید آن را از حالت فشرده خارج کنید. هیچ نیازی به نصب نیست!

::: warning
در صورت تمایل به استفاده از **بروز رسان خودکار**، لطفاً مطمئن شوید که آن هر جایی که حساب کاربری شما دسترسی نوشتاری دارد، از حالت فشرده خارج شده باشد. به صورت پیش فرض حساب کاربری شما به احتمال زیاد **دسترسی نوشتاری** برای قرار گرفتن در آدرس هایی نظیر `C:\Program Files (x86)` یا `C:\Program Files` را ندارد.
:::

در این صورت می ‌توانید `QOwnNotes.exe` را از پوشه `QOwnNotes` خود به طور مستقیم و بدون نیاز به نصب اجرا کنید.

### حالت قابل حمل

برای اجرای QOwnNotes در **حالت قابل حمل** که در آن هر چیزی (از جمله یادداشت های شما) فقط در پوشه `QOwnNotes` شما ذخیره می شوند از  `QOwnNotesPortable.bat` استفاده کنید.

::: tip
در صورتی که که اجازه مدیریت رایانه خود را ندارید، نیازی هم به حالت قابل حمل نخواهید داشت. QOwnNotes نیازی به نصب شدن ندارد!
:::

## ویندوز XP

Qt با نسخه 5.8 از ویندوز XP پشتیبانی نمی کند ولی QOwnNotes در حال حاضر نیز با Qt 5.7 ایجاد شده است تا به کاربران ویندوز XP همچنان امکان استفاده از آن را بدهد.

You need to download the ZIP file from [AppVeyor](https://ci.appveyor.com/project/pbek/qownnotes/build/artifacts) yourself and unzip it to a folder of your liking.

You can then directly run `QOwnNotes.exe` from that folder, no installation is required.

::: tip Info
The automatic update mechanism does not work with the AppVeyor build for Windows XP! You will have to download new releases yourself.
:::

## چاکلتلی

There is a community maintained package of QOwnNotes at [Chocolatey](https://chocolatey.org/packages/qownnotes/).

You can install it with:

```shell
choco install qownnotes
```

## اسکوپ

There is a [community maintained package of QOwnNotes](https://github.com/ScoopInstaller/Extras/blob/master/bucket/qownnotes.json) at [Scoop](https://scoop.sh/). If you add the Extras bucket you can use it to install QOwnNotes in portable mode.

```shell
scoop bucket add extras
scoop update
scoop install qownnotes
```
