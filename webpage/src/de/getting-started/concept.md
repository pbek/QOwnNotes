# Konzept

```mermaid
graph TB
    subgraph Your computer
        qon((QOwnNotes))-->md{{"Markdown files"}}
        sync("Nextcloud Sync")-->md
        qon-comp("Browser extension")-->qon
        qc("Command-line snippet manager")-->qon
    end
    subgraph Your Nextcloud server
        qon-api("QOwnNotesApi")-->ncs[("Nextcloud server")]
        nc-notes-app("Nextcloud Notes")-->ncs
        nc-deck-app("Nextcloud Deck")-->ncs
    end

    nc-notes-mob("Nextcloud Notes mobile app")-->nc-notes-app
    qon-web-app("QOwnNotes web application")-->qon
    qon-->qon-api
    qon-->ncs
    qon-->nc-deck-app
    sync-->ncs
    qon-.->qon-web-api("api.qownnotes.org")
    qon-web-api-->github("GitHub")

    style qon fill:#d0d0ff,stroke:#333,stroke-width:4px
    click qon "/getting-started/concept.html#qownnotes" "QOwnNotes Desktop Application for managing your notes on your desktop computer"
    click md "/getting-started/concept.html#markdown-note-files" "Markdown, media and attachment files in your note folder"
    click qon-comp "/getting-started/concept.html#qownnotes-browser-extension" "QOwnNotes browser extension for managing bookmarks in markdown files and as web clipper"
    click qc "/getting-started/concept.html#qownnotes-command-line-snippet-manager" "QOwnNotes command-line snippet manager"
    click sync "/getting-started/concept.html#nextcloud-desktop-sync-client" "Nextcloud desktop sync client to sync your notes to your server"
    click ncs "/getting-started/concept.html#nextcloud-server" "Nextcloud server to host your notes and other files"
    click qon-api "/getting-started/concept.html#qownnotesapi-nextcloud-app" "QOwnNotesAPI Nextcloud app to access your server-side trash and note versions"
    click nc-notes-app "/getting-started/concept.html#nextcloud-notes-server-app" "Nextcloud Notes server app to manage your notes in the web"
    click nc-notes-mob "/getting-started/concept.html#nextcloud-notes-mobile-app" "Nextcloud Notes mobile app to manage your notes on your mobile phone"
    click nc-deck-app "/getting-started/concept.html#nextcloud-deck-server-app" "Nextcloud Deck server app to manage reminders and todo lists in the web"
    click qon-web-app "/getting-started/concept.html#qownnotes-web-app" "QOwnNotes Web App to send photos from your mobile phone"
    click qon-web-api "/getting-started/concept.html#api-qownnotes-org"
```

## QOwnNotes

- QOwnNotes **speichert Notizen** in Ihren **Notizordner als Markdown-Dateien**
- Es kann mit Ihrem Nextcloud / ownCloud-Server **kommunizieren, um Notizen** öffentlich mit anderen Personen zu teilen
- Sie können auch **über die [QOwnNotesApi Nextcloud-App](#qownnotesapi-nextcloud-app) auf Ihren Notizverlauf und Papierkorb** auf Ihrem Nextcloud / ownCloud-Server zugreifen
- Auf Todo-Listen auf Ihrem Nextcloud / ownCloud-Server kann über QOwnNotes zugegriffen werden
- **QOwnNotes synchronisiert Ihre Notizen** und Medien- / Anhangsdateien nicht!
    - Die Dateisynchronisierung ist ein komplexes Unterfangen. Es gibt bereits großartige Dateisynchronisierungslösungen (siehe [Nextcloud Desktop Sync Client](#nextcloud-desktop-sync-client)).


## Markdown-Notizdateien

- Sie **besitzen** alle Ihre Notizen und Medien- / Anhangsdateien!
- Jegyzeteit **egyszerű szöveges Markdown-fájlokként** tárolja az asztali számítógépe
- Sie können einen beliebigen Texteditor neben QOwnNotes verwenden, um Ihre Notizdateien anzuzeigen oder zu bearbeiten
- **Synchronisieren Sie Ihre Notizen** mit anderen Geräten (Desktop & Mobile) mit Ihrer [Nextcloud](https://nextcloud.com/) oder [ownCloud](https://owncloud.org/) Client mit Ihrem Server synchronisieren


## QOwnNotes-Browsererweiterung

Sie können Ihre **Browser-Lesezeichen** mit QOwnNotes verwalten oder als **Webclipper** verwenden.

::: tip
Die Browsererweiterungen funktionieren **offline**, keine Internetverbindung erforderlich. Weitere Informationen finden Sie unter [QOwnNotes Web Companion-Browsererweiterung](browser-extension.md).
:::

## QOwnNotes command-line snippet manager

Sie können Ihre **Befehlsschnipsel** mit QOwnNotes verwalten und auf der Befehlszeile ausführen.

::: tip
Weitere Informationen finden Sie unter [QOwnNotes-Befehlszeilen-Snippet-Manager](command-line-snippet-manager.md).
:::

## Nextcloud Desktop Sync Client

**Synchronisieren Sie Ihre Notizen** mit anderen Geräten (desktop & mobile) mit Ihrem [Nextcloud](https://nextcloud.com/)- oder [ownCloud](https://owncloud.org/)-Sync-Client mit Ihrem Server.

::: tip
Natürlich können auch andere Lösungen wie **Dropbox**, **Syncthing**, **Seafile** oder BitTorrent Sync verwendet werden, um Ihre Notizen und andere Dateien zu synchronisieren.

Sie können auch **git** verwenden, um mit Tools wie [gitomatic](https://github.com/muesli/gitomatic/) zu synchronisieren.
:::

## Nextcloud Server

Um online mit Ihren Notizen zu arbeiten, können Sie Server wie [Nextcloud](https://nextcloud.com/) oder [ownCloud](https://owncloud.org/) verwenden.

Sie können Ihren eigenen Server hosten oder gehostete Lösungen verwenden.

Es gibt eine [von der Community gepflegte Liste der Nextcloud-Anbieter](https://github.com/nextcloud/providers#providers) sowie eine [Liste der Geräte mit Nextcloud](https://nextcloud.com/devices/).

[Portknox](https://portknox.net) hat gemeldet, dass [QOwnNotesAPI installiert ist](https://portknox.net/en/app_listing).

::: tip
Natürlich können auch andere Lösungen wie **Dropbox**, **Syncthing**, **Seafile** oder BitTorrent Sync verwendet werden, um Ihre Notizen und andere Dateien zu hosten.
:::

## QOwnNotesAPI Nextcloud app

Mit [**QOwnNotesAPI**](https://github.com/pbek/qownnotesapi) können Sie auf Ihre zugreifen serverseitige **verworfene Notizen** und **Notizenversionen**.

::: tip
Bitte besuchen Sie [QOwnNotesAPI Nextcloud App](qownnotesapi.md) für weitere Informationen.
:::

## Nextcloud Notes Server App

Verwenden Sie [**Nextcloud Notes**](https://github.com/nextcloud/notes), um Ihre Notizen im **Web** zu bearbeiten.

::: warning
Beachten Sie, dass Nextcloud Notes derzeit nur bis zu einer Ebene von Unterordnern unterstützt.
:::

## Nextcloud Deck server app

You can use QOwnNotes to quickly create **cards** in [**Nextcloud Deck**](https://github.com/nextcloud/deck).

## Nextcloud Notes mobile app

To access your Nextcloud / ownCloud notes from your **mobile device** you can use different apps.

### Android

- [Nextcloud Notes for Android](https://play.google.com/store/apps/details?id=it.niedermann.owncloud.notes) (3rd party)

::: tip
You could also use any sync-tool like *Synchronize Ultimate* or *FolderSync* to sync your note files and use software like *neutriNotes* to edit your notes.
:::

### iOS

- [CloudNotes für iOS](https://itunes.apple.com/de/app/cloudnotes-owncloud-notes/id813973264?mt=8) (Drittanbieter)

::: tip
You can also use [Notebooks](https://itunes.apple.com/us/app/notebooks-write-and-organize/id780438662) and sync your notes via WebDAV, there is a good tutorial at [Taking Notes with Nextcloud, QOwnNotes, and Notebooks](https://lifemeetscode.com/blog/taking-notes-with-nextcloud-qownnotes-and-notebooks)
:::

## api.qownnotes.org

Dies ist ein Onlinedienst von QOwnNotes, mit dem überprüft werden kann, ob eine neue Version der Anwendung verfügbar ist.

It is talking to GitHub and checks for the latest release, gets a suited download url and compiles the changes from the changelog compared to the version of QOwnNotes you are currently using as html to show in the update dialog.

In addition, it also provides the [Release RSS Feed](http://api.qownnotes.org/rss/app-releases) and an implementation of the legacy update checking api for older versions of QOwnNotes.

::: tip
You can access the source code for [api.qownnotes.org](https://api.qownnotes.org) on [GitHub](https://github.com/qownnotes/api).
:::

## QOwnNotes Web App

You can insert photos from your mobile phone into the current note in QOwnNotes on your desktop via the **web application** on [app.qownnotes.org](https://app.qownnotes.org/).

::: tip
Please visit [QOwnNotes Web App](web-app.md) for more information.
:::
