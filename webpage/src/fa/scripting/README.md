# اسکریپت نویسی در QOwnNotes

اسکریپت QOwnNotes اغلب در [پرونده های Qt QML](https://doc.qt.io/qt-5/qtqml-index.html) به صورت **JavaScript** است.

```js
import QtQml 2.0
import QOwnNotesTypes 1.0

Script {
    /**
     * Will be run when the scripting engine initializes
     */
    function init() {
        script.log("Hello world!");
    }
}
```

شما می توانید این پرونده های QML را در هر جایی که دوست دارید قرار داده و آنها را با افزودن به **تنظیمات اسکریپت نویسی** در **QOwnNotes** اضافه کنید (دکمه `افزودن اسکریپت`/`افزودن اسکریپت محلی` در دسترس قرار دارد).

::: tip
Take a look at the [example scripts](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples) to get started fast.
:::

In the **Scripting settings** you can also install scripts directly from the [**Script repository**](https://github.com/qownnotes/scripts). یک برنامه تحت وب با میزبانی انجمن برای جستجوی اسکریپت ها در [beurt.github.io/QOwnNoteScriptsList](https://beurt.github.io/QOwnNoteScriptsList/) موجود است.

در مورد مسائل، سؤالات یا درخواست های مهم مربوط به اسکریپت ها از **مخزن اسکریپت** لطفاً یک مسئله را در [صفحه مسائل مخزن اسکریپت QOwnNotes](https://github.com/qownnotes/scripts/issues) باز کنید.

::: tip
در صورت تمایل جهت پیشنهاد یک اسکریپت برای **مخزن اسکریپت**، لطفاً دستورالعمل های مندرج در [مخزن اسکریپت QOwnNotes](https://github.com/qownnotes/scripts) را دنبال کنید.
:::

در صورت نیاز به دسترسی برای عملکرد معینی در QOwnNotes یا داشتن سؤال و ایده ای، لطفاً یک مسئله را در [صفحه مسائل QOwnNotes](https://github.com/pbek/QOwnNotes/issues) باز کنید.

::: tip
For logging, you can use the `script.log()` command to log to the _Log panel_, which you can turn on in the _Window / Panels_ menu.
:::
