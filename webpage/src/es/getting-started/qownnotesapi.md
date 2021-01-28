# QOwnNotesAPI Nextcloud App


![qownnotesapi](/img/qownnotesapi.png)

[**QOwnNotesAPI**](https://github.com/pbek/qownnotesapi) can handle the **access** to **trashed notes** and **note versions** for you.

::: tip
You can enable QOwnNotesAPI directly in your **Nextcloud** or **ownCloud** instance by searching for `qownnotes` in your server's **Apps page** at `https://<your-cloud-domain>/settings/apps`.

- [Nextcloud Apps page](https://apps.nextcloud.com/apps/qownnotesapi)
- [ownCloud Apps page](https://marketplace.owncloud.com/apps/qownnotesapi)
:::

El intercambio de notas se realiza a través de la API proporcionada por Nextcloud / ownCloud. Se accede a las tareas directamente a través de CalDAV.

::: tip
Use your **Nextcloud** / **ownCloud** **sync client** to **sync your notes**, your notes are simple markdown files. No es necesario inventar la rueda dos veces. Y también hay personas que utilizan diferentes servicios para sincronizar sus archivos de notas (como Dropbox, Syncthing o Resilio Sync).
:::

## What QOwnNotesAPI does

- Accediendo a sus notas en la papelera
- Acceder a las versiones de sus notas

## Lo que QOwnNotesAPI no hace

- Sincronizar sus notas (utilice su cliente de sincronización ownCloud / Nextcloud)
- Compartir notas (que se hace directamente a través de la API ownCloud / Nextcloud)
- Acceder a imágenes desde [Nextcloud Text](https://github.com/nextcloud/text) (que se realiza directamente a través de la API de Nextcloud)
