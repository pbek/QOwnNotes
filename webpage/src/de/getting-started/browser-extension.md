---
image: /img/bookmarks.png
---

# QOwnNotes Web Companion-Browsererweiterung

Allows for clipping from a browser page and managing browser bookmarks across browsers and operating systems.

::: tip
Info
- QOwnNotes must be running for the Web Companion browser extension to work.
- No internet connection needed. The browser extensions works **offline**.
:::

## Installation

1. Get the extension
    - [Chrome Web Store](https://chrome.google.com/webstore/detail/qownnotes-web-companion/pkgkfnampapjbopomdpnkckbjdnpkbkp)
    - [Firefox Add-ons page](https://addons.mozilla.org/firefox/addon/qownnotes-web-companion)
    - You can also find the extension on [GitHub](https://github.com/qownnotes/web-companion/).
2. Add the Security Token to configure the extension.
    - The first time you click on the QOwnNotes browser extension icon you will receive a dialog box with a security token. Copy the token.
    - Go to your browser's extension management location. Click into the QOwnNotes extension details.
    - Paste the token into the Security Token field.

## Web clipper

![web-clipper](/img/web-clipper.png)

Klicken Sie mit der rechten Maustaste auf eine Webseite oder einen ausgewählten Text, um die Funktion **Web-Clipper** zu verwenden. Dort können Sie auch eine neue Notiz mit einem ** Screenshot ** der aktuellen Webseite erstellen.

::: tip
Der Web Clipper ist auch skriptfähig! Schauen Sie sich den [websocketRawDataHook](../scripting/hooks.md#websocketrawdatahook) an, wenn Sie steuern möchten, was Sie von Webseiten ausschneiden.
:::

## Lesezeichen

![Lesezeichen](/img/bookmarks.png)

Wenn Sie die volle Kontrolle über Ihre Browser-Lesezeichen haben und **sie in verschiedenen Browsern und Betriebssystemen verwenden möchten**, dann ist die QOwnNotes-Browsererweiterung genau das Richtige für Sie.

Standardmäßig zeigt die Browsererweiterung alle **Links der aktuellen Notiz** in einem Popup an, wenn Sie in Ihrem Browser auf das QOwnNotes-Symbol klicken. Diese Links erhalten einen `current` Tag.

Sie können **Ihre Lesezeichen auch in Notizen** mit dem Notiz-Tag `Lesezeichen` verwalten (in den Einstellungen änderbar). Diese Links können auch Tags und eine Beschreibung enthalten, die in der Browsererweiterung angezeigt wird.

Neue Lesezeichen werden in einer Notiz namens `Lesezeichen` gespeichert (in den Einstellungen änderbar).

::: tip
Sie können Ihre Browser-Lesezeichen auch mit der Web-Companion-Browser-Erweiterung in QOwnNotes importieren!
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
