---
image: /img/blog/web-application-settings.png
---

# QOwnNotes Web App

You can insert photos from your mobile phone into the current note in QOwnNotes
on your desktop via the **web application** on [app.qownnotes.org](https://app.qownnotes.org/).

![QOwnNotes Web Application browser](/img/blog/web-application-browser.png "Send photos from your mobile phone to QOwnNotes on the desktop")

The web application on your phone communicates with your QOwnNotes desktop application over the internet.
The communication between QOwnNotes and the web service is **transport encrypted**.
No images will be stored on the server.

There was a settings page **Web application** to add support for the web application.

![QOwnNotes Web Application settings](/img/blog/web-application-settings.png "Setup communication to web application")

Don't forget to **copy your secret token** from the QOwnNotes settings page to [app.qownnotes.org](https://app.qownnotes.org/) to be able to communicate with your local instance of QOwnNotes. You can also scan the token from the QR code with your phone in the web application.

You can find the source code of the web application at [web-app on GitHub](https://github.com/qownnotes/web-app).

You can also host this web service yourself, the freshly built docker images are available at
[qownnotes-web-app on DockerHub](https://hub.docker.com/repository/docker/pbeke/qownnotes-web-app).
