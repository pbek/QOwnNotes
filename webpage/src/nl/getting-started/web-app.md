---
image: /img/blog/web-application-settings.png
---

# QOwnNotes-webapp

U kunt foto's van uw mobiele telefoon invoegen in de huidige notitie in QOwnNotes op uw desktop via de **webapplicatie** op [app.qownnotes.org](https://app.qownnotes.org/).

![QOwnNotes-webtoepassingsbrowser](/img/blog/web-application-browser.png "Stuur foto's vanaf uw mobiele telefoon naar QOwnNotes op het bureaublad")

De webtoepassing op uw telefoon communiceert via internet met uw QOwnNotes-desktoptoepassing. De communicatie tussen QOwnNotes en de webservice is **transport versleuteld**. No images will be stored on the server.

Er was een instellingenpagina **Webapplicatie** om ondersteuning voor de webapplicatie toe te voegen.

![QOwnNotes Web Application-instellingen](/img/blog/web-application-settings.png "Setup communicatie naar webapplicatie")

Don't forget to **copy your secret token** from the QOwnNotes settings page to [app.qownnotes.org](https://app.qownnotes.org/) to be able to communicate with your local instance of QOwnNotes. You can also scan the token from the QR code with your phone in the web application.

Je kunt de broncode van de webapplicatie vinden op [web-app op GitHub](https://github.com/qownnotes/web-app).

U kunt deze webservice ook zelf hosten, de pas gebouwde docker-afbeeldingen zijn beschikbaar op [qownnotes-web-app op DockerHub](https://hub.docker.com/repository/docker/pbeke/qownnotes-web-app).
