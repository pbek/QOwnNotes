# التثبيت كـ AppImage

يمكنك تنزيل AppImage الأخيرة من [صفحة إصدارات QOwnNotes](https://github.com/pbek/QOwnNotes/releases). من المفترض أن يكون اسم ملفها شيئا مثل `QOwnNotes-x86_64.AppImage` في هذه الصفحة.

::: tip
If you have [jq](https://stedolan.github.io/jq/) installed, you can also download the latest AppImage directly:

```bash
<span dir="rtl"># استعلم عن إصدارة لينكس الأخيرة عبر QOwnNotes API، وحلل JSON الناتج للحصول على الرابط، ونزّلها</span>
curl -L https://api.qownnotes.org/latest_releases/linux | jq .url | xargs curl -Lo QOwnNotes-x86_64.AppImage
```

:::

عندئذٍ يمكنك جعل الملف قابلا للتنفيذ:

```bash
chmod a+x QOwnNotes-*.AppImage
```

::: tip
صور AppImage تحتاج مكتبة `libfuse2` حتى تعمل! على أوبنتو مثلا، يمكنك تثبيتها بالأمر `sudo apt install libfuse2`.
:::

بعدئذٍ ستتمكن من تنفيذ الـ&nbsp;AppImage لتشغيل QOwnNotes.

::: warning
إذا أردت استخدام **التحديث التلقائي** فنرجو التأكد من وضع ملف AppImage في مكان مسموح لحسابك بالكتابة فيه، مثل مجلد المنزل.
:::

::: tip
إذا واجهتك مشكلة في تشغيل الـ&nbsp;AppImage لأن نسخة مكتبة glibc التي لديك قديمة، يمكنك تجربة [الـ&nbsp;AppImage المبنية على OBS](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/AppImage/QOwnNotes-latest-x86_64.AppImage)؛ من المفترض أنها مبنية مع glibc النسخة 2.16.
:::

## Portable mode

You can run the AppImage in **portable mode** by passing the `--portable` argument:

```bash
./QOwnNotes-x86_64.AppImage --portable
```

In portable mode all application data is stored in a `Data` folder next to the AppImage file instead of in your home directory. This includes:

- Settings (`Data/QOwnNotes.ini`)
- The notes database (`Data/QOwnNotes.sqlite`)
- Your notes (`Data/Notes/`)
- Scripts, spell-check dictionaries, and other application data

This makes it easy to carry QOwnNotes together with all your notes on a USB drive or any other portable storage device.

::: tip
Make sure the directory containing the AppImage file is writable, so that the `Data` folder can be created next to it.
:::

::: tip
If QOwnNotes logs `Could not write secret to keychain`, install the missing Secret Service packages on your Linux distribution and restart your desktop session.

For GNOME and other Secret Service based desktops, install `gnome-keyring`, `libsecret` and `seahorse`.

For KDE Plasma, install KWallet support such as `kwalletmanager`.

QOwnNotes will fall back to legacy encryption if the desktop keychain is unavailable.
:::
