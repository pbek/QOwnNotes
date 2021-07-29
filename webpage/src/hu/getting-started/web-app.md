---
image: /img/blog/web-application-settings.png
---

# QOwnNotes webalkalmazás

A mobiltelefonról származó fotókat beillesztheti az asztali QOwnNotes aktuális jegyzetébe az **webalkalmazás** segítségével az [app.qownnotes.org](https://app.qownnotes.org/) oldalon.

![QOwnNotes webalkalmazás böngésző](/img/blog/web-application-browser.png "Küldjön fényképeket mobiltelefonjáról az asztali QOwnNotes-ra")

A telefonon található webalkalmazás az interneten keresztül kommunikál a QOwnNotes asztali alkalmazásával. A QOwnNotes és a webszolgáltatás közötti kommunikáció ** szállítás titkosított **. Nem tárol képeket a szerveren.

A webalkalmazás támogatásának megadásához volt egy beállítási oldal **Webalkalmazás**.

![QOwnNotes webalkalmazás beállításai](/img/blog/web-application-settings.png "A kommunikáció beállítása a webalkalmazáshoz")

Ne felejtse el **másolni a titkos tokent** a QOwnNotes beállítások oldaláról az [app.qownnotes fájlba.org](https://app.qownnotes.org/), hogy kommunikálni tudjon a QOwnNotes helyi példányával. A tokent a QR-kódból is beolvashatja a telefonjával a webalkalmazásban.

A webalkalmazás forráskódját a [webalkalmazás a GitHub webhelyen találja meg](https://github.com/qownnotes/web-app).

Ezt a webszolgáltatást saját maga is üzemeltetheti, a frissen épített dokkoló képek a [ qownnotes-web-app oldalon érhetők el a DockerHubon](https://hub.docker.com/repository/docker/pbeke/qownnotes-web-app).
