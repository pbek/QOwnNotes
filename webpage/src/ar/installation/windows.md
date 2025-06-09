# التثبيت على ميكروسوفت ويندوز

نزّل **الملف المضغوط لويندوز** من [إصدارات QOwnNotes على GitHub](https://github.com/pbek/QOwnNotes/releases) (ابحث عن ملف اسمه `QOwnNotes.zip`) واستخرجه إلى أي مكانٍ تشاء. لا حاجة إلى تثبيت أي شيء!

::: warning
إذا أردت استخدام **التحديث التلقائي** فنرجو التأكد من استخراجه إلى مكان مسموح لحسابك بالكتابة فيه. غالبًا ليس لحسابك **إذن الكتابة** في مجلدات مثل <code dir="ltr">C:\Program Files (x86)</code> أو <code dir="ltr">C:\Program Files</code>.
:::

يمكنك إذًا تشغيل `QOwnNotes.exe` من مجلد `QOwnNotes` مباشرةً؛ لا حاجة إلى تثبيت أي شيء.

### الوضع المحمول

استخدم `QOwnNotesPortable.bat` لتشغيل QOwnNotes في **الوضع المحمول** حيث سيتم تخزين كل شيء (بما في ذلك ملاحظاتك) في مجلد `QOwnNotes` الخاص بك.

::: tip
لستَ مضطرًا إلى استخدام الوضع المحمول إن لم تكن لديك صلاحيات إدارية في حاسوبك. لا يحتاج QOwnNotes إلى تثبيته ليعمل!
:::

## ويندوز إكس بي

أوقف إطار Qt دعم ويندوز إكس&nbsp;بي منذ النسخة 5.8، لكن يتم بناء QOwnNotes الآن بنسخة 5.7 من Qt لجعله متاحًا لمستخدمي ويندوز إكس&nbsp;بي.

عليك تنزيل مِلف ZIP من [AppVeyor](https://ci.appveyor.com/project/pbek/qownnotes/build/artifacts) بنفسك واستخراجه إلى أي مجلد تشاء.

يمكنك إذًا تشغيل `QOwnNotes.exe` من هذا المجلد مباشرةً؛ لا حاجة إلى تثبيت أي شيء.

::: tip
معلومة
آلية التحديث التلقائي لا تعمل مع نسخة AppVeyor المبنية لويندوز إكس&nbps;بي!
عليك تنزيل الإصدارات الجديدة بنفسك.
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
