# QOwnNotesAPI Nextcloud App

![qownnotesapi](/img/qownnotesapi.png)

[**QOwnNotesAPI**](https://github.com/pbek/qownnotesapi) może obsłużyć dla Ciebie **dostęp** do **notatek w koszu** i **wersji notatek**.

::: tip
Możesz włączyć QOwnNotesAPI bezpośrednio w instancji **Nextcloud** lub **ownCloud**, wyszukując `qownnotes` na **stronie Aplikacje** Twojego serwera pod adresem `https://<your-cloud-domain>/settings/apps`.

- [Strona aplikacji NextCloud](https://apps.nextcloud.com/apps/qownnotesapi)
- [strona ownCloud Apps](https://marketplace.owncloud.com/apps/qownnotesapi)
:::

Udostępnianie notatek odbywa się za pośrednictwem interfejsu API dostarczanego przez Nextcloud / ownCloud. Dostęp do zadań można uzyskać bezpośrednio przez CalDAV.

::: tip
Użyj swojego **Nextcloud** / **ownCloud** **klienta synchronizacji** do **synchronizacji notatek**, Twoje notatki są prostymi plikami przecen. Nie ma potrzeby dwukrotnego wymyślania koła. Są też ludzie, którzy używają różnych usług do synchronizacji ich pliki notatek (takie jak Dropbox, Syncthing lub Resilio Sync).
:::

## Co robi QOwnNotesAPI

- Uzyskiwanie dostępu do usuniętych notatek
- Uzyskiwanie dostępu do wersji notatek

## Czego nie robi QOwnNotesAPI

- Synchronizowanie notatek (użyj własnego klienta synchronizacji Cloud / Nextcloud)
- Udostępnianie notatek (odbywa się to bezpośrednio przez ownCloud / Nextcloud API)
- Uzyskiwanie dostępu do obrazów z [Nextcloud Text](https://github.com/nextcloud/text) (odbywa się to bezpośrednio przez API Nextcloud)
