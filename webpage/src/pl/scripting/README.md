# QOwnNotes Scripting

Skrypt QOwnNotes to głównie **JavaScript** w [plikach Qt QML](https://doc.qt.io/qt-5/qtqml-index.html).

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

Możesz umieścić te pliki QML w dowolnym miejscu i **dodać je w QOwnNotes**, dodając je w **Ustawieniach skryptów** (jest przycisk `Dodaj skrypt`/ `Dodaj lokalny skrypt`).

::: tip
Spójrz na [przykładowe skrypty](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples), aby szybko rozpocząć.
:::

W **Ustawieniach skryptów** można również zainstalować skrypty bezpośrednio z [**Repozytorium skryptów**](https://github.com/qownnotes/scripts).

W przypadku problemów, pytań lub próśb o dodanie funkcji skryptów z **repozytorium skryptów** otwórz zgłoszenie na [stronie problemu z repozytorium skryptów QOwnNotes](https://github.com/qownnotes/scripts/issues).

::: tip
Jeśli chcesz zaproponować skrypt dla **repozytorium skryptów**, postępuj zgodnie z instrukcjami w [repozytorium skryptów QOwnNotes](https://github.com/qownnotes/scripts).
:::

Jeśli potrzebujesz dostępu do określonej funkcji w QOwnNotes lub masz pytania lub pomysły, otwórz problem na [stronie problemu QOwnNotes](https://github.com/pbek/QOwnNotes/issues).

::: tip
Do logowania możesz użyć polecenia `script.log()`, aby logować się do widżet log.
:::