---
image: /img/blog/web-application-settings.png
---

# Application Web QOwnNotes

Vous pouvez insérer des photos de votre téléphone mobile dans la note actuelle dans QOwnNotes sur votre bureau via l'**application Web** sur [app.qownnotes.org](https://app.qownnotes.org/).

![Navigateur d'applications Web QOwnNotes](/img/blog/web-application-browser.png "Envoyez des photos de votre téléphone mobile à QOwnNotes sur le bureau")

L'application Web sur votre téléphone communique avec votre application de bureau QOwnNotes via Internet. La communication entre QOwnNotes et le service Web est **transport crypté**. Aucune image ne sera stockée sur le serveur.

Il y avait une page de paramètres **Application Web** pour ajouter la prise en charge de l'application Web.

![Paramètres de l'application Web QOwnNotes](/img/blog/web-application-settings.png "Configurer la communication avec l'application Web")

N'oubliez pas de **copier votre jeton secret** de la page des paramètres de QOwnNotes vers [app.qownnotes. org](https://app.qownnotes.org/) pour pouvoir communiquer avec votre instance locale de QOwnNotes. Vous pouvez également scanner le jeton à partir du code QR avec votre téléphone dans l'application Web.

Vous pouvez trouver le code source de l'application Web sur [web-app sur GitHub](https://github.com/qownnotes/web-app).

Vous pouvez également héberger ce service Web vous-même, les images Docker fraîchement construites sont disponibles sur [qownnotes-web-app sur DockerHub](https://hub.docker.com/repository/docker/pbeke/qownnotes-web-app).
