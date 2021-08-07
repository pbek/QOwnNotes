---
image: /img/blog/web-application-settings.png
---

# QOwnNotes Web App

Puoi inserire foto dal tuo cellulare nella nota corrente in QOwnNotes sul desktop tramite l'**applicazione web** su [app.qownnotes.org](https://app.qownnotes.org/).

![Browser dell'applicazione web QOwnNotes](/img/blog/web-application-browser.png "Invia foto dal tuo cellulare a QOwnNotes sul desktop")

L'applicazione web sul telefono comunica con l'applicazione desktop QOwnNotes tramite Internet. La comunicazione tra QOwnNotes e il servizio web è **cifrata in transito**. Nessuna immagine verrà archiviata sul server.

C'era una pagina delle impostazioni **Applicazione web** per aggiungere il supporto per l'applicazione web.

![Impostazioni dell'applicazione web QOwnNotes](/img/blog/web-application-settings.png "Imposta la comunicazione con l'applicazione web")

Non dimenticarti di **copiare il tuo token segreto** dalla pagina delle impostazione di QOwnNotes su [app.qownnotes.org](https://app.qownnotes.org/) per poter comunicare con la tua istanza locale di QOwnNotes. Puoi anche leggere il token dal codice QR con il tuo telefono nell'applicazione web.

Puoi trovare il codice sorgente dell'applicazione web su [web-app su GitHub](https://github.com/qownnotes/web-app).

Puoi anche ospitare questo servizio web da solo, le immagini docker appena create sono disponibili su [qownnotes-web-app su DockerHub](https://hub.docker.com/repository/docker/pbeke/qownnotes-web-app).
