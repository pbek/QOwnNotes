# QOwnNotesAPI Nextcloud App

![qownnotesapi](/img/qownnotesapi.png)

[**QOwnNotesAPI**](https://github.com/pbek/qownnotesapi) kan de **toegang** tot **weggegooide notities** en **notitieversies** voor u afhandelen.

U kunt QOwnNotesAPI rechtstreeks inschakelen in uw **Nextcloud** of **ownCloud**-instantie door te zoeken naar `qownnotes` in de **Apps page** op `https://<your-cloud-domain>/settings/apps` van uw server.

- [Nextcloud Apps-pagina](https://apps.nextcloud.com/apps/qownnotesapi)
- [ownCloud Apps page](https://marketplace.owncloud.com/apps/qownnotesapi)

:::

Het delen van notities gebeurt via de API van Nextcloud / ownCloud. Taken zijn rechtstreeks toegankelijk via CalDAV.

::: tip
Gebruik uw **Nextcloud** / **ownCloud** **synchronisatieclient** om **uw notities te synchroniseren**, uw notities zijn eenvoudige markdown-bestanden. Het wiel hoeft niet twee keer uit te vinden. En er zijn ook mensen die verschillende services gebruiken om hun notitiebestanden te synchroniseren (zoals Dropbox, Syncthing of Resilio Sync).
:::

## Wat QOwnNotesAPI doet

- Toegang tot uw weggegooide notities
- Toegang tot uw notitieversies

## Wat QOwnNotesAPI niet doet

- Uw notities synchroniseren (gebruik uw ownCloud / Nextcloud-synchronisatieclient)
- Notities delen (dat gebeurt rechtstreeks via de ownCloud / Nextcloud API)
- Toegang tot afbeeldingen van [Nextcloud-tekst](https://github.com/nextcloud/text) (dat gebeurt rechtstreeks via de Nextcloud-API)
