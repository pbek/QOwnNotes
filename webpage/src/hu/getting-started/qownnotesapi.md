# QOwnNotesAPI Nextcloud App

![qownnotesapi](/img/qownnotesapi.png)

A [**QOwnNotesAPI**](https://github.com/pbek/qownnotesapi) képes kezelni az **hozzáférést** az **kukába helyezett jegyzetekhez** és az **jegyzetek verzióihoz**.

::: tip
A QOwnNotesAPI-t közvetlenül engedélyezheti a **Nextcloud** vagy a **ownCloud** példányban, ha a szerver **Apps page** `https://<your-cloud-domain>/settings/apps`.

- [Nextcloud Apps page](https://apps.nextcloud.com/apps/qownnotesapi)
- [ownCloud Apps page](https://marketplace.owncloud.com/apps/qownnotesapi)
:::

A jegyzetmegosztás a Nextcloud / ownCloud által biztosított API-n keresztül történik. A feladatokhoz közvetlenül a CalDAV-on keresztül férhet hozzá.

::: tip
Használja a **Nextcloud** / **ownCloud** **szinkronizáló klienst** a jegyzetek **szinkronizálásához**, a jegyzetek egyszerű jelölőfájlok. Nem szükséges kétszer feltalálni a kereket. És vannak olyan emberek is, akik különböző szolgáltatásokat használnak a jegyzetfájlok szinkronizálására (például Dropbox, Syncthing vagy Resilio Sync).
:::

## Mit tesz a QOwnNotesAPI

- Hozzáférés a kukába helyezett jegyzetekhez
- Hozzáférés a jegyzet verzióihoz

## Amit a QOwnNotesAPI nem tesz

- A jegyzetek szinkronizálása (kérjük, használja a sajátCloud / Nextcloud szinkronizáló klienst)
- Jegyzetek megosztása (ez közvetlenül a ownCloud / Nextcloud API-n keresztül történik)
- Képek elérése a [Nextcloud Text](https://github.com/nextcloud/text) ből (ez közvetlenül a Nextcloud API-n keresztül történik)
