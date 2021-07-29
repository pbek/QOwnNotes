---
image: /img/blog/web-application-settings.png
---

# QOwnNotes-webapp

U kunt foto's van uw mobiele telefoon invoegen in de huidige notitie in QOwnNotes op uw desktop via de **webapplicatie** op [app.qownnotes.org](https://app.qownnotes.org/).

![QOwnNotes-webtoepassingsbrowser](/img/blog/web-application-browser.png "Stuur foto's vanaf uw mobiele telefoon naar QOwnNotes op het bureaublad")

De webtoepassing op uw telefoon communiceert via internet met uw QOwnNotes-desktoptoepassing. De communicatie tussen QOwnNotes en de webservice is **transport versleuteld**. Er worden geen afbeeldingen op de server opgeslagen.

Er was een instellingenpagina **Webapplicatie** om ondersteuning voor de webapplicatie toe te voegen.

![QOwnNotes Web Application-instellingen](/img/blog/web-application-settings.png "Setup communicatie naar webapplicatie")

Vergeet niet **uw geheime token te kopiëren** van de instellingenpagina van QOwnNotes naar [app.qownnotes.org](https://app.qownnotes.org/) om te kunnen communiceren met uw lokale instantie van QOwnNotes. U kunt de token van de QR-code ook scannen met uw telefoon in de webapplicatie.

Je kunt de broncode van de webapplicatie vinden op [web-app op GitHub](https://github.com/qownnotes/web-app).

U kunt deze webservice ook zelf hosten, de pas gebouwde docker-afbeeldingen zijn beschikbaar op [qownnotes-web-app op DockerHub](https://hub.docker.com/repository/docker/pbeke/qownnotes-web-app).
