---
image: /img/bookmarks.png
---

# QOwnNotes Web Companion browser extension

Allows for clipping from a browser page and managing browser bookmarks across browsers and operating systems.

::: tip Info
**QOwnNotes must be running** for the Web Companion browser extension to work.
But no internet connection is needed, the browser extensions works **offline**.
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
3. To use the private mode, you need to allow the extension to run in private windows in the extension settings.

## Web clipper

![web-clipper](/img/web-clipper.png)

Right-click on a webpage or selected text to use the **web-clipper** functionality. There you can also create a new note with a **screenshot** of the current webpage.

::: warning Info
If the web clipper doesn't work, please check if the QOwnNotes desktop application is
running, and the Web Companion browser extension is configured with the security token.

Also, make sure you accept the consent dialog when you first click the extension
icon in the browser toolbar.

If you are using Firefox, you also need to make sure that you have turned on the optional
permission `Access your data for all websites` in the _Permission_ extension settings,
otherwise you might get an error `TypeError: chrome.tabs.captureVisibleTab is not a function`.
:::

::: tip
The web clipper is also scriptable! Take a look at the [websocketRawDataHook](../scripting/hooks.md#websocketrawdatahook) if you want to control what you clip from webpages.
:::

## Bookmarks

![bookmarks](/img/bookmarks.png)

If you want full control over your browser bookmarks and **use them across different browsers and operating systems** then the QOwnNotes browser extension is for you.

By default, the browser extension will show all **links of the current note** in a popup when you click the QOwnNotes icon in your browser. These links will get a tag `current`.

You can also **manage your bookmarks in notes** with the note tag `bookmarks` (changeable in the settings). These links can also have tags and a description that will be shown in the browser extension.

New bookmarks are stored in a note called `Bookmarks` (also changeable in the settings).

::: tip
You can also import your browser bookmarks into QOwnNotes with the web companion browser extension!
:::

### Syntax of bookmark links

```markdown
- [Webpage name](https://www.example.com)
- [Webpage name](https://www.example.com) #tag1 #tag2
- [Webpage name](https://www.example.com) some description only
- [Webpage name](https://www.example.com) #tag1 #tag2 some description and tags

* [Webpage name](https://www.example.com) the alternative list character also works
```

You are able to search for name, url tags or description in the browser extension.
In the current note all other links inside of text are parsed, but without tags or description.
