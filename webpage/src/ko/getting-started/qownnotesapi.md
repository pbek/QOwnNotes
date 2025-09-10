# QOwnNotesAPI Nextcloud 앱

![qownnotesapi](/img/qownnotesapi.png)

[**QOwnNotesAPI**](https://github.com/pbek/qownnotesapi)은 **휴지통에 버린 노트** 및 **노트 버전**에 대한 액세스를 처리할 수 있습니다.

::: 팁 서버의 **앱 페이지** `https://<your-cloud-domain>/settings/apps`에서 `qownnotes`를 검색하여 **Nextcloud** 또는 **ownCloud** 인스턴스에서 직접 QOwnNotesAPI를 활성화할 수 있습니다.

- [Nextcloud 앱 페이지](https://apps.nextcloud.com/apps/qownnotesapi)
- [ownCloud 앱 페이지](https://marketplace.owncloud.com/apps/qownnotesapi)

노트 공유는 Nextcloud/ownCloud에서 제공하는 API를 통해 이루어집니다. 작업은 CalDAV를 통해 직접 액세스할 수 있습니다.

::: 팁 **Nextcloud** / **ownCloud** **sync client**를 사용하여 **노트를 동기화**합니다. 노트는 단순한 마크다운 파일입니다. 바퀴를 두 번 발명할 필요가 없습니다. 또한 다른 서비스를 사용하여 노트 파일 (예: Dropbox, Syncing 또는 Resilio Sync)을 동기화하는 사람들도 있습니다. :::

## QOwnNotesAPI의 기능

- 휴지통에 버린 노트 액세스
- 노트 버전 액세스

## QOwnNotesAPI가 하지 않는 일

- 노트 동기화 (ownCloud / Nextcloud 클라이언트를 사용하십시오)
- 노트 공유 (ownCloud / Nextcloud API)를 통해 직접 수행)
- [Nextcloud Text](https://github.com/nextcloud/text)에서 이미지 액세스 (Nextcloud API를 통해 직접 수행)
