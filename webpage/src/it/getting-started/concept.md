# Concept

```mermaid
graph TB
    subgraph Your computer
        qon((QOwnNotes))-->md{{"Markdown files"}}
        sync("Nextcloud Sync")-->md
        qon-comp("QOwnNotes browser extension")-->qon
    end
    subgraph Your Nextcloud server
        qon-api("QOwnNotesApi")-->ncs[("Nextcloud server")]
        nc-notes-app("Nextcloud Notes")-->ncs
    end

    nc-notes-mob("Nextcloud Notes mobile app")-->nc-notes-app
    qon-web-app("QOwnNotes web application")-->qon
    qon-->qon-api
    qon-->ncs
    sync-->ncs
    qon-.->qon-web-api("api.qownnotes.org")
    qon-web-api-->github("GitHub")

    style qon fill:#d0d0ff,stroke:#333,stroke-width:4px
    click qon "/getting-started/concept.html#qownnotes" "QOwnNotes Desktop Application for managing your notes on your desktop computer"
    click md "/getting-started/concept.html#markdown-note-files" "Markdown, media and attachment files in your note folder"
    click qon-comp "/getting-started/concept.html#qownnotes-browser-extension" "QOwnNotes browser extension for managing bookmarks in markdown files and as web clipper"
    click sync "/getting-started/concept.html#nextcloud-desktop-sync-client" "Nextcloud desktop sync client to sync your notes to your server"
    click ncs "/getting-started/concept.html#nextcloud-server" "Nextcloud server to host your notes and other files"
    click qon-api "/getting-started/concept.html#qownnotesapi-nextcloud-app" "QOwnNotesAPI Nextcloud app to access your server-side trash and note versions"
    click nc-notes-app "/getting-started/concept.html#nextcloud-notes-server-app" "Nextcloud Notes server app to manage your notes in the web"
    click nc-notes-mob "/getting-started/concept.html#nextcloud-notes-mobile-app" "Nextcloud Notes mobile app to manage your notes on your mobile phone"
    click qon-web-app "/getting-started/concept.html#qownnotes-web-app" "QOwnNotes Web App to send photos from your mobile phone"
    click qon-web-api "/getting-started/concept.html#api-qownnotes-org"
```

## QOwnNotes

- QOwnNotes **memorizza le note** nella cartella **note come file markdown**
- Può parlare al tuo server Nextcloud / ownCloud **per condividere pubblicamente le note** con altre persone
- Puoi anche **accedere alla cronologia delle note e al cestino** sul tuo server Nextcloud / ownCloud tramite l '[app QOwnNotesApi Nextcloud](#qownnotesapi-nextcloud-app)
- È possibile accedere agli elenchi di cose da fare sul tuo server Nextcloud / ownCloud da QOwnNotes
- **QOwnNotes non sincronizza le tue note** e i file multimediali / allegati!
    - La sincronizzazione dei file è un'attività complessa, esistono già ottime soluzioni per la sincronizzazione dei file (vedi [Client di sincronizzazione desktop Nextcloud](#nextcloud-desktop-sync-client))


## File di note di Markdown

- **Possiedi** tutte le tue note e file multimediali / allegati!
- Le tue note vengono archiviate come **file Markdown di testo normale** sul tuo computer desktop
- Puoi utilizzare qualsiasi editor di testo che ti piace accanto a QOwnNotes per visualizzare o modificare i file delle note
- **Sincronizza le tue note** con altri dispositivi (desktop e mobili) con il tuo client di sincronizzazione [Nextcloud](https://nextcloud.com/) o [ownCloud](https://owncloud.org/) sul tuo server


## Estensione del browser QOwnNotes

Puoi gestire i **segnalibri del tuo browser** con QOwnNotes o usarlo come **web clipper**.

::: tip
Per ulteriori informazioni, visita l '[estensione del browser QOwnNotes Web Companion](browser-extension.md).
:::

## Client di sincronizzazione desktop Nextcloud

**Sincronizza le tue note** con altri dispositivi (desktop & mobili) con il tuo client di sincronizzazione [Nextcloud](https://nextcloud.com/) o [ownCloud](https://owncloud.org/) sul tuo server.

::: tip
Ovviamente altre soluzioni, come **Dropbox**, **Syncthing**, **Seafile** o BitTorrent Sync possono essere utilizzate anche per sincronizzare le tue note e altri file.

Puoi anche utilizzare **git** per eseguire la sincronizzazione con strumenti come [gitomatic](https://github.com/muesli/gitomatic/).
:::

## Server Nextcloud

Per lavorare con le tue note online puoi utilizzare server come [Nextcloud](https://nextcloud.com/) o [ownCloud](https://owncloud.org/).

Puoi ospitare il tuo server o utilizzare soluzioni ospitate.

Esiste un [elenco gestito dalla community di provider Nextcloud](https://github.com/nextcloud/providers#providers), nonché un [elenco di dispositivi con Nextcloud](https://nextcloud.com/devices/).

[Portknox](https://portknox.net) ha segnalato di avere [QOwnNotesAPI installato](https://portknox.net/en/app_listing).

::: tip
Ovviamente altre soluzioni, come **Dropbox**, **Syncthing**, **Seafile** o BitTorrent Sync possono essere utilizzate anche per ospitare le tue note e altri file :::
:::

## QOwnNotesAPI Nextcloud app

[**QOwnNotesAPI**](https://github.com/pbek/qownnotesapi) ti consente di accedere alle **note spostate nel cestino** e alle **versioni delle note** lato server.

::: tip
Per ulteriori informazioni, visita l '[app QOwnNotesAPI Nextcloud](qownnotesapi.md).
:::

## Nextcloud Notes server app

Usa [**Nextcloud Notes**](https://github.com/nextcloud/notes) per modificare le tue note nel **web**.

::: warning
Tieni presente che Nextcloud Notes attualmente supporta solo fino a un livello di sottocartelle.
:::

## Nextcloud Notes mobile app

Per accedere alle tue note Nextcloud / ownCloud dal tuo **dispositivo mobile** puoi utilizzare diverse app.

### Android

- [Nextcloud Notes for Android](https://play.google.com/store/apps/details?id=it.niedermann.owncloud.notes) (3rd party)

::: tip
Puoi anche usare qualsiasi strumento di sincronizzazione come *Synchronize Ultimate* o *FolderSync* per sincronizzare i file delle note e utilizzare software come *neutriNotes* per modificare le tue note.
:::

### iOS

- [CloudNotes for iOS](https://itunes.apple.com/de/app/cloudnotes-owncloud-notes/id813973264?mt=8) (3rd party)

::: tip
Puoi anche utilizzare [Notebook](https://itunes.apple.com/us/app/notebooks-write-and-organize/id780438662) e sincronizzare le tue note tramite WebDAV, c'è un buon tutorial su [Prendere appunti con Nextcloud, QOwnNotes e Notebooks](https://lifemeetscode.com/blog/taking-notes-with-nextcloud-qownnotes-and-notebooks)
:::

## api.qownnotes.org

Si tratta di un servizio in linea fornito da QOwnNotes per verificare se è disponibile una nuova versione dell'applicazione.

Sta parlando con GitHub e controlla l'ultima versione, ottiene un URL di download adatto e compila le modifiche dal changelog rispetto alla versione di QOwnNotes che stai attualmente utilizzando come html da mostrare nella finestra di dialogo di aggiornamento.

Inoltre, fornisce anche il [Feed RSS di rilascio](http://api.qownnotes.org/rss/app-releases) e un'implementazione dell'API di controllo degli aggiornamenti legacy per le versioni precedenti di QOwnNotes.

::: tip
Puoi accedere al codice sorgente di [api.qownnotes.org](https://api.qownnotes.org) su [GitHub](https://github.com/qownnotes/api).
:::

## QOwnNotes Web App

You can insert photos from your mobile phone into the current note in QOwnNotes on your desktop via the **web application** on [app.qownnotes.org](https://app.qownnotes.org/).

::: tip
Please visit [QOwnNotes Web App](web-app.md) for more information.
:::
