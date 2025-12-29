# App QOwnNotesAPI Nextcloud

![qownnotesapi](/img/qownnotesapi.png)

[**QOwnNotesAPI**](https://github.com/pbek/qownnotesapi) peut gérer l'**accès** aux **notes mises à la corbeille** et aux **notes de versions** pour vous.

::: tip
Vous pouvez activer QOwnNotesAPI directement dans votre instance **Nextcloud** ou **ownCloud** en recherchant `qownnotes` sur la page **Apps** de votre serveur à l'adresse `https://<your-cloud-domain>/settings/apps`.

- [Page des Apps de Nextcloud](https://apps.nextcloud.com/apps/qownnotesapi)
- [ownCloud Apps page](https://marketplace.owncloud.com/apps/qownnotesapi)

:::

Le partage des notes se fait via l'API fournie par Nextcloud / ownCloud. L'accès aux tâches se fait directement via CalDAV.

::: tip
Utilisez votre client de **synchronisation** **Nextcloud** / **ownCloud** pour **synchroniser vos notes** qui sont de simples fichiers Markdown. Il n'est pas nécessaire de réinventer la roue. Et il y a aussi des personnes qui utilisent différents services pour synchroniser leurs fichiers de notes (comme Dropbox, Syncthing ou Resilio Sync).
:::

## Ce que fait QOwnNotesAPI

- Accéder à vos notes dans la corbeille
- Accéder à vos versions de notes

## Ce que QOwnNotesAPI ne fait pas

- Synchroniser vos notes (veuillez utiliser votre client de synchronisation ownCloud / Nextcloud)
- Partager des notes (ceci se fait directement via l'API ownCloud / Nextcloud)
- Accéder à des images depuis [Nextcloud Text](https://github.com/nextcloud/text) (cela se fait directement via l'API Nextcloud)
