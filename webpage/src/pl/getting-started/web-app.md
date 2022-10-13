---
image: /img/blog/web-application-settings.png
---

# QOwnNotes Web App

Zdjęcia z telefonu komórkowego można wstawiać do bieżącej notatki w aplikacji QOwnNotes na pulpicie za pomocą **aplikacji internetowej** na stronie [app.qownnotes.org](https://app.qownnotes.org/).

![Przeglądarka aplikacji internetowych QOwnNotes](/img/blog/web-application-browser.png "Wysyłaj zdjęcia z telefonu komórkowego do QOwnNotes na pulpicie")

Aplikacja internetowa w telefonie komunikuje się z aplikacją komputerową QOwnNotes przez Internet. Komunikacja pomiędzy QOwnNotes a serwisem internetowym jest **transportowo szyfrowana**. Żadne obrazy nie będą przechowywane na serwerze.

Istnieje strona ustawień **aplikacja internetowa** aby dodać obsługę aplikacji sieci Web.

![Ustawienia aplikacji internetowej QOwnNotes](/img/blog/web-application-settings.png "Skonfiguruj komunikację z aplikacją internetową")

Nie zapomnij **skopiować swojego poufnego tokena** ze strony ustawień QOwnNotes do [app.qownnotes.org](https://app.qownnotes.org/), aby móc komunikować się z lokalną instancją QOwnNotes. Możesz również zeskanować token z kodu QR telefonem w aplikacji internetowej.

Kod źródłowy aplikacji internetowej można znaleźć na stronie [web-app na GitHubie](https://github.com/qownnotes/web-app).

Możesz również hostować tę usługę internetową samodzielnie, świeżo zbudowane obrazy docker są dostępne na [qownnotes-web-app na DockerHub](https://hub.docker.com/repository/docker/pbeke/qownnotes-web-app).
