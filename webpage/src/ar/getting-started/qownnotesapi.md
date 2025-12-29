# QOwnNotesAPI Nextcloud App

![qownnotesapi](/img/qownnotesapi.png)

يتيح [**QOwnNotesAPI**](https://github.com/pbek/qownnotesapi) لك **الوصول** إلى **الملاحظات المهملة** و**نسخ الملاحظات**.

::: tip
يمكنك تفعيل QOwnNotesAPI مباشرةً في خادم **Nextcloud** أو **ownCloud** الخاص بك بالبحث عن `qownnotes` في **صفحة التطبيقات (Apps)** على خادمك في `https://<your-cloud-domain>/settings/apps`.

- [صفحة تطبيقات Nextcloud](https://apps.nextcloud.com/apps/qownnotesapi)
- [ownCloud Apps page](https://marketplace.owncloud.com/apps/qownnotesapi)

:::

تتم مشاركة الملاحظات عبر API الذي يوفره Nextcloud / ownCloud. يتم الوصول إلى المهام مباشرةً عبر CalDAV.

::: tip
استخدم **عميل مزامنة Nextcloud أو ownCloud لمزامنة ملاحظاتك**؛ ملاحظاتك ملفات ماركداون بسيطة. لا حاجة لاختراع العجلة مرتين. وأيضا يوجد مَن يستخدمون خدمات أخرى لمزامنة ملفات ملاحظاتهم (مثل Dropbox أو Syncthing أو Resilio Sync).
:::

## ما يفعله QOwnNotesAPI

- يتيح لك الوصول إلى ملاحظاتك المهملة
- يتيح لك الوصول إلى نسخ ملاحظاتك

## ما لا يفعله QOwnNotesAPI

- يزامن ملاحظاتك (برجاء استخدام عميل مزامنة ownCloud / Nextcloud)
- يشارك ملاحظاتك (هذا يتم مباشرةً عبر ownCloud / Nextcloud API)
- يتيح الوصول إلى صورك من [Nextcloud Text](https://github.com/nextcloud/text) (هذا يتم مباشرةً عبر Nextcloud API)
