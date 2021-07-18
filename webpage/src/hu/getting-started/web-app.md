---
image: /img/blog/web-application-settings.png
---

# QOwnNotes webalkalmazás

A mobiltelefonról származó fotókat beillesztheti az asztali QOwnNotes aktuális jegyzetébe az **webalkalmazás** segítségével az [app.qownnotes.org](https://app.qownnotes.org/) oldalon.

![QOwnNotes webalkalmazás böngésző](/img/blog/web-application-browser.png "Küldjön fényképeket mobiltelefonjáról az asztali QOwnNotes-ra")

A telefonon található webalkalmazás az interneten keresztül kommunikál a QOwnNotes asztali alkalmazásával. A QOwnNotes és a webszolgáltatás közötti kommunikáció ** szállítás titkosított **. No images will be stored on the server.

A webalkalmazás támogatásának megadásához volt egy beállítási oldal **Webalkalmazás**.

![QOwnNotes webalkalmazás beállításai](/img/blog/web-application-settings.png "A kommunikáció beállítása a webalkalmazáshoz")

Don't forget to **copy your secret token** from the QOwnNotes settings page to [app.qownnotes.org](https://app.qownnotes.org/) to be able to communicate with your local instance of QOwnNotes. You can also scan the token from the QR code with your phone in the web application.

A webalkalmazás forráskódját a [webalkalmazás a GitHub webhelyen találja meg](https://github.com/qownnotes/web-app).

Ezt a webszolgáltatást saját maga is üzemeltetheti, a frissen épített dokkoló képek a [ qownnotes-web-app oldalon érhetők el a DockerHubon](https://hub.docker.com/repository/docker/pbeke/qownnotes-web-app).
