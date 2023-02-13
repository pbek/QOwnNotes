# بناء QOwnNotes

## البناء من GitHub

يمكنك كذلك بناء QOwnNotes بنفسك من الكود المصدري.

استنسخ الكود مباشرةً من مستودع git:

```bash
git clone https://github.com/pbek/QOwnNotes.git -b release
cd QOwnNotes
git submodule update --init
```

Then download [Qt Creator](https://www.qt.io/download-open-source), you will also need the packages `qtwebsockets` and `qt5compat` to build QOwnNotes with Qt6 (only `qtwebsockets` for Qt5). If you build under Windows, you want to stick to *MinGw 64-bit*.

Afterwards open the project file `src/QOwnNotes.pro` and click on **Build** / **Build Project QOwnNotes**.

أو يمكنك بناؤه مباشرةً من الطرفية:

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

يمكنك العثور على الملفات المضغوطة لكود QOwnNotes المصدري في [أرشيف مصادر QOwnNotes](https://download.tuxfamily.org/qownnotes/src/).

يمكن الوصول إلى الملفات المضغوطة للكود المصدري برابط مثل هذا:

`https://download.tuxfamily.org/qownnotes/src/qownnotes-20.10.1.tar.xz`
