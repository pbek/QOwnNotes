# QOwnNotesAPI Nextcloud 앱


![qownnotesapi](/img/qownnotesapi.png)

[**QOwnNotesAPI**](https://github.com/pbek/qownnotesapi) can handle the **access** to **trashed notes** and **note versions** for you.

::: tip You can enable QOwnNotesAPI directly in your **Nextcloud** or **ownCloud** instance by searching for `qownnotes` in your server's **Apps page** at `https://<your-cloud-domain>/settings/apps`.

- [Nextcloud 앱 페이지](https://apps.nextcloud.com/apps/qownnotesapi)
- [ownCloud 앱 페이지](https://marketplace.owncloud.com/apps/qownnotesapi)

노트 공유는 Nextcloud/ownCloud에서 제공하는 API를 통해 이루어집니다. 작업은 CalDAV를 통해 직접 액세스할 수 있습니다.

::: tip Use your **Nextcloud** / **ownCloud** **sync client** to **sync your notes**, your notes are simple markdown files. There is no need to invent the wheel twice. And there are also people who are using different services to sync their note files (like Dropbox, Syncthing or Resilio Sync). :::

## What QOwnNotesAPI does

- Accessing your trashed notes
- Accessing your note versions

## What QOwnNotesAPI doesn't do

- Syncing your notes (please use your ownCloud / Nextcloud sync client)
- Sharing notes (that is done directly via the ownCloud / Nextcloud API)
- Accessing images from [Nextcloud Text](https://github.com/nextcloud/text) (that is done directly via the Nextcloud API)
