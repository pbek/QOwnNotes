# بناء QOwnNotes

## البناء من GitHub

يمكنك كذلك بناء QOwnNotes بنفسك من الكود المصدري.

استنسخ الكود مباشرةً من مستودع git:

```bash
git clone https://github.com/pbek/QOwnNotes.git -b release
cd QOwnNotes
git submodule update --init
```

For debugging, it's easiest to use QtCreator. When downloading [Qt Creator](https://www.qt.io/download-qt-installer-oss), you will also need the packages `qtwebsockets` and `qt5compat` to build QOwnNotes with Qt6 (only `qtwebsockets` for Qt5). وإذا كنت تبني على ويندوز، ربما تفضل البقاء مع *MinGw 64-bit*. On Linux, you most likely have a package for QtCreator in your repository.

بعدئذٍ، افتح ملف المشروع `src/QOwnNotes.pro` وانقر على **Build** ثم **Build Project QOwnNotes**.

If you k now what you are doing, you can also build QOwnNotes directly in your terminal:

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
إذا أردت المساهمة في المشروع بكود مصدري، برجاء عمل طلبات السحب على فرع `main`.
:::

## ملفات الكود المصدري المضغوطة

يمكنك العثور على الملفات المضغوطة لكود QOwnNotes المصدري في [أرشيف مصادر QOwnNotes](https://github.com/pbek/QOwnNotes/releases).

يمكن الوصول إلى الملفات المضغوطة للكود المصدري برابط مثل هذا:

`https://github.com/pbek/QOwnNotes/releases/download/v23.7.0/qownnotes-23.7.0.tar.xz`
