# Obniżka cen

Ta ściągawka Markdown ma służyć jako szybkie odniesienie i prezentacja składni Markdown w QOwnNotes.

## Nagłówki

Użyj nagłówków, aby uporządkować swoje teksty.

```markdown
# H1
## H2
### H3
#### H4
##### H5
###### H6
```

::: tip
**Panel nawigacyjny** przedstawia strukturę nagłówków.
:::

Alternatywnie, dla H1 i H2, styl podkreślony:

```markdown
Alt-H1
======

Alt-H2
------
```

::: tip
Domyślnie QOwnNotes tworzy **nazwę pliku notatki** z **nagłówka 1** (h1).
:::

## Nacisk

```markdown
Wyróżnienie (aka kursywa) z *gwiazdkami*.

Silne wyróżnienie, czyli pogrubienie, z **gwiazdkami**.
```

::: tip
Aby pogrubić tekst, możesz użyć [skrótu](./shortcuts.md) <kbd>Ctrl + B</kbd> i <kbd>Ctrl + I</kbd>, aby uczynić go kursywą.
:::

## Podkreślenie

There also is an optional setting to enable underline rendering in the *Preview settings*.

```markdown
_podkreślać_
```

::: tip
Możesz użyć [skrótu](./shortcuts.md) <kbd>Ctrl + U</kbd>, aby podkreślić tekst.
:::

## Skreślony

```markdown
~~strike out~~
```

::: tip
Możesz użyć [skrótu](./shortcuts.md) <kbd>Alt + Shift + S</kbd>, aby przekreślić tekst.
:::

## Listy

Istnieje wiele sposobów tworzenia list.

```markdown
1. Pierwsza zamówiona pozycja na liście
2. Kolejny przedmiot
  * Unordered sub-list.
1. Rzeczywiste liczby nie mają znaczenia, tylko to, że to liczba
  1. Uporządkowana podlista (działa tylko w edytorze, nie w podglądzie)
4. I kolejny przedmiot.

* Lista nieuporządkowana może zawierać gwiazdki
- Lub minusy
+ I plusy
```

::: tip
Jeśli naciśniesz <kbd>Enter</kbd> na końcu listy, zostanie utworzony nowy element listy.
:::

## Łączy

Istnieje wiele sposobów tworzenia linków.

```markdown
[I'm an inline-style link](https://www.google.com)

[I'm an inline-style link with title](https://www.google.com "Google's Homepage")

[You can use numbers for reference-style link definitions][1]

Adresy URL i adresy URL w nawiasach ostrych zostaną automatycznie przekształcone w linki w podglądzie. 
http://www.example.com or <http://www.example.com>

[1]: https://www.qownnotes.org
```

::: tip
Możesz użyć [skrótu](./shortcuts.md) <kbd>Ctrl + L</kbd>, aby **utworzyć łącza do stron internetowych lub innych notatek**.

Użycie klawiszy <kbd>Ctrl + Shift + X</kbd> spowoduje wyświetlenie okna dialogowego ułatwiającego **wstawianie załączników** do notatki.

Możesz nacisnąć klawisze <kbd>Ctrl + spacja</kbd>, gdy kursor znajduje się na łączu w edycji notatki, aby podążać za linkiem.
:::

### Zakładki

Zakładki używane przez rozszerzenie przeglądarki [QOwnNotes Web Companion](./browser-extension.md) używają łączy na listach.

```markdown
- [Nazwa strony internetowej] (https://www.example.com) #tag1 #tag2 jakiś opis i tagi
```

## Obrazy

Obrazy można osadzać w QOwnNotes. Zostaną wyświetlone w podglądzie.

```markdown
![alt text](media/my-image.jpg)
```

::: tip
Możesz użyć [skrótu](./shortcuts.md) <kbd>Ctrl + Shift + I</kbd>, aby wstawić obraz do notatki. Obraz może również znajdować się w schowku, okno dialogowe wykryje go i pokaże podgląd.

Możesz także wkleić obraz ze schowka bezpośrednio do notatki za pomocą <kbd>Ctrl + Shift + V</kbd>.
:::


## Kod wbudowany i bloki kodu

```markdown
Wbudowany „kod” ma „wsteczne kleszcze”.
```

::: tip
Możesz użyć [skrótu](./shortcuts.md) <kbd>Ctrl + Shift + C</kbd> na zaznaczonym tekście wbudowanym lub bezpośrednio w tekście, aby utworzyć wbudowany blok kodu.
:::

Bloki kodu są albo odgrodzone liniami z trzema znacznikami wstecznymi, albo wcięte czterema spacjami.

### Ogrodzenie 4-polowe

Dodaj cztery spacje przed kodem, aby oznaczyć go jako blok kodu.

```markdown
    s = "Code with space indent"
    print s
```

### Ogrodzenie Backtick

Możesz także użyć trzech lewych apostrofów, aby utworzyć blok kodu.
~~~markdown
```
Kod idzie tutaj
Kod idzie tutaj
```
~~~

::: tip
Możesz użyć [skrótu](./shortcuts.md) <kbd>Ctrl + Shift + C</kbd> na
wiele wybranych wierszy tekstu lub w pustym wierszu, aby utworzyć blok kodu. 
:::

### Tylne ogrodzenie z podświetleniem kodu

W QOwnNotes jest również podświetlanie składni za pomocą bloków kodu.

~~~markdown
```bash
# I am a comment
cd Notes
```
~~~

Obecnie obsługiwane języki (i identyfikatory bloków kodu) to:

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

## Tabele

Tabele nie są częścią rdzenia specyfikacji Markdown, ale podgląd QOWNOTES ich obsługuje. 

~~~markdown
Dwukropków można używać do wyrównywania kolumn.

| Tables        | Are           | Cool  |
| ------------- |:-------------:| -----:|
| col 3 is      | right-aligned | $1600 |
| col 2 is      | centered      |   $12 |
| zebra stripes | are neat      |    $1 |

There must be at least 3 dashes separating each header cell.

Możesz także użyć wbudowanego Markdowna.

| Markdown | Less | Pretty |
| --- | --- | --- |
| *Still* | `renders` | **nicely** |
| 1 | 2 | 3 |
~~~

::: tip
Naciśnij klawisze <kbd>Alt + Shift + T</kbd>, aby aktywować okno dialogowe, które może pomóc w tworzeniu tabel. Możesz nawet importować pliki CSV w tym oknie dialogowym.

Użyj <kbd>Ctrl + spacja</kbd> wewnątrz tabeli przecen, aby automatycznie ją sformatować.
:::

## Cytaty blokowe

```markdown
> Cytaty blokowe są bardzo przydatne w wiadomościach e-mail do emulacji tekstu odpowiedzi.
> Ten wiersz jest częścią tego samego cytatu.

Przerwa na wycenę.

> Jest to bardzo długa linijka, która nadal będzie poprawnie cytowana, gdy zostanie zawinięta. Och chłopcze, piszmy dalej, aby upewnić się, że to wystarczająco długo, aby rzeczywiście zapakować dla wszystkich. Och, możesz *wstawić* **Markdown** do cytatu blokowego. 
```

::: tip
Możesz powiedzieć QOwnNotes, aby w pełni podświetlał cytaty lub tylko znak cytatu w *ustawieniach edytora*

Możesz użyć [skrótu](./shortcuts.md) <kbd>Ctrl + Shift + B</kbd>, aby zaznaczyć tekst jako cytat blokowy.
:::

## Linia pozioma

Istnieją trzy sposoby uzyskania reguły horyzontalnej: łączniki, gwiazdki lub podkreślenia.

```markdown
Trzy lub więcej ...

Myślniki

---

Gwiazdki

***

Podkreślenia
```

## Podziały wierszy

- Możesz podzielić akapit na więcej niż jeden wiersz, aby ułatwić edycję, nadal renderują się jako pojedynczy akapit bez przerw.
- Podział wiersza wewnątrz akapitu można wymusić, kończąc wiersz dwiema spacjami.
- Możesz utworzyć oddzielny akapit, oddzielając go pustymi wierszami.

::: tip
Możesz wprowadzić dwie spacje i nowy wiersz za pomocą <kbd>⇧ Shift</kbd> + <kbd>Return</kbd>.
:::

```markdown
Oto kwestia, od której możemy zacząć.

Ten wiersz jest oddzielony od poprzedniego dwoma znakami nowego wiersza, więc będzie to *oddzielny akapit*.

Ta linia również zaczyna się osobnym akapitem, ale...
Ta linia jest oddzielona tylko dwoma końcowymi spacjami i pojedynczym znakiem nowej linii, 
więc jest oddzielną linią w *tym samym akapicie*.
```

::: tip
Spacje końcowe są domyślnie podświetlone w edytorze.
:::

## Komentarze

Komentarze nie są wyświetlane w podglądzie.

```markdown
[comment]: # (Ten komentarz nie pojawi się w podglądzie)

<!-- HTML komentarze są również ukryte -->
```

::: tip
Wiodący blok komentarza html w notatce zostanie również zignorowany podczas automatycznego generowania nazwy pliku notatki.
:::

## Listy wyboru

Możesz tworzyć proste listy rzeczy do zrobienia z listami wyboru.

```markdown
- [x] done
- [ ] todo
```

::: tip
Możesz zaznaczyć / odznaczyć pola wyboru w podglądzie.
:::

## Frontmatter

W QOwnNotes możesz użyć frontmatera (np.YAML), aby dodać dodatkowe meta informacje. Nie będzie on **wyświetlany w podglądzie** i nie będzie **przeszkadzać w automatycznym generowaniu nazw plików z notatkami**.

```markdown
---
tytuł: Jakieś imię
opis: Jakiś opis
---

# Nagłówek notatki zaczyna się tutaj

Jakiś tekst
```

Nazwa pliku tej przykładowej notatki byłaby następująca: `Nagłówek notatki zaczyna się tutaj.md`.
