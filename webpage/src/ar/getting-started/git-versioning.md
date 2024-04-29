# إدارة الإصدارات عبر git

يمكن ضبط QOwnNotes ليتابع التعديلات التي تتم في مجلد ملاحظاتك ويودعها آليًّا في مستودع git في مجلد ملاحظاتك.

يمكن ضبطه من إعدادات «إدارة الإصدارات عبر git» في شاشة الإعدادات «عام».

![git-note-folder-settings](/img/git/settings.png)

تحتاج إلى وجود git مثبتًّا على نظامك، وأن يكون في المسارات التنفيذية (PATH) وإلا فحدد مسار ملفه التنفيذي في إعدادات «إدارة الإصدارات عبر git».

تحتاج كذلك إلى تفعيل دعم git في كل مجلد ملاحظات تريد إداراته عبر git!

![git-note-folder-settings](/img/git/note-folder-settings.png)

::: tip
Don't forget to set your git username and email address in the terminal:

```bash
git config --global user.email "your-email@domain.com"
git config --global user.name "Your name"
```

:::
