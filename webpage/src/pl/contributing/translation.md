# Tłumaczenie

Dziękuję za pomoc!

Jak mogę pomóc w tłumaczeniu QOwnNotes?

::: warning
Please don't attempt to edit the translation files in `src/languages` directly. They are all generated automatically via [Crowdin](https://crowdin.com/project/qownnotes).
:::

## Dołącz do nas na platformie Crowdin, aby tłumaczyć QOwnNotes

![crowdin](/img/crowdin.png)

**Crowdin** to popularna platforma do zarządzania procesem lokalizacji takich zasobów, jak aplikacje mobilne, zasoby sieci Web, oprogramowanie komputerowe i tym podobne.

You can sign up at [Crowdin](https://crowdin.com/project/qownnotes) and **translate** **QOwnNotes** to your language with easy tools.

**[QOwnNotes w Crowdin](https://crowdin.com/project/qownnotes)**

Po prostu [wrzuć wiadomość do właściciela projektu na Crowdin](https://crowdin.com/profile/pbek) lub jako [problem na GitHubie](https://github.com/pbek/QOwnNotes/issues) jeśli chcesz przetłumaczyć QOwnNotes na nowy język.

Jeśli chcesz dodać nowe tłumaczenia do **istniejących języków**, możesz to zrobić od razu.

## Korekta

Jeśli jesteś doskonałym lub nawet rodzimym użytkownikiem języka, który nie został jeszcze poddany korekcie, będziemy wdzięczni, jeśli pomożesz nam uczynić QOwnNotes bardziej wiarygodnym i wygodnym poprzez sprawdzanie i, jeśli to konieczne, poprawianie niezatwierdzonych tłumaczeń. Musisz poprosić właściciela o pozwolenie na bycie korektorem Twojego języka.

Kliknij tutaj, aby sprawdzić, czy oprogramowanie jest już przetłumaczone na Twój język.

[QOwnNotes w Crowdin](https://translate.qownnotes.org/)

Kiedy już [skontaktujesz się z właścicielem projektu na platformie Crowdin](https://crowdin.com/profile/pbek) i otrzymasz pozwolenie na przeprowadzenie korekty w swoim języku, możesz przystąpić do pracy!

## Ogólne wskazówki

Pracując nad projektem, weź pod uwagę kilka kwestii.

### Dokładność

Lokalizuj z maksymalną możliwą dokładnością, dbając jednocześnie o płynność w języku docelowym.

### Precyzja

Obiekt docelowy musi reprezentować źródło dokładnie i bez pomijania lub dodawania jakichkolwiek części ciągu.

### Kontekst

Jeśli potrzebujesz więcej kontekstu lub pomocy z pewnym ciągiem znaków, a znasz inny język, możesz zmienić język i zobaczyć, co inni tłumacze zrobili z danego segmentu. Ale nie krępuj się zostawić wiadomość i poprosić o wyjaśnienie po prawej stronie strony.

### Często używane kluczowe terminy

Konieczne jest sprawdzenie i użycie tych samych tłumaczeń dla często używanych terminów kluczowych, na przykład "uwaga" musi być przetłumaczona zawsze w ten sam sposób.

Jeśli `trashed notes` to po niemiecku `gelöschte Dateien`, zawsze należy tak tłumaczyć to wyrażenie. Wyjątkiem są przypadki, kiedy gramatyka języka zmusza Cię do użycia alternatywnego terminu, ale i tu należy unikać zbyt skomplikowanych wyrażeń.

![zrzut ekranu](/img/crowdin/screenshot-7.png)

Aby znaleźć wszystkie identyczne wyrażenia, przeszukaj Crowdin, aby zobaczyć, czy wszystkie wystąpienia terminu rzeczywiście mają to samo tłumaczenie, jeśli wiesz lepiej, możesz zasugerować lepsze tłumaczenie i zagłosować w dół (lub w górę) na lepsze / poprawne tłumaczenie.

Zlokalizuj wszystkie wystąpienia identycznie tak, jak to tylko możliwe.

`Note` zdecydowanie powinno zawsze być tłumaczone jednakowo.

### Formy liczby mnogiej

W przypadku liczby mnogiej podaj odpowiednie tłumaczenie dla każdego bloku.

![zrzut ekranu](/img/crowdin/screenshot-4.png)

### Zmienne

Tagi `%1` należy zapisywać bez spacji. W przypadku języków RTL "pisanych od prawej do lewej strony" możesz wstawić tagi z tekstu źródłowego lub wpisać je w tłumaczenie.

![zrzut ekranu](/img/crowdin/screenshot-1.png)

![zrzut ekranu](/img/crowdin/screenshot-5.png)

![zrzut ekranu](/img/crowdin/screenshot-3.png)

### Klawisze dostępu

Użyj symbolu `&` przed literą, która ma być zastosowana w skrócie klawiaturowym. Może to być ta sama litera, co w oryginale, ale nie jest to konieczne.

![zrzut ekranu](/img/crowdin/screenshot-4.png)

### Kontenery niestandardowe

Ciągi takie jak `::: tip`, `::: warning` i `::: info` w dokumentacji są znacznikami dla [Kontenerów niestandardowych](https://vuepress.vuejs.org/guide/markdown.html#custom-containers) w VuePress i nie powinny być tłumaczone.

### Inne ciągi znaków, których nie należy tłumaczyć

Niektóre segmenty nie tłumaczymy w Dokumentacji:

- Nigdy nie tłumacz kodu lub linków, "tag 'etykieta'" muszą być pozostawione tak jak są (i bez dodanych spacji, ponieważ niektóre tłumaczenia maszynowe tak robią, dlatego należy je usunąć)
- Nie tłumaczymy daty i godziny (pozostaw puste)
- Nie tłumaczymy również odniesień (które QOwnNotes dostał w wielu językach tłumaczenia), więc możesz je po prostu pominąć

## Tłumaczenie i korekta mogą być ekscytujące!

Będziesz miał do czynienia z językiem tego oprogramowania, nauczysz się nowych terminów i otrzymasz wiele wewnętrznych informacji na temat projektu.

Jeśli jesteś gotów pomóc w korekcie tłumaczenia swojego języka, skontaktuj się z [Patrizio](https://crowdin.com/profile/pbek) i zapytaj, jeśli masz jakieś pytania i / lub uzyskaj prawa do korekty w swoim języku ojczystym.

Wielkie dzięki za pomoc! 🙂
