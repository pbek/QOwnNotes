---
image: /img/bookmarks.png
---

# QOwnNotes Web Companion-Browsererweiterung

![Lesezeichen](/img/bookmarks.png)

Besuchen Sie den [Chrome Web Store](https://chrome.google.com/webstore/detail/qownnotes-web-companion/pkgkfnampapjbopomdpnkckbjdnpkbkp) oder die [Firefox-Add-On-Seite](https://addons.mozilla.org/firefox/addon/qownnotes-web-companion), um die Browsererweiterung [**QOwnNotes Web Companion**](https://github.com/qownnotes/web-companion/) zu installieren.

Sie finden die Erweiterung auch unter [GitHub](https://github.com/qownnotes/web-companion/).

## Web clipper

Klicken Sie mit der rechten Maustaste auf eine Webseite oder einen ausgewählten Text, um die Funktion **Web-Clipper** zu verwenden. There you can also create a new note with a **screenshot** of the current webpage.

::: tip
Der Web Clipper ist auch skriptfähig! Schauen Sie sich den [websocketRawDataHook](../scripting/hooks.md#websocketrawdatahook) an, wenn Sie steuern möchten, was Sie von Webseiten ausschneiden.
:::

## Lesezeichen

Standardmäßig zeigt die Browsererweiterung alle **Links der aktuellen Notiz** in einem Popup an, wenn Sie in Ihrem Browser auf das QOwnNotes-Symbol klicken. Diese Links erhalten einen `current` Tag.

Sie können **Ihre Lesezeichen auch in Notizen** mit dem Notiz-Tag `Lesezeichen` verwalten (in den Einstellungen änderbar). Diese Links können auch Tags und eine Beschreibung enthalten, die in der Browsererweiterung angezeigt wird.

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
