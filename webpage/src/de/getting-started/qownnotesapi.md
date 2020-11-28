# QOwnNotesAPI Nextcloud App


![qownnotesapi](/img/qownnotesapi.png)

[**QOwnNotesAPI**](https://github.com/pbek/qownnotesapi) kann den **Zugriff** auf **Papierkorb** und **Notizenversionen** für Sie erledigen.

::: tip
Sie können QOwnNotesAPI direkt in Ihrer **Nextcloud** - oder **ownCloud** -Instanz aktivieren, indem Sie nach `qownnotes` auf der **Apps-Seite Ihres Servers** unter `https://<your-cloud-domain> /settings/apps` suchen.

- [Nextcloud Apps page](https://apps.nextcloud.com/apps/qownnotesapi)
- [ownCloud Apps page](https://marketplace.owncloud.com/apps/qownnotesapi)
:::

Das Teilen von Notizen erfolgt über die von Nextcloud / ownCloud bereitgestellte API. Auf Aufgaben wird direkt über CalDAV zugegriffen.

::: tip
Verwenden Sie Ihren **Nextcloud** / **ownCloud** **Synchronisierungsclienten**, um **Ihre Notizen** zu synchronisieren. Ihre Notizen sind einfache Markdown-Dateien. Das Rad muss nicht zweimal erfunden werden. Es gibt auch Leute, die verschiedene Dienste zum Synchronisieren ihrer Notizdateien verwenden (wie Dropbox, Syncthing oder Resilio Sync).
:::

## What QOwnNotesAPI does

- Zugriff auf Ihre Papierkorbnotizen
- Zugriff auf Ihre Notizversionen

## Was QOwnNotesAPI nicht tut

- Synchronisieren Sie Ihre Notizen (bitte verwenden Sie Ihren ownCloud / Nextcloud-Synchronisierungsclient)
- Notizen teilen (dies erfolgt direkt über die ownCloud / Nextcloud-API)
- Zugriff auf Bilder über [Nextcloud Text](https://github.com/nextcloud/text) (Dies erfolgt direkt über die Nextcloud-API.)
