---
image: /img/bookmarks.png
---

# إضافة المتصفح رفيقة ويب QOwnNotes

تتيح لك قص المحتوى من صفحات المتصفح وكذلك إدارة إشارات المتصفح المرجعية عبر المتصفحات وأنظمة التشغيل المختلفة.

::: tip

- يجب أن يكون QOwnNotes مفتوحا حتى تعمل إضافة المتصفح رفيقة الويب.
- لا حاجة للاتصال بالإنترنت. إضافة المتصفح تعمل **بدون اتصال**.
:::

## التثبيت

1. الحصول على الإضافة
   - [‏سوق Chrome الإلكتروني](https://chrome.google.com/webstore/detail/qownnotes-web-companion/pkgkfnampapjbopomdpnkckbjdnpkbkp)
   - [صفحة إضافات فيرفكس](https://addons.mozilla.org/firefox/addon/qownnotes-web-companion)
   - يمكنك أيضا أن تجد الإضافة على [GitHub](https://github.com/qownnotes/web-companion/).
2. إضافة رمز الأمان لإعداد الإضافة.
   - المرة الأولى التي ستنقر فيها على أيقونة إضافة المتصفح QOwnNotes، سيظهر لك صندوق حواري فيه رمز أمان. انسخ الرمز.
   - اذهب إلى مكان إدارة الإضافات في متصفحك. انقر على تفاصيل إضافة QOwnNotes.
   - ألصق الرمز إلى خانة رمز الأمان.
3. لاستعمال الوضع الخصوصي، عليك السماح للإضافة بالعمل في النوافذ الخصوصية، في إعدادات الإضافة.

## مقص الويب

![مقص الويب](/img/web-clipper.png)

انقر بزر الفأرة الأيمن على صفحة ويب أو حدد نصًّا حتى تستخدم وظيفة **مقص الويب**. يمكنك كذلك إنشاء ملاحظة جديدة **بلقطة شاشة** من صفحة الويب الحالية.

::: warning
Info إن لم يعمل مقص الويب، فتأكد أن تطبيق QOwnNotes الحاسوبي مفتوح، وأن رمز الأمان مضبوط في إضافة المتصفح الرفيقة.

Also, make sure you accept the consent dialog when you first click the extension icon in the browser toolbar.

If you are using Firefox, you also need to make sure that you have turned on the optional permission `Access your data for all websites` in the _Permission_ extension settings, otherwise you might get an error `TypeError: chrome.tabs.captureVisibleTab is not a function`.
:::

::: tip
مقص الويب قابل للبرمجة أيضا! ألقِ نظرة على [websocketRawDataHook](../scripting/hooks.md#websocketrawdatahook) إذا أردت التحكم في ما تقصه من صفحات الويب.
:::

## الإشارات المرجعية

![الإشارات المرجعية](/img/bookmarks.png)

إذا أردت تحكمًا كاملًا في إشارات المتصفح المرجعية الخاصة بك وتريد **استخدامها عبر متصفحات وأنظمة تشغيل مختلفة** فإن إضافة متصفح QOwnNotes من أجلك.

ستُظهر إضافة المتصفح مبدئيا جميع **روابط الملاحظة الحالية** في نافذة منبثقة عندما تضغط على أيقونة QOwnNotes في متصفحك. سيكون لهذه الروابط الوسم `current`.

يمكنك أيضا **إدارة إشاراتك المرجعية في ملاحظات** ذات الوسم `bookmarks` (والذي يمكن تغييره في الإعدادات). يمكن أن تعطى لهذه الروابط وسومًا ووصفًا ليظهروا في إضافة المتصفح.

سيتم تخزين الإشارات المرجعية الجديدة في ملاحظة اسمها `Bookmarks` (والذي يمكن تغييره أيضا في الإعدادات).

::: tip
يمكنك استيراد إشاراتك المرجعية من المتصفح إلى QOwnNotes باستخدام إضافة المتصفح رفيقة الويب!
:::

### صيغ روابط الإشارات المرجعية

```markdown
- [Webpage name](https://www.example.com)
- [Webpage name](https://www.example.com) #tag1 #tag2
- [Webpage name](https://www.example.com) some description only
- [Webpage name](https://www.example.com) #tag1 #tag2 some description and tags

* [Webpage name](https://www.example.com) the alternative list character also works
```

يمكنك البحث في إضافة المتصفح بالاسم أو وسوم الرابط أو الوصف. تُحلَّل كل الروابط الأخرى في الملاحظة الحالية، لكن بغير وسوم أو وصف.
