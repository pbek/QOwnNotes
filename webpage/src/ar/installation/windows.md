# التثبيت على ميكروسوفت ويندوز

نزّل **الملف المضغوط لويندوز** من [إصدارات QOwnNotes على GitHub](https://github.com/pbek/QOwnNotes/releases) (ابحث عن ملف اسمه `QOwnNotes.zip`) واستخرجه إلى أي مكانٍ تشاء. لا حاجة إلى تثبيت أي شيء!

::: warning
إذا أردت استخدام **التحديث التلقائي** فنرجو التأكد من استخراجه إلى مكان مسموح لحسابك بالكتابة فيه. غالبًا ليس لحسابك **إذن الكتابة** في مجلدات مثل <code dir="ltr">C:\Program Files (x86) أو <code dir="ltr">C:\Program Files.
:::

يمكنك إذًا تشغيل `QOwnNotes.exe` من مجلد `QOwnNotes` مباشرةً؛ لا حاجة إلى تثبيت أي شيء.

### الوضع المحمول

استخدم `QOwnNotesPortable.bat` لتشغيل QOwnNotes في **الوضع المحمول** حيث سيتم تخزين كل شيء (بما في ذلك ملاحظاتك) في مجلد `QOwnNotes` الخاص بك.

::: tip
لستَ مضطرًا إلى استخدام الوضع المحمول إن لم تكن لديك صلاحيات إدارية في حاسوبك. لا يحتاج QOwnNotes إلى تثبيته ليعمل!
:::

## Windows 8.1 and older

The Windows release is built with **Qt6** and needs at least **Windows 10**.

The legacy Qt 5.7 build for Windows XP to Windows 8.1 is no longer updated. The final version is **QOwnNotes 26.10.0**, which you can download as [QOwnNotes-Qt5.7-legacy.zip](https://github.com/pbek/QOwnNotes/releases/download/v26.10.0/QOwnNotes-Qt5.7-legacy.zip) ([SHA-256 checksum](https://github.com/pbek/QOwnNotes/releases/download/v26.10.0/QOwnNotes-Qt5.7-legacy.zip.sha256sum)) from the [26.10.0 release on GitHub](https://github.com/pbek/QOwnNotes/releases/tag/v26.10.0).

Unzip it to a folder of your liking and run `QOwnNotes.exe` from that folder, no installation is required.

::: tip
Info
The automatic update mechanism does not work with the legacy build!
:::

## Chocolatey

There is a community-maintained package of QOwnNotes at [Chocolatey](https://chocolatey.org/packages/qownnotes/).

يمكنك تثبيتها بالأمر:

```shell
choco install qownnotes
```

## Scoop

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
