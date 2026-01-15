# Markdown Cheatsheet

المراد من هذه الورقة أن تكون مرجعا سريعا وأن تُظهر صياغة ماركداون في QOwnNotes.

## Headings

استخدم العنوان لتنظيم نصوصك.

```markdown
# H1

## H2

### H3

#### H4

##### H5

###### H6
```

::: tip
**لوحة التنقل** تُظهر هيكل عناوينك.
:::

كبديل، للعناوين من المستويين الأول والثاني، يمكن تسطيرهم هكذا:

```markdown
Alt-H1
======

Alt-H2
------
```

::: tip
بشكل افتراضي QOwnNotes ينشئ **اسم ملف الملاحظة** من **عنوان المستوى الأول** (h1).
:::

## التوكيد

```markdown
Emphasis, aka italics, with *asterisks* or _underscores_.

Strong emphasis, aka bold, with **asterisks** or __underscores__.

Combined emphasis with **asterisks and _underscores_**.
```

::: tip
يمكنك استخدام [الاختصار](./shortcuts.md) <kbd>Ctrl + B</kbd> لجعل النص عريضًا وأيضا <kbd>Ctrl + I</kbd> لجعله مائلًا.
:::

## التسطير

There also is an optional setting to enable underline rendering in the _Preview settings_.

```markdown
_مسطر_
```

::: tip
يمكنك استخدام [الاختصار](./shortcuts.md) <kbd>Ctrl + U</kbd> لتسطير نص.
:::

## الشطب

```markdown
~~مشطوب~~
```

::: tip
يمكنك استخدام [الاختصار](./shortcuts.md) <kbd>Alt + Shift + S</kbd> لشطب نص.
:::

## القوائم

توجد طرق عديدة لإنشاء قوائم.

```markdown
1. العنصر الأول في قائمة مرقمة
2. Another item

- Unordered sub-list.

1. Actual numbers don't matter, just that it's a number
1. And another item.

* Unordered list can use asterisks

- Or minuses

+ Or pluses
```

::: tip
إذا ضغطت <kbd>Enter</kbd> في نهاية قائمة، سيتم إنشاء عنصر قائمة جديد.
:::

## الروابط

أكثر استخدام الروابط للإشارة إلى صفحات ويب أو إلى ملاحظات أخرى. وتُعرَّف الروابط بأكثر من طريقة.

### الروابط الخارجية

```markdown
[أنا رابط مُعرَّف في مكاني](https://www.google.com)

[أنا رابط مُعرَّف في مكاني ولي عنوان](https://www.google.com "الصفحة الرئيسية لـGoogle")

[يمكنك استخدام الأرقام لتعريف الروابط بأسلوب المراجع][1]

الروابط المجردة والروابط المحاطة بأقواس زاويّة ستُحوّل آليًا إلى روابط في المعاينة.
http://www.example.com أو <http://www.example.com>

[1]: https://www.qownnotes.org
```

### الروابط الداخلية

You can link to other notes or attachments like this:

```markdown
[هذا رابط لملف ملاحظة اسمه Journal.md](Journal.md)

<Journal.md> يعمل أيضا.
```

::: tip
يمكنك استخدام [الاختصار](./shortcuts.md) <kbd>Ctrl + L</kbd> **لإنشاء روابط إلى صفحات ويب أو إلى ملاحظات أخرى**.

اضغط <kbd>Ctrl + Shift + X</kbd> لإظهار حوار يساعدك في **إدراج مرفق** في الملاحظة.

يمكنك ضغط <kbd>Ctrl + Space</kbd> والمؤشر على رابط في محرر الملاحظات لفتحه.
:::

::: warning
Info Internal links with a title are not supported:

```markdown
[I link to the Journal.md note](Journal.md "Title not supported")
```

:::

### الإشارات المرجعية

الإشارات المرجعية المستخدمة في [إضافة المتصفح رفيقة ويب QOwnNotes](./browser-extension.md) تستخدم روابط في قوائم.

```markdown
- [اسم صفحة الوب](https://www.example.com) #وسم١ #وسم٢ شيء من الوصف والوسوم
```

## الصور

يمكن تضمين الصور في QOwnNotes. وستظهر في المعاينة.

```markdown
![نص بديل](media/my-image.jpg)
```

::: tip
استخدم [الاختصار](./shortcuts.md) <kbd>Ctrl + Shift + I</kbd> لإدراج صورة في الملاحظة. يمكن أن تكون الصورة أيضا في الحافظة؛ سيكتشفها الحوار ويظهر معاينة لها.

يمكنك أيضا لصق الصورة من الحافظة مباشرةً في الملاحظة بالاختصار <kbd>Ctrl + Shift + V</kbd>.
:::

::: warning
Info Image links with a title are not supported:

```markdown
![alt text](media/my-image.jpg "Title not supported")
```

:::

## الأكواد المضمّنة وكتل الأكواد

```markdown
`الأكواد` المضمّنة في السطر حولها `فاصلة عُليا مائلة` (زر `ذ` في تخطيط لوحة المفاتيح الإنجليزي الشائع، أو `Shift + ع` في التخطيط العربي الشائع).
```

::: tip
يمكنك استخدام [الاختصار](./shortcuts.md) <kbd>Ctrl + Shift + C</kbd> على نص محدد، أو مجرد داخل نص، لجعله كود مضمّن.
:::

كتل الأكواد إما أن تكون مُسوّرة بسطرين كلٍ منهما به ثلاث فاصلات عُليا مائلة، وإما أن تكون سطورها مبدوءة بأربع مسافات.

### سور المسافات الأربعة

أضف أربع مسافات في بداية كل سطر من سطور كودك لتنسيقه ككتلة كود.

```markdown
    <div dir="ltr">s = "كود بمسافات بادئة"
    print s</div>
```

### سور الفاصلات العليا المائلة

يمكنك أيضا استخدام ثلاث فاصلات عليا مائلة لإنشاء كتلة كود.
````markdown
```
الكود يأتي هنا
الكود يأتي هنا
```
````

::: tip
يمكنك استخدام [الاختصار](./shortcuts.md) <kbd>Ctrl + Shift + C</kbd> على
عدد من السطور المحددة أو في سطر فارغ لإنشاء كتلة كود.
:::

### سور الفاصلات مع تلوين الأكواد

يوجد كذلك شيء من التلوين النحوي لكتل الأكواد في QOwnNotes.

````markdown
```bash
# I am a comment
cd Notes
````
````

Currently, supported languages (and code block identifiers) are:

- BASh scripting, `bash`
- C, `c`
- C++, `cpp`
- C++, `cxx`
- C++, `c++`
- C#, `c#`
- CMake, `cmake`
- C#, `csharp`
- CSS, `css`
- Go, `go`
- HTML, `html`
- INI, `ini`
- Java, `java`
- JavaScript, `javascript`
- JavaScript, `js`
- JSON, `json`
- Makefile, `make`
- PHP, `php`
- Python, `py`
- Python, `python`
- QML, `qml`
- Rust, `rust`
- Shell scripting, `sh`
- SQL, `sql`
- TypeScript, `ts`
- TypeScript, `typescript`
- V, `v`
- Vex, `vex`
- XML, `xml`
- YAML, `yml`
- YAML, `yaml`

## Tables

Tables aren't part of the core Markdown spec, but the QOwnNotes preview supports them.

```markdown
Colons can be used to align columns.

| Tables        |      Are      |  Cool |
| ------------- | :-----------: | ----: |
| col 3 is      | right-aligned | $1600 |
| col 2 is      |   centered    |   $12 |
| zebra stripes |   are neat    |    $1 |

There must be at least 3 dashes separating each header cell.

يمكنك أيضا استخدام ماركداون داخل خلايا الجداول.

| Markdown | Less      | Pretty     |
| -------- | --------- | ---------- |
| _Still_  | `renders` | **nicely** |
| 1        | 2         | 3          |
````

::: tip
اضغط <kbd>Alt + Shift + T</kbd> لإظهار حوار يساعدك في إنشاء الجداول. يمكنك حتى استيراد ملفات CSV في ذلك الحوار.

استخدم <kbd>Ctrl + Space</kbd> داخل جدول ماركداون لتنسيقه آليًا.
:::

## كتل الاقتباس

```markdown
> كتل الاقتباس مفيدة جدا في رسائل البريد الإلكتروني لمحاكاة الرد على نص.
> هذا السطر جزء من نفس الاقتباس.

كسر الاقتباس.

> هذا سطر طويل جدا ولكنه سيُنسّق بشكل سليم كاقتباس عندما يلف إلى السطر التالي. ياه يا فتى، لنستمر في الكتابة حتى نضمن أن هذا بالطول الكافي لكي يلف فعلا عند الجميع. Oh, you can _put_ **Markdown** into a blockquote.
```

::: tip
You can tell QOwnNotes to fully highlight blockquotes or just the blockquote character in the _Editor settings_

يمكنك استخدام [الاختصار](./shortcuts.md) <kbd>Ctrl + Shift + B</kbd> لجعل النص المحدد كتلة اقتباس.
:::

## مسطرة أفقية

توجد ثلاث طرق للحصول على مسطرة أفقية: الشَرطات، والشرطات السفلية، والنجوم.

```markdown
ثلاث أو أكثر...

Hyphens

---

Asterisks

***

Underscores

___
```

## فصل السطور

- يمكنك فصل فقرة إلى أكثر من سطر واحد لتسهيل التحرير، ستظل هذه السطور تُعرض في المعاينة كفقرة واحدة بلا فواصل.
- يمكن فرض سطر جديد داخل فقرة بإنهاء السطر بمسافتين.
- يمكنك إنشاء فقرة مستقلة بفصلها عما قبلها وبعدها بسطر فارغ.

::: tip
يمكنك إضافة مسافتين وسطر جديد بالاختصار <kbd>⇧ Shift</kbd> + <kbd>Enter</kbd>.
:::

```markdown
إليك سطر لنبدأ به.

This line is separated from the one above by two newlines, so it will be a _separate paragraph_.

This line also begins a separate paragraph, but...  
This line is only separated by two trailing spaces and a single newline, so it's a separate line in the _same paragraph_.
```

::: tip
يظلّل المحرر بشكل افتراضي المسافات التي في آخر السطر.
:::

## التعليقات

لا تظهر التعليقات في المعاينة.

```markdown
[comment]: # "This comment will not appear in the preview"

<!-- HTML comments are also hidden -->
```

::: tip
سيتم كذلك تجاهل أي تعليق HTML في بداية الملاحظة عند توليد اسم ملف الملاحظة آليًا.
:::

## قوائم مربعات الاختيار

يمكنك إنشاء قوائم مهام بسيطة باستخدام قوائم مربعات الاختيار.

```markdown
- [x] تم
- [ ] افعل
```

::: tip
يمكنك اختيار أو تفريغ مربعات الاختيار في المعاينة.
:::

## التمهيد (Frontmatter)

في QOwnNotes، يمكنك استخدام تمهيد (مثلا YAML) لإضافة بعض المعلومات الوصفية الإضافية. هذه المعلومات **لن تظهر في المعاينة** وكذلك **لن تعيق التوليد الآلي لاسم ملف الملاحظة**.

```markdown
---
title: اسمٌ ما
description: شيءٌ من الوصف
---

# العنوان الرئيسي للملاحظة يبدأ هنا

شيءٌ من النص
```

اسم ملف الملاحظة في هذا المثال سيكون `العنوان الرئيسي للملاحظة يبدأ هنا.md`.
