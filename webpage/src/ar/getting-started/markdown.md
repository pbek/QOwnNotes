# ماركداون (Markdown)

المراد من هذه الورقة أن تكون مرجعا سريعا وأن تُظهر صياغة ماركداون في QOwnNotes.

## العناوين

استخدم العنوان لتنظيم نصوصك.

```markdown
# عنوان مستوى أول
## عنوان مستوى ثان
### عنوان مستوى ثالث
#### عنوان مستوى رابع
##### عنوان مستوى خامس
###### عنوان مستوى سادس
```

::: tip
**لوحة التنقل** تُظهر هيكل عناوينك.
:::

كبديل، للعناوين من المستويين الأول والثاني، يمكن تسطيرهم هكذا:

```markdown
شكل بديل لعناوين المستوى الأول
==================================

شكل بديل لعناوين المستوى الثاني
------------------------------------
```

::: tip
بشكل افتراضي QOwnNotes ينشئ **اسم ملف الملاحظة** من **عنوان المستوى الأول** (h1).
:::

## التوكيد

```markdown
التوكيد البسيط، أو الخط المائل، يكون *بنجمة واحدة على كل جانب*.

التوكيد القوي، أو الخط العريض، يكون **بنجمتين اثنتين على كل جانب**.
```

::: tip
يمكنك استخدام [الاختصار](./shortcuts.md) <kbd>Ctrl + B</kbd> لجعل النص عريضًا وأيضا <kbd>Ctrl + I</kbd> لجعله مائلًا.
:::

## التسطير

يوجد كذلك إعداد اختياري لتفعيل عرض التسطير في *إعدادات المعاينة*.

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
2. عنصر آخر
  * قائمة منقوطة فرعية.
1. الرقم الفعلي غير مهم؛ المهم أنه رقم
  1. قائمة مرقمة فرعية (تعمل فقط في المحرر، ولا تعمل في المعاينة)
4. وعنصر آخر.

* القوائم المنقوطة قد تستخدم النجوم
- أو الشرطات
+ أو علامات الجمع
```

::: tip
إذا ضغطت <kbd>Enter</kbd> في نهاية قائمة، سيتم إنشاء عنصر قائمة جديد.
:::

## الروابط

Two of the simpler uses links can be put to is in pointing to webpages and other notes. There are multiple ways each of these can be done.

### External links

```markdown
[I'm an inline-style link](https://www.google.com)

[I'm an inline-style link with title](https://www.google.com "Google's Homepage")

[You can use numbers for reference-style link definitions][1]

Plain URLs and URLs in angle brackets will automatically get turned into links in the preview. 
http://www.example.com أو <http://www.example.com>

[1]: https://www.qownnotes.org
```

### Internal links

```markdown
[I link to the Journal.md note](Journal.md)

<Journal.md> works similarly.
```

::: tip
يمكنك استخدام [الاختصار](./shortcuts.md) <kbd>Ctrl + L</kbd> **لإنشاء روابط إلى صفحات ويب أو إلى ملاحظات أخرى**.

اضغط <kbd>Ctrl + Shift + X</kbd> لإظهار حوار يساعدك في **إدراج مرفق** في الملاحظة.

يمكنك ضغط <kbd>Ctrl + Space</kbd> والمؤشر على رابط في محرر الملاحظات لفتحه.
:::

### Bookmarks

الإشارات المرجعية المستخدمة في [إضافة المتصفح رفيقة ويب QOwnNotes](./browser-extension.md) تستخدم روابط في قوائم.

```markdown
- [Webpage name](https://www.example.com) #tag1 #tag2 some description and tags
```

## الصور

يمكن تضمين الصور في QOwnNotes. وستظهر في المعاينة.

```markdown
![alt text](media/my-image.jpg)
```

::: tip
استخدم [الاختصار](./shortcuts.md) <kbd>Ctrl + Shift + I</kbd> لإدراج صورة في الملاحظة. يمكن أن تكون الصورة أيضا في الحافظة؛ سيكتشفها الحوار ويظهر معاينة لها.

يمكنك أيضا لصق الصورة من الحافظة مباشرةً في الملاحظة بالاختصار <kbd>Ctrl + Shift + V</kbd>.
:::


## الأكواد المضمّنة وكتل الأكواد

```markdown
Inline `code` has `back-ticks around` it.
```

::: tip
يمكنك استخدام [الاختصار](./shortcuts.md) <kbd>Ctrl + Shift + C</kbd> على نص محدد، أو مجرد داخل نص، لجعله كود مضمّن.
:::

كتل الأكواد إما أن تكون مُسوّرة بسطرين كلٍ منهما به ثلاث فاصلات عُليا مائلة، وإما أن تكون سطورها مبدوءة بأربع مسافات.

### 4-Spaces fence

أضف أربع مسافات في بداية كل سطر من سطور كودك لتنسيقه ككتلة كود.

```markdown
    s = "Code with space indent"
    print s
```

### Backtick fence

يمكنك أيضا استخدام ثلاث فاصلات عليا مائلة لإنشاء كتلة كود.
~~~markdown
```
Code goes here
Code goes here
```
~~~

::: tip
You can use the [shortcut](./shortcuts.md) <kbd>Ctrl + Shift + C</kbd> on
multiple selected lines of text or in an empty line to create a code block. 
:::

### Backtick fence with code highlighting

There also is some syntax highlighting with code blocks in QOwnNotes.

~~~markdown
```bash
# I am a comment
cd Notes
```
~~~

Currently, supported languages (and code block identifiers) are:

* BASh scripting, `bash`
* C, `c`
* C++, `cpp`
* C++, `cxx`
* C++, `c++`
* C#, `c#`
* CMake, `cmake`
* C#, `csharp`
* CSS, `css`
* Go, `go`
* HTML, `html`
* INI, `ini`
* Java, `java`
* JavaScript, `javascript`
* JavaScript, `js`
* JSON, `json`
* Makefile, `make`
* PHP, `php`
* Python, `py`
* Python, `python`
* QML, `qml`
* Rust, `rust`
* Shell scripting, `sh`
* SQL, `sql`
* TypeScript, `ts`
* TypeScript, `typescript`
* V, `v`
* Vex, `vex`
* XML, `xml`
* YAML, `yml`
* YAML, `yaml`

## Tables

Tables aren't part of the core Markdown spec, but the QOwnNotes preview supports them. 

~~~markdown
Colons can be used to align columns.

| Tables        | Are           | Cool  |
| ------------- |:-------------:| -----:|
| col 3 is      | right-aligned | $1600 |
| col 2 is      | centered      |   $12 |
| zebra stripes | are neat      |    $1 |

There must be at least 3 dashes separating each header cell.

You can also use inline Markdown.

| Markdown | Less | Pretty |
| --- | --- | --- |
| *Still* | `renders` | **nicely** |
| 1 | 2 | 3 |
~~~

::: tip
اضغط <kbd>Alt + Shift + T</kbd> لإظهار حوار يساعدك في إنشاء الجداول. يمكنك حتى استيراد ملفات CSV في ذلك الحوار.

استخدم <kbd>Ctrl + Space</kbd> داخل جدول ماركداون لتنسيقه آليًا.
:::

## كتل الاقتباس

```markdown
> Blockquotes are very handy in email to emulate reply text.
> This line is part of the same quote.

Quote break.

> This is a very long line that will still be quoted properly when it wraps. Oh boy let's keep writing to make sure this is long enough to actually wrap for everyone. Oh, you can *put* **Markdown** into a blockquote. 
```

::: tip
يمكنك إخبار QOwnNotes أن يظلّل كتل الاقتباس بكاملها أو أن يظلّل حرف الاقتباس فقط، في *إعدادات المحرر*

يمكنك استخدام [الاختصار](./shortcuts.md) <kbd>Ctrl + Shift + B</kbd> لجعل النص المحدد كتلة اقتباس.
:::

## مسطرة أفقية

توجد ثلاث طرق للحصول على مسطرة أفقية: الشَرطات، والشرطات السفلية، والنجوم.

```markdown
Three or more...

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
Here's a line for us to start with.

This line is separated from the one above by two newlines, so it will be a *separate paragraph*.

This line is also begins a separate paragraph, but...  
This line is only separated by two trailing spaces and a single newline, so it's a separate line in the *same paragraph*.
```

::: tip
يظلّل المحرر بشكل افتراضي المسافات التي في آخر السطر.
:::

## التعليقات

لا تظهر التعليقات في المعاينة.

```markdown
[comment]: # (This comment will not appear in the preview)

<!-- HTML comments are also hidden -->
```

::: tip
سيتم كذلك تجاهل أي تعليق HTML في بداية الملاحظة عند توليد اسم ملف الملاحظة آليًا.
:::

## قوائم مربعات الاختيار

يمكنك إنشاء قوائم مهام بسيطة باستخدام قوائم مربعات الاختيار.

```markdown
- [x] done
- [ ] todo
```

::: tip
يمكنك اختيار أو تفريغ مربعات الاختيار في المعاينة.
:::

## التمهيد (Frontmatter)

في QOwnNotes، يمكنك استخدام تمهيد (مثلا YAML) لإضافة بعض المعلومات الوصفية الإضافية. هذه المعلومات **لن تظهر في المعاينة** وكذلك **لن تعيق التوليد الآلي لاسم ملف الملاحظة**.

```markdown
---
title: Some name
description: Some description
---

# Note headline starts here

Some text
```

اسم ملف الملاحظة في هذا المثال سيكون `العنوان الرئيسي للملاحظة يبدأ هنا.md`.
