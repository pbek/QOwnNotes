---
image: /img/blog/web-application-settings.png
---

# QOwnNotes Web App

Vous pouvez insérer des photos de votre téléphone mobile dans la note actuelle dans QOwnNotes sur votre bureau via l'**application Web** sur [app.qownnotes.org](https://app.qownnotes.org/).

![QOwnNotes Web Application browser](/img/blog/web-application-browser.png "Envoyez des photos de votre téléphone mobile à QOwnNotes sur le bureau")

L'application Web sur votre téléphone communique avec votre application de bureau QOwnNotes via Internet. La communication entre QOwnNotes et le service Web est **transport crypté**. Aucune image ne sera stockée sur le serveur.

Il y avait une page de paramètres **Application Web** pour ajouter la prise en charge de l'application Web.

![Paramètres de l'application Web QOwnNotes](/img/blog/web-application-settings.png "Configurer la communication avec l'application Web")

Don't forget to **copy your secret token** from the QOwnNotes settings page to [app.qownnotes.org](https://app.qownnotes.org/) to be able to communicate with your local instance of QOwnNotes. You can also scan the token from the QR code with your phone in the web application.

Vous pouvez trouver le code source de l'application Web sur [web-app sur GitHub](https://github.com/qownnotes/web-app).

Vous pouvez également héberger ce service Web vous-même, les images Docker fraîchement construites sont disponibles sur [qownnotes-web-app sur DockerHub](https://hub.docker.com/repository/docker/pbeke/qownnotes-web-app).
