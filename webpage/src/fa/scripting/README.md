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
برای شروع سریع [اسکریپت های نمونه](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples) را ملاحظه فرمایید.
:::

در **تنظیمات اسکریپت نویسی** می توانید اسکریپت ها را مستقیماً از [**مخزن اسکریپت**](https://github.com/qownnotes/scripts) نیز نصب کنید.

در مورد مسائل، سؤالات یا درخواست های مهم مربوط به اسکریپت ها از **مخزن اسکریپت** لطفاً یک مسئله را در [صفحه مسائل مخزن اسکریپت QOwnNotes](https://github.com/qownnotes/scripts/issues) باز کنید.

::: tip
در صورت تمایل جهت پیشنهاد یک اسکریپت برای **مخزن اسکریپت**، لطفاً دستورالعمل های مندرج در [مخزن اسکریپت QOwnNotes](https://github.com/qownnotes/scripts) را دنبال کنید.
:::

در صورت نیاز به دسترسی برای عملکرد معینی در QOwnNotes یا داشتن سؤال و ایده ای، لطفاً یک مسئله را در [صفحه مسائل QOwnNotes](https://github.com/pbek/QOwnNotes/issues) باز کنید.

::: tip
برای ورود می توانید از فرمان `script.log()` استفاده کنید تا وارد ابزارک ورود شوید.
:::