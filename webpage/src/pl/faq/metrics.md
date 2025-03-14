# Dlaczego metryki?

## Dlaczego metryki są przydatne?

QOwnNotes (as in the application itself) is **developed by mostly one person in his spare time**.

Wydania są budowane dla wielu platform i dystrybucji (często bardzo starych wersji). Utrzymanie ich to dużo pracy.

Metryki są jedynym niezawodnym źródłem informacji o tym, na których wersjach platform i dystrybucji działa QOwnNotes. Pokazują również, które funkcje są najbardziej popularne, dzięki czemu można podejmować decyzje dotyczące wersji, które mają zostać wycofane, które funkcje wymagają więcej pracy, a które są rzadko używane.

To samo dotyczy wyboru preferowanych języków przez użytkownika, co sprzyja ulepszeniu odpowiednich tłumaczeń.

## Gdzie przechowywane są metryki?

Metryki są przesyłane do i przechowywane na serwerze self-hosted [Matomo](https://matomo.org/). Dostęp do danych mają tylko dwaj główni programiści. **Żadne dane nie są udostępniane osobom trzecim.**

## W jaki sposób zbierane są metryki?

Metryki są **anonimizowane** (np. ip address stripped out) na serwerze Matomo po ich odebraniu.

Żadne dane osobowe, takie jak wprowadzony tekst lub nazwy notatek nie są gromadzone lub przechowywane na serwerze!

## Co jest zbierane?

Przesyłane są podstawowe informacje, takie jak rozdzielczość ekranu, system operacyjny i wersja używanego programu QOwnNotes.

Ponadto do serwera wysyłane są np. zdarzenia, gdy w aplikacji wykonywane są akcje.

QOwnNotes jest darmowym oprogramowaniem open source. Możesz przeglądać metryki przez cały czas, wyszukując na przykład dla [sendVisitIfEnabled](https://github.com/pbek/QOwnNotes/search?q=sendVisitIfEnabled) i [sendEventIfEnabled](https://github.com/pbek/QOwnNotes/search?q=sendEventIfEnabled) w kodzie źródłowym, aby zobaczyć, co dokładnie jest wysyłane.

**Nie są zbierane żadne dane osobowe, takie jak wprowadzony tekst lub nazwy notatek!**

## Czy mogę wyłączyć metrykę?

Można je wyłączyć przy pierwszym uruchomieniu aplikacji w oknie dialogowym powitalnym lub później w oknie dialogowym ustawień.

## Dlaczego metryki nie są domyślnie wyłączone?

Domyślne są królem. Domyślne wyłączenie danych spowodowałoby brak znaczących danych.

Większość ludzi nie dba o wskaźniki. Ci, którzy dbają o metryki i nie chcą głosować swoimi metrykami na to, nad czym pracuje się lub co jest obsługiwane, mogą je łatwo wyłączyć przy pierwszym uruchomieniu aplikacji.

Otrzymasz nawet drugie okno dialogowe, które powie Ci o wskaźnikach, jeśli zakończysz okno powitalne zbyt wcześnie.
