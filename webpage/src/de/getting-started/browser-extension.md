---
image: /img/bookmarks.png
---

# QOwnNotes Web Companion-Browsererweiterung

Ermöglicht das Ausschneiden von einer Browserseite und das Verwalten von Browser-Lesezeichen über Browser und Betriebssysteme hinweg.

::: tip
Info
- QOwnNotes muss ausgeführt werden, damit die Web Companion-Browsererweiterung funktioniert.
- Es ist keine Internetverbindung erforderlich. Die Browsererweiterungen funktionieren **offline**.
:::

## Installation

1. Holen Sie sich die Erweiterung
    - [Chrome-Webstore](https://chrome.google.com/webstore/detail/qownnotes-web-companion/pkgkfnampapjbopomdpnkckbjdnpkbkp)
    - [Firefox-Add-ons-Seite](https://addons.mozilla.org/firefox/addon/qownnotes-web-companion)
    - Sie finden die Erweiterung auch auf [GitHub](https://github.com/qownnotes/web-companion/).
2. Fügen Sie das Sicherheitstoken hinzu, um die Erweiterung zu konfigurieren.
    - Wenn Sie zum ersten Mal auf das Symbol der QOwnNotes-Browsererweiterung klicken, erhalten Sie ein Dialogfeld mit einem Sicherheitstoken. Kopieren Sie das Token.
    - Gehen Sie zum Speicherort der Erweiterungsverwaltung Ihres Browsers. Klicken Sie auf die Details der QOwnNotes-Erweiterung.
    - Fügen Sie das Token in das Feld Sicherheitstoken ein.

## Web clipper

![web-clipper](/img/web-clipper.png)

Klicken Sie mit der rechten Maustaste auf eine Webseite oder einen ausgewählten Text, um die Funktion **Web-Clipper** zu verwenden. Dort können Sie auch eine neue Notiz mit einem ** Screenshot ** der aktuellen Webseite erstellen.

::: tip
Der Web Clipper ist auch skriptfähig! Schauen Sie sich den [websocketRawDataHook](../scripting/hooks.md#websocketrawdatahook) an, wenn Sie steuern möchten, was Sie von Webseiten ausschneiden.
:::

## Lesezeichen

![Lesezeichen](/img/bookmarks.png)

Wenn Sie die volle Kontrolle über Ihre Browser-Lesezeichen haben und **sie in verschiedenen Browsern und Betriebssystemen verwenden möchten**, dann ist die QOwnNotes-Browsererweiterung genau das Richtige für Sie.

Standardmäßig zeigt die Browsererweiterung alle **Links der aktuellen Notiz** in einem Popup an, wenn Sie in Ihrem Browser auf das QOwnNotes-Symbol klicken. Diese Links erhalten ein `current` Schlagwort.

Sie können **Ihre Lesezeichen auch in Notizen** mit dem Notiz-Schlagwort `bookmarks` verwalten (in den Einstellungen änderbar). Diese Links können auch Schlagworte und eine Beschreibung enthalten, die in der Browsererweiterung angezeigt wird.

Neue Lesezeichen werden in einer Notiz namens `Bookmarks` gespeichert (in den Einstellungen änderbar).

::: tip
Sie können Ihre Browser-Lesezeichen auch mit der Web-Companion-Browsererweiterung in QOwnNotes importieren!
:::

### Syntax von Lesezeichen-Links

```markdown
- [Name der Webseite] (https://www.example.com)
- [Name der Webseite] (https://www.example.com) #tag1 #tag2
- [Name der Webseite] (https://www.example.com) nur eine Beschreibung
- [Name der Webseite] (https://www.example.com) #tag1 #tag2 einige Beschreibungen und Tags
* [Name der Webseite] (https://www.example.com) das alternative Listenzeichen funktioniert ebenfalls
```

Sie können in der Browsererweiterung nach Namen, URL-Tags oder Beschreibungen suchen.
