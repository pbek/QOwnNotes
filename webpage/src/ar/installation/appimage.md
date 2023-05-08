# التثبيت كـ AppImage

يمكنك تنزيل AppImage الأخيرة من [صفحة إصدارات QOwnNotes](https://github.com/pbek/QOwnNotes/releases). من المفترض أن يكون اسم ملفها شيئا مثل `QOwnNotes-x86_64.AppImage` في هذه الصفحة.

::: tip
إذا كان مثبتًا لديك [jq](https://stedolan.github.io/jq/) فيمكنك أيضا تنزيل AppImage الأخيرة مباشرةً:

```bash
<span dir="rtl"># استعلم عن إصدارة لينكس الأخيرة عبر QOwnNotes API، وحلل JSON الناتج للحصول على الرابط، ونزّلها</span>
curl -L https://api.qownnotes.org/latest_releases/linux | jq .url | xargs curl -Lo QOwnNotes-x86_64.AppImage
```
:::

عندئذٍ يمكنك جعل الملف قابلا للتنفيذ:

```bash
chmod a+x QOwnNotes-*.AppImage
```

بعدئذٍ ستتمكن من تنفيذ الـ&nbsp;AppImage لتشغيل QOwnNotes.

::: warning
إذا أردت استخدام **التحديث التلقائي** فنرجو التأكد من وضع ملف AppImage في مكان مسموح لحسابك بالكتابة فيه، مثل مجلد المنزل.
:::

::: tip
إذا واجهتك مشكلة في تشغيل الـ&nbsp;AppImage لأن نسخة مكتبة glibc التي لديك قديمة، يمكنك تجربة [الـ&nbsp;AppImage المبنية على OBS](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/AppImage/QOwnNotes-latest-x86_64.AppImage)؛ من المفترض أنها مبنية مع glibc النسخة 2.16.
:::
