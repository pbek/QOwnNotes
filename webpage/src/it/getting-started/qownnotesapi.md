# QOwnNotesAPI Nextcloud App

![qownnotesapi](/img/qownnotesapi.png)

[**QOwnNotesAPI**](https://github.com/pbek/qownnotesapi) può gestire l'**accesso** alle **note spostate nel cestino** e alle **versioni delle note** per te.

::: tip
Puoi abilitare QOwnNotesAPI direttamente nella tua istanza **Nextcloud** o **ownCloud** cercando `qownnotes` nella **pagina delle app** del tuo server su `https://<your-cloud-domain>/settings/apps`.

- [Pagina delle App Nextcloud](https://apps.nextcloud.com/apps/qownnotesapi)
- [ownCloud Apps page](https://marketplace.owncloud.com/apps/qownnotesapi)

:::

La condivisione delle note viene eseguita tramite l'API fornita da Nextcloud / ownCloud. Le attività sono accessibili direttamente tramite CalDAV.

Usa il tuo **client di sincronizzazione** **Nextcloud** / **ownCloud** per **sincronizzare le tue note**, che sono semplici file markdown. Non è necessario reinventare la ruota. E ci sono anche persone che usano servizi diversi per sincronizzare i propri file di note (come Dropbox, Syncthing o Resilio Sync).
:::

## Cosa fa QOwnNotesAPI

- Accesso alle note spostate nel cestino
- Accesso alle versioni delle note

## Cosa non fa QOwnNotesAPI

- Sincronizzazione delle note (si prega di usare il proprio client di sincronizzazione ownCloud / Nextcloud)
- Condivisione delle note (che viene eseguita direttamente tramite l'API ownCloud / Nextcloud)
- Accesso alle immagini da [Nextcloud Text](https://github.com/nextcloud/text) (che viene fatto direttamente tramite l'API Nextcloud)
