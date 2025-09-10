---
title: Firefox extension on Firefox Add-ons page
date: 2019-01-09T05:50:11+00:00
order: 20190109055011
---

# Firefox extension on Firefox Add-ons page

<v-subheader class="blog">Date: 2019-01-09</v-subheader>

The [QOwnNotes Web Companion browser extension](https://github.com/qownnotes/web-companion) is now available on the **[Firefox Add-ons page](https://addons.mozilla.org/firefox/addon/qownnotes-web-companion/)**.

It was completely rewritten with generator-web-extension, webextension-toolbox, webpack and VueJS.

### Current features

- creating a new note (only text) from the **current selection** by right-clicking it
- creating a new note from the content of the **current webpage** by right-clicking on the page
  - the html will be converted to markdown and images of the page will be downloaded (might take a while)
- creating a new note with a **screenshot** of the visible part of the current webpage by right-clicking on the page
- there is a browser option page to set and reset the server socket port
