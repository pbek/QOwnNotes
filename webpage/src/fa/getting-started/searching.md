# جستجوی یادداشت ها

To search for notes or for text inside notes you can use the shortcut <kbd>Ctrl</kbd> + <kbd>⇧ Shift</kbd> + <kbd>F</kbd> (or <kbd>⌘ Cmd</kbd> + <kbd>⇧ Shift</kbd> + <kbd>F</kbd> on macOS) to jump to the _Note search panel_.

روش های مختلفی برای جستجوی یادداشت ها وجود دارد:

- `note book` will search for notes containing both `note` and `book` in the file name or note text
- `"note book"` دقیقاً به شکل  `note book` در میان یادداشت ها و در نام پرونده یا متن یادداشت مورد جستجو قرار می گیرد
- `n:book` یا `name:book` با کلمه `book` در میان یادداشت ها و درست در نام یادداشت یا نام پرونده جستجو می شود
- `n:"note book"` or `name:"note book"` will search for notes with exactly `note book` just in the note name or file name
- `w:note` or `word:note` will search for `note` as a whole word, excluding partial matches like `notebook`
- Search operators can be combined, for example `n:w:note` searches for the whole word `note` only in the note name or file name

![جستجو](/img/searching.png)

::: tip
Searching starts after typing at least two characters.
:::
