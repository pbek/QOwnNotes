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
3. To use the private mode, you need to allow the extension to run in private windows in the extension settings.

## Web clipper

![web-clipper](/img/web-clipper.png)

Klicken Sie mit der rechten Maustaste auf eine Webseite oder einen ausgewählten Text, um die Funktion **Web-Clipper** zu verwenden. Dort können Sie auch eine neue Notiz mit einem ** Screenshot ** der aktuellen Webseite erstellen.

::: warning
Info If the web clipper doesn't work, please check if the QOwnNotes desktop application is running, and the Web Companion browser extension is configured with the security token.

Also, make sure you accept the consent dialog when you first click the extension icon in the browser toolbar.

If you are using Firefox, you also need to make sure that you have turned on the optional permission `Access your data for all websites` in the _Permission_ extension settings, otherwise you might get an error `TypeError: chrome.tabs.captureVisibleTab is not a function`.
:::

::: tip
Der Web Clipper ist auch skriptfähig! Schauen Sie sich den [websocketRawDataHook](../scripting/hooks.md#websocketrawdatahook) an, wenn Sie steuern möchten, was Sie von Webseiten ausschneiden.
:::

## Lesezeichen

![Lesezeichen](/img/bookmarks.png)

Wenn Sie die volle Kontrolle über Ihre Browser-Lesezeichen haben und **sie in verschiedenen Browsern und Betriebssystemen verwenden möchten**, dann ist die QOwnNotes-Browsererweiterung genau das Richtige für Sie.

By default, the browser extension will show all **links of the current note** in a popup when you click the QOwnNotes icon in your browser. Diese Links erhalten ein `current` Schlagwort.

Sie können **Ihre Lesezeichen auch in Notizen** mit dem Notiz-Schlagwort `bookmarks` verwalten (in den Einstellungen änderbar). Diese Links können auch Schlagworte und eine Beschreibung enthalten, die in der Browsererweiterung angezeigt wird.

Neue Lesezeichen werden in einer Notiz namens `Bookmarks` gespeichert (in den Einstellungen änderbar).

::: tip
Sie können Ihre Browser-Lesezeichen auch mit der Web-Companion-Browsererweiterung in QOwnNotes importieren!
:::

### Syntax von Lesezeichen-Links

```markdown
- [Webpage name](https://www.example.com)
- [Webpage name](https://www.example.com) #tag1 #tag2
- [Webpage name](https://www.example.com) some description only
- [Webpage name](https://www.example.com) #tag1 #tag2 some description and tags

* [Webpage name](https://www.example.com) the alternative list character also works
```

You are able to search for name, url tags or description in the browser extension. In the current note all other links inside of text are parsed, but without tags or description.
