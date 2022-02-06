# Als AppImage installieren

Sie können das neueste AppImage von der Seite [QOwnNotes-Versionen](https://github.com/pbek/QOwnNotes/releases) herunterladen. Es sollte auf dieser Seite in etwa `QOwnNotes-x86_64.AppImage` heißen.

::: tip
Wenn Sie [jq](https://stedolan.github.io/jq/) installiert haben, können Sie auch das neueste AppImage direkt herunterladen:

```bash
# die neueste Linux-Version von der QOwnNotes-API abfragen, den JSON nach der URL analysieren und herunterladen
curl -L https://api.qownnotes.org/latest_releases/linux | jq .url | xargs curl -Lo QOwnNotes-x86_64.AppImage
```
:::

Dann können Sie die Ausführungsberechtigungen für die Datei ändern:

```bash
chmod a+x QOwnNotes-*.AppImage
```

Anschließend sollten Sie AppImage ausführen können, um QOwnNotes zu starten.

::: warning
Wenn Sie den **automatischen Updater** verwenden möchten, stellen Sie bitte sicher, dass Sie Ihr AppImage an einem Ort ablegen, auf den Ihr Benutzerkonto Schreibzugriff hat, z. B. irgendwo in Ihrem Home-Verzeichnis.
:::

::: tip
Ha problémái vannak az AppImage futtatásával, mert a glibc verziója túl régi, próbálkozzon az [OBS-re épített AppImage](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/AppImage/QOwnNotes-latest-x86_64.AppImage) programmal, amelyet a glibc 2.16-os verziójával kell elkészíteni.
:::
