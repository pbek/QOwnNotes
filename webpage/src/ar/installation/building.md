# بناء QOwnNotes

## البناء من GitHub

يمكنك كذلك بناء QOwnNotes بنفسك من المصدر البرمجي.

Checkout the code of the last release directly from the git repository:

```bash
git clone https://github.com/pbek/QOwnNotes.git -b release
cd QOwnNotes
git submodule update --init
```

للتنقيح، من الأسهل استعمال QtCreator. عند تنزيل [Qt Creator](https://www.qt.io/download-qt-installer-oss)، ستحتاج كذلك إلى حزمة `qtwebsockets` لبناء QOwnNotes على Qt6. If you build under Windows, you want to stick to _MinGw 64-bit_. على لينكس، غالبا ستجد QtCreator في مستودعات توزيعتك.

بعدئذٍ، افتح ملف المشروع `src/QOwnNotes.pro` وانقر على **Build** ثم **Build Project QOwnNotes**.

ويمكنك كذلك بناء QOwnNotes من الطرفية إذا كنت تعرف ما تفعل:

```bash
cd src

# ابنِ الملفات الثنائية للترجمات إذا أردت لغة غير الإنجليزية
lrelease QOwnNotes.pro

# حضّر عملية البناء وابنِ التطبيق
qmake
make
```

رجاءً لا تتردد في المشاركة في هذا المشروع بكود برمجي أو اقتراحات أو إبلاغ عن مشاكل على [صفحة مسائل QOwnNotes](https://github.com/pbek/QOwnNotes/issues).

قم بزيارة [QOwnNotes على GitHub](https://github.com/pbek/QOwnNotes).

::: tip
If you want to contribute source code to the project please make your pull requests to the `main` branch.
:::

## ملفات المصدر البرمجي المضغوطة

يمكنك العثور على الملفات المضغوطة لمصدر QOwnNotes في [أرشيف مصادر QOwnNotes](https://github.com/pbek/QOwnNotes/releases).

يمكن الوصول إلى الملفات المضغوطة للكود المصدري برابط مثل هذا:

`https://github.com/pbek/QOwnNotes/releases/download/v23.7.0/qownnotes-23.7.0.tar.xz`

## البناء بـ&nbsp;Nix

إذا كان لديك [مدير الحزم Nix](https://wiki.nixos.org/wiki/Nix_package_manager) فيمكنك استنساخ المصدر بــجت، كما شُرح بالأعلى، ثم بناءه بالأوامر التالية:

```bash
# Get devenv if you don't have it already
nix-shell -p devenv

# Open a shell with all the tools you need
devenv shell

# Build the application
just nix-build

# Run the built application
just nix-run
```
