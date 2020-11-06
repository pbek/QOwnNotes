# QOwnNotesAPI Nextcloud App


![qownnotesapi](/img/qownnotesapi.png)

[** QOwnNotesAPI **](https://github.com/pbek/qownnotesapi) kann den ** Zugriff ** auf ** Papierkorb ** und ** Notizenversionen ** für Sie erledigen.

::: tip
Sie können QOwnNotesAPI direkt in Ihrer ** Nextcloud </ 0> - oder ** ownCloud ** -Instanz aktivieren, indem Sie nach ` qownnotes ` auf der ** Apps-Seite Ihres Servers </ 0> unter ` https: // <your-cloud-domain> / settings / apps ` suchen.</p>

- [Nextcloud Apps page](https://apps.nextcloud.com/apps/qownnotesapi)
- [ownCloud Apps page](https://marketplace.owncloud.com/apps/qownnotesapi)
:::

Das Teilen von Notizen erfolgt über die von Nextcloud / ownCloud bereitgestellte API. Auf Aufgaben wird direkt über CalDAV zugegriffen.

::: tip
Verwenden Sie Ihren ** Nextcloud ** / ** ownCloud ** ** Synchronisierungsclient **, um ** Ihre Notizen ** zu synchronisieren. Ihre Notizen sind einfache Markdown-Dateien. There is no need to invent the wheel twice. Es gibt auch Leute, die verschiedene Dienste zum Synchronisieren ihrer Notizdateien verwenden (wie Dropbox, Syncthing oder Resilio Sync).
:::

## What QOwnNotesAPI does

- Zugriff auf Ihre Papierkorbnotizen
- Zugriff auf Ihre Notizversionen

## Was QOwnNotesAPI nicht tut

- Synchronisieren Sie Ihre Notizen (bitte verwenden Sie Ihren ownCloud / Nextcloud-Synchronisierungsclient)
- Notizen teilen (dies erfolgt direkt über die ownCloud / Nextcloud-API)
- Zugriff auf Bilder über [ Nextcloud Text ](https://github.com/nextcloud/text) (Dies erfolgt direkt über die Nextcloud-API.)
