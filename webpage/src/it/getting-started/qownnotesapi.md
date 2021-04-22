# QOwnNotesAPI Nextcloud App


![qownnotesapi](/img/qownnotesapi.png)

[**QOwnNotesAPI**](https://github.com/pbek/qownnotesapi) può gestire l '**accesso** alle **note spostate nel cestino** e alle <1versioni delle note</strong> per te.

::: tip
Puoi abilitare QOwnNotesAPI direttamente nella tua istanza **Nextcloud** o **ownCloud** cercando `qownnotes` nella **pagina delle app** del tuo server su `https://<your-cloud-domain>/settings/apps`.

- [Nextcloud Apps page](https://apps.nextcloud.com/apps/qownnotesapi)
- [ownCloud Apps page](https://marketplace.owncloud.com/apps/qownnotesapi)
:::

La condivisione delle note viene eseguita tramite l'API fornita da Nextcloud / ownCloud. Si accede alle attività direttamente tramite CalDAV.

Usa il tuo **Nextcloud** / **ownCloud** **client di sincronizzazione** per **sincronizzare le tue note**, le tue note sono semplici file markdown. Non è necessario inventare due volte la ruota. E ci sono anche persone che utilizzano diversi servizi per sincronizzare i propri file di note (come Dropbox, Syncthing o Resilio Sync).
:::

## Cosa fa QOwnNotesAPI

- Accesso alle note spostate nel cestino
- Accesso alle versioni delle note

## Cosa QOwnNotesAPI non fa

- Sincronizzazione delle note (si prega di utilizzare il proprio client di sincronizzazione Cloud / Nextcloud)
- Condivisione delle note (che viene eseguita direttamente tramite l'API ownCloud / Nextcloud)
- Accesso alle immagini da [Nextcloud Text](https://github.com/nextcloud/text) (che viene fatto direttamente tramite l'API Nextcloud)
