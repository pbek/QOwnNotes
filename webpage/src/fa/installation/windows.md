# نصب سیستم عامل ویندوز مایکروسافت

Download the latest **Windows archive** from [QOwnNotes Releases on GitHub](https://github.com/pbek/QOwnNotes/releases) (look for a file called `QOwnNotes.zip`) and unzip it to anywhere you like. هیچ نیازی به نصب نیست!

::: warning
در صورت تمایل به استفاده از **بروز رسان خودکار**، لطفاً مطمئن شوید که آن هر جایی که حساب کاربری شما دسترسی نوشتاری دارد، از حالت فشرده خارج شده باشد. By default, your user account most likely **doesn't have write access** to places like `C:\Program Files (x86)` or `C:\Program Files`.
:::

در این صورت می ‌توانید `QOwnNotes.exe` را از پوشه `QOwnNotes` خود به طور مستقیم و بدون نیاز به نصب اجرا کنید.

### حالت قابل حمل

برای اجرای QOwnNotes در **حالت قابل حمل** که در آن هر چیزی (از جمله یادداشت های شما) فقط در پوشه `QOwnNotes` شما ذخیره می شوند از  `QOwnNotesPortable.bat` استفاده کنید.

::: tip
در صورتی که که اجازه مدیریت رایانه خود را ندارید، نیازی هم به حالت قابل حمل نخواهید داشت. QOwnNotes نیازی به نصب شدن ندارد!
:::

## Windows 8.1 and older

The Windows release is built with **Qt6** and needs at least **Windows 10**.

The legacy Qt 5.7 build for Windows XP to Windows 8.1 is no longer updated. The final version is **QOwnNotes 26.10.0**, which you can download as [QOwnNotes-Qt5.7-legacy.zip](https://github.com/pbek/QOwnNotes/releases/download/v26.10.0/QOwnNotes-Qt5.7-legacy.zip) ([SHA-256 checksum](https://github.com/pbek/QOwnNotes/releases/download/v26.10.0/QOwnNotes-Qt5.7-legacy.zip.sha256sum)) from the [26.10.0 release on GitHub](https://github.com/pbek/QOwnNotes/releases/tag/v26.10.0).

Unzip it to a folder of your liking and run `QOwnNotes.exe` from that folder, no installation is required.

::: tip
Info
The automatic update mechanism does not work with the legacy build!
:::

## چاکلتلی

There is a community-maintained package of QOwnNotes at [Chocolatey](https://chocolatey.org/packages/qownnotes/).

می توانید آن را با عبارت زیر نصب کنید:

```shell
choco install qownnotes
```

## اسکوپ

There is a [community-maintained package of QOwnNotes](https://github.com/ScoopInstaller/Extras/blob/master/bucket/qownnotes.json) at [Scoop](https://scoop.sh/). If you add the Extras bucket, you can use it to install QOwnNotes in portable mode.

```shell
scoop bucket add extras
scoop update
scoop install qownnotes
```

## WinGet

There is a [community-maintained package of QOwnNotes](https://github.com/microsoft/winget-pkgs/tree/master/manifests/p/pbek/QOwnNotes) for [WinGet](https://github.com/microsoft/winget-cli).

You can install it with:

```shell
winget install qownnotes
```

## MSYS2

There is a [community-maintained package of QOwnNotes](https://packages.msys2.org/base/mingw-w64-qownnotes) for [MSYS2](hhttps://www.msys2.org/).

You can install it with:

```shell
pacman -S mingw-w64-qownnotes
```
