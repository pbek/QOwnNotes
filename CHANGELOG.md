# QOwnNotes Changelog

## 1.2.3
- added some alternate shortcuts for moving between notes
    - <kbd>Alt</kbd> + <kbd>Up</kbd> jumps to the previous visible note
    - <kbd>Alt</kbd> + <kbd>Down</kbd> jumps to the next visible note
- added more Portuguese translation (thank you mlimacarlos)

## 1.2.2
- fixed a crash that could happen when a recent notes folder was selected
- added shortcuts to quickly increase or decrease the font size of the notes 
  in edit mode
    - <kbd>Ctrl</kbd> + <kbd>+</kbd> increases the note text font size by one point
    - <kbd>Ctrl</kbd> + <kbd>-</kbd> decreases the note text font size by one point
    - <kbd>Ctrl</kbd> + <kbd>/</kbd> resets the note text font size
- added more German, Portuguese, Hungarian and Dutch translation (thank you 
  mlimacarlos and Fabijenna)

## 1.2.1
- client side **note encryption** changed from SimpleCrypt to the much 
  more secure **AES-256** encryption
    - old encrypted notes will be decrypted with SimpleCrypt and encrypted 
      with AES-256 when you edit them
    - some major changes had to be made to make this possible, please report 
      if you run into troubles
- added more German translation (thank you calis2002)

## 1.2.0
- added bold and italic formatting helpers
    - <kbd>Ctrl</kbd> + <kbd>B</kbd> formats the selected text bold 
    - <kbd>Ctrl</kbd> + <kbd>I</kbd> formats the selected text italic 
- when inserting code blocks there will be added a new line on top and at the 
  bottom of the selected text if the selected text has multiple lines
  (for [Issue #113](https://github.com/pbek/QOwnNotes/issues/113))
- added more German, Portuguese, Hungarian and Dutch translation (thank you 
  mlimacarlos and Fabijenna)

## 1.1.9
- fixed a problem with continuous list editing where text from the previous 
  line got copied to the next line
- scroll to the cursor if we are at the bottom of the document when creating 
  a new list item in a note
- if return is pressed in a markdown list and there is just a list symbol then
  the list symbol is now removed
- you can now also search in the current note with <kbd>F3</kbd> and 
  <kbd>⇧</kbd> + <kbd>F3</kbd> even if the search widget doesn't have the focus
- you can now show the current note in your file manager with a new menu item
  (for [Issue #106](https://github.com/pbek/QOwnNotes/issues/106))
    - use <kbd>Alt</kbd> + <kbd>⇧</kbd> + <kbd>F</kbd> as shortcut
    - under OS X the folder of the note is opened in Finder, but the note is 
      not selected
- added more German and Spanish translation (thank you calis2002 and 
  moblesmarti)

## 1.1.8
- added rich text pasting fixes for Internet Explorer
  (for [Issue #112](https://github.com/pbek/QOwnNotes/issues/112))

## 1.1.7
- you can now also drop images from websites directly to a note
  (for [Issue #105](https://github.com/pbek/QOwnNotes/issues/105))
- if there is no plain text to paste with <kbd>Ctrl</kbd> + <kbd>V</kbd> then
  it will be checked if there html, notes or images to paste automatically
  (for [Issue #105](https://github.com/pbek/QOwnNotes/issues/105))
- added more Spanish translation (thank you moblesmarti)
- implemented continuous list editing for unordered markdown lists starting 
  with `-`, `+` or `*`
    - <kbd>Tab</kbd> after a list character indents the list item
    - <kbd>⇧</kbd> + <kbd>Tab</kbd> after a list character un-indents the list item
    - <kbd>Return</kbd> in lists creates a new list item

## 1.1.6
- added **Spanish translation** (a big thank you to moblesmarti!)
    - join us at [QOwnNotes on Crowdin](https://crowdin.com/project/qownnotes/invite)
      to make QOwnNotes available in more languages
- added more Hungarian translation (thank you Fabijenna)
- you can now **paste html** in notes with <kbd>Ctrl</kbd> + <kbd>⇧</kbd> + 
  <kbd>V</kbd> (for [Issue #112](https://github.com/pbek/QOwnNotes/issues/112))
  - html tags like `strong`, `b`, `em`, `i`, `a`, `h1`, `h2`, `h3`, `h4`, 
    `h5`, `br` will be translated to markdown
  - images from `img` tags will be downloaded and inserted as markdown images

## 1.1.5
- added more German, Hungarian and Dutch translation (thank you Fabijenna)
    - join us at [QOwnNotes on Crowdin](https://crowdin.com/project/qownnotes/invite)
      to make QOwnNotes available in more languages
- fixed some typos
- added several fixes for Windows
  (for [Issue #105](https://github.com/pbek/QOwnNotes/issues/105))
    - dragging and dropping of images into notes
    - the image preview in markdown mode
    - opening of links to local files

## 1.1.4
- added support for copying notes from a file manager to QOwnNotes via drag and 
  drop (for [Issue #100](https://github.com/pbek/QOwnNotes/issues/100))
- added support for dragging and dropping images into notes
  (for [Issue #105](https://github.com/pbek/QOwnNotes/issues/105))
- added support for copying and pasting images into notes from a file manager
  (for [Issue #105](https://github.com/pbek/QOwnNotes/issues/105))
    - use <kbd>Ctrl</kbd> + <kbd>⇧</kbd> + <kbd>V</kbd> as shortcut for 
    pasting this way
- added support for copying and pasting images from an image manipulation 
  software into a note
  (for [Issue #105](https://github.com/pbek/QOwnNotes/issues/105))
    - a new png image will be created in the `media` folder of your notes folder 
    - use <kbd>Ctrl</kbd> + <kbd>⇧</kbd> + <kbd>V</kbd> as shortcut for 
    pasting image data
    - please visit [QOwnNotes shortcuts](http://www.qownnotes.org/shortcuts/QOwnNotes)
      for a list of all shortcuts
- added more Hungarian translation (thank you Fabijenna)

## 1.1.3
- added **Hungarian translation** (a big thank you to Fabijenna!)
    - join us at [QOwnNotes on Crowdin](https://crowdin.com/project/qownnotes/)
      to make QOwnNotes available in more languages
- added shortcuts for note printing
  (for [Issue #99](https://github.com/pbek/QOwnNotes/issues/99))
    - <kbd>Ctrl</kbd> + <kbd>P</kbd> prints the current note as text
    - <kbd>Ctrl</kbd> + <kbd>⇧</kbd> + <kbd>P</kbd> prints the current note 
    as markdown
- you now get a feedback while note versions or trashed notes are loaded and 
  the buttons are disabled while you are loading them 
  (for [Issue #97](https://github.com/pbek/QOwnNotes/issues/97))
- added more German translation

## 1.1.2
- added more Dutch translation (thank you Fabijenna)
- fixed the *leave distraction free mode* button icon
- the notes viewed in the distraction free mode now have an adjustable viewport 
  width (for [Issue #86](https://github.com/pbek/QOwnNotes/issues/86))
    - the viewport will resize automatically if the width of the window changes
    - you can select between four width modes in the *Window* menu 
- you can now toggle visibility of the tool bar in the *Windows* menu
  (for [Issue #103](https://github.com/pbek/QOwnNotes/issues/103))
- added more German translation

## 1.1.1
- added **Dutch translation** (a big thank you to Fabijenna!)
- pressing <kbd>Ctrl</kbd> + <kbd>⇧</kbd> + <kbd>F</kbd> to do a note search 
  now leaves the distraction free mode
- the notes in the distraction free mode now have a padding 
  (for [Issue #86](https://github.com/pbek/QOwnNotes/issues/86))
- added more German translation
- there now is a **Gentoo Linux overlay** for QOwnNotes (thanks a lot to 
  @internethering and @suhr for the help)
    - please visit the [QOwnNotes installation page](http://www.qownnotes.org/installation#Gentoo) for instructions

## 1.1
- QOwnNotes now allows the usage of `# ` markdown headlines as note title
  (for [Issue #87](https://github.com/pbek/QOwnNotes/issues/87))
    - the characters will be removed in the note name
    - keep in mind that this behaviour may not be supported by other note 
      clients, that may add the `# ` back again into the file name 
- added more Russian translation (thank you SpacePotato)

## 1.0.9
- improved the in-note search, so that it will not be able to block the note 
  text any more

## 1.0.8
- you can now enter a distraction free mode with a new button in the toolbar
  (for [Issue #86](https://github.com/pbek/QOwnNotes/issues/86))
    - you can also toggle it with <kbd>Ctrl</kbd> + <kbd>⇧</kbd> + <kbd>D</kbd> 

## 1.0.7
- the Chinese language got renamed internally for Crowdin, it's possible that 
  you have to re-select it in the settings if you were using it 
  (for [Issue #95](https://github.com/pbek/QOwnNotes/issues/95))
    - join us on [QOwnNotes on Crowdin](https://crowdin.com/project/qownnotes/)
      to make QOwnNotes available in your language
- **fixed note encryption** that got broken in version 1.0.6
    - if you have encrypted notes in version 1.0.6 you have to first **decrypt 
      them with version 1.0.6** before using them in version 1.0.7! 

## 1.0.6
- you can now enter **network proxy settings** in the new network settings 
  tab to make QOwnNotes work over network proxies
  (for [Issue #79](https://github.com/pbek/QOwnNotes/issues/79))
- added more German translation
- use <kbd>Ctrl</kbd> + <kbd>PgUp</kbd> and <kbd>Ctrl</kbd> + <kbd>PgDn</kbd>
  to **jump between visible notes**
  (for [Issue #80](https://github.com/pbek/QOwnNotes/issues/80))
    - please visit [QOwnNotes shortcuts](http://www.qownnotes.org/shortcuts/QOwnNotes)
      for a list of all shortcuts
    
## 1.0.5
- Google Analytics got removed in favor of Piwik
- code blocks will now wrap around selected text when inserted
  (for [Issue #91](https://github.com/pbek/QOwnNotes/issues/91))
- increased the color contrast in the markdown highlighting for better 
  readability
- added more Russian translation (thank you @elnull)

## 1.0.4
- added a button to insert a markdown code block at the current cursor position
  (for [Issue #82](https://github.com/pbek/QOwnNotes/issues/82))
  - you can use 'Ctrl + Shift + C` as shortcut
- fixed a display problem with the update notification button under Windows
  (for [Issue #90](https://github.com/pbek/QOwnNotes/issues/90))
- added more German translation

## 1.0.3
- added **Portuguese translation** by @mlimacarlos, thank you a lot!
- added more German, French and Russian translation (thank you @Dalan94 and @elnull)
- you can now select an external editor in the settings that will be used
  instead of the default editor when you open a note file with `Open note in 
  external editor`
  (for [Issue #77](https://github.com/pbek/QOwnNotes/issues/77))
- added an option to disabled the app-heartbeat in the settings
  (for [Issue #84](https://github.com/pbek/QOwnNotes/issues/84))

## 1.0.2
- you can now set a note bookmark with `Ctrl + ⇧ + 0..9` on slot 0..9
  - use `Ctrl + 0..9` jump to the bookmark on slot 0..9
- fixed a markdown highlighting problem with certain characters like `»` and `·`
  (for [Issue #73](https://github.com/pbek/QOwnNotes/issues/73))
- added more German and Russian translation (thank you @calis2002 and @elnull)
- you can now disable the automatic update dialog in the settings
    - for self-build versions of QOwnNotes or versions from repositories 
      the automatic update dialog is disabled by default
- in any case you will get a flat button on the bottom right if there is an 
  update available

## 1.0.1
- added more German and Russian translation (thank you, @elnull)
- added a *Get involved* menu entry and a link in the update dialog
- added support for newlines in code blocks in the markdown highlighting
  (for [Issue #71](https://github.com/pbek/QOwnNotes/issues/71))

## 1.0
- yippee, **version 1.0**, so there were already 100 releases!?
    - want to get involved with QOwnNotes? read:
    [Getting involved](http://www.qownnotes.org/Knowledge-base/How-can-I-get-involved-with-QOwnNotes)
- updated German and Russian translation (thanks for your help, @elnull)
- fixed the visual blocking of the note search bar by the scroll bar
- the code font in the note Markdown view can now be configured in the settings
  (for [Issue #59](https://github.com/pbek/QOwnNotes/issues/59))

## 0.99
- implemented plural translations
  (for [Issue #65](https://github.com/pbek/QOwnNotes/issues/65))
- updated German translation
- added missing icons in dialogs
- added plural translations for English and German
- added **Russian translation** (a big thank you to @elnull!)
  (for [Issue #67](https://github.com/pbek/QOwnNotes/issues/67))
- the languages are now selectable in their native language
  (for [Issue #67](https://github.com/pbek/QOwnNotes/issues/67))
- add some links to knowledge base articles in the settings dialog

## 0.98
- made a workaround for the lacking OS X theming support, icons should work 
  again
- the search bar in the text edits now also uses themes

## 0.97
- I adopted the **AUR package** **`qownnotes`**, the repository 
  `qownnotes-pbek` will be discontinued (a big thank you to Midov for 
  managing `qownnotes` until now)
- QOwnNote now uses default **freedesktop theme icons**
  (for [Issue #39](https://github.com/pbek/QOwnNotes/issues/39))
  - if the operating system hasn't a theme set (like for Windows and OS X)
    there is a bundled fallback theme with Breeze icons that will be used 
  - this allows the usage of QOwnNotes with **dark system themes**

## 0.96
- fixed the debug output in the *Debug* settings tab
- you can now select a font for the code in the markdown enabled text edits
- you can now reset the fonts to the operating system's default font

## 0.95
- code in the Markdown edit now uses the `Courier` font
  (for [Issue #59](https://github.com/pbek/QOwnNotes/issues/59))
- the update checker was modified to fix a crash on OS X
- there now is a new *Interface* tab in the settings dialog to make room for 
  more configuration items
- there now is a new button in the *general settings*, that allows you to clear 
  all settings, remove the local todo database and exit QOwnNotes
  (for [Issue #56](https://github.com/pbek/QOwnNotes/issues/56))

## 0.94
- pressing `Delete` in the note list widget now attempts to remove all 
  selected notes, not just the current one
  (for [Issue #63](https://github.com/pbek/QOwnNotes/issues/63))
- canceling the update dialog now really cancels the update process like it 
  used to
- fixed a problem with a wrong note showing up after you change your note folder
  (for [Issue #60](https://github.com/pbek/QOwnNotes/issues/60))
  
## 0.93
- added more German translation by @calis2002
- attempted to fix a problem with removing multiple notes
- fixed a problem with two notes with the same name (but different extension) 
  showing up in the note list
- fixed a problem with empty entries in the recent note folders list 

## 0.92
- you can now choose to use `.md` instead of `.txt` as your default 
  file-extension for your notes in the settings
  (for [Issue #25](https://github.com/pbek/QOwnNotes/issues/25))
    - *ownCloud Notes* and *Cloudnotes* finally also support `.md` as 
      file-extension 
    - please try not to use `.md` and `.txt` notes with the same name
    - you have to update the ownCloud app `QOwnNotesAPI` to version 0.4.2 to
      make use of the `.md` support
- added more German translation for the new settings texts and added some 
  changes by @calis2002

## 0.91
- the OS X dmg and Ubuntu deb files, that are created by Travis CI now have 
  the version number in the filename
- the markdown view mode can now be selected as default view mode in the 
  settings (a big thank you to @dasmaetthes for the contribution)
- added **Chinese translation** by @LiangWei88, a big thank you!
- you can now select Chinese in the settings

## 0.90
- more strings made translatable and add **French translation** by @Dalan94, a 
  big thank you!
- added **Polish translation** by @Olhur, thank you a lot!
- you can now select the two new language in the settings

## 0.89
- you can now select the interface language in the settings
    - by default the system language will be used
    - the fallback language is English
- added **German translation** from @calis2002, thanks a lot for the great job!
- if you feel like translating QOwnNotes to an other language please read
 [How can I help to translate QOwnNotes?](http://www.qownnotes.org/Knowledge-base/How-can-I-help-to-translate-QOwnNotes)
- fixed a character in link titles that was disorienting the markdown 
  highlighter

## 0.88
- more app internationalization implementation and integration in deployment 
  systems (for [Issue #40](https://github.com/pbek/QOwnNotes/issues/40))
- start of German translation of QOwnNotes
    - please leave a message at the
    [QOwnNotes issues page](https://github.com/pbek/QOwnNotes/issues)
    if you want to help out with translation
- fixed a file watcher error on app startup
- the demo notes are now only created once instead of every time a notes folder
  is empty (for [Issue #41](https://github.com/pbek/QOwnNotes/issues/41))
- fixed a problem with an empty note in the note list
- fixed a problem with the note text after the last note was deleted in the 
  note list

## 0.87
- start of app internationalization implementation
  (for [Issue #40](https://github.com/pbek/QOwnNotes/issues/40))
- fixed a problem with the crypto key expiration of encrypted messages where one
  had to enter the password every minute while editing the note
  (for [Issue #42](https://github.com/pbek/QOwnNotes/issues/42))
    - now the crypto key will expire after 10min without editing the note
- did some fixes and layout changes in the settings dialog
- added basic app metrics
    - anonymous usage data helps to decide what parts of QOwnNotes to 
    improve next and to find and fix bugs
    - you can disable the app metrics in the settings

## 0.86
- tried to fix a sporadic crash on OS X
- there now is a software repository for **Debian Linux 8.0**
    - please visit the [QOwnNotes installation page](http://www.qownnotes.org/installation) for instructions

## 0.85
- you can now export the current note as markdown file
- beside the regular QOwnNotes software repository for Arch Linux there now 
  also is an official package on the **Arch User Repository** called 
 `qownnotes-pbek`

## 0.84
- now non Travis CI or AppVeyor builds can perform an update check again, but 
  will open the [QOwnNotes installation page](http://www.qownnotes.org/installation) 
  instead of downloading the app
- there is a new software repository for **Arch Linux**
    - please visit the [QOwnNotes installation page](http://www.qownnotes.org/installation) for instructions

## 0.83
- switched two buttons in the menu bar
- fixed a problem with updating and creating the todo list item database (for [Issue #35](https://github.com/pbek/QOwnNotes/issues/35))

## 0.82
- you are now able to open the current note in an external editor with `Ctrl + Shift + O`
- fixed a problem with editing encrypted notes while there are changes in the note directory
- you will now get a warning if you enter a wrong password when you are 
  decrypting a note
- the password dialog will now ask you for your password a 2nd time when 
  encrypting a note
- added more information to the debug output in the settings dialog

## 0.81
- fixed a problem where the password of an encrypted note was overwritten 
  while editing the encrypted note
    - **please decrypt your notes from v0.80 before upgrading to v0.81 or you 
    will not be able to decrypt them!**
- fixed a problem with encrypting empty notes
- crypto keys of encrypted notes will now expire after 10min, you will then 
  have to enter the password again

## 0.80
- the OSX build of QOwnNotes now uses Qt 5.5.1 to fix a few bugs
- fixed a few small issues in the todo list
- you are now able to **encrypt** and **decrypt** notes in QOwnNotes
    - you are also able to edit encrypted notes without the unencrypted note getting saved to disk
    - this functionality is still in alpha stage

## 0.79
- the search widget in the text edits will now move out of the way if it blocks the search result 
- some margin changes in the search widget were done and some key events were fixed
- you can now search for todo items in the todo list

## 0.78
- you are now able to configure the icon size of the main toolbar in the general settings
- you are now able to duplicate text with `Ctrl + Alt + Down` in all markdown edits
- searching for text is now possible in all markdown edits with `Ctrl + F`
- added a clear button to most of the text lines
- you can now jump to all visited note folders with a select box above the notes list (for [Issue #31](https://github.com/pbek/QOwnNotes/issues/31))
    - this select box is turned off by default and can be turned on in the general settings
- small margin tuning on the main screen

## 0.77
- the icon size in the main toolbar was set to a fixed `38x38` to try to prevent high-dpi problems in KDE Plasma
- `libqt5sql5-sqlite` was added to the dependencies for the Ubuntu PPA build and the Ubuntu deb-package (for [Issue #30](https://github.com/pbek/QOwnNotes/issues/30))
- if you don't want to miss a single version of QOwnNotes you might want to take a look at [New release version RSS feed](http://www.qownnotes.org/Blog/New-release-version-RSS-feed)

## 0.76
- the Windows version of QOwnNotes now also includes the OpenSSL library, please report if you have any problems on the [QOwnNotes issue page](https://github.com/pbek/QOwnNotes/issues))
- clicking links in all text edit boxes with markdown highlighting should work in the same way now

## 0.75
- added markdown highlighting to the debug information text box
- added link clicking support, 4 characters tab stops, custom font support and block indention to all markdown highlighting editors

## 0.74
- fixed another problem with todo lists if the server url contains a path (thanks again to Dirk for the report)

## 0.73
- fixed a bug with the todo list calendars if the server url had a path in it (a big thank you to Dirk for the report)
- more information was added to the debug information and you can now store it to a markdown file to help us finding problems
- pressing return in the summary text line of a todo item now saves it
- added basic markdown highlighting to the description edit field of the todo items

## 0.72
- fixed a crash that occurred when all notes were removed
- fixed a crash that occurred when all todo items were removed
- added a menu entry to show the changelog in the help menu
- fixed a design glitch with the visibility of the todo reminder date selector and some other user interface elements
- now a warning is issued if you want to open the todo lists, but haven't configured them yet
- fixed a bug where newly created todo list items were stored to the wrong todo list

## 0.71
- you are now able to print notes (text or markdown)
- reduced many margins in the main window to get more room
- you can now clear the recent note folders history in the settings (for [Issue #26](https://github.com/pbek/QOwnNotes/issues/26))
- you can now insert images in notes with `Ctrl+I`
    - the image will be copied to the `media` folder in the current notes folder
    - the image will be shown in the markdown view (maximum width is 980px)
    - the image is clickable in the edit view
- there now is a new debug tab in the settings dialog where you can find a dump of data you can use in an Issue on the [QOwnNotes issue page](https://github.com/pbek/QOwnNotes/issues))

## 0.70
- beside the new software repositories for **Ubuntu Linux** and **openSUSE Linux** there are now also repositories for **Fedora Linux**
    - please visit the [QOwnNotes installation page](http://www.qownnotes.org/installation) for instructions
- tried to fix a crash that sometimes occurs on OSX after waking from sleep
- you can now also export the text of notes as PDF

## 0.69
- if you are using **Ubuntu Linux** or **openSUSE Linux** you can now take advantage of the brand new repositories for both distributions
    - take a look at the [QOwnNotes installation page](http://www.qownnotes.org/installation) for instructions
    - the update check for these releases have been removed from the menu and you will not get a report for new software versions, since updates are done by your operating system
    - you can always visit the [QOwnNotes changelog](http://www.qownnotes.org/changelog/QOwnNotes) to see what has changed in the versions
- you can now export notes as PDF (for [Issue #23](https://github.com/pbek/QOwnNotes/issues/23))

## 0.68
- fixed a problem with creating new todo items after changing the enabled todo lists
- fixed a problem with newly created todo items that are set to completed
- fixed an other problem with storing todo items with a reminder and an empty description
- fixed some design glitches in the todo list
- if a todo item has no priority it now counts as medium priority
- added a button to reload the todo list from the server to the todo list dialog

## 0.67
- fixed a problem with storing todo items with a reminder and an empty description
- added a button to reinitialize the database to the settings dialog
- now a simple alert is shown when the reminder of a todo item is due

## 0.66
- fixed a problem with storing wrong todo list urls in the settings
- fixed the cursor after releasing the `Ctrl` key in the edit mode
- fixed troubles with storing new todo items
- you can now set a reminder date for todo items, this date will just be set on the server, QOwnNotes doesn't remind you (yet)
- added a menu bar button and a shortcut (`Ctrl + Shift + T`) for the todo list

## 0.65
- you are now able to create new todo items on your ownCloud server
- you are now able to delete todo items on your ownCloud server
- you are now able to mark todo items as done on your ownCloud server
- made a workaround for a server problem with *Calendar Plus* / *Tasks Plus*
- fixed the todo priorities according to [RFC2445](http://www.ietf.org/rfc/rfc2445.txt)

## 0.64
- fixed some network issues
- fixed a problem with `,` in todo list items
- fixed a problem with missing todo list items
- now a progress bar is shown while the todo list items are loaded from the server
- note: **Merry Christmas everyone**

## 0.63
- fixed some problems with special characters in todo list names
- the default todo item priority is now 0 (not set)
- todo items are now locally stored in a sqlite database and only updated from server if they were changed on the server
- vcalendar and vjournal items are now ignored when fetching items from the server, only vtodo items will be fetched

## 0.62
- you can now also use the todo items from the *Calendar Plus* / *Tasks Plus* ownCloud apps
    - select which backend you want to use in the todo settings
- you can now **save local changes** of existing **todo items** on your ownCloud calendar server
    - this functionality is in **alpha phase**, you may not want to use it in production yet
    - please report if you have any troubles with it

## 0.61
- only todo items (not calendar items) are now shown in your todo lists
- you can now choose to hide completed todo items at the todo list dialog
- the last selected todo list is now remembered at the todo list dialog
- the completed check for todo items was improved
- the note path error messages in the settings dialog are a bit more verbose now

## 0.60
- if the `Ctrl` key is pressed in the note text edit to click a link the mouse cursor is now a pointing hand
- you are now able to view your **ownCloud todo lists** (ownCloud tasks) in QOwnNotes (readonly so far)
    - you can setup which todo lists to view in the new *todo* tab in the settings dialog

## 0.59
- an *Insert current time* button was added to the toolbar so that the feature also works in Gnome on GNU/Linux
- the markdown view also supports tables now (for [Issue #20](https://github.com/pbek/QOwnNotes/issues/20))

## 0.58
- fixed a problem with linking notes with special characters in their name (everything but charactes and numbers)
- html entities from the titles of inserted links are now decoded
- whitespaces are now trimmed from the start and end of the title of inserted links
- you can now select a local file to link to in the link dialog
- added a new shortcut `Ctrl + T` to insert the current date in ISO 8601 format

## 0.57
- `Ctrl + Click` in edit mode now works on the whole portion of links like `[QOwnNotes - cross-platform open source notepad](http://www.qownnotes.org/)`, not just on the URL alone
- added a menu entry to the new knowledge base in the help menu
- you can now change the font of the note text edit and the note markdown view in the settings (for [Issue #19](https://github.com/pbek/QOwnNotes/issues/19))

## 0.56
- added a shortcut list to the help menu and to the startup notes
- you can now also link to URLs with `Ctrl + L` (or `Cmd + L` on OSX), if possible the title of the webpage will be fetched and used as link text

## 0.55
- added some different shortcuts for the note history on the mac
- tried to fix the duplicate line shortcut for the Ubuntu Unity desktop
- the OSX package is now code signed (but there are still some troubles with the signature)

## 0.54
- fixed a problem with canceling the notes folder selection dialog
- the note history is now cleared when switching to an other notes directory
- when moving forward and back in the note history the search field will be cleared so that all notes are visible

## 0.53
- fixed a problem that the note list did not get reloaded after the current note was renamed
- you can now jump back and forward to your last used notes with *Alt+left/right* and the *back* and *forward* mouse buttons
- you can now press *Shift+tab* to remove tabs even if nothing was selected

## 0.52
- notes will now be parsed for markdown highlighting immediately after a note was selected instead of waiting up to 1 second
- added functionality to duplicate the selected text or the current line (shortcut: *Ctrl+D*)
- it is  now (after quite some work) possible to *Ctrl+click* links to open them in the note edit mode
- you now can indent and unindent the selected text with the *Tab* and *Shift+Tab* keys
- the current note name now shows up in the window title

## 0.51
- fixed the flickering in the note list, that could happen when you edit the current note
- fixed a redraw error that could happen after storing the settings in the settings dialog
- fixed the note storing interval time (it was set far too low in version 0.50)

## 0.50
- the link color in the markdown view was a bit darkened
- the *externally modified* dialog isn't shown any more by default when the current note wasn't edited for a minute
    - this behaviour can be changed in the settings dialog
    - feel free to contact me if you have thoughts on that
- you are now able to set the note save interval time in the settings dialog
    - default is still 10 seconds

## 0.49
- added some markdown view styling
- more improvements in the current note search feature
- the position of the menu bar will now be stored and will be restored when the application starts
- about dialog styling changes and link fixup

## 0.48
- lots of improvements in the current note search feature
- added more shortcuts

## 0.47
- added the ability to search in the current note
- OSX builds are finally made on Travis CI too
- added some new icons

## 0.46
- improved some error messages
- added some more items to the main menu (so that Ubuntu's unity desktop recognizes the shortcuts)

## 0.45
- making sure the welcome note is selected after it was created
- added the ability to link the selected text to a note or create a link to a note at the current cursor position in markdown
- there is a new dialog to search for notes to link to

## 0.44
- added more explanations in the settings dialog
- added a check if the notes path was found on the server in the settings dialog (needs QOwnNotesAPI v0.4.1)

## 0.43
- added functionality to restore a trashed note directly on the ownCloud server, restoring the note's versions with it. (needs QOwnNotesAPI v0.4)
- updating to QOwnNotesAPI v0.4 is suggested, because it also contains two important bugfixes for the trashed notes api
- fixed a bug when the ownCloud path and the notes path are the same
- added write permissions to the demo notes
- note editing is now disallowed if note file is read only
- some improvements on storing notes to disk

## 0.42.1
- fixed a missing svg

## 0.42
- improvements and fixes in the settings connection troubleshooter
- minimum QOwnNotesAPI version is now detected
- detection for ownCloud core apps "Versions" and "Deleted files" (needs QOwnNotesAPI v0.3.1)
- you can open your ownCLoud server Apps page from the settings dialog

## 0.41
- added an interactive ownCloud server connection troubleshooting helper and other improvements to the settings page
- added a better connection error handling
- some menu restructuring
- added the ability to select all notes
- the current notes folder isn't shown in the recent notes folders any more
- some message box text styling
- notes are now restored from trash with their original name if their names are not taken

## 0.40
- you now are able to select multiple notes and remove them or copy/move them to a recent note folder
- fixed a possible crash with empty note files

## 0.39
- turned on native file dialogs again
- some text changes and svg image fixes
- added an automatic update check every 6h
- you now are able to skip an update, no more update notifications as long there is no new update

## 0.38
- you will now start with some demo notes if your notes directory is empty
- settings dialog improvements
- added some icons and made some minor layout changes

## 0.37
- trashed notes can now be viewed and restored from your ownCloud server
    - please install QOwnNotesAPI v0.3 for trash support

## 0.36
- fixed a problem with the OSX build

## 0.35
- added information text for Windows users to install OpenSSL
- added more detailed network connection error messages
- fixed a problem with Windows 8.1

## 0.34
- old versions of your notes can now be restored from your ownCloud server
- introduced a settings page
- fixed a text
- OSX: QOwnNotes.dmg is now compressed and includes a symbolic link to the *Applications* folder

## 0.33
- fixed a bug where quitting the app didn't store the settings
- sorting support (thank you pmarki)
- system tray support (thank you pmarki)

## 0.32
- added a help menu entry to report problems or ideas
- fixed problems with file dialogs by using the QT file dialog
- fixed the crash that can happen on the first start of the application after choosing the notes directory

## 0.31
- implemented an update service to check if a new version of QOwnNotes is available
- the update check will be performed automatically at the start of the application and can be performed manually in the menu

## 0.30
- help screen improvements
- added version number to build number

## 0.29
- fixed a problem with disappearing newlines

## 0.28
- more utf-8 fixes
- icons for Windows 8.1.

## 0.27
- added a character set fix for Windows

## 0.26
- updated hoedown to 3.0.3

## 0.25
- brings better url handling in the markdown preview mode.
- links to other notes or files are now possible

## 0.24
- added a "recent note folder list" feature

## 0.23
- fixed a crash when opening empty note-files

## 0.21
- clickable links in markdown html preview

## 0.20
- fixes for the Linux package

## 0.19
- Markdown HTML preview
- menu - toolbar

## 0.18
- fixed some troubles with the Windows package.
- added a hotkey to create new note with date in name
- turned off pasting of rich text in the note text edit box to avoid troubles
