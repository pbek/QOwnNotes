# [QOwnNotes](https://www.qownnotes.org "QOwnNotes Official Site")

[Installation](https://www.qownnotes.org/installation) |
[Changelog](https://github.com/pbek/QOwnNotes/blob/develop/CHANGELOG.md) |
[Issues](https://github.com/pbek/QOwnNotes/issues) |
[Shortcuts](https://github.com/pbek/QOwnNotes/blob/develop/docs/content/getting-started/shortcuts.md) |
[Documentation](http://docs.qownnotes.org) |
[Knowledge base](https://www.qownnotes.org/Knowledge-base) |
[Screenshots](https://www.qownnotes.org) |
[ownCloud API](https://marketplace.owncloud.com/apps/qownnotesapi) |
[Nextcloud API](https://apps.nextcloud.com/apps/qownnotesapi) |
[Nextcloud App Page](https://apps.nextcloud.com/apps/qownnotes) |
[Telegram Group](https://t.me/QOwnNotes) |
[IRC Chat](https://kiwiirc.com/client/irc.freenode.net/#qownnotes) |
[Gitter Chat](https://gitter.im/qownnotes/qownnotes) |
[QOwnNotes Web Companion Chrome extension](https://chrome.google.com/webstore/detail/qownnotes-web-companion/pkgkfnampapjbopomdpnkckbjdnpkbkp)

[![Build Status GitHub Actions](https://github.com/pbek/QOwnNotes/workflows/build-test/badge.svg?branch=develop)](https://github.com/pbek/QOwnNotes/actions)
[![Build Status Windows](https://ci.appveyor.com/api/projects/status/github/pbek/QOwnNotes)](https://ci.appveyor.com/project/pbek/qownnotes)
[![Snap Status](https://build.snapcraft.io/badge/pbek/QOwnNotes.svg)](https://build.snapcraft.io/user/pbek/QOwnNotes)
[![Coverage Status](https://coveralls.io/repos/pbek/QOwnNotes/badge.svg?branch=develop&service=github)](https://coveralls.io/github/pbek/QOwnNotes?branch=develop)
[![Percentage of issues still open](http://isitmaintained.com/badge/open/pbek/QOwnNotes.svg)](http://isitmaintained.com/project/pbek/QOwnNotes "Percentage of issues still open")
[![Crowdin](https://d322cqt584bo4o.cloudfront.net/qownnotes/localized.svg)](https://crowdin.com/project/qownnotes)
[![Gitter chat](https://badges.gitter.im/gitterHQ/gitter.png)](https://gitter.im/qownnotes/qownnotes)
[![Documentation](https://readthedocs.org/projects/qownnotes/badge/?version=develop)](http://docs.qownnotes.org)

[QOwnNotes](https://www.qownnotes.org) is the **open source notepad** with
[**markdown support**](https://github.com/pbek/QOwnNotes/blob/develop/src/demonotes/Markdown%20Cheatsheet.md)
and **todo list manager** for **GNU/Linux**, **Mac OS X** and **Windows**,
that works together with [**Nextcloud Notes**](https://github.com/Nextcloud/notes)
and [**ownCloud Notes**](https://github.com/owncloud/notes).

You are able to **write down** your **thoughts** with **QOwnNotes** and **edit or search** for them later from your **mobile device**, like with [CloudNotes](http://peterandlinda.com/cloudnotes/) or the **ownCloud / Nextcloud web-service**.

The notes are stored as **plain text markdown files** and are **synced with Nextcloud's/ownCloud's file sync** functionality.
Of course other software, like [Syncthing](https://syncthing.net) or [Dropbox](https://www.dropbox.com) can be used too.

If you like the concept of having notes accessible in plain text files, like it is done in the Nextcloud / ownCloud notes apps to gain a maximum of **freedom** then QOwnNotes is for you.

Visit the project page here: [QOwnNotes project page](https://www.qownnotes.org)

To manage your **todo lists** in the web and on your mobile devices, you need to
install the Tasks backend on [Nextcloud](https://apps.nextcloud.com/apps/tasks)
or [ownCloud](https://marketplace.owncloud.com/apps/tasks).

To get more on-line features for your notes, like **versioning** and **trash** access, you might also want to install the [QOwnNotesAPI](https://github.com/pbek/qownnotesapi) app on your server.

To access your ownCloud notes from your **mobile device** you may want to get these:
- For Android and OwnCloud: [MyOwnNotes](https://f-droid.org/app/org.aykit.MyOwnNotes)
- For Android and Nextcloud: [Notes](https://f-droid.org/packages/it.niedermann.owncloud.notes)
- For iOS: [CloudNotes](https://itunes.apple.com/app/cloudnotes-owncloud-notes/id813973264)

On Android you could also use any sync-tool like *Synchronize Ultimate* or *FolderSync* to sync your note files and use software like *neutriNotes* or [**Markor**](https://f-droid.org/packages/net.gsantner.markor/) to edit your notes.

On iOS [Notebooks](https://itunes.apple.com/us/app/notebooks-write-and-organize/id780438662) may also work well (syncing notes via WebDAV).

## Screenshot

![Screenhot](screenshots/screenshot.png)

You can visit the [QOwnNotes project page](https://www.qownnotes.org) for more **screenshots**.

## Features

- **multiple note folders** can be used
- you can **use your existing text or markdown files**, no need for an import most of the times
- older **versions of your notes** can be restored from your Nextcloud / ownCloud server (install [QOwnNotesAPI](https://github.com/pbek/qownnotesapi) on your server)
- **trashed notes** can be restored from your Nextcloud / ownCloud server (install [QOwnNotesAPI](https://github.com/pbek/qownnotesapi) on your server)
    - there also is a local trash
- sub-string searching of notes is possible and search results are highlighted in the notes
- application can be operated with **customizable keyboard shortcuts**
- external changes of note files are watched (notes or note list are reloaded)
- differences between current note and externally changed note are shown in a dialog
- **markdown highlighting** of notes and a markdown **preview**
- **[spellchecking](https://docs.qownnotes.org/en/develop/spellchecking.html)** support
- **tabbing support** for editing notes
- scripting support and an online [script repository](https://github.com/qownnotes/scripts)
  where you can install scripts inside the application
- [QOwnNotes Web Companion browser extension](https://github.com/qownnotes/web-companion)
  to a add notes from the selected text and other features
	- visit the [Chrome Web Store](https://chrome.google.com/webstore/detail/qownnotes-web-companion/pkgkfnampapjbopomdpnkckbjdnpkbkp)
	  page to install the app on Google Chrome
	- visit the [Firefox Add-ons page](https://addons.mozilla.org/firefox/addon/qownnotes-web-companion)
	  page to install the app on Mozilla Firefox
- notes are getting their name from the first line of the note text (just like in the Nextcloud / ownCloud notes web-application) and the note text files are automatically renamed, if the the first line changes
    - this feature can also be turned off and you can use any filename you like
- manage your Nextcloud / ownCloud todo lists (ownCloud tasks or Tasks Plus / Calendar Plus)
- **encryption of notes** (AES-256 is built in or you can use custom encryption methods like
  **[Keybase.io](https://keybase.io)** ([encryption-keybase.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/content/scripting/examples/encryption-keybase.qml)) or
  **PGP** ([encryption-pgp.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/content/scripting/examples/encryption-pgp.qml)))
- **dark mode** theme support
- all **panels can be placed wherever you want**, they can even float or stack (fully dockable)
- **toolbars** are **fully customizable**
- support for **freedesktop theme icons**, so you can use QOwnNotes with your
  native desktop icons and with your favorite dark desktop theme
  QOwnNotes supports Freedesktop icon themes
- support for hierarchical **note tagging** and **note subfolders**
- support for **sharing notes** on your Nextcloud / ownCloud server
- **portable mode** for carrying QOwnNotes around on USB sticks
- **Vim mode**
- **distraction free mode**, **full-screen mode**, **typewriter mode**
- Evernote import
- QOwnNotes is **available in many different languages** like English, German, French, Polish, Chinese, Japanese, Russian, Portuguese, Hungarian, Dutch and Spanish
	- [Your help](https://www.qownnotes.org/Knowledge-base/How-can-I-help-to-translate-QOwnNotes) is very much appreciated to improve these translations or to translate QOwnNotes in more languages
	- Join the fun at **[Crowdin](https://crowdin.com/project/qownnotes/invite)** to **help** with the **translations**

## Building QOwnNotes

To get the most current features you can build the application from the source
code. Download the latest source here:
[QOwnNotes Source on GitHub as ZIP](https://github.com/pbek/QOwnNotes/archive/develop.zip)

Alternatively you can also checkout the code directly from the git repository:

```shell
git clone https://github.com/pbek/QOwnNotes.git -b master
cd QOwnNotes
git submodule update --init
```

Then download [Qt Creator](http://www.qt.io/download-open-source), open the
project file `src/QOwnNotes.pro` and click on *Build / Run*.

Or you can build it directly in your terminal:

```shell
cd src

# build binary translation files if you want another language than English
lrelease QOwnNotes.pro

qmake
make
```

Please feel free to contribute source code to this project, make suggestions or report troubles on the [QOwnNotes issues page](https://github.com/pbek/QOwnNotes/issues)!

You can also visit [QOwnNotes on GitHub](https://github.com/pbek/QOwnNotes).

## Minimum software requirements

- A desktop operating system, that supports [Qt](http://www.qt.io/)
- Qt 5.3+
- gcc 4.8+

## Disclaimer

This SOFTWARE PRODUCT is provided by THE PROVIDER "as is" and "with all faults." THE PROVIDER makes no representations or warranties of any kind concerning the safety, suitability, lack of viruses, inaccuracies, typographical errors, or other harmful components of this SOFTWARE PRODUCT.

There are inherent dangers in the use of any software, and you are solely responsible for determining whether this SOFTWARE PRODUCT is compatible with your equipment and other software installed on your equipment. You are also solely responsible for the protection of your equipment and backup of your data, and THE PROVIDER will not be liable for any damages you may suffer in connection with using, modifying, or distributing this SOFTWARE PRODUCT.

[![Matomo Stats](https://p.bekerle.com/piwik.php?idsite=3&rec=1)](https://www.qownnotes.org)
