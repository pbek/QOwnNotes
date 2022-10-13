# التثبيت على ميكروسوفت ويندوز

قم بتنزيل **الملف المضغوط لويندوز** من [إصدارات QOwnNotes على GitHub](https://github.com/pbek/QOwnNotes/releases) واستخرجه إلى أي مكانٍ تشاء. لا حاجة إلى تثبيت أي شيء!

::: warning
إذا أردت استخدام **التحديث التلقائي** فنرجو التأكد من استخراجه إلى مكان مسموح لحسابك بالكتابة فيه. بشكل افتراضي حسابك غالبا **ليس لديه إذن الكتابة** في مجلدات مثل <code dir="ltr">C:\Program Files (x86)</code> أو <code dir="ltr">C:\Program Files</code>.
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

يرعى المجتمع حزمة QOwnNotes في [Chocolatey](https://chocolatey.org/packages/qownnotes/).

يمكنك تثبيتها بالأمر:

```shell
choco install qownnotes
```

## Scoop

توجد [حزمة يرعاها المجتمع من QOwnNotes](https://github.com/ScoopInstaller/Extras/blob/master/bucket/qownnotes.json) في [Scoop](https://scoop.sh/). يمكنك تثبيت QOwnNotes في الوضع المحمول إذا أضفت «Extras bucket».

```shell
scoop bucket add extras
scoop update
scoop install qownnotes
```
