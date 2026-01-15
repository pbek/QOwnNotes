# Markdown Cheatsheet

برگه تقلب مارک داون به صورت یک مرجع سریع و ویترینی از چیدمان مارک داون در QOwnNotes در نظر گرفته شده است.

## Headings

برای سر و شکل دادن به متن خود از سرفصل ها استفاده کنید.

```markdown
# H1

## H2

### H3

#### H4

##### H5

###### H6
```

::: tip
**پنل جهت یابی** ساختار سرفصل های شما را نشان می دهد.
:::

از سوی دیگر، برای H1 و H2، یک سبک زیرخط دار:

```markdown
Alt-H1
======

Alt-H2
------
```

::: tip
به صورت پیش فرض QOwnNotes ** نام پرونده یادداشت** را از **سرفصل 1** ایجاد می کند (h1).
:::

## تکیه

```markdown
Emphasis, aka italics, with *asterisks* or _underscores_.

Strong emphasis, aka bold, with **asterisks** or __underscores__.

Combined emphasis with **asterisks and _underscores_**.
```

::: tip
می‌توانید از [میانبر](./shortcuts.md) <kbd>Ctrl + B</kbd> برای پررنگ کردن متن و از <kbd>Ctrl + I</kbd> برای ایتالیک کردن آن استفاده کنید.
:::

## خط زیرین

There also is an optional setting to enable underline rendering in the _Preview settings_.

```markdown
_خط زیرین_
```

::: tip
می توانید از [میانبر](./shortcuts.md) <kbd>Ctrl + U</kbd> برای زیر خط دار کردن یک متن استفاده کنید.
:::

## ابطال

```markdown
~~ابطال~~
```

::: tip
می توانید از [میانبر](./shortcuts.md) <kbd>Alt + Shift + S</kbd> برای ابطال یک متن استفاده کنید.
:::

## لیست ها

روش های زیادی برای ایجاد لیست وجود دارد.

```markdown
1. نخستین مورد لیست مرتب شده
2. Another item

- Unordered sub-list.

1. Actual numbers don't matter, just that it's a number
1. And another item.

* Unordered list can use asterisks

- Or minuses

+ Or pluses
```

::: tip
با فشار دادن دکمه <kbd>ورود</kbd> در انتهای هر لیست، یک مورد لیست جدید ایجاد می شود.
:::

## پیوندها

Two of the simpler uses for links are pointing to webpages or other notes. There are multiple ways each of these links can look.

### External links

```markdown
[I'm an inline-style link](https://www.google.com)

[I'm an inline-style link with title](https://www.google.com "Google's Homepage")

[You can use numbers for reference-style link definitions][1]

Plain URLs and URLs in angle brackets will automatically get turned into links in the preview.
http://www.example.com or <http://www.example.com>

[1]: https://www.qownnotes.org
```

### Internal links

You can link to other notes or attachments like this:

```markdown
[I link to the Journal.md note](Journal.md)

<Journal.md> works similarly.
```

::: tip
می توانید از [میانبر](./shortcuts.md) <kbd>Ctrl + L</kbd> برای **ایجاد پیوند به صفحات وب یا سایر یادداشت ها** استفاده کنید.

استفاده از <kbd>Ctrl + Shift + X</kbd> یک کادر گفتگو را ظاهر می کند که به شما کمک می کند تا ** پیوست ها** را در یادداشت خود درج کنید.

می توانید با فشار دادن <kbd>Ctrl + Space</kbd> و در حالی که مکان نما را بر روی پیوند در ویرایش یادداشت نگه داشته اید، پیوند را دنبال کنید.
:::

::: warning
Info Internal links with a title are not supported:

```markdown
[I link to the Journal.md note](Journal.md "Title not supported")
```

:::

### Bookmarks

نشانک های بکار رفته توسط [ افزونه مرورگر همراه وب QOwnNotes ](./browser-extension.md) از پیوندهای موجود در لیست ها استفاده می کنند.

```markdown
- [Webpage name](https://www.example.com) #tag1 #tag2 some description and tags
```

## تصاویر

امکان تعبیه تصاویر در QOwnNotes وجود دارد. آنها در پیش نمایش نشان داده می شوند.

```markdown
![alt text](media/my-image.jpg)
```

::: tip
می توانید از [میانبر](./shortcuts.md) <kbd>Ctrl + Shift + I</kbd> برای درج تصویر در یادداشت استفاده کنید. همچنین تصاویر می‌توانند در کلیپ بورد قرار بگیرند؛ به طوری که کادر گفتگو آن را شناسایی کرده و یک پیش نمایش نشان می دهد.

علاوه بر این، می‌توانید با <kbd>Ctrl + Shift + V</kbd> یک تصویر را مستقیماً از کلیپ بورد به یادداشت خود بچسبانید.
:::

::: warning
Info Image links with a title are not supported:

```markdown
![alt text](media/my-image.jpg "Title not supported")
```

:::

## کد توکار و کد بلاک ها

```markdown
Inline `code` has `backticks around` it.
```

::: tip
می توانید برای ایجاد یک کد بلاک توکار از [میانبر](./shortcuts.md) <kbd>Ctrl + Shift + C</kbd> در متن توکار منتخب یا درست در داخل متن استفاده کنید.
:::

Blocks of code are either fenced by lines with three backticks, or are indented with four spaces.

### 4-Spaces fence

چهار فاصله در جلوی کدتان برای نشان دار کردن آن به عنوان کد بلاک اضافه کنید.

```markdown
    s = "Code with space indent"
    print s
```

### Backtick fence

علاوه بر این می توانید از سه بک تیک برای ایجاد یک کد پلاک استفاده کنید.
````markdown
```
Code goes here
Code goes here
```
````

::: tip
You can use the [shortcut](./shortcuts.md) <kbd>Ctrl + Shift + C</kbd> on
multiple selected lines of text or in an empty line to create a code block.
:::

### Backtick fence with code highlighting

There also is some syntax highlighting with code blocks in QOwnNotes.

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

You can also use inline Markdown.

| Markdown | Less      | Pretty     |
| -------- | --------- | ---------- |
| _Still_  | `renders` | **nicely** |
| 1        | 2         | 3          |
````

::: tip
<kbd>Alt + Shift + T</kbd> را فشار دهید تا یک کادر گفتگو فعال شود که می تواند به شما در ایجاد جداول کمک کند. حتی می توانید پرونده های CSV را در آن کادر گفتگو وارد کنید.

به منظور قالب بندی خودکار یک جدول مارک داون از <kbd>Ctrl + Space</kbd> در داخل آن استفاده کنید.
:::

## نقل قول های بلند

```markdown
> Blockquotes are very handy in email to emulate reply text.
> This line is part of the same quote.

Quote break.

> This is a very long line that will still be quoted properly when it wraps. Oh boy let's keep writing to make sure this is long enough to actually wrap for everyone. Oh, you can _put_ **Markdown** into a blockquote.
```

::: tip
You can tell QOwnNotes to fully highlight blockquotes or just the blockquote character in the _Editor settings_

می توانید از [میانبر](./shortcuts.md) <kbd>Ctrl + Shift + B</kbd> برای علامت گذاری متن به صورت نقل قول بلند استفاده کنید.
:::

## خط افقی جدا کننده

سه راه برای رسم یک خط افقی جدا کننده وجود دارد: خط ربط، ستاره یا زیرین خط.

```markdown
Three or more...

Hyphens

---

Asterisks

***

Underscores

___
```

## شکست خط

- برای ویرایش راحت‌تر می‌توانید پاراگراف را به بیش از یک خط منفرد تقسیم کنید؛ البته همچنان به عنوان یک پاراگراف منفرد و بدون تقسیمات ارائه می شود.
- از طریق خاتمه یک خط با دو فاصله می توانید درون پاراگراف خط شکنی انجام دهید.
- شما می‌توانید پاراگراف جداگانه‌ای را از طریق خطوط خالی با تحدید حدود ایجاد کنید.

::: tip
می توانید با استفاده از <kbd>⇧ Shift</kbd> + <kbd>بازگشت</kbd> دو فاصله و یک خط نو وارد کنید.
:::

```markdown
Here's a line for us to start with.

This line is separated from the one above by two newlines, so it will be a _separate paragraph_.

This line also begins a separate paragraph, but...  
This line is only separated by two trailing spaces and a single newline, so it's a separate line in the _same paragraph_.
```

::: tip
فاصله های مؤخر به صورت پیش فرض در ویرایشگر برجسته هستند.
:::

## توضیحات

توضیحات در پیش نمایش نشان داده نمی شود.

```markdown
[comment]: # "This comment will not appear in the preview"

<!-- HTML comments are also hidden -->
```

::: tip
هنگام ایجاد خودکار نام پرونده یادداشت، بلوک توضیح html اصلی در یادداشت هم نادیده گرفته می شود.
:::

## لیست های صندوق بررسی

شما می توانید لیست های ساده انجام دادنی را با لیست های صندوق بررسی ایجاد کنید.

```markdown
- [x] done
- [ ] todo
```

::: tip
شما قادر خواهید بود صندوق های بررسی را در پیش نمایش علامت بزنید یا علامت آنها را بردارید.
:::

## پیش‌ گفتار

در QOwnNotes می توانید از یک پیش گفتار (مانند YAML) برای افزودن مقداری ابرداده اضافی استفاده کنید. **در پیش نمایش نشان داده نمی شود** و **برای تشکیل خودکار نام پرونده یادداشت اختلال ایجاد نمی کند**.

```markdown
---
title: Some name
description: Some description
---

# Note headline starts here

Some text
```

نام پرونده این یادداشت نمونه `Note headline starts here.md`خواهد بود.
