# إدارة الإصدارات عبر git

يمكن ضبط QOwnNotes ليتابع التعديلات التي تتم في مجلد ملاحظاتك ويودعها آليًّا في مستودع git في مجلد ملاحظاتك.

This can be done in the _Git versioning_ settings in the _General settings_.

![git-note-folder-settings](/img/git/settings.png)

You need to have git installed on your system and the git executable needs to be in your PATH or
you can set the path to the git executable in the _Git versioning_ settings.

تحتاج كذلك إلى تفعيل دعم git في كل مجلد ملاحظات تريد إداراته عبر git!

![git-note-folder-settings](/img/git/note-folder-settings.png)

::: tip
Don't forget to set your git username and email address in the terminal:

```bash
git config --global user.email "your-email@domain.com"
git config --global user.name "Your name"
```

:::
