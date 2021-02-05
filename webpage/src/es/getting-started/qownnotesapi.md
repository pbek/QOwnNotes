# QOwnNotesAPI Nextcloud App


![qownnotesapi](/img/qownnotesapi.png)

[**QOwnNotesAPI**](https://github.com/pbek/qownnotesapi) puede manejar el **acceso** a **notas en la papelera** y **versiones de notas** por usted.

::: tip
Puede habilitar QOwnNotesAPI directamente en su instancia de **Nextcloud** o **ownCloud** buscando `qownnotes` en la **página de aplicaciones** de su servidor en `https://<your-cloud-domain>/settings/apps`.

- [Página de aplicaciones de Nextcloud](https://apps.nextcloud.com/apps/qownnotesapi)
- [página de aplicaciones ownCloud](https://marketplace.owncloud.com/apps/qownnotesapi)
:::

El intercambio de notas se realiza a través de la API proporcionada por Nextcloud / ownCloud. Se accede a las tareas directamente a través de CalDAV.

::: tip
Utilice su **Nextcloud** / **ownCloud** **cliente de sincronización** para **sincronizar sus notas**, sus notas son simples archivos de rebajas. No es necesario inventar la rueda dos veces. Y también hay personas que utilizan diferentes servicios para sincronizar sus archivos de notas (como Dropbox, Syncthing o Resilio Sync).
:::

## Qué hace QOwnNotesAPI

- Accediendo a sus notas en la papelera
- Acceder a las versiones de sus notas

## Lo que QOwnNotesAPI no hace

- Sincronizar sus notas (utilice su cliente de sincronización ownCloud / Nextcloud)
- Compartir notas (que se hace directamente a través de la API ownCloud / Nextcloud)
- Acceder a imágenes desde [Nextcloud Text](https://github.com/nextcloud/text) (que se realiza directamente a través de la API de Nextcloud)
