# QOwnNotesAPI Nextcloud App


![qownnotesapi](/img/qownnotesapi.png)

[**QOwnNotesAPI**](https://github.com/pbek/qownnotesapi) can handle the **access** to **trashed notes** and **note versions** for you.

::: tip
You can enable QOwnNotesAPI directly in your **Nextcloud** or **ownCloud** instance by searching for `qownnotes` in your server's **Apps page** at `https://<your-cloud-domain>/settings/apps`.

- [Nextcloud Apps-pagina](https://apps.nextcloud.com/apps/qownnotesapi)
- [ownCloud Apps page](https://marketplace.owncloud.com/apps/qownnotesapi)
:::

Het delen van notities gebeurt via de API van Nextcloud / ownCloud. Taken zijn rechtstreeks toegankelijk via CalDAV.

::: tip
Use your **Nextcloud** / **ownCloud** **sync client** to **sync your notes**, your notes are simple markdown files. Het wiel hoeft niet twee keer uit te vinden. En er zijn ook mensen die verschillende services gebruiken om hun notitiebestanden te synchroniseren (zoals Dropbox, Syncthing of Resilio Sync).
:::

## What QOwnNotesAPI does

- Toegang tot uw weggegooide notities
- Toegang tot uw notitieversies

## Wat QOwnNotesAPI niet doet

- Uw notities synchroniseren (gebruik uw ownCloud / Nextcloud-synchronisatieclient)
- Notities delen (dat gebeurt rechtstreeks via de ownCloud / Nextcloud API)
- Toegang tot afbeeldingen van [Nextcloud-tekst](https://github.com/nextcloud/text) (dat gebeurt rechtstreeks via de Nextcloud-API)
