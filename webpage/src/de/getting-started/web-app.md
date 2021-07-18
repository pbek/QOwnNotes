---
image: /img/blog/web-application-settings.png
---

# QOwnNotes Web App

Sie können Fotos von Ihrem Mobiltelefon über die **Webanwendung** auf [app.qownnotes.org](https://app.qownnotes.org/) in die aktuelle Notiz in QOwnNotes auf Ihrem Desktop einfügen.

![QOwnNotes Webanwendungsbrowser](/img/blog/web-application-browser.png "Senden Sie Fotos von Ihrem Mobiltelefon an QOwnNotes auf dem Desktop")

Die Webanwendung auf Ihrem Telefon kommuniziert über das Internet mit Ihrer QOwnNotes-Desktopanwendung. Die Kommunikation zwischen QOwnNotes und dem Webdienst ist **transportverschlüsselt**. Es werden keine Bilder auf dem Server gespeichert.

Es gab eine Einstellungsseite **Webanwendung**, um Unterstützung für die Webanwendung hinzuzufügen.

![QOwnNotes Webanwendungseinstellungen](/img/blog/web-application-settings.png "Richten Sie die Kommunikation zur Webanwendung ein")

Don't forget to **copy your secret token** from the QOwnNotes settings page to [app.qownnotes.org](https://app.qownnotes.org/) to be able to communicate with your local instance of QOwnNotes. You can also scan the token from the QR code with your phone in the web application.

Den Quellcode der Webanwendung finden Sie unter [Webanwendung auf GitHub](https://github.com/qownnotes/web-app).

Sie können diesen Webdienst auch selbst hosten. Die frisch erstellten Docker-Bilder finden Sie unter [qownnotes-web-app auf DockerHub](https://hub.docker.com/repository/docker/pbeke/qownnotes-web-app).
