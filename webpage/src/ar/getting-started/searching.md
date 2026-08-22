# البحث عن الملاحظات

To search for notes or for text inside notes you can use the shortcut <kbd>Ctrl</kbd> + <kbd>⇧ Shift</kbd> + <kbd>F</kbd> (or <kbd>⌘ Cmd</kbd> + <kbd>⇧ Shift</kbd> + <kbd>F</kbd> on macOS) to jump to the _Note search panel_.

توجد عدة طرق مختلفة للبحث عن الملاحظات:

- `note book` will search for notes containing both `note` and `book` in the file name or note text
- ستبحث العبارة `"كشكول ملاحظات"` عن الملاحظات التي فيها النص `كشكول ملاحظات` في اسم الملف أو محتوى الملاحظة
- ستبحث العبارة `n:كشكول` أو `name:كشكول` عن الملاحظات التي فيها كلمة `كشكول` فقط في اسم الملاحظة أو اسم الملف
- `n:"note book"` or `name:"note book"` will search for notes with exactly `note book` just in the note name or file name
- `w:note` or `word:note` will search for `note` as a whole word, excluding partial matches like `notebook`
- Search operators can be combined, for example `n:w:note` searches for the whole word `note` only in the note name or file name

![البحث عن الملاحظات](/img/searching.png)

::: tip
Searching starts after typing at least two characters.
:::
