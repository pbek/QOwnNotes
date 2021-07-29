---
image: /img/blog/web-application-settings.png
---

# QOwnNotes Web App

Sie können Fotos von Ihrem Mobiltelefon über die **Webanwendung** auf [app.qownnotes.org](https://app.qownnotes.org/) in die aktuelle Notiz in QOwnNotes auf Ihrem Desktop einfügen.

![QOwnNotes Webanwendungsbrowser](/img/blog/web-application-browser.png "Senden Sie Fotos von Ihrem Mobiltelefon an QOwnNotes auf dem Desktop")

Die Webanwendung auf Ihrem Telefon kommuniziert über das Internet mit Ihrer QOwnNotes-Desktopanwendung. Die Kommunikation zwischen QOwnNotes und dem Webdienst ist **transportverschlüsselt**. Es werden keine Bilder auf dem Server gespeichert.

Es gab eine Einstellungsseite **Webanwendung**, um Unterstützung für die Webanwendung hinzuzufügen.

![QOwnNotes Webanwendungseinstellungen](/img/blog/web-application-settings.png "Richten Sie die Kommunikation zur Webanwendung ein")

Vergessen Sie nicht, **Ihr geheimes Token** von der QOwnNotes-Einstellungsseite nach [app.qownnotes zu kopieren. org](https://app.qownnotes.org/), um mit Ihrer lokalen Instanz von QOwnNotes kommunizieren zu können. Sie können den Token auch aus dem QR-Code mit Ihrem Telefon in der Webanwendung scannen.

Den Quellcode der Webanwendung finden Sie unter [Webanwendung auf GitHub](https://github.com/qownnotes/web-app).

Sie können diesen Webdienst auch selbst hosten. Die frisch erstellten Docker-Bilder finden Sie unter [qownnotes-web-app auf DockerHub](https://hub.docker.com/repository/docker/pbeke/qownnotes-web-app).
