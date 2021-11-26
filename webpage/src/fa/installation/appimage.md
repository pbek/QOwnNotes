# نصب به صورت AppImage

شما می توانید آخرین نسخه AppImage را از [صفحه انتشار های QOwnNotes](https://github.com/pbek/QOwnNotes/releases) بارگیری کنید. آن باید در صفحه مورد نظر به شکلی نظیر `QOwnNotes-x86_64.AppImage` نامگذاری شود.

::: tip
If you have [jq](https://stedolan.github.io/jq/) installed you can also download the latest AppImage directly:

```bash
# query the latest Linux release from the QOwnNotes API, parse the JSON for the URL and download it
curl -L https://api.qownnotes.org/latest_releases/linux | jq .url | xargs curl -Lo QOwnNotes-x86_64.AppImage
```
:::

Then you can change the execute-permissions on the file:

```bash
chmod a+x QOwnNotes-*.AppImage
```

Afterwards you should be able to execute the AppImage to run QOwnNotes.

::: warning
If you want to use the **automatic updater** please make sure to put your AppImage in a place where your user account has write-access to, like somewhere in your home directory.
:::
