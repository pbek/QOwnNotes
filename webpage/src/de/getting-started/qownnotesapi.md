# QOwnNotesAPI Nextcloud App

![qownnotesapi](/img/qownnotesapi.png)

[**QOwnNotesAPI**](https://github.com/pbek/qownnotesapi) kann den **Zugriff** auf **gelöschte Notizen** und **Notizversionen** für Sie erledigen.

::: tip
Sie können QOwnNotesAPI direkt in Ihrer **Nextcloud**- oder **ownCloud**-Instanz aktivieren, indem Sie nach `qownnotes` auf der **Apps-Seite Ihres Servers** unter `https://<your-cloud-domain>/settings/apps` suchen.

- [Nextcloud Apps-Seite](https://apps.nextcloud.com/apps/qownnotesapi)
- [ownCloud Apps-Seite](https://marketplace.owncloud.com/apps/qownnotesapi)
:::

Das Teilen von Notizen erfolgt über die von Nextcloud / ownCloud bereitgestellte API. Auf Aufgaben wird direkt über CalDAV zugegriffen.

::: tip
Verwenden Sie Ihren **Nextcloud** / **ownCloud** **Sync-Client**, um **Ihre Notizen zu synchronisieren**. Ihre Notizen sind einfache Markdown-Dateien. Das Rad muss nicht zweimal erfunden werden. Es gibt auch Leute, die andere Dienste zum Synchronisieren ihrer Notizdateien verwenden (wie Dropbox, Syncthing oder Resilio Sync).
:::

## Was QOwnNotesAPI macht

- Auf Ihre gelöschten Notizen zugreifen
- Auf Ihre Notizversionen zugreifen

## Was QOwnNotesAPI nicht macht

- Ihre Notizen synchronisieren (bitte verwenden Sie Ihren ownCloud / Nextcloud-Synchronisierungsclient)
- Notizen teilen (dies erfolgt direkt über die ownCloud- / Nextcloud-API)
- Auf Ihre Bilder von [Nextcloud Text](https://github.com/nextcloud/text) zugreifen (dies erfolgt direkt über die Nextcloud-API.)
