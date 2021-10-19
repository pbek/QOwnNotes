# QOwnNotes Changelog

## 21.10.9
- there now will be a warning that a restart of the application is needed when
  changing the default note file extension (for [#2250](https://github.com/pbek/QOwnNotes/pull/2250))
- more features were added to the **linking of note heading** (for [#1977](https://github.com/pbek/QOwnNotes/issues/1977))
    - if a note and a heading were selected in the note link dialog the heading
      text will be added to the link title by default
    - if the current note is selected in the note link dialog the correct note
      file name will be used now instead of `.` to allow links to headings in
      the current note
    - when a note is renamed or moved to another sub-folder note links with
      heading fragments can now also be automatically renamed
        - this also works with note links to the current note
        - warning: this feature (especially the part with links to the current note)
          was very complex and might introduce regressions
- the build for very old Qt versions was (hopefully) fixed again
- a possible crash was fixed when renaming a note directly in the note list when
  <kbd>Enter</kbd> was pressed (for [#1837](https://github.com/pbek/QOwnNotes/issues/1837))
- added more translation (thank you amirkbrpr, marterra, danilosf, evildeepblue,
  marcoxbresciani, rawfreeamy)

## 21.10.8
- more features were added to the **linking of note heading** (for [#1977](https://github.com/pbek/QOwnNotes/issues/1977))
    - the search panel inside the note is now closed after jumping to the heading
    - the broken note link checker now also works with links with an url fragment
    - you can now select a heading to link to in the note link dialog
- the broken note link checker now also works with links like `<myfile.md>`
  (for [#2250](https://github.com/pbek/QOwnNotes/pull/2250))

## 21.10.7
- you can now add an url-encoded heading to a note link as url fragment to
  attempt to **jump to that heading in the linked note when clicking the link**
  (for [#1977](https://github.com/pbek/QOwnNotes/issues/1977))
    - this works with opening links in the note text edit and the preview 
    - `[my note](my-note.md#First%20heading)` will attempt to jump to a heading
      `First heading` in note `my-note.md` (note that `%20` is decoded as space)
    - the heading will be searched with the in-note search, so for the link
      `[my note](my-note.md#First%20heading)` there will be searched for the
      text `## First heading` inside the note
    - this feature is still *work in progress*
- links like `www.github.com` (without any url-schema like `http://`) are now
  also highlighted as links the note text edit and can also be opened there
  (for [#2260](https://github.com/pbek/QOwnNotes/issues/2260))

## 21.10.6
- the broken link highlighting was fixed (broken in 21.10.1) and support was added for
  the selected default note file extension (for [#2250](https://github.com/pbek/QOwnNotes/pull/2250))
- the build time was reduced (for [#2257](https://github.com/pbek/QOwnNotes/pull/2257), thank you @Waqar144)
- added more Hungarian translation (thank you rawfreeamy)

## 21.10.5
- the problem for some people with the block syntax highlighting in the
  preview panel only working after editing the note was hopefully fixed
  (for [#2165](https://github.com/pbek/QOwnNotes/issues/2165), thank you @Waqar144)
- added more Catalan translation (thank you mguerra)

## 21.10.4
- the current line will now also  be highlighted again the current note is
  modified externally (if you have enabled it in the settings)
  (for [#2253](https://github.com/pbek/QOwnNotes/issues/2253))
- now no (broken) note will be created when pressing <kbd>Enter</kbd> in an
  empty note search text line (for [#2255](https://github.com/pbek/QOwnNotes/issues/2255))

## 21.10.3
- the [Web Companion browser extension](https://github.com/qownnotes/web-companion)
  was updated to version 21.10.0
    - when adding a bookmark and pressing enter to store it, now the 2nd
      `Add bookmark` dialog, that was triggered, is suppressed
    - updated dependencies
    - added a lot more translation
- the cursor position of the current note is now also restored when you manually
  reload the note folder or the current note is modified externally (if you have
  enabled `Restore cursor position when opening a note` in the *General settings*)
  (for [#2253](https://github.com/pbek/QOwnNotes/issues/2253))
- added more translation (thank you marterra, amirkbrpr, rawfreeamy, marcoxbresciani)

## 21.10.2
- more issues with the translation of the default buttons under macOS were
  tried to be fixed (for [#783](https://github.com/pbek/QOwnNotes/issues/783))
- the restart information in single app instance mode was improved to make it
  more understandable (for [#2235](https://github.com/pbek/QOwnNotes/issues/2235))
- the build for Qt <= 5.5 was fixed
- added more Portuguese translation (thank you smarquespt)

## 21.10.1
- when copying or moving notes to another sub-folder now notes will not be copied
  or moved to the same sub-folder anymore (for [#2249](https://github.com/pbek/QOwnNotes/issues/2249))
- issues with the translation of the default buttons under Windows and macOS were
  tried to be fixed (for [#783](https://github.com/pbek/QOwnNotes/issues/783))
- logging before the user interface is loaded was improved (for [#783](https://github.com/pbek/QOwnNotes/issues/783))
- some debug information was added to help reproduce a preview issue
  (for [#2165](https://github.com/pbek/QOwnNotes/issues/2165), thank you @Waqar144)
- the general performance of handling notes was improved
  (for [#2250](https://github.com/pbek/QOwnNotes/pull/2250), thank you @Waqar144)
- added more Dutch, Sinhala, Catalan translation (thank you rawfreeamy, helabasa, mguerra)

## 21.10.0
- log messages that are output before the log panel is loaded are now also logged
  to the log file if logging to file is enabled in the *Debug options* settings
  (for [#783](https://github.com/pbek/QOwnNotes/issues/783))
- now the translation files that are loaded are logged to allow better debugging
  of the translation system (for [#783](https://github.com/pbek/QOwnNotes/issues/783))
    - you need to enable the log file or run QOwnNotes from the terminal to see
      those logs, because this happens before the log panel is loaded 
- added more Lithuanian translation (thank you atvejis)

## 21.9.13
- now named links can be imported in the *Evernote import* (for [#2242](https://github.com/pbek/QOwnNotes/issues/2242))
- there now is a new script [Black mode](https://github.com/qownnotes/scripts/tree/master/black-mode)
  in the *Script repository* that makes the UI black in dark mode

## 21.9.12
- an issue with showing twice the number of untagged notes in the *Tags panel* after
  creating a new note on Windows was fixed (for [#2241](https://github.com/pbek/QOwnNotes/issues/2241))
- improved tag delete icon in dark-mode (for [#2243](https://github.com/pbek/QOwnNotes/issues/2243))
- now links to all file extensions from the list in the `Note file extensions` section
  of the *General settings* are recognized as notes in the note editor and preview
- added more Persian, Catalan translation (thank you amirkbrpr, mguerra)

## 21.9.11
- an issue with the note counts initially not showing up in the *Tags panel* on
  Windows was fixed (for [#2239](https://github.com/pbek/QOwnNotes/issues/2239))
- fixed an issue with the application doesn't getting the old cli parameters
  after a restart by a dialog
- added more translation (thank you mguerra, fitoschido, lucjan, marcoxbresciani,
  rakekniven, marterra, rawfreeamy, amirkbrpr)

## 21.9.10
- now `md` or `txt` files will not be shown anymore in the note list when they
  were removed from the list in the `Note file extensions` section of the
  *General settings* (for [#2236](https://github.com/pbek/QOwnNotes/issues/2236))
- some tooltips were added to the file extensions and a removal confirmation
  dialog was added to the `Note file extensions` section of the *General settings*
  (for [#2236](https://github.com/pbek/QOwnNotes/issues/2236))

## 21.9.9
- when you answer `No` when asked to automatically switch to dark/light mode
  under Windows 10 the answer will now really be remembered if you told the 
  dialog to do so (for [#2234](https://github.com/pbek/QOwnNotes/issues/2234))
- the `Note file extensions` section in the *General settings* has now a better
  description and a new `list-remove` icon (for [#2236](https://github.com/pbek/QOwnNotes/issues/2236))
- you are now able to also remove the `md` or `txt` note file extensions in the
  `Note file extensions` section in the *General settings* if you don't want one
  of them to be used to recognize note files in your note folder (for [#2236](https://github.com/pbek/QOwnNotes/issues/2236))
- added more German, Persian, Brazilian Portuguese translation (thank you amirkbrpr, marterra)

## 21.9.8
- the colors in the log panel were more adapted for dark mode
- added more Persian, Catalan translation (thank you amirkbrpr, MGuerra)

## 21.9.7
- there now is no need for a restart any more after an automatic dark/light mode switch under
  Windows 10 when the application starts (for [#2234](https://github.com/pbek/QOwnNotes/issues/2234))
- you can now also restart the application automatically in single app instance mode
  when it is required, instead of being forced to do it manually (for [#2235](https://github.com/pbek/QOwnNotes/issues/2235))
- there now a script `Toggle note list sort order` in the script repository that creates a
  custom action that toggles the sort order of the note list between `Alphabetical` and `By date`
- added more German, Persian, Brazilian Portuguese translation (thank you rakekniven, amirkbrpr, marterra)

## 21.9.6
- now the required restart will be mentioned for the automatic dark/light mode switch
  under Windows 10 (for [#2234](https://github.com/pbek/QOwnNotes/issues/2234))
- the build for Qt < 5.6 is fixed again
- added more Brazilian Portuguese translation (thank you marterra)

## 21.9.5
- under Windows 10 the application will now ask you if you want to turn on 
  dark mode if Windows is in dark mode and the application is not
  (for [#2234](https://github.com/pbek/QOwnNotes/issues/2234))
    - the application will now also ask you if you want to turn off dark mode if
      Windows is in light mode and the application has dark mode turned on
    - you can remember those decisions so that the switch will be made
      automatically next time
- added more Catalan, Brazilian Portuguese translation (thank you mguerra, marterra)

## 21.9.4
- now a more descriptive user agent is used for the login flow to generate an
  app password on your Nextcloud server, so you can more easily spot the app
  password created for QOwnNotes (for [#2233](https://github.com/pbek/QOwnNotes/issues/2233))
- added more French translation (thank you gbabin)

## 21.9.3
- you can now create an app password for your Nextcloud account automatically to 
  grant QOwnNotes access to your account by just entering the server url and 
  clicking on `Login to Nextcloud to grant access` in your *Nextcloud / ownCloud settings*
  (for [#2233](https://github.com/pbek/QOwnNotes/issues/2233))
    - then just follow the instructions in your web browser until QOwnNotes can
      fetch your username and app password in the background

## 21.9.2
- there now is highlighting and preview support for disabled checkboxes `[-]`
  (no markdown standard) in the note editor and preview

## 21.9.1
- the [Web Companion browser extension](https://github.com/qownnotes/web-companion)
  was updated to version 21.9.0
  - added a link to the [Web clipper documentation](https://www.qownnotes.org/getting-started/browser-extension.html#web-clipper)
    to the hamburger menu on the top left to give more exposure to that feature
  - updated dependencies
  - added more Sinhala translation
- you can now move a todo item into another todo item in the todo list dialog
  (for [#2225](https://github.com/pbek/QOwnNotes/issues/2225))
- added more Italian translation (thank you marcoxbresciani)

## 21.9.0
- WebP images are now detected as media file when dropped into the note editor or inserted as data-url
- added more Sinhala, Catalan, Brazilian Portuguese, Turkish translation (thank you helabasa, mguerra, marterra)

## 21.8.12
- since notes in Nextcloud Notes on the web and on Android now allow renaming of
  notes this also is the new default setting for note folders in QOwnNotes
    - existing note folders will not be modified automatically
- added more translation (thank you amirkbrpr, mguerra, rakekniven, ihatemyself, rawfreeamy, marterra)

## 21.8.11
- a few features were added to the todo dialog (for [#2210](https://github.com/pbek/QOwnNotes/issues/2210))
    - the due date is now shown in the todo items tree
    - you can now sort the tree by summary or due date
    - you can now choose to just view items that are due today
        - note that this doesn't work for sub-items, because they may
          be hidden by the parent item
- added more Brazilian Portuguese, Italian, Russian translation (thank you marterra, marcoxbresciani, rolayis927)

## 21.8.10
- the performance of notes with (especially large) images was improved massively
  (for [#2078](https://github.com/pbek/QOwnNotes/issues/2078), thank you @Waqar144)
    - the general load time of notes was improved as well

## 21.8.9
- there now is a new scripting function `script.insertAttachmentFile()` to insert
  an attachment file into the `attachments` folder (for [#2215](https://github.com/pbek/QOwnNotes/issues/2215))
    - for more information please take a look at the [scripting documentation](https://www.qownnotes.org/scripting/methods-and-objects.html#inserting-an-attachment-file-into-the-attachmentn-folder)
- added more translation (thank you rawfreeamy, marterra, helabasa, marcoxbresciani, amirkbrpr)

## 21.8.8
- highlighting support for numbered list checkboxes was added
  (for [#134](https://github.com/pbek/qmarkdowntextedit/issues/134), thank you @Waqar144)
- fixed a problem with strikeout and code inline formatting in the same line
  (for [#2205](https://github.com/pbek/QOwnNotes/issues/2205), thank you @Waqar144)
- added more Sinhala, French, Spanish, Danish, German, Hungarian, Italian, Dutch
  translation (thank you helabasa, rawfreeamy)

## 21.8.7
- fixed a status bar message without timeout
- fall back to just counting a minimum of 3 headline separator characters in a Markdown
  table when pressing <kbd>Ctrl</kbd> + <kbd>Space</kbd>, because it looks more beautiful
  and no converters seem to have issues with it (for [#2206](https://github.com/pbek/QOwnNotes/issues/2206))
- added more Polish, Hungarian, Dutch translation (thank you manuelasmiley, rawfreeamy)

## 21.8.6
- alignment characters in the headline separator line of a Markdown table are now
  not counted anymore to the minimum of 3 headline separator characters that are needed
  to produce a valid Markdown table when pressing <kbd>Ctrl</kbd> + <kbd>Space</kbd>
  in a table (for [#2206](https://github.com/pbek/QOwnNotes/issues/2206))

## 21.8.5
- a minimum of 3 headline separator characters are now used to produce valid
  Markdown tables when pressing <kbd>Ctrl</kbd> + <kbd>Space</kbd> in a table
  (for [#2206](https://github.com/pbek/QOwnNotes/issues/2206))

## 21.8.4
- in Markdown tables you now can also automatically shrink the headline
  separator columns if needed when pressing <kbd>Ctrl</kbd> + <kbd>Space</kbd>
  (for [#2206](https://github.com/pbek/QOwnNotes/issues/2206))

## 21.8.3
- UTF-8 problems on Windows with the Joplin note import were fixed
  (for [#2200](https://github.com/pbek/QOwnNotes/issues/2200))
- added more translation (thank you marcoxbresciani, fitoschido, marterra, mguerra, amirkbrpr)

## 21.8.2
- if you don't have note subfolders enabled for the current note folder the
  import of folders is now automatically disabled in the
  **[Joplin](https://joplinapp.org/) note import**
  (for [#2189](https://github.com/pbek/QOwnNotes/issues/2189))
- the [Web Companion browser extension](https://github.com/qownnotes/web-companion)
  was updated to version 21.8.1
    - update dependencies
- added more Italian translation (thank you marcoxbresciani)

## 21.8.1
- the [Web Companion browser extension](https://github.com/qownnotes/web-companion)
  was updated to version 21.8.0
    - migrated to webextension-toolbox 4 and webpack 5 to fix vulnerabilities
    - added more Croatian translation
- added more French, Spanish translation (thank you rawfreeamy)

## 21.8.0
- after notes are imported by dragging and dropping text files into the application
  the note folder is now reloaded automatically so the notes show up in the note list
- added more translation (thank you manuelasmiley, rawfreeamy, milotype)

## 21.7.14
- the first app is now shown if a 2nd instance is started in single instance mode
  (for [#2198](https://github.com/pbek/QOwnNotes/issues/2198))

## 21.7.13
- added a new editor color schema *Oomox Shack* by @LetsHaveKiddos
  (for [#2196](https://github.com/pbek/QOwnNotes/issues/2196))
- added more translation (thank you rawfreeamy, manuelasmiley, p-bo, amirkbrpr, marcoxbresciani)

## 21.7.12
- you can now jump to the same headline multiple times in the navigation panel
  (for [#2192](https://github.com/pbek/QOwnNotes/issues/2192))
- the focus will now stay in the navigation panel after using it via mouse or keyboard
  (for [#2193](https://github.com/pbek/QOwnNotes/issues/2193))
- added more translation (thank you marcoxbresciani, rawfreeamy, rakekniven, marterra, amirkbrpr)

## 21.7.11
- the **[Joplin](https://joplinapp.org/) note import** was improved
  (for [#2189](https://github.com/pbek/QOwnNotes/issues/2189))
    - if Joplin didn't set the file extension of attachments or images in the export
      we now will try to get it from the original file name
    - importing of images from image html tags like `<img src=":/ID"/>` is now
      also supported (@laurent22, thank you for mentioning)
    - the metadata of notes can now optionally be imported as frontmatter
- added more Persian translation (thank you amirkbrpr)

## 21.7.10
- the **[Joplin](https://joplinapp.org/) note import** was improved
  (for [#2189](https://github.com/pbek/QOwnNotes/issues/2189))
    - the whole notebook / folder structure can now now also be imported as note sub-folders
- added more Italian, Brazilian Portuguese translation (thank you rawfreeamy, i****myself)

## 21.7.9
- the **[Joplin](https://joplinapp.org/) note import** was improved
  (for [#2189](https://github.com/pbek/QOwnNotes/issues/2189))
    - tags, images and attachments are now also be imported
    - the raw metadata part at the bottom of the note are now be not imported any more
    - numbers and points are now only removed from the beginning of the note
      name instead of the whole note name
- added more translation (thank you i****myself, amirkbrpr, rawfreeamy)

## 21.7.8
- you now can **import notes** from a *RAW - Joplin Export Directory* of
  **[Joplin](https://joplinapp.org/)** with a new import dialog in the *Note menu*
  (for [#2189](https://github.com/pbek/QOwnNotes/issues/2189))
    - this first draft of the feature supports only importing all files that are
      marked as "note" in a rudimentary fashion
    - if you just have a *JEX - Joplin Export File* please extract the content
      of that file to a directory and select that directory in the import dialog
    - note that this import is done by reverse engineering, since there is no
      documentation for the Joplin export format
    - no tags, images or attachments are yet imported
- added more Persian translation (thank you amirkbrpr)

## 21.7.7
- there now also is a `Reload script engine` button for scripts from the script
  repository in the settings dialog (for [#2187](https://github.com/pbek/QOwnNotes/issues/2187))
    - this makes it for example easier to test changes to the user interface 

## 21.7.6
- version 0.1.11 of the **QOwnNotes Web application** [app.qownnotes.org](https://app.qownnotes.org/)
  was released (for [#2104](https://github.com/pbek/QOwnNotes/issues/2104))
    - you are now able to view the token
    - you can now **scan the token from a QR code** in the settings dialog of
      QOwnNotes 21.7.6 or newer
    - all `console.log` commands are now `console.debug`
- you are now able to **generate a QR code** from your QOwnNotes Web application token
  in the settings dialog that can be scanned with the QOwnNotes Web application
  [app.qownnotes.org](https://app.qownnotes.org/) (for [#2104](https://github.com/pbek/QOwnNotes/issues/2104))
- added more translation (thank you rakekniven, amirkbrpr, mguerra, pgrules7, marcoxbresciani)

## 21.7.5
- if you uncheck `Bold` for `Strong text` in the *Editor fonts & colors* settings
  the text is now really not bold anymore in the editor (for [#2182](https://github.com/pbek/QOwnNotes/issues/2182))
    - the preview now also allows tags like headings and strong to not be bold
      when the editor highlighting styles are used for the preview
- added more translation (thank you marcoxbresciani, rawfreeamy, amirkbrpr, mguerra)

## 21.7.4
- the execution of git log commands has been fixed for Windows
  (for [#2182](https://github.com/pbek/QOwnNotes/issues/2182))
- the [QOwnNotesAPI Nextcloud App](https://apps.nextcloud.com/apps/qownnotesapi)
  was updated to version 21.7.0 to support Nextcloud 22
- added more translation (thank you rawfreeamy, marcoxbresciani, amirkbrpr, mguerra, marterra)

## 21.7.3
- the restoring of the focus after switching workspaces was fixed
  (for [#2180](https://github.com/pbek/QOwnNotes/issues/2180))
- added more translation (thank you mguerra, marcoxbresciani, amirkbrpr, rawfreeamy)

## 21.7.2
- the color of the current line number is now adapted if dark mode is disabled
  (for [#2091](https://github.com/pbek/QOwnNotes/issues/2091))
- a build error for Qt < 5.5 was fixed (for [#2091](https://github.com/pbek/QOwnNotes/issues/2091))
- added more translation (thank you mguerra, amirkbrpr, marcoxbresciani, marterra, rakekniven)

## 21.7.1
- you can now turn on **line numbers** for the note editor in the *Editor settings*
  (for [#2091](https://github.com/pbek/QOwnNotes/issues/2091))
  - they cannot be used with turned off `Only use editor width setting in distraction free mode`
  - in the distraction free mode line numbers are turned off as well
  - the colors of the line number of the current line are currently only suited for dark mode 
- under Linux the `Update` button will now only be shown for the AppImage version
  of QOwnNotes, like it should (for [#2176](https://github.com/pbek/QOwnNotes/issues/2176))

## 21.7.0
- added more translation (thank you rawfreeamy, amirkbrpr, whenwesober)

## 21.6.9
- the x86 64bit AppImage is now built with Ubuntu 18.04 instead of Ubuntu 16.04,
  because it was now made possible by [linuxdeployqt](https://github.com/probonopd/linuxdeployqt/issues/340)
- you now are able to **update the AppImage version of QOwnNotes** directly from
  within the application if you have write permissions to the AppImage executable
  (for [#2176](https://github.com/pbek/QOwnNotes/issues/2176))
    - you can now also choose to just download the latest app image in the update dialog
- an attempt was made to fix more builds for older Qt versions
  (for [#2134](https://github.com/pbek/QOwnNotes/issues/2134), thank you @Waqar144)
- added more translation (thank you rawfreeamy, mguerra, amirkbrpr)

## 21.6.8
- the *Insert image* dialog now allows the selection of webp images by default
  (for [#2172](https://github.com/pbek/QOwnNotes/issues/2172))
- when automatic downscaling of images is enabled in the *General settings* the images
  are now not scaled up any more when they are smaller than the chosen resolution
  (for [#2170](https://github.com/pbek/QOwnNotes/issues/2170))
- when automatic downscaling of images is enabled in the *General settings* and a too large
  SVG image is inserted into a note then it now will not get resized automatically, because
  Qt can't resize SVG images and this this would result into a 0 byte file
  (for [#2171](https://github.com/pbek/QOwnNotes/issues/2171))
- an attempt was made to fix the builds for Qt versions older than 5.8 for the fakevim library
  (for [#2134](https://github.com/pbek/QOwnNotes/issues/2134), thank you @Waqar144)

## 21.6.7
- the fakevim library was updated and adapted to work with qmake and cmake and integrated
  into QOwnNotes (for [#2134](https://github.com/pbek/QOwnNotes/issues/2134), thank you @Waqar144)
- added more Spanish, Chinese Traditional, Polish translation thank you hubertm, rawfreeamy)

## 21.6.6
- the monospaced font detection was improved by comparing the width of `iiiii`
  to `WWWWW` (for [#2163](https://github.com/pbek/QOwnNotes/issues/2163))
- added more Italian, Catalan translation (thank you marcoxbresciani, mguerra)

## 21.6.5
- when you are changing the editor font in the settings you will get now a dialog box that
  you might need to restart the application (for [#2163](https://github.com/pbek/QOwnNotes/issues/2163))
- an attempt was made to improve the editor margin calculation by searching
  for the word `mono` in the font family and handling those potentially monospaced
  fonts differently (for [#2163](https://github.com/pbek/QOwnNotes/issues/2163))

## 21.6.4
- fixed a problem when turning on the sub-folders panel after it was turned off
  for note folders with sub-folders (for [#2162](https://github.com/pbek/QOwnNotes/issues/2162))
- the note preview can now also highlight code blocks with uppercase identifiers
  (for [#2161](https://github.com/pbek/QOwnNotes/issues/2161))
- now the shorter <https://web.libera.chat/#qownnotes> link is used for the 
  IRC channel in the chat menu

## 21.6.3
- updated the link of the IRC chat menu entry to Libera.Chat because the Freenode
  IRC channel had a "hostile takeover"
- added more Italian translation (thank you marcoxbresciani)

## 21.6.2
- added more Hungarian, German, Dutch, Persian, Russian translation (thank you
  rawfreeamy, mariiaalt, amirkbrpr)

## 21.6.1
- the code block syntax highlighting for python triple quotes was improved
  (for [#2147](https://github.com/pbek/QOwnNotes/pull/2147), thank you @Waqar144)
- spell checking is now disabled in encrypted notes as long as the encrypted text
  is shown (for [#2156](https://github.com/pbek/QOwnNotes/issues/2156))
- added more Persian translation (thank you amirkbrpr)

## 21.6.0
- the [Web Companion browser extension](https://github.com/qownnotes/web-companion)
  was updated to version 21.6.0 to fix a problem with cut-off security tokens when
  they start with a number (for [#33](https://github.com/qownnotes/web-companion/issues/33))
- fixed build process on Qt 5.3
- added more Persian, Catalan translation (thank you amirkbrpr, mguerra)

## 21.5.5
- the cli parameter `--action <name>` that triggers a menu action after the application
  was started now also works in single application mode when the application is
  started a second time (for [#2143](https://github.com/pbek/QOwnNotes/issues/2143))
    - additionally, desktop actions to create a new note and to open the todo list
      dialog were added to the desktop files for Linux (thank you @fabianski7)
- now the `>` character will also be checked at automatic bracket closing when
  it was already entered (for [#2144](https://github.com/pbek/QOwnNotes/issues/2144))
- added more translation (thank you starship10, baptistou, rawfreeamy)

## 21.5.4
- there now is a new cli parameter `--action <name>` that triggers a menu action after
  the application was started (for [#2143](https://github.com/pbek/QOwnNotes/issues/2143))
    - for example `QOwnNotes --action actionShow_Todo_List` will open the todo
      list dialog after startup
    - this parameter also works with scripted custom actions 
    - for more information please take a look at
      [Trigger menu actions after startup](https://www.qownnotes.org/getting-started/cli-parameters.html#trigger-menu-actions-after-startup)
- added more translation (thank you yanjun_sun, amirkbrpr, rawfreeamy, whenwesober,
  mguerra, rogepix, seanos, gbabin)

## 21.5.3
- there now is a new shortcut <kbd>Ctrl</kbd> + <kbd>⇧</kbd> + <kbd>L</kbd> that opens
  the link dialog on the tab to add a note link (for [#2137](https://github.com/pbek/QOwnNotes/issues/2137))
    - please visit [QOwnNotes shortcuts](https://www.qownnotes.org/getting-started/shortcuts.html)
      for a list of all shortcuts
- version 0.1.10 of the **QOwnNotes Web application** [app.qownnotes.org](https://app.qownnotes.org/)
  was released (for [#2104](https://github.com/pbek/QOwnNotes/issues/2104))
    - heic/heif images can now also be loaded by converting them to jpg images
    - the form elements in the mobile layout were improved
    - you can now set the maximum image height and width
    - you can now set the output image format
    - in case you don't see the version number in the headline of the app page
      you need to clear the browser cache for the `app.qownnotes.org` webpage
- added more Sinhala, Polish translation (thank you helabasa, rawfreeamy)

## 21.5.2
- a problem with showing a `&` character in the tab text of notes was fixed
  (for [#2135](https://github.com/pbek/QOwnNotes/issues/2135))
- a problem with a resetting note sub-folder panel when clicking links to notes
  in sub-folders was fixed (for [#2128](https://github.com/pbek/QOwnNotes/issues/2128))

## 21.5.1
- fixed more Qt 6 build errors (for [#1304](https://github.com/pbek/QOwnNotes/issues/1304), thank you @Waqar144)
- version 0.1.6 of the **QOwnNotes Web application** [app.qownnotes.org](https://app.qownnotes.org/)
  was released that features an image preview and allows rotating images before sending them
  to your **QOwnNotes desktop app** (for [#2104](https://github.com/pbek/QOwnNotes/issues/2104))
    - in case you don't see the version number in the headline of the app page
      you need to clear the browser cache for the `app.qownnotes.org` webpage

## 21.5.0
- a PPA for Ubuntu 21.10 (Impish Indri) was added, a repository for Fedora 34
  was added and another attempt to fix the Fedora 33 build was made
- added more translation (thank you unchase, golublarysa, polkillas, smarquespt,
  rawfreeamy, starship10, marcoxbresciani)

## 21.4.6
- a possible problem with generating the header tree in the navigation panel when
  starting the application minimized was fixed (for [#2110](https://github.com/pbek/QOwnNotes/issues/2110))
- added more Indonesian, Catalan translation (thank you whenwesober, mguerra)

## 21.4.5
- now toggling of checkboxes at the cursor position with <kbd>Ctrl + Space</kbd>
  also supports lists with `* [ ]`, `+ [ ]` and numbered lists, like `1. [ ]` in
  the note text edit (for [#2048](https://github.com/pbek/QOwnNotes/issues/2048))
- added more translation (thank you amirkbrpr, rakekniven, rawfreeamy)

## 21.4.4
- more improvements for the web application integration have been made
  (for [#2104](https://github.com/pbek/QOwnNotes/issues/2104))
    - now a confirmation is requested before an image received from the
      QOwnNotes web application is inserted into the current note
- added more translation (thank you rawfreeamy, rakekniven, marcoxbresciani,
  amirkbrpr, marterra)

## 21.4.3
- more improvements for the web application integration have been made
  (for [#2104](https://github.com/pbek/QOwnNotes/issues/2104))
    - you can now edit the security token manually
    - the link to the QOwnNotes web app was fixed in the settings dialog
    - the web app client service will now check the socket connection periodically
      and will attempt to reconnect if the connection was lost
- fixed the creation of subfolders when clicking a note link that wasn't found
  with a note with subfolders when the current note is also in a subfolder  
  (for [#2106](https://github.com/pbek/QOwnNotes/issues/2106))
- added more Italian, Polish, German translation (thank you rawfreeamy)

## 21.4.2
- when inserting media files into the current note note like via pasting or the QOwnNotes
  web companion browser extension now an attempt will be made to decide the file extension
  from the mime type of the image (for [#2105](https://github.com/pbek/QOwnNotes/issues/2105))
- there now is a new settings page **Web application** to add support for the new
  [app.qownnotes.org](https://app.qownnotes.org/) web service that allows users to
  **send photos** from their **mobile phone** (over the internet) to their local
  **QOwnNotes desktop app** (for [#2104](https://github.com/pbek/QOwnNotes/issues/2104))
    - this feature and the web service is currently in alpha state, feel free to
      test and report what your think in above issue
    - the communication between QOwnNotes and the web service is transport encrypted
    - don't forget to copy your secret token from the settings page to the web
      service to be able to communicate with your local instance of QOwnNotes
    - you can find the source code of the web service at [web-app](https://github.com/qownnotes/web-app)
    - you can also host this web service yourself, the freshly built docker images
      are at [qownnotes-web-app](https://hub.docker.com/repository/docker/pbeke/qownnotes-web-app)
- added more translation (thank you whenwesober, fitoschido, rawfreeamy, mguerra, amirkbrpr)

## 21.4.1
- in vim mode the setup indent size is now also used as shift width
  (for [#2099](https://github.com/pbek/QOwnNotes/issues/2099))
- fixed more Qt 6 build errors (for [#1304](https://github.com/pbek/QOwnNotes/issues/1304), thank you @Waqar144)
- added more translation (thank you amirkbrpr, polkillas, mguerra, rogepix,
  whenwesober, i****myself, rawfreeamy, marcoxbresciani)

## 21.4.0
- added the **new editor color schema** *Gruvbox*
  (for [#2085](https://github.com/pbek/QOwnNotes/issues/2092), thank you @Yoshua-chan)
- there is now a new shortcut <kbd>F8</kbd> to jump to the *navigation panel* that
  jumps to its search field if it is enabled or to the the navigation tree if the
  search field was hidden (for [#2093](https://github.com/pbek/QOwnNotes/issues/2093))
    - please visit [QOwnNotes shortcuts](https://www.qownnotes.org/getting-started/shortcuts.html)
      for a list of all shortcuts
- fixed more Qt 6 build errors (for [#1304](https://github.com/pbek/QOwnNotes/issues/1304))
- added more translation (thank you yoshua-chan, rakekniven, mguerra, rogepix,
  gregory.k, i****myself, gbabin, whenwesober, rawfreeamy)

## 21.3.9
- added the **new editor color schema** *Serious*
  (for [#2085](https://github.com/pbek/QOwnNotes/issues/2085), thank you @marcoXbresciani)
- a possible crash with multiple selected images/attachments are inserted into 
  the current note in the stored images/attachments dialogs was fixed
  (for [#2086](https://github.com/pbek/QOwnNotes/issues/2086))
- added more German, Persian translation (thank you rakekniven, amirkbrpr)

## 21.3.8
- a leading html comment block in a note will now be ignored in the automatic note
  filename generation (for [#2084](https://github.com/pbek/QOwnNotes/issues/2084))
- more improvements to the new *Stored images dialog* have been made
  (for [#2073](https://github.com/pbek/QOwnNotes/issues/2073))
    - added a an *Open image* context menu entry
    - added some tooltip texts
- more improvements to the new *Stored attachments dialog* have been made
  (for [#2073](https://github.com/pbek/QOwnNotes/issues/2073))
    - added some tooltip texts
- added more translation (thank you mguerra, whenwesober, rakekniven, gregory.k,
  rogepix, amirkbrpr)

## 21.3.7
- some button text in more new *Stored attachments dialog* was updated to match
  the text in the *Stored images dialog* (for [#2073](https://github.com/pbek/QOwnNotes/issues/2073))
- more improvements to the new *Stored images dialog* have been made
  (for [#2073](https://github.com/pbek/QOwnNotes/issues/2073))
    - there now is an *Open image* button to open the current image externally
    - there now is an *Open folder* button to open the folder of the current image
- added more Dutch, German, Persian translation (thank you rawfreeamy, rakekniven, amirkbrpr)

## 21.3.6
- more improvements to the new *Stored images dialog* have been made
  (for [#2073](https://github.com/pbek/QOwnNotes/issues/2073))
    - there now is a context menu in the note list
    - fixed a typo in the file list context menu
    - small margin adaptions were made in the UI
- instead of the *Orphaned attachments dialog* there now is the new
  *Stored attachments dialog* which also allows managing non-orphaned attachments
  (for [#2073](https://github.com/pbek/QOwnNotes/issues/2073))
    - you are now also able to search for attachments by name
    - you are now able to rename attachment files with the context menu or by
      pressing <kbd>F2</kbd>
    - there now is a context menu for the attachment file list
    - the file size of the attachments will now also be shown in the tooltip of
      the file list
    - you can now also double-click on an item in the attachment list to insert
      it into the current note
    - there now is a *Refresh* button to reload the attachment file list
    - the filepath of the attachment file is now truncated if there is not enough
      space in the dialog, but you are still able to select and copy the full path
    - if notes are using the current attachment a list of those notes will be shown
    - you can double-click on an item in the note list or use the context menu
      to open the note in a new tab
- the new *Stored images dialog* and *Stored attachments dialog* were moved one
  level up in the *Edit menu* because of their added functionality
  (for [#2073](https://github.com/pbek/QOwnNotes/issues/2073))
- added more translation (thank you gregory.k, afischer211, amirkbrpr)

## 21.3.5
- the warning `fromIccProfile: failed minimal tag size sanity` is now ignored in
  the *Log panel* (for [#2078](https://github.com/pbek/QOwnNotes/issues/2078))
- more improvements to the new *Stored images dialog* have been made
  (for [#2073](https://github.com/pbek/QOwnNotes/issues/2073))
    - you are now able to to decide to also update the image file names in
      all notes after renaming an image
    - the image file list is now sorted alphabetically
    - there now is an informative tooltip for each note in the note list
    - when double-clicking on an item in the note list the note will now be
      opened in a new tab
    - various issues with duplicate notes and refreshing were fixed
- added more translation (thank you rawfreeamy, amirkbrpr, rakekniven,
  gregory.k, mlaarebi)

## 21.3.4
- the filename-clipping of the basename of inserted images was increased to
  200 characters (for [#2073](https://github.com/pbek/QOwnNotes/issues/2073))
- more improvements to the new *Stored images dialog* have been made
  (for [#2073](https://github.com/pbek/QOwnNotes/issues/2073))
    - there now is a *Refresh* button to reload the image file list
    - you can now also double-click on an item in the image list to insert it
      into the current note
    - you can now double-click on an item in the note list to open the note
    - a context menu was added for the image file list
    - you are now able to rename image files with the context menu or by pressing <kbd>F2</kbd>
- added more translation (thank you fitoschido, rawfreeamy, filipedias321,
  amirkbrpr, rakekniven, unchase, mlaarebi)

## 21.3.3
- added a note about creating an app password in the *Nextcloud / ownCloud settings*
- if you now insert an image or attachment directly from a file the original
  file name will be used if the file doesn't exist in the media/attachments
  folder (for [#2073](https://github.com/pbek/QOwnNotes/issues/2073))
    - previously a random string was added to the basename to make it unique
    - if the file already exists a number will be added to the basename of the
      file (like `my-image-1.jpg`) and increased until a filename is available
- instead of the *Orphaned images dialog* there now is the new *Stored images dialog*
  which also allows managing non-orphaned images (for [#2073](https://github.com/pbek/QOwnNotes/issues/2073))
    - you are now also able to search for images by name
    - the file size of the images will now also be shown in the tooltip of the file list
    - if notes are using the current image a list of those notes will be shown
- added more translation (thank you mlaarebi, rawfreeamy, mguerra, rakekniven,
  amirkbrpr, gregory.k)

## 21.3.2
- now the misleading `[Botan Error]  Invalid CBC padding` and `Invalid version or not a cyphertext`
  warnings while checking if a note is encrypted are ignored in the *Log panel*
- now a sha256sum checksum file is generated for the AppImage version of QOwnNotes,
  which is part of the release (for [#2070](https://github.com/pbek/QOwnNotes/issues/2070))
- added more translation (thank you rawfreeamy, mguerra, amirkbrpr)

## 21.3.1
- the status messages in the Nextcloud / ownCloud settings are now translatable
- there now is a new QOwnNotesAPI release 21.3.0 for ownCloud in the ownCloud marketplace
  that added a workaround for the failing username detection in ownCloud 10.3+
  (for [#1725](https://github.com/pbek/QOwnNotes/issues/1725))
- added more translation (thank you carmenc, rawfreeamy)

## 21.3.0
- added more Chinese Simplified translation (thank you warfast)

## 21.2.5
- fixed a problem with exporting notes as PDF or printing them for installations
  of QOwnNotes previous from version 21.2.0 (for [#2062](https://github.com/pbek/QOwnNotes/issues/2062))

## 21.2.4
- fixed an issue with showing the application window with a global shortcut for the
  `Show/Hide application` action (for [#2061](https://github.com/pbek/QOwnNotes/issues/2061))

## 21.2.3
- the toggling of a checkbox at the cursor position has now a higher priority at the
  <kbd>Ctrl + Space</kbd> shortcut (for [#2048](https://github.com/pbek/QOwnNotes/issues/2048))
- the restoring of workspaces after a window resize was improved
  (for [#2056](https://github.com/pbek/QOwnNotes/pull/2056), thank you @fnkabit)

## 21.2.2
- when setting your initial note folder and no Nextcloud or ownCloud folders are found
  now a `Notes` folder is suggested in your home directory (for [#2054](https://github.com/pbek/QOwnNotes/issues/2054))
- added more translation (thank you amirkbrpr, lelaw, rawfreeamy)

## 21.2.1
- you now can also toggle a checkbox at the cursor position with <kbd>Ctrl + Space</kbd>
  in the note text edit (for [#2048](https://github.com/pbek/QOwnNotes/issues/2048))
- added more translation (thank you psardinha, amirkbrpr, anastazja.k1, mguerra,
  rawfreeamy, gregory.k, evildeepblue)

## 21.2.0
- fixed a lot of Qt 5.15 deprecation warnings (for [#1304](https://github.com/pbek/QOwnNotes/issues/1304))
- the **Find action** dialog was further improved (for [#2036](https://github.com/pbek/QOwnNotes/pull/2036), thank you @Waqar144)
- added more translation (thank you rawfreeamy, amirkbrpr, agnieszka_de_en_pl,
  rakekniven, evildeepblue)

## 21.1.8
- Windows 10 pdf export issues are now fixed (for [#2015](https://github.com/pbek/QOwnNotes/issues/2015))
- added more translation (thank you amirkbrpr, zmni, afischer211, rawfreeamy, mguerra, gregory.k)
- two new translations were integrated into the webpage (for [#1859](https://github.com/pbek/QOwnNotes/issues/1859))
  - [QOwnNotes webpage in Hungarian](www.qownnotes.org/hu)
  - [QOwnNotes webpage in Persian](www.qownnotes.org/fa)

## 21.1.7
- fixed line wraps in code blocks in the preview (for [#2017](https://github.com/pbek/QOwnNotes/issues/2017))
- added more French, Dutch, German, Hungarian, Persian translation (thank you rawfreeamy, amirkbrpr, rakekniven)

## 21.1.6
- there now is a new and slick *Action dialog* with fuzzy filtering replacing the old dialog
  (for [#2018](https://github.com/pbek/QOwnNotes/pull/2018), thank you @Waqar144)
  - you can reach the dialog with the shortcut <kbd>Ctrl</kbd> + <kbd>⇧</kbd> + <kbd>A</kbd>
- the links and names of the messenger `Riot` got renamed to `Element` since the project was rebranded
  - you can reach the [QOwnNotes Matrix room](https://app.element.io/#/room/#qownnotes:matrix.org)
    via its new link

## 21.1.5
- the `Use softwrap in note editor and preview` setting now also works for code blocks in
  the preview, the pdf export and for printing (for [#2017](https://github.com/pbek/QOwnNotes/issues/2017))

## 21.1.4
- the `Return` key will now be handled the same as the `Enter` key in the note text edit
  for things like automatically creating list items (for [#2013](https://github.com/pbek/QOwnNotes/issues/2013), thank you @fnkabit)
- added more translation (thank you amirkbrpr, afischer211, rawfreeamy, amyd.lang,
  rakekniven, mguerra, gregory.k)

## 21.1.3
- there now are new scripting commands `mainWindow.getWorkspaceUuidList()`,
  `mainWindow.getWorkspaceUuid()` and `mainWindow.setCurrentWorkspace()` to
  access workspaces and a new hook `windowStateChangedHook` to notice window
  state changes (for [#2007](https://github.com/pbek/QOwnNotes/issues/2007), thank you @fnkabit)
  - for more information please take a look at the
    [MainWindow scripting documentation](https://www.qownnotes.org/scripting/classes.html#mainwindow)
    and the [Hook scripting documentation](https://www.qownnotes.org/scripting/hooks.html#windowstatechangedhook)
  - there also is a new script `Dynamic Workspace` in the script repository that
    you can use to toggle workspaces when you maximize the window

## 21.1.2
- now the extra-selections of the search results in the note text are also
  highlighted when searching for notes in the note search panel
  (for [#1981](https://github.com/pbek/QOwnNotes/issues/1981))
- now the main heading in the experimental note list preview is shown if the main
  headings is allowed to be different than the note name in the current note folder  
  (for [#2004](https://github.com/pbek/QOwnNotes/issues/2004))

## 21.1.1
- now the first occurrence of the search term is found first in the note text,
  when searching for notes in the note search panel (instead of the the last
  occurrence) (for [#1981](https://github.com/pbek/QOwnNotes/issues/1981))
- added repositories for **openSUSE Leap 15.3**
  - please visit the [openSUSE installation page](https://www.qownnotes.org/installation/opensuse.html)
    for instructions
- added more translation (thank you amirkbrpr, afischer211, rawfreeamy, zmni, martastan, silviact)

## 21.1.0
- if you switch between previous and next note tabs now the note text edit is focused
  (for [#2001](https://github.com/pbek/QOwnNotes/issues/2001))
  - the cursor position in the notes is remembered when switching to another tab
    and restored when you come back to the tab
- added more translation (thank you rawfreeamy, amirkbrpr, mguerra, martastan, cberetta, unchase)

## 20.12.9
- the in-note search performance was improved
  (for [#1985](https://github.com/pbek/QOwnNotes/issues/1985), thank you @Waqar144)

## 20.12.8
- fixed an issue with decrypting notes after encrypting for the first time, but
  never saving them (for [#1992](https://github.com/pbek/QOwnNotes/issues/1992))

## 20.12.7
- now storing notes in different cases is also prohibited to prevent more problems on 
  case-insensitive filesystems, like NTFS (for [#1988](https://github.com/pbek/QOwnNotes/issues/1988))
- added more Slovenian, Ukrainian, Urdu (Pakistan), Polish, Sinhala translation
  (thank you radovanm, nelkobud, farhan1985, martastan, helabasa)

## 20.12.6
- different cases in note names for different notes are now prohibited to prevent problems
  on case-insensitive filesystems, like NTFS (for [#1988](https://github.com/pbek/QOwnNotes/issues/1988))

## 20.12.5
- if you are searching for text in the note list now for the search term is also
  searched again inside the notes when you click on other notes in the the note list
  (for [#1980](https://github.com/pbek/QOwnNotes/issues/1980))

## 20.12.4
- the scripting engine was fixed for Windows 10
  (for [#1979](https://github.com/pbek/QOwnNotes/issues/1979))

## 20.12.3
- the Windows releases of QOwnNotes are now built with Qt 5.15.2
  (for [#1976](https://github.com/pbek/QOwnNotes/issues/1976))
- added more Chinese Simplified, Russian translation (thank you unchase,
  ReverseDmitry, iclementine)

## 20.12.2
- the macOS, Snap and AppImage releases of QOwnNotes are now built with Qt 5.15.2
  (for [#1972](https://github.com/pbek/QOwnNotes/issues/1972))
- the setting `Editor/removeTrainingSpaces` with a typo was migrated to `Editor/removeTrailingSpaces`

## 20.12.1
- the `fileCreated` date will now also be set for new notes before the note folder
  is reloaded (for [#117](https://github.com/qownnotes/scripts/issues/117))

## 20.12.0
- you can now also copy in-line code blocks in the note text edit with the context menu
  (for [#1967](https://github.com/pbek/QOwnNotes/issues/1967), thank you @fnkabit)
- improved spellcheck auto-detection performance (for [#1969](https://github.com/pbek/QOwnNotes/pull/1969), thank you @Waqar144)
- added more Persian, Urdu (Pakistan), Portuguese, Brazilian translation (thank you amirkbrpr, Farhan1985, danilosf)

## 20.11.11
- the todo list CalDAV sync can now handle UTC dates from for example macOS Reminders or
  [OpenTasks](https://opentasks.app/) (for [#1966](https://github.com/pbek/QOwnNotes/issues/1966))
- the disk database integrity check error handling was improved
- added more translation (thank you rawfreeamy, MGuerra, gbabin, waqar144)

## 20.11.10
- the note tagging was improved (for [#1960](https://github.com/pbek/QOwnNotes/pull/1960), thank you @Waqar144)
    - the tag tree reloads are much faster now, even with huge trees with 3000+ note tag links
    - the tag tree is now not reloaded if the tag panel is not visible
- there now is a button to check the disk database integrity in the *General settings*,
  which will also fix some problems
- added more translation (thank you Farhan1985, rawfreeamy, filipedias321,
  Tommy903, amirkbrpr, silviact, rakekniven)

## 20.11.9
- the rename/remove subfolder context menu option are now not shown if no subfolder
  was clicked in the subfolder panel (for [#1957](https://github.com/pbek/QOwnNotes/pull/1957), thank you @fnkabit)

## 20.11.8
- the note language auto-detection performance was improved
  (for [#1954](https://github.com/pbek/QOwnNotes/pull/1954), thank you @Waqar144)
- the falsely shown note subfolder menu when clicking on an empty space in the
  note list is now disabled (for [#1955](https://github.com/pbek/QOwnNotes/pull/1955))

## 20.11.7
- there now are new scripting commands `script.noteTextEditSetCursorPosition()`
  and `script.noteTextEditCursorPosition()` to set the text cursor in the note
  text edit to a certain position and to get the current position
  (for [#1950](https://github.com/pbek/QOwnNotes/issues/1950))
    - for more information please take a look at the
      [scripting documentation](https://www.qownnotes.org/scripting/methods-and-objects.html#set-the-text-cursor-in-the-note-text-edit-to-a-certain-position)
- added more Greek translation (thank you Gregory.K)

## 20.11.6
- the setting of an empty note folder name is now prevented
  (for [#1949](https://github.com/pbek/QOwnNotes/pull/1949), thank you @fnkabit)

## 20.11.5
- the html heading code highlighting was fixed (for [#1933](https://github.com/pbek/QOwnNotes/issues/1933), thank you @fnkabit)
- added more Spanish translation (thank you polkillas)

## 20.11.4
- the different *Jump to* actions in the *Windows menu* are now disabled if the panels
  they jump to are disabled (for [#1941](https://github.com/pbek/QOwnNotes/issues/1941), thank you @fnkabit)
- the link to the `Markdown cheatsheet` to the new [Markdown](https://www.qownnotes.org/getting-started/markdown.html)
  documentation was updated

## 20.11.3
- fixed a bug where after deselecting notes from a multiple selection, the last selected
  note was not the current node (for [#1939](https://github.com/pbek/QOwnNotes/issues/1939), thank you @fnkabit)
- QOwnNotes is now available for **Ubuntu 21.04 Hirsute Hippo** via our PPA at Ubuntu Launchpad
    - please visit the [Ubuntu installation page](https://www.qownnotes.org/installation/ubuntu.html)
      for instructions
- added more Bengali and Spanish translation (thank you Basudeb_Dhaka, richarson)

## 20.11.2
- a preview code block highlighting issue was fixed
  (for [#1933](https://github.com/pbek/QOwnNotes/issues/1933), thank you @fnkabit)
- the *distraction free mode* is now disabled when no note is selected
  (for [#1936](https://github.com/pbek/QOwnNotes/pull/1936), thank you @fnkabit)
- a new scripting hook `insertAttachmentHook` was added that is executed when an
  attachment file is inserted into the current note (for [#1645](https://github.com/pbek/QOwnNotes/issues/1645))
    - please take a look at the
      [insertAttachmentHook documentation](https://www.qownnotes.org/scripting/hooks.html#insertattachmenthook)
      for more information

## 20.11.1
- a possibly not updating note count when notes were removed was fixed
  (for [#1934](https://github.com/pbek/QOwnNotes/pull/1934), thank you @fnkabit)

## 20.11.0
- there now is a new scripting class `NoteSubFolderApi` to fetch note subfolders and their notes
  (for [#1165](https://github.com/pbek/QOwnNotes/issues/1165), thank you @fnkabit)
    - for more information please take a look at the
      [scripting documentation](https://www.qownnotes.org/scripting/classes.html#notesubfolder)
- added more translation (thank you filipedias321, MGuerra, rakekniven, unchase,
  zmni, gbabin, mlimacarlos, amirkbrpr)

## 20.10.11
- in the issue assistant there now are tooltips to show how many characters are needed
  at minimum and it's not mandatory to post the log-output for issues any more
- there now is a software repository for **Fedora 33**
    - please visit the [Fedora installation page](https://www.qownnotes.org/installation/fedora.html#fedora-33)
      for instructions
- added more Catalan translation (thank you MGuerra)

## 20.10.10
- there now is a new scripting command `mainWindow.reloadCurrentNoteByNoteId()`
  that you can use to reload the current note, for example because the path changed
    - for more information please take a look at the
      [scripting documentation](https://www.qownnotes.org/scripting/classes.html#mainwindow)
- added more translation (thank you unchase, Tommy903, Zack-83, mlimacarlos, amirkbrpr, gbabin, rakekniven)

## 20.10.9
- you can now also choose to create the note of broken note links when clicking
  on the note in the note preview (for [#1698](https://github.com/pbek/QOwnNotes/issues/1698), thank you @fnkabit)
    - if note subfolders are disabled for your note folder you will now also get
      a proper error message when trying to create notes in subfolders 
- added more translation (thank you gbabin, rakekniven, mlimacarlos, Tommy903, amirkbrpr, unchase)

## 20.10.8
- you can now choose to create the note of broken note links when you <kbd>Ctrl</kbd> + Click
  on the note in the note text edit (for [#1698](https://github.com/pbek/QOwnNotes/issues/1698), thank you @fnkabit)
- the legacy update service is up again for older versions of QOwnNotes
  (see [Legacy update service up again](https://www.qownnotes.org/blog/2020-10-26-Legacy-update-service-up-again.html))
- added more translation (thank you pgrules7, rawfreeamy, VasilikiBotsou, danilosf)

## 20.10.7
- it is now also possible to move text in the note text edit via drag and drop
  (for [#1888](https://github.com/pbek/QOwnNotes/issues/1888), thank you @fnkabit)

## 20.10.6
- you can now also change the alignment of your markdown table by setting it in
  the table header and pressing <kbd>Ctrl</kbd> + <kbd>Space</kbd>
  (for [#1905](https://github.com/pbek/QOwnNotes/issues/1905), thank you @fnkabit)

## 20.10.5
- the `All notes` count in the tags panel showing the count for the previously
  selected item was fixed (for [#1903](https://github.com/pbek/QOwnNotes/issues/1903), thank you @Waqar144)
- indented code blocks were fixed when previous line is not a heading
  (for [#1908](https://github.com/pbek/QOwnNotes/issues/1908), thank you @Waqar144)
- improved auto-removal of `*` and tick characters (for [#105](https://github.com/pbek/qmarkdowntextedit/issues/105), thank you @Waqar144)
- inline code spans will now not be spell checked (for [#1911](https://github.com/pbek/QOwnNotes/issues/1911), thank you @Waqar144)
- added more translation (thank you rawfreeamy, danilosf)

## 20.10.4
- the update service now makes use of the new api on https://api.qownnotes.org/
  to check for application updates (for [#1900](https://github.com/pbek/QOwnNotes/issues/1900))
    - the source code to the api is available on https://github.com/qownnotes/api
    - the new api also supports requests to the old api for backward compatibility  
- added more Chinese Traditional, Greek, Chinese Simplified, Hungarian, Dutch
  translation (thank you rawfreeamy, VasilikiBotsou)

## 20.10.3
- the links in the application were updated for the new webpage
- added more translation (thank you MGuerra, gbabin, tiwi90, unchase, mlimacarlos,
  amirkbrpr, fitoschido, diego2214, rakekniven, MartaStan)

## 20.10.2
- the url of the update service was changed to make it possible to detach the
  api from the webpage in the future

## 20.10.1
- you can now also search only in the note name (for [#1890](https://github.com/pbek/QOwnNotes/issues/1890))
    - `n:book` or `name:book` will search for notes with `book` just in the note
      name or file name
    - for more information about searching please take a look at the new
      [searching documentation](https://www.qownnotes.org/getting-started/searching.html)

## 20.10.0
- the feature to jump to the start/end of the document when in the first/last line
  was improved to detect the visible line (for [#1892](https://github.com/pbek/QOwnNotes/issues/1892))
- added more translation (thank you zmni, gbabin, silviact, VasilikiBotsou, filipedias321,
  unchase, mlimacarlos, cberetta, richarson, rakekniven, i****myself, amirkbrpr)

## 20.9.11
- the web socket server for the QOwnNotes browser extension is now only listening
  on localhost any more (for [#1885](https://github.com/pbek/QOwnNotes/issues/1885))
- more tagging system speed improvements were made (for [#943](https://github.com/pbek/QOwnNotes/issues/943), thank you @Waqar144)
- added more Greek, Slovenian translation (thank you VasilikiBotsou, silviact)

## 20.9.10
- the amd64 stable release snap of QOwnNotes will now be built by GitHub Actions to prevent
  two desktop files being deployed with the snap (for [#1019](https://github.com/pbek/QOwnNotes/issues/1019))
    - the other architectures will still be built by Ubuntu Launchpad
- fixed the incorrect setExpandTab setting for fakevim (for [#1875](https://github.com/pbek/QOwnNotes/issues/1875), thank you @Waqar144)
- the speed of the tagging system was slightly improved (for [#943](https://github.com/pbek/QOwnNotes/issues/943), thank you @Waqar144)

## 20.9.9
- tab stop width and tab versus space settings are now also respected in fakevim mode
  (for [#1875](https://github.com/pbek/QOwnNotes/issues/1875), thank you @Waqar144)
- added a snap build to the release (for [#1019](https://github.com/pbek/QOwnNotes/issues/1019))

## 20.9.8
- resetting the note text font size will now not move the text cursor
  (for [#1869](https://github.com/pbek/QOwnNotes/issues/1869))
- the QOwnNotesAPI Nextcloud app  was updated to version 20.9.0 to support
  Nextcloud 20/21 (for [#35](https://github.com/pbek/qownnotesapi/issues/35))
- the [Web Companion browser extension](https://github.com/qownnotes/web-companion)
  was updated to version 20.9.4 to fix a problem with opening all bookmarks in new tabs
- added more Bengali translation (thank you Basudeb_Dhaka)

## 20.9.7
- increasing and decreasing the editor font size with the mouse wheel now also works if
  not on beginning or end of note (for [#1870](https://github.com/pbek/QOwnNotes/issues/1870))
- the note text bookmark slot 0 was sacrificed to allow <kbd>Ctrl</kbd> + <kbd>0</kbd>
  to reset the note text font size (for [#1869](https://github.com/pbek/QOwnNotes/issues/1869))

## 20.9.6
- you now can increase and decrease the editor font size also with the mouse wheel
  while holding <kbd>Ctrl</kbd> pressed (for [#1870](https://github.com/pbek/QOwnNotes/issues/1870))
- fixed building snap stable release (for [#1019](https://github.com/pbek/QOwnNotes/issues/1019))
- code block highlighting was improved (for [#1873](https://github.com/pbek/QOwnNotes/issues/1873), thank you @Waqar144) 
- added more Greek translation (thank you VasilikiBotsou)

## 20.9.5
- fixed removal of the original desktop file while building a snap with snapcraft to prevent
  two desktop files being deployed with the snap (for [#1019](https://github.com/pbek/QOwnNotes/issues/1019))
- a great amount of translations into Chinese Simplified were fixed so the menu items are
  correctly shown again (for [#1866](https://github.com/pbek/QOwnNotes/issues/1866), thank you @iclementine)
- added more translation (thank you pehtranslator, VasilikiBotsou, rawfreeamy)

## 20.9.4
- the original desktop file is now removed while building a snap with snapcraft to prevent
  two desktop files being deployed with the snap (for [#1019](https://github.com/pbek/QOwnNotes/issues/1019))
- added more translation (thank you rawfreeamy, diego2214, silviact)

## 20.9.3
- fixed the status message `No equation was found in front of the cursor` when solving equations
- the deployment of the original desktop file is now prohibited when building a
  snap with snapcraft to prevent two desktop files being deployed with the snap
  (for [#1019](https://github.com/pbek/QOwnNotes/issues/1019))
- added more translation (thank you Lubinka, muhannad.albayk.jaam, khoirul, rawfreeamy,
  MartaStan, Zamalor, VasilikiBotsou, Basudeb_Dhaka, Iva16, Farhan1985, amirkbrpr)

## 20.9.2
- the desktop file of the snap release of QOwnNotes was renamed to `qownnotes.desktop`
  to prevent problems with Gnome (for [#1019](https://github.com/pbek/QOwnNotes/issues/1019))
- added more translation (thank you amirkbrpr, Farhan1985, MartaStan, cberetta,
  khoirul, pehtranslator, Iva16, diego2214, rawfreeamy, VasilikiBotsou, Basudeb_Dhaka,
  MGuerra, IvonkaTran, I_L_D_A_R, Aya_Fouad, muhannad.albayk.jaam, je1311, danilosf,
  filipedias321, rakekniven, gbabin)

## 20.9.1
- the action `Previous note` was renamed to to `Move up in note list` and the 
  action `Next note` to `Move down in note list` to make the meaning of the
  actions more clear (for [#1862](https://github.com/pbek/QOwnNotes/issues/1862))
- added more translation (thank you amirkbrpr, muhannad.albayk.jaam, pehtranslator,
  Farhan1985, abdo25, MGuerra, rafacerutti.translator, Gregory.K, danilosf,
  secondjay, Iva16, mdirice, CarmenC)

## 20.9.0
- a tooltip in the *Settings dialog* was changed to make it more comprehensive
- a lot of translation and proofreading took place (thank you amirkbrpr, CarmenC,
  Aya_Fouad, DavidLinek, i****myself, pehtranslator, vaniamartins, AhmedElsayed,
  abdo25, rawfreeamy, wribeiro86, matheussantosaraujo12, nandhpc, MartaStan,
  christopher.ruppenthal1997, hamza.0404, brittonando, manowski.pawel, Tommy903,
  natallia.andranaki, Zamalor, rafacerutti.translator, secondjay, Basudeb_Dhaka,
  Farhan1985, IvonkaTran, hitoshi2544, VasilikiBotsou, rakekniven, gbabin, je1311,
  Koduja, tiwi90, Suci_R)

## 20.8.10
- the QOwnNotes AppImage for Linux and the DMG for macOS are now built with Qt 5.15.1
    - this also fixed a script execution problem under macOS, because QtQml wasn't found

## 20.8.9
- fixed a problem with pressing `Backspace` after a `'` in lists
  (for [#1857](https://github.com/pbek/QOwnNotes/issues/1857), thank you @Waqar144)

## 20.8.8
- fixed a problem where you could loose changes to an encrypted note if you switch
  to another note and back to the encrypted note and then immediately edit it again
  before the encrypted note was stored to disk (for [#1858](https://github.com/pbek/QOwnNotes/issues/1858))

## 20.8.7
- the note scripting api will now not attempt to automatically decrypt a note in the background because
  this would happen every time the current note changes (for [#104](https://github.com/qownnotes/scripts/issues/104))

## 20.8.6
- the `readFromFile` script command now has a new parameter `codec` to specify the encoding
  of the file (for [#1817](https://github.com/pbek/QOwnNotes/issues/1817))
    - for more information please take a look at the
      [scripting documentation](https://www.qownnotes.org/scripting/methods-and-objects.html#reading-text-from-a-file)
    - the script `dictionary-autocomplete` in the script repository was updated to use this parameter
      in a script setting to set the encoding of the dictionary file

## 20.8.5
- fixed a regression from 20.8.4 that causes to crash the application when right-clicking
  on an empty area in the note list (for [#1854](https://github.com/pbek/QOwnNotes/issues/1854))

## 20.8.4
- improvements where made to the **note / subfolder tree mode**
  (for [#790](https://github.com/pbek/QOwnNotes/issues/790))
    - the notes of the note folder root are now shown
    - if your right-click a sub-folder the sub-folder context menu is now shown
- added more translation (thank you MGuerra, richarson, rakekniven, Gregory.K,
  rawfreeamy, unchase, filipedias321, i****myself, tiwi90, mlimacarlos, Utkirbek, gbabin)

## 20.8.3
- added the **new editor color schema** *Breeze dark*
  (for [#1847](https://github.com/pbek/QOwnNotes/pull/1847), thank you @fabianski7)
- added some speed improvements (for [#1848](https://github.com/pbek/QOwnNotes/pull/1848), thank you @Waqar144)
- there now is an option to disable the note count in the tags panel to improve the
  performance on slower hardware (for [#943](https://github.com/pbek/QOwnNotes/issues/943), thank you @Waqar144)
- added more Greek, Portuguese, Brazilian Portuguese translation (thank you Gregory.K, filipedias321, i****myself)

## 20.8.2
- Bold and italic highlighting now also works between inline code blocks in the note editor
  (for [#1842](https://github.com/pbek/QOwnNotes/issues/1842), thank you @Waqar144)

## 20.8.1
- you can now also turn off/on the spellchecking inside encrypted notes
  (for [#1841](https://github.com/pbek/QOwnNotes/issues/1841))
- you can now also use <kbd>Tab</kbd> or <kbd>Enter</kbd> to switch from the
  note list to the note edit panel if note renaming in the note list is enabled
  (for [#1837](https://github.com/pbek/QOwnNotes/issues/1837))
- added more Brazilian Portuguese, German translation (thank you i****myself, rawfreeamy)

## 20.8.0
- added new scripting methods `cacheDir` and `clearCacheDir`
  (for [#1838](https://github.com/pbek/QOwnNotes/pull/1838), thank you @r00tr4v3n)
    - please take a look at the documentation for
      [cacheDir](https://www.qownnotes.org/scripting/methods-and-objects.html#creating-a-cache-directory)
      and [clearCacheDir](https://www.qownnotes.org/scripting/methods-and-objects.html#clearing-a-cache-directory)
      for more information
- added more translation (thank you rawfreeamy, i****myself, mlimacarlos,
  rakekniven, Zack-83, Gregory.K)

## 20.7.9
- if you minimized or maximized the application window before entering full-screen mode that state will now
  be restored after exiting full-screen mode (for [#1835](https://github.com/pbek/QOwnNotes/issues/1835))
- you are now able to also delete trashed notes on your server in the trash dialog
  (for [#1833](https://github.com/pbek/QOwnNotes/issues/1833))
- added more translation (thank you MGuerra, rawfreeamy, richarson, unchase)

## 20.7.8
- fixed a possible crash when encrypting notes with less than two lines
  (for [#1831](https://github.com/pbek/QOwnNotes/issues/1831))

## 20.7.7
- the name and description fields that got moved to the 2nd tab by accident were fixed in the
  note link dialog (for [#1828](https://github.com/pbek/QOwnNotes/issues/1828#issuecomment-663835422))
- fixed warnings when encrypting notes if application is built with Qt 5.14 or newer
  (for [#1832](https://github.com/pbek/QOwnNotes/issues/1832))
- changes to individual notes are now also ignored if the setting to ignore all
  external note folder changes is active
- added more Brazilian Portuguese, German, Italian translation (thank you i****myself, rakekniven, Zack-83)

## 20.7.6
- the note link dialog can now be resized smaller than the height needed for its 2nd tab
  (for [#1828](https://github.com/pbek/QOwnNotes/issues/1828#issuecomment-663217669))
- added **Yiddish translation** (a big thank you to Fabijenna)
    - visit [QOwnNotes Translation](http://docs.qownnotes.org/en/latest/contributing/translation/)
      if you want to make QOwnNotes available in more languages or help with the
      current translation
- added more Greek, Hungarian, Brazilian Portuguese, Russian, French translation
  (thank you Gregory.K, rawfreeamy, mlimacarlos, unchase, gbabin)

## 20.7.5
- **releases for Windows and macOS** are now built with **GitHub Actions in 64 bit**
  (for [#1828](https://github.com/pbek/QOwnNotes/issues/1828))
    - there is a macOS DMG with Qt 5.14.2 and Windows ZIP file with Qt 5.13.2 in every release
    - the updating api was adapted to use the new releases, please report any
      troubles with updating or installing
    - in addition there is a Linux AppImage 64 Bit with Qt 5.14.2

## 20.7.4
- added the **new editor color schema** *Dracula*
  (for [#1823](https://github.com/pbek/QOwnNotes/issues/1823), thank you @TheChiefMeat)
- added **Thai translation** (a big thank you to Fabijenna)
    - visit [QOwnNotes Translation](http://docs.qownnotes.org/en/latest/contributing/translation/)
      if you want to make QOwnNotes available in more languages or help with the
      current translation

## 20.7.3
- fixed a possible crash when showing note versions of a note with note links
  (for [#1805](https://github.com/pbek/QOwnNotes/pull/1805))
- more minor fixes for preview syntax highlighter where made
  (for [#1821](https://github.com/pbek/QOwnNotes/pull/1821), thank you @Waqar144)
- added more translation (thank you Fabijenna, MGuerra, rakekniven, mlocati, unchase, gbabin)

## 20.7.2
- scripts in the script repository are now checked for updates on application
  start (for [#1820](https://github.com/pbek/QOwnNotes/issues/1820))
    - if an update was found you'll be asked if you want to open the script
      update dialog 
- more highlighting improvements were made in the note text edit
  (for [#122](https://github.com/pbek/qmarkdowntextedit/pull/122), thank you @Waqar144)
- added more translation (thank you manuelarodsilva, MGuerra, Gregory.K,
  rakekniven, unchase, mlimacarlos, richarson, Zack-83)

## 20.7.1
- if the filename that is generated from the headline of a note turns out to be
  empty it will now be substituted by `Note.md` (for [#1813](https://github.com/pbek/QOwnNotes/issues/1813))
    - if that file already exists in the current folder `Note 1.md` will be
      used (and so on) 
- more C++11/20 keywords and some types were added to the C++ highlighter
  (for [#120](https://github.com/pbek/qmarkdowntextedit/pull/120), thank you @data-man)
- there now is a context menu for note tabs which allows you to toggle the note stickiness
  and to close all other note tabs (for [#1814](https://github.com/pbek/QOwnNotes/issues/1814))
- added more Italian, German, French translation (thank you mlocati, rakekniven, gbabin, Zack-83)

## 20.7.0
- you can now press <kbd>Ctrl</kbd> + <kbd>U</kbd> to format the selected text underline
  (for [#1811](https://github.com/pbek/QOwnNotes/issues/1811))
- the QOwnNotes Gentoo overlay was added to the official Gentoo repository for
  easier installation (for [#1807](https://github.com/pbek/QOwnNotes/issues/1807))
    - please visit the [Gentoo installation page](https://www.qownnotes.org/installation#Gentoo)
      for the updated instructions
- added more translation (thank you Fabijenna)

## 20.6.11
- the *Note dialog* for opening notes in a new window can now also use custom
  preview fonts (for [#1806](https://github.com/pbek/QOwnNotes/issues/1806))

## 20.6.10
- incorrect headings like `#headline` will now not be rendered as heading in the preview
 (for [#1802](https://github.com/pbek/QOwnNotes/issues/1802), thank you @Waqar144)
- the note tabs will now be hidden in distraction free mode
- the *Note dialog* for opening notes in a new window now remembers whether it
  showed the note text or the preview last time so you can use the dialog as
  preview in the *Distraction free mode* (for [#1804](https://github.com/pbek/QOwnNotes/issues/1804))
- added more translation (thank you ardakilicdagi, MGuerra, rakekniven, richarson)

## 20.6.9
- fixed unwanted `def foo[A](a: A)` note link generation in code blocks in the preview
  (for [#1794](https://github.com/pbek/QOwnNotes/issues/1794), thank you @Waqar144)
- you can now turn off support for the QOwnNotesAPI Nextcloud/ownCloud app
- added more translation (thank you unchase, gbabin, Gregory.K, tiwi90)

## 20.6.8
- the creation date is now restored when a note gets renamed under Windows
  (for [#1743](https://github.com/pbek/QOwnNotes/issues/1743))

## 20.6.7
- you can now remove a tabulator list indent of a new list item with `Shift + Tab`
  if the setting `Use tab character instead of spaces` is disabled
  (for [#1793](https://github.com/pbek/QOwnNotes/issues/1793))

## 20.6.6
- fixed note links if note is inside a sym-linked subfolder that lies outside
  the note folder (for [#1792](https://github.com/pbek/QOwnNotes/issues/1792))

## 20.6.5
- fixed preview of inserted images from another drive without copying them to the
  media library on Windows if filename has spaces in it (for [#1787](https://github.com/pbek/QOwnNotes/issues/1787))

## 20.6.4
- fixed string not being highlighted if there is a char before it
  (for [#1786](https://github.com/pbek/QOwnNotes/pull/1786), thank you @Waqar144)

## 20.6.3
- fixed preview of inserted images from another drive without copying them to the
  media library on Windows (for [#1785](https://github.com/pbek/QOwnNotes/issues/1785))
- fixed quote behavior when cursor is at word start (for [#118](https://github.com/pbek/qmarkdowntextedit/pull/118),
  thank you @Waqar144)
- added more Greek translation (thank you Gregory.K)

## 20.6.2
- if `Use editor highlighting styles also for the preview` is turned on now the
  preview fonts will be disabled at all and will not leak into the preview any more
- fixed possible crash if no screen is present

## 20.6.1
- fixed a problem with empty shortcuts if the menubar is disabled and settings
  were stored in the *Settings dialog* (for [#1708](https://github.com/pbek/QOwnNotes/issues/1708))

## 20.6.0
- fixed problem with adding QOwnNotes installed from snap as favorite
  in Ubuntu 20.04 (for [#1713](https://github.com/pbek/QOwnNotes/issues/1713))
- fixed markdown heading highlighting issue (for [#1777](https://github.com/pbek/QOwnNotes/issues/1777), thank you @Waqar144)
- the scripting commands `preNoteToMarkdownHtmlHook` and `preNoteToMarkdownHtmlHook`
  now have a new parameter `forExport` (for [#1769](https://github.com/pbek/QOwnNotes/pull/1769), thank you @r00tr4v3n)
    - please take a look at the documentation for
      [preNoteToMarkdownHtmlHook](https://www.qownnotes.org/scripting/hooks.html#prenotetomarkdownhtmlhook)
      and [noteToMarkdownHtmlHook](https://www.qownnotes.org/scripting/hooks.html#notetomarkdownhtmlhook)
      for more information

## 20.5.13
- improved autocompletion for "bold" and "italic" markdown markers
  (for [#1753](https://github.com/pbek/QOwnNotes/issues/1753), thank you @Waqar144)
- fixed hex numbers highlighting (for [#117](https://github.com/pbek/qmarkdowntextedit/pull/117), thank you @Waqar144)
- added more translation (thank you Zack-83, mlimacarlos, MGuerra, rakekniven)

## 20.5.12
- fixed copying and moving of notes into sub-folders and note-folders and other
  things that were broken by [#1749](https://github.com/pbek/QOwnNotes/pull/1749) 
  (for [#1764](https://github.com/pbek/QOwnNotes/issues/1764)
  and [#1743](https://github.com/pbek/QOwnNotes/issues/1743))
- the scripting hook `noteToMarkdownHtmlHook` will now be called after modifications
  to images in the note are made (for [#1766](https://github.com/pbek/QOwnNotes/issues/1766))
- the `QHotkey` library was updated
- added more translation (thank you unchase, richarson, gbabin, Fabijenna)

## 20.5.11
- some startup performance improvements were made (for [#1755](https://github.com/pbek/QOwnNotes/pull/1755), thank you @Waqar144)
- markdown checkbox list items now have a square as list-style in the preview
  (for [#1761](https://github.com/pbek/QOwnNotes/issues/1761))
- the scripting command `startDetachedProcess` now has new parameters to define
  a callback identifier that can later be implemented via the new scripting hook
  `onDetachedProcessCallback` to be able to handle the output of a detached process
  in a script (for [#1746](https://github.com/pbek/QOwnNotes/issues/1746), thank you @r00tr4v3n)
    - please take a look at the documentation for
      [startDetachedProcess](https://www.qownnotes.org/scripting/methods-and-objects.html#starting-an-external-program-in-the-background)
      and [onDetachedProcessCallback](https://www.qownnotes.org/scripting/hooks.html#ondetachedprocesscallback)
      for more information
- if the tab of current note was clicked now the subfolder of the note is
  activated if that is needed to show the note in the note list
  (for [#1754](https://github.com/pbek/QOwnNotes/issues/1754))
- you can now enable a checkbox in the *Editor settings* to automatically
  **remove trailing spaces** from the current note text when the note is stored
  to disk (for [#1759](https://github.com/pbek/QOwnNotes/issues/1759))
    - you will only see the changes once the note edit panel is refreshed to
      prevent cursor jumping while you are editing the note
- added more translation (thank you unchase, Gregory.K, Fabijenna, rakekniven, gbabin)

## 20.5.10
- fixed file links interpreted in code blocks (for [#1748](https://github.com/pbek/QOwnNotes/issues/1748), thank you @Waqar144)
- migrated from `sub-folder` to `subfolder` in menu items, documentation, comments and general text
- added more translation (thank you Fabijenna)

## 20.5.9
- added a **new editor color schema** *Solarized Dark*
  (for [#1742](https://github.com/pbek/QOwnNotes/issues/1742), thank you @sauces1313)
- the note files are now actually renamed if the headline of a note changes instead
  of a new note file being created (for [#1743](https://github.com/pbek/QOwnNotes/issues/1743))
    - if if the note file was successfully renamed now a copy will not be move
      to the local trash
- added more Spanish translation (thank you richarson)

## 20.5.8
- you can now copy an image to the clipboard by right-clicking on it in the preview
  (for [#1741](https://github.com/pbek/QOwnNotes/issues/1741))
    - this works for local and as well for remote images
- the binary translation files were removed from the git repository, they are now
  built in all build systems (for [#1744](https://github.com/pbek/QOwnNotes/issues/1744))
- added more French, German, Russian, Catalan, Italian, Bengali translation
  (thank you gbabin, rakekniven, unchase, MGuerra, tiwi90, Oymate)

## 20.5.7
- there now is a new method `insertHtmlAsMarkdownIntoCurrentNote` for `mainWindow`
  in the scripting engine that inserts html in the current note as markdown
  (for [#1740](https://github.com/pbek/QOwnNotes/issues/1740))
    - this method also downloads remote images and transforms `data:image` urls
      to local images stored in the media directory
    - for more information please take a look at the
      [Mainwindow scripting documentation](https://www.qownnotes.org/scripting/classes.html#mainwindow)
- added a new scripting hook `websocketRawDataHook` that is called when data is
  sent from the QOwnNotes Web Companion browser extension via the web browser's
  context menu (for [#1740](https://github.com/pbek/QOwnNotes/issues/1740))
    - you need the QOwnNotes Web Companion 20.5.0 or higher for this to work
    - this works with either the `send page` and `send selection` feature of the
      web companion
    - if a hook implementation is found in a script no new note will be created
    - take a look at the
      [handleNoteNameHook documentation](https://www.qownnotes.org/scripting/hooks.html#websocketrawdatahook)
      for more information
- note tabs will now be reloaded correctly if the note-folder is reloaded
  (for [#1720](https://github.com/pbek/QOwnNotes/issues/1720))
    - all note assignments were previously lost because reloading the note-folder
      causes all note ids to be different than before
- fixed a possible crash when restoring note tabs (for [#1720](https://github.com/pbek/QOwnNotes/issues/1720))
- added new scripting methods `fileExists` and `readFromFile` and a new parameter
  `createParentDirs` for method `writeToFile` (for [#1736](https://github.com/pbek/QOwnNotes/issues/1736), thank you @r00tr4v3n)
    - for more information please take a look at the
      [scripting documentation](https://www.qownnotes.org/scripting/methods-and-objects.html)
- there now is a new sub-menu for custom actions from scripts in the note-edit
  context menu to prevent that too many menu items are shown on the first level
  of the menu (for [#1737](https://github.com/pbek/QOwnNotes/issues/1737), thank you @r00tr4v3n)
- added more Bengali, Brazilian Portuguese, French translation (thank you Oymate,
  Fabijenna, gbabin, bepolymathe)

## 20.5.6
- leaving the distraction free mode with <kbd>Escape</kbd> now doesn't cause you
  to enter the read-only mode automatically, you have to press <kbd>Escape</kbd>
  a 2nd time to enter the read-only mode (for [#1733](https://github.com/pbek/QOwnNotes/issues/1733))
- fixed a possible focus problem when entering the distraction free mode
  (for [#1733](https://github.com/pbek/QOwnNotes/issues/1733))
- fixed a possible crash in code block preview (for [#1731](https://github.com/pbek/QOwnNotes/pull/1731),
  thank you @Waqar144)
- added more translation (thank you Fabijenna)

## 20.5.5
- now a tooltip is shown for note tabs to guide you how to stick and unstick notes
  (for [#1730](https://github.com/pbek/QOwnNotes/issues/1730))
- the note tab sticky status will now be shown correctly if a note is double-clicked
  in the note list (for [#1730](https://github.com/pbek/QOwnNotes/issues/1730))
- added more translation (thank you rakekniven,
  gbabin, unchase, mlocati, MGuerra, Oymate, Fabijenna, mlimacarlos)

## 20.5.4
- you can now stick a note to a tab with the new action `Toggle note stickiness of current tab`
  in the *Note / Navigation* menu or by double-clicking on the tab
  (for [#1730](https://github.com/pbek/QOwnNotes/issues/1730))
    - this will cause a new tab being opened automatically if you click on
      another note while in this tab
    - the stickiness status will get also restored when the note folder is
      switched or the application is started
- if the note of the current tab is renamed now the tab also will be renamed
  (for [#1720](https://github.com/pbek/QOwnNotes/issues/1720))

## 20.5.3
- fixed a problem with restoring note tabs of notes from sub-subfolders
  (for [#1726](https://github.com/pbek/QOwnNotes/issues/1726))
- fixed `Show note in file manager` for file manager Caja on Linux
  (for [#1727](https://github.com/pbek/QOwnNotes/issues/1727), thank you @r00tr4v3n)
- added more debug output in case note folder was not found on server
  (for [#1725](https://github.com/pbek/QOwnNotes/issues/1725))
- added more Spanish, Catalan, French, German, Italian, Russian translation
  (thank you richarson, MGuerra, Fabijenna, gbabin, rakekniven, mlocati, unchase)

## 20.5.2
- **open note tabs are now restored** when the note folder is switched or the
  application is started (for [#1726](https://github.com/pbek/QOwnNotes/issues/1726))
    - restoring the last open note when switching note folders was also improved
    - you can turn note tab restoring off in the *Panels settings*
- added more Bengali translation (thank you Oymate)

## 20.5.1
- new tabbing shortcuts were added (for [#1720](https://github.com/pbek/QOwnNotes/issues/1720))
    - you can now jump to the previous or next note tab with <kbd>Alt + PgUp</kbd>
      and <kbd>Alt + PgDn</kbd>
    - you can now close the current note tab with <kbd>Ctrl + W</kbd>
    - you can now open a new note in a new tab with <kbd>Ctrl + Shift + N</kbd>
    - do keep in mind that you can change or disable all shortcuts in the *Shortcuts settings*
    - please visit [QOwnNotes shortcuts](https://www.qownnotes.org/getting-started/shortcuts.html)
      for a list of all shortcuts
- some glitches with duplicate tabs when the application was jumping to notes
  while the note was already open as a tab were fixed (for [#1720](https://github.com/pbek/QOwnNotes/issues/1720))
- tabs of removed notes are now also removed automatically (for [#1720](https://github.com/pbek/QOwnNotes/issues/1720))
- there now is a software repository for **Fedora 32**
    - please visit the [Fedora installation page](https://www.qownnotes.org/installation#Fedora)
      for instructions
- added more Russian, German, Catalan translation (thank you unchase, rakekniven, MGuerra)

## 20.5.0
- you now can right-click on a note in the note list and click `Open note in tab`
  to **open the note in a tab** in the note edit panel (for [#1720](https://github.com/pbek/QOwnNotes/issues/1720))
    - those tabs can be individually closed and moved
    - opening notes without the new context menu action will open the note in
      the current tab
    - if only one tab is open the tabbed interface will be hidden
    - since right-clicking a note in the note list also already opens the note
      a click on the new context menu action will attempt to open the last note
      to really open a new tab if only one tab was open
    - double clicking a note in the note list will now cause the note to be
      loaded in a tab if no `noteDoubleClickedHook` was registered in the
      scripting engine
- you will now get a warning dialog if you attempt to create a note subfolder
  that will be ignored by the application, because the name is either used
  internally or ignored in the *Panels settings* (for [#1718](https://github.com/pbek/QOwnNotes/issues/1718))
- added more German, Spanish, Russian, Catalan, Bengali, Italian translation
  (thank you rakekniven, richarson, unchase, MGuerra, 2fake1987, Oymate, tiwi90)

## 20.4.18
- the counts in the tag tree are now showing the amount of linked notes and not
  the amount of note-links of a tag (for [#1709](https://github.com/pbek/QOwnNotes/issues/1709))
- you now are able to show invisible subfolders (folders starting with `.`) in
  the note subfolder panel if you clear the default `Ignored subfolders`
  setting in the *Panels settings* (for [#1718](https://github.com/pbek/QOwnNotes/issues/1718))

## 20.4.17
- if you insert media files or attachments now a stripped-down version of the
  file's original basename will be included into the filename of resulting file
  in the `media` or `attachments` directory (for [#1716](https://github.com/pbek/QOwnNotes/issues/1716))

## 20.4.16
- there now are new scripting functions `note.renameNoteFile()` to rename a note
  file and `note.allowDifferentFileName()` to check if it is allowed to have a
  different note file name than the headline (for [#1704](https://github.com/pbek/QOwnNotes/issues/1704))
    - the [meeting-note](https://github.com/qownnotes/scripts/tree/master/meeting-note)
      script now makes use of these scripting functions
    - for more information please take a look at the
      [scripting documentation](https://www.qownnotes.org/scripting/classes.html#note)
- fixed tag adding when moving tags to another parent in the tag panel when
  handled by the scripting hook `noteTaggingByObjectHook` (for [#1705](https://github.com/pbek/QOwnNotes/issues/1705))

## 20.4.15
- when the setting to show all notes of a tag including the notes of their
  children-tags is turned on now the note-count in the tag tree is adapted to
  also count the notes of the children-tags (for [#1709](https://github.com/pbek/QOwnNotes/issues/1709))
- some safety measures against note folder update race-conditions when renaming
  tags while using the note-tagging scripting hooks `noteTaggingHook` or
  `noteTaggingByObjectHook` were included (for [#1705](https://github.com/pbek/QOwnNotes/issues/1705))

## 20.4.14
- there now is a new scripting function `getTagByNameBreadcrumbList` to create
  or fetch a tag by its name breadcrumb list (for [#1705](https://github.com/pbek/QOwnNotes/issues/1705))
    - for more information please take a look at the
      [scripting documentation](https://www.qownnotes.org/scripting/methods-and-objects.html#create-or-fetch-a-tag-by-its-name-breadcrumb-list)
- moving of tags to another parent in the tag panel can now also be handled by
  the scripting hook `noteTaggingByObjectHook` (for [#1705](https://github.com/pbek/QOwnNotes/issues/1705))
    - the application will trigger a series of `add` and `remove` actions for
      all selected tags and their children on all notes if tags are moved
    - for more information please take a look at the
      [scripting documentation](https://www.qownnotes.org/scripting/hooks.html#notetaggingbyobjecthook)
- added more Albanian, Basque, Chinese Traditional, Hindi, French, Khmer,
  Kurdish translation (thank you Fabijenna)

## 20.4.13
- there now is a new scripting hook `noteTaggingByObjectHook` to implement your
  own note tagging mechanism in a script (for [#1703](https://github.com/pbek/QOwnNotes/issues/1703))
    - similarly to `noteTaggingHook` you can implement your own note tagging
      mechanism, but you are not bound to tag names in the tag tree root,
      this way you can make use of the whole tag tree instead of only a tag list
    - for more information please take a look at the
      [scripting documentation](https://www.qownnotes.org/scripting/hooks.html#notetaggingbyobjecthook)
- you can now fetch a tag by name (if there already exists a tag object) with
  `tag.fetchByName()` via the scripting engine (for [#1703](https://github.com/pbek/QOwnNotes/issues/1703))
    - for more information please take a look at the
      [scripting documentation](https://www.qownnotes.org/scripting/classes.html#tag)
- you can now fetch the parent tag names of a tag with `tag.getParentTagNames()`
  via the scripting engine (for [#1703](https://github.com/pbek/QOwnNotes/issues/1703))
    - for more information please take a look at the
      [scripting documentation](https://www.qownnotes.org/scripting/classes.html#tag)

## 20.4.12
- when tags in the tag tree are double-clicked now the tag also is added or
  removed via the scripting engine (for [#84](https://github.com/qownnotes/scripts/issues/84))

## 20.4.11
- links to urls ending with `.md` are now not highlighted as broken note links
  in the editor any more
- the column widths of the tag tree panel are now also recalculated when the
  tree items are expanded or collapsed, not only when the tree is rebuilt or the
  application window is resized (for [#1690](https://github.com/pbek/QOwnNotes/issues/1690))
- added more translation (thank you Fabijenna, MGuerra, tiwi90)

## 20.4.10
- for all question dialogs where you clicked the checkbox `Don't ask again!` you
  will be now asked again next time if you answer the dialog question with `No`
  and remembering that decisions makes no sense (for [#1697](https://github.com/pbek/QOwnNotes/issues/1697))
    - this will help you in case when you accidentally clicked the checkbox and
      then pressed `No`, for example with shortcut-triggered dialogs like
      `Remove current note`
    - for all dialogs where remembering your `No` makes sense all of your
      decisions will be still remembered, for example with the dialog
      `Note editing is currently disabled, do you want to allow it again?`
      when you try to edit a note in read-only mode
- added more Russian, German translation (thank you unchase, rakekniven)

## 20.4.9
- notes in 2nd level subfolders (or deeper) are now shown correctly in the
  note-list after clicking on `All notes` in the subfolder panel and filtering
  by tag (for [#1696](https://github.com/pbek/QOwnNotes/issues/1696))
- if the application is in "portable mode" then this will now also be shown in
  the title bar (for [#1677](https://github.com/pbek/QOwnNotes/issues/1677))
- added more translation (thank you MGuerra, Fabijenna, richarson)

## 20.4.8
- you can now also **clear local shortcuts** in the *Shortcuts settings*
  (for [#1693](https://github.com/pbek/QOwnNotes/issues/1693))
- added more Hungarian, Dutch, Basque, Croatian, Czech, Danish, Estonian,
  Filipino, Finnish, Greek, Hausa, Hebrew, Icelandic, Irish, Italian, German,
  Russian translation (thank you Fabijenna, mlocati, rakekniven, unchase)

## 20.4.7
- tags are now not lost if a note in a subfolder is renamed after clicking on
  `All notes` in the subfolder panel (for [#1691](https://github.com/pbek/QOwnNotes/issues/1691))
- added a **new editor color schema** *Atom One Dark Reasonable* by @leoniscsem
  (for [#1694](https://github.com/pbek/QOwnNotes/issues/1694))
- added more Shona, Brazilian Portuguese, French, Icelandic, Indonesian, Irish,
  Italian, Khmer, Persian, Polish, Portuguese, Punjabi, Ukrainian, Urdu (Pakistan),
  Uzbek, Vietnamese, Spanish, Catalan translation (thank you Fabijenna, mlimacarlos,
  richarson, MGuerra)

## 20.4.6
- fixed renaming of note subfolders under Windows if the folders contain other
  subfolders (for [#1683](https://github.com/pbek/QOwnNotes/issues/1683))
- the note folder will now be reloaded if a note subfolder was deleted in the *Subfolders panel*
- fixed optional automatic database closing under Windows (for [#926](https://github.com/pbek/QOwnNotes/issues/926))
- notes are now correctly filtered by tag even if notes with the same name are
  in different note subfolders (for [#1686](https://github.com/pbek/QOwnNotes/issues/1686))
- added more Bosnian, Albanian, Arabic, Basque, Bengali, Bulgarian, Catalan,
  Chinese Simplified, Chinese Traditional, Croatian, Czech, Danish, Dutch,
  Estonian, Filipino, Finnish, Galician, Greek, Hausa, Hebrew, Hungarian,
  Turkish, Swedish, Slovenian, Slovak, Serbian (Cyrillic), Romanian, Polish,
  Norwegian, Macedonian, Lithuanian, Latvian, Kurdish, Korean, Japanese
  translation (thank you Fabijenna)

## 20.4.5
- fixed the time formats link in the *Editor settings*
  (for [#1685](https://github.com/pbek/QOwnNotes/issues/1685))
- now an error message in the log is shown if renaming of a note subfolder fails
  (for [#1683](https://github.com/pbek/QOwnNotes/issues/1683))
- fixed note linking with subfolders if the note folder is outside of the application
  directory in portable mode (for [#1682](https://github.com/pbek/QOwnNotes/issues/1682))
- added more German, Russian, Italian translation (thank you rakekniven, unchase, tiwi90)

## 20.4.4
- some improvements for possible "cursor jumping to the top" issues were made
  (for [#1681](https://github.com/pbek/QOwnNotes/issues/1681))

## 20.4.3
- when right-clicking a note in the note list and clicking `Open note in different window`
  you can now also show a **preview of your note** instead of the note text
  (for [#1678](https://github.com/pbek/QOwnNotes/issues/1678))
- added more Catalan, German, Spanish, Albanian, Khmer, Arabic, Korean, Basque,
  Kurdish, Bengali, Latvian, Bosnian, Lithuanian, Bulgarian, Macedonian, Cebuano,
  Malay, Norwegian, Chinese Simplified, Romanian, Chinese Traditional,
  Serbian (Cyrillic), Croatian, Slovak, Czech, Slovenian, Estonian, Swedish,
  Filipino, Turkish, Finnish, Ukrainian, Galician, Urdu (Pakistan), Greek, Uzbek,
  Hausa, Vietnamese, Hebrew, Hindi, Icelandic, Indonesian, Dutch, Irish,
  Hungarian, Danish, Japanese, Italian, Russian translation (thank you MGuerra,
  rakekniven, richarson, Fabijenna, tiwi90, unchase)

## 20.4.2
- you can now choose to **show your note folders** in your *Note folders panel*
  **as buttons** in the *Panels settings* (for [#1676](https://github.com/pbek/QOwnNotes/issues/1676))
    - you can use this button list vertically or horizontally, the buttons will
      float to the next line if there is no space left
- some title bar improvements were made (for [#1677](https://github.com/pbek/QOwnNotes/issues/1677))
    - if more than one note folder exists then the name of the current note folder
      will be shown in the title bar
    - if a session was set with the `--session` cli parameter then it will be
      shown in the title bar
- added more German, Zulu, Shona, Maori, Xhosa, Filipino, Icelandic, Malay,
  Serbian (Cyrillic), Vietnamese, Finnish, Indonesian, Irish, Slovak, French,
  Japanese, Slovenian, Galician, Norwegian, Albanian, Arabic, Basque, Bengali,
  Bosnian, Khmer, Persian, Bulgarian, Swedish, Polish, Greek, Cebuano, Korean,
  Portuguese, Tagalog, Turkish, Chinese Simplified, Hausa, Ukrainian,
  Chinese Traditional, Punjabi, Hebrew, Kurdish, Latvian, Croatian, Urdu (Pakistan),
  Romanian, Czech, Hindi, Lithuanian, Uzbek, Danish, Estonian, Macedonian,
  Hungarian, Dutch translation (thank you Fabijenna)

## 20.4.1
- you can now use the **Meta key in keyboard shortcuts** in the *Shortcuts settings*
  (for [#1671](https://github.com/pbek/QOwnNotes/issues/1671))
    - if you have troubles with a 2nd Meta key showing up while entering the shortcut
      you can press <kbd>Enter</kbd> or <kbd>Escape</kbd> to finish the shortcut entering
- added more Catalan translation (thank you MGuerra)

## 20.4.0
- you now can **show and hide the application window** with a new action `Show/Hide application`
  in the *Window / Show* menu (for [#1324](https://github.com/pbek/QOwnNotes/issues/1324))
    - this should work with and without enabled tray icon
    - assign a global keyboard shortcut to the action in the *Shortcuts settings*
      to make use of it properly
- added more Spanish, Russian, Swedish, German, Italian, Galician, Greek, Hausa,
  Albanian, Arabic, Basque, Bengali, Bosnian, Bulgarian, Cebuano, Chinese Simplified,
  Dutch, Estonian, Filipino, Finnish translation (thank you richarson, unchase,
  druus, rakekniven, tiwi90, Fabijenna)

## 20.3.9
- you can now also manage **global keyboard shortcuts** in the *Shortcuts settings*
  (for [#44](https://github.com/pbek/QOwnNotes/issues/44))
    - there you can set a global shortcut to every action of the main menu,
      including custom scripting actions
    - you can also search for global shortcuts in the `Search for shortcut` line edit
    - by default there are no global shortcuts activated
- there now is a new property `relativeNoteFileDirPath` for notes in the scripting engine
  (for [#1667](https://github.com/pbek/QOwnNotes/issues/1667))
    - for more information please take a look at the
      [Note scripting documentation](https://www.qownnotes.org/scripting/classes.html#note)
- there now is a new property `createNewNoteSubFolder` for `mainWindow` in the
  scripting engine that creates a new note subfolder in the current subfolder
  (for [#1667](https://github.com/pbek/QOwnNotes/issues/1667))
    - for more information please take a look at the
      [Mainwindow scripting documentation](https://www.qownnotes.org/scripting/classes.html#mainwindow)
- fixed a possible crash on entering a backtick character
  (for [#114](https://github.com/pbek/qmarkdowntextedit/pull/114), thank you @Waqar144)

## 20.3.8
- added experimental **global keyboard shortcut support** (for [#44](https://github.com/pbek/QOwnNotes/issues/44))
    - <kbd>Meta + Shift + N</kbd> will now create a new note (for [#1048](https://github.com/pbek/QOwnNotes/issues/1048))
- no 2nd backtick will now be entered if you enter one at the end of a word with an
  US keyboard layout (for [#111](https://github.com/pbek/qmarkdowntextedit/pull/111), thank you @Waqar144)
- fixed a build problem under Qt 5.15 (for [#1666](https://github.com/pbek/QOwnNotes/issues/1666), thank you @data-man)
- added more Indonesian, Catalan, Khmer, Sinhala, Shona, Dutch, Hungarian,
  Danish, Spanish, Brazilian Portuguese, German, Russian, Italian translation
  (thank you zmni, MGuerra, Fabijenna, richarson, mlimacarlos, rakekniven, unchase, tiwi90)

## 20.3.7
- you now can also <kbd>Ctrl + Click</kbd> legacy attachment urls like
  `file://attachments/1260976404.pdf` in the note editor to open them externally
  (for [#1663](https://github.com/pbek/QOwnNotes/issues/1663))
- added **Shona translation** (a big thank you to Fabijenna)
    - visit [QOwnNotes Translation](http://docs.qownnotes.org/en/latest/contributing/translation/)
      if you want to make QOwnNotes available in more languages or help with the
      current translation
- added more Italian, Brazilian Portuguese, Khmer, Xhosa, Sinhala, German,
  Russian translation (thank you Fabijenna, mlocati, mlimacarlos, rakekniven,
  unchase)

## 20.3.6
- you can now **search for a language** in the *Interface settings* to make it
  easier to find the translation you want to use
    - you can search for the English name, the local name or the language code
- added **Zulu, Sinhala and Khmer translation** (a big thank you to Fabijenna)
    - visit [QOwnNotes Translation](http://docs.qownnotes.org/en/latest/contributing/translation/)
      if you want to make QOwnNotes available in more languages or help with the
      current translation
- added more Xhosa, Bengali, Spanish, Indonesian, Italian, Russian, German,
  Catalan, Brazilian Portuguese, Arabic, Basque, Bosnian, Bulgarian, Cebuano,
  Chinese Simplified, Chinese Traditional, Croatian, Czech, Danish, Dutch,
  Estonian, Filipino, Finnish, Galician, Greek, Hausa, Albanian translation
  (thank you Fabijenna, omarmear, richarson, zmni, tiwi90, unchase, rakekniven,
  Altanzar, MGuerra, mlimacarlos, athrouss)

## 20.3.5
- you can now **import CSV** directly **from the clipboard** in the *Table dialog*
    - you can for example copy CSV from a table in a web-browser
- an attempt to **detect the separator** when importing CSV will now be made in the *Table dialog*
- added more Spanish, Italian, Russian, Brazilian Portuguese, German translation
  (thank you richarson, Fabijenna, tiwi90, unchase, fitoschido, mlimacarlos, rakekniven)

## 20.3.4
- the setting `Allow note file name to be different from headline` can now be
  set per note folder instead of only globally
    - this allows you to use different settings for your note file names for each note folder
    - the currently existing setting will be used as setting for all your note folders initially
- if you open the *Note folders settings* now the currently active note folder will be selected
- improved html list importing of the *Paste html or media* action
- added **Malay and Xhosa translation** (a big thank you to Fabijenna)
    - join us at [QOwnNotes on Crowdin](https://crowdin.com/project/qownnotes)
      to make QOwnNotes available in more languages or help with the current
      translation
- added more Catalan, Chinese Traditional, Filipino, Spanish translation (thank you Fabijenna)

## 20.3.3
- the **documentation page** was **re-done in Markdown** and will be generated
  automatically with **MkDocs** at [docs.qownnotes.org](https://docs.qownnotes.org)
    - feel free to add or improve the information
- improved GTK and KDE Plasma support for the snap version of QOwnNotes
  (for [#1651](https://github.com/pbek/QOwnNotes/pull/1651))
    - the Breeze styling will now be used by default along with the GTK file selectors 
- the *Table dialog* now also supports importing csv files with tabulator
  characters as separator
- added more Catalan, Chinese Traditional, Filipino, Spanish, French, Hiligaynon,
  Bengali, Persian, Dutch, Albanian, Arabic, Ukrainian, Italian, Russian translation
  (thank you Fabijenna, MGuerra, mlocati, unchase, richarson, tiwi90)

## 20.3.2
- when removing single or multiple notes the note folder will now not be reloaded,
  instead the notes will just be removed in the note list and the next existing
  note will be selected (for [#1650](https://github.com/pbek/QOwnNotes/issues/1650))
- added **Māori translation** (a big thank you to Fabijenna)
    - join us at [QOwnNotes on Crowdin](https://crowdin.com/project/qownnotes)
      to make QOwnNotes available in more languages or help with the current
      translation

## 20.3.1
- fixed inline code not being highlighted if there is a link in the same line
  (for [#1649](https://github.com/pbek/QOwnNotes/issues/1649), thank you @Waqar144)
- added more Arabic, Vietnamese, Cebuano, Turkish, Hebrew, Galician, Hindi,
  Kurdish, Urdu (Pakistan), Chinese Simplified, Uzbek, German, Spanish,
  Brazilian Portuguese, Italian, Catalan, French, Filipino, Albanian, Basque,
  Bosnian, Bulgarian, Chinese Traditional, Croatian, Czech, Danish, Dutch,
  Estonian, Finnish, Greek, Hungarian, Icelandic, Indonesian, Irish, Japanese,
  Korean, Latvian, Lithuanian, Macedonian, Norwegian, Polish, Portuguese,
  Punjabi, Romanian, Serbian (Cyrillic), Slovak, Slovenian, Swedish, Russian
  translation (thank you Fabijenna, rakekniven, fitoschido, mlimacarlos, mlocati,
  MGuerra, primokorn, unchase)

## 20.3.0
- there now is a new predefined layout **Preview only** to choose from at the
  initial setup and in the *Layout settings* that shows no note edit panel, but
  the preview panel instead
    - you could use a workspace with this layout to switch between a workspace
      with only the note edit and a workspace with only the preview with the
      shortcut <kbd>Alt + Shift + P</kbd>
        - please visit [QOwnNotes shortcuts](https://www.qownnotes.org/getting-started/shortcuts.html)
          for a list of all shortcuts
    - using a new layout in the *Layout settings* now creates a new workspace
      instead of replacing the initial workspace, but keep in mind that workspaces
      that demand that there is no central widget will not work properly if the
      central widget is enabled
- you can now also select text with they keyboard in the *Debug information* of
  the *Debug settings*
- fixed headings retaining old formatting when removing the heading marker
  in the next line (for [#1641](https://github.com/pbek/QOwnNotes/issues/1641), thank you @Waqar144)
- fixed strike-through highlighting being applied to links
  (for [#1642](https://github.com/pbek/QOwnNotes/issues/1642), thank you @Waqar144)
- fixed cpp being used for unknown code block languages in the note edit highlighter
  (for [#1643](https://github.com/pbek/QOwnNotes/issues/1643), thank you @Waqar144)

## 20.2.11
- fixed strange line edit resizing problem in the *Nextcloud/ownCloud settings*
  that was sighted on KDE
    - other user interface elements were also slightly improved
- fixed a note folder reload problem if application was built on Qt 5.14 or higher
  (for [#1640](https://github.com/pbek/QOwnNotes/issues/1640))

## 20.2.10
- fixed a possible crash with the CSS code block highlighter
  (for [#1638](https://github.com/pbek/QOwnNotes/issues/1638), thank you @Waqar144)
- url-encoded links to notes will now also be correctly adapted if those notes
  are renamed (for [#1546](https://github.com/pbek/QOwnNotes/issues/1546))

## 20.2.9
- note links generated from the *Link dialog* will now get url-encoded to be
  better compatible with other tools that can link to files, like GitHub or
  GitLab (for [#1546](https://github.com/pbek/QOwnNotes/issues/1546))
    - for example `Note with one bracket].md` will now get `Note%20with%20one%20bracket%5D.md`
    - links with just spaces still work in QOwnNotes, as well as `&#32;` as spaces
    - the new note links are click-able in both the note edit and the preview
      and the broken-note-link highlighter will also handle them correctly
- the default shortcut for `Insert attachment` was changed to <kbd>Ctrl + Shift + X</kbd>
  because of clashing with `Edit encrypted note` (for [#1637](https://github.com/pbek/QOwnNotes/issues/1637))
    - please visit [QOwnNotes shortcuts](https://www.qownnotes.org/getting-started/shortcuts.html)
      for a list of all shortcuts
- added more Indonesian, Spanish, Danish, Vietnamese, Dutch, Estonian, Filipino,
  Finnish, Greek, Hausa, Hebrew, Hindi, Hungarian, Urdu (Pakistan), Icelandic,
  Turkish, Irish, Japanese, Korean, Kurdish, Latvian, Lithuanian, Macedonian,
  Norwegian, Persian, Polish, Portuguese, Punjabi, Romanian, Serbian (Cyrillic),
  Slovak, Slovenian, Swedish, Ukrainian, Uzbek, Albanian, Arabic, Basque, Bengali,
  Bosnian, Bulgarian, Chinese Simplified, Chinese Traditional, Croatian, Czech,
  German, Russian translation (thank you zmni, richarson, Fabijenna, rakekniven, unchase)

## 20.2.8
- conflicted copies of the note folder database `notes.sqlite` are now
  immediately deleted if tags and tag assignments are not different to the ones
  from the current `notes.sqlite` (for [#1625](https://github.com/pbek/QOwnNotes/issues/1625))
    - all other table differences can be safely ignored

## 20.2.7
- some improvements to the todo list dialog were made
    - if the todo items in a list will be reloaded from the server the current
      text search will be applied again
    - if the todo list is switched the current text search will be removed to
      not confuse the user 
- fixed removing of the 3rd `"` from `test "test""` with <kbd>Backspace</kbd>
  (for [#1629](https://github.com/pbek/QOwnNotes/issues/1629), thank you @Waqar144)

## 20.2.6
- more improvements to the link dialog were made
  (for [#1632](https://github.com/pbek/QOwnNotes/issues/1632))
    - entering of `]` characters in link name line edit is now disabled since
      they would break the markdown link
        - they also will be removed from the link name automatically if they
          were part of the note file name 
    - if one of the link characters `<>()` were found in the note url the legacy
      way of linking will be used because otherwise the url would break the
      markdown link
- fixed fetching of todo calendar list in *Settings dialog* if cloud connection
  was switched (for [#1631](https://github.com/pbek/QOwnNotes/issues/1631))

## 20.2.5
- fetching of remote note folders in the *Note folders settings* now should
  work properly with Nextcloud (for [#1631](https://github.com/pbek/QOwnNotes/issues/1631))
    - using different cloud connections now also works properly with the remote
      note folder selector
    - switching to another cloud connection in the *Note folders settings* now
      also triggers a reload of the remote note folder selector if it is visible
- attempted to fix random crashes while editing notes on macOS
  (for [#1626](https://github.com/pbek/QOwnNotes/issues/1626), thank you @Waqar144)
- attempted to fix rare QOwnNotes disk image mount problems under macOS
  (for [#1630](https://github.com/pbek/QOwnNotes/issues/1630))
- added repositories for **openSUSE Leap 15.2**
    - please visit the [openSUSE installation page](https://www.qownnotes.org/installation#openSUSE)
      for instructions
- if you accept the *Link dialog* while on the *Note tab* now a possible URL in
  the *URL tab* will be removed so that your selected note will be inserted
  instead of the URL
- automatic note file naming schema in case of duplicate note file names was
  changed from appending a number like ` (1)` to just the number like ` 1`
  because of problems with brackets in note links
- added more Russian, Catalan, Brazilian Portuguese, Croatian, Czech, Danish,
  Dutch, Estonian, Finnish, Greek, Hungarian, Irish, Japanese, Korean, Latvian,
  Lithuanian, Macedonian, Norwegian, Persian, Polish, Portuguese, Punjabi,
  Romanian, Serbian (Cyrillic), Slovak, Slovenian, Swedish, Turkish, Uzbek,
  Albanian, Basque, Bosnian, Bulgarian, Chinese Simplified, Chinese Traditional,
  Indonesian, French, German, Italian translation (thank you unchase, MGuerra,
  mlimacarlos, Fabijenna, zmni, primokorn, rakekniven, mlocati)

## 20.2.4
- conflicted copies of the note folder database `notes.sqlite` are now
  immediately deleted if they are not different to the current `notes.sqlite`
  (for [#1625](https://github.com/pbek/QOwnNotes/issues/1625))

## 20.2.3
- improved Setext-style headers highlighting with leading spaces
  (for [#101](https://github.com/pbek/qmarkdowntextedit/issues/101), thank you @Waqar144)
- allowed CMake and Make highlighting in the preview
  (for [#1617](https://github.com/pbek/QOwnNotes/pull/1617), thank you @Waqar144)
- fixed tilde style code blocks in preview when the number of tildes blocks were odd
  (for [#1620](https://github.com/pbek/QOwnNotes/pull/1620), thank you @Waqar144)

## 20.2.2
- automatic closing of brackets (and other characters) will now happen even if
  you are in the middle of the line (for [#96](https://github.com/pbek/qmarkdowntextedit/pull/96), thank you @Waqar144)
    - automatic removal of brackets (and other characters) for closing characters
      for example `]` or `)` now works (previously automatic removal only
      happened if the character was an opening character)
- fixed Go lang highlighting and improved makefile highlighting
  (for [#97](https://github.com/pbek/qmarkdowntextedit/pull/97), thank you @Waqar144)
- improved comment block highlighting (for [#99](https://github.com/pbek/qmarkdowntextedit/pull/99), thank you @Waqar144)

## 20.2.1
- improved the space padding of the automatic markdown table formatter
  (for [#1052](https://github.com/pbek/QOwnNotes/issues/1052))
- fixed highlighting of list items that start with tabulator characters and
  checkboxes that don't have a space in the brackets (for [#1615](https://github.com/pbek/QOwnNotes/issues/1615), thank you @Waqar144)

## 20.2.0
- remote images and Nextcloud text images are now cached after being downloaded
  for the preview until another note was be selected (for [#1244](https://github.com/pbek/QOwnNotes/issues/1244))
    - this improves writing speed because the images now don't need to be
      downloaded again every time the preview is refreshed
- remote images and Nextcloud text images are now (like local images) resized in
  the preview if the width of the preview panel is smaller than the image width
  (for [#1597](https://github.com/pbek/QOwnNotes/issues/1597))

## 20.1.19
- fixed a possible crash in the broken-note-link highlighter
  (for [#1611](https://github.com/pbek/QOwnNotes/issues/1611), thank you @Waqar144)
- enabled underline rendering in the note text edit
  (for [#1613](https://github.com/pbek/QOwnNotes/pull/1613), thank you @Waqar144)
- added more Russian translation (thank you mikhail-khokhlov)

## 20.1.18
- added Cmake and make languages to the code block syntax highlighter
  (for [#93](https://github.com/pbek/qmarkdowntextedit/pull/93), thank you @Waqar144)
- fixed checking of checkboxes in checkbox lists in the preview
  (for [#1607](https://github.com/pbek/QOwnNotes/issues/1607), thank you @Waqar144)
- fixed displaying of the context menu in the task list of the todo list dialog
  if no task is present (for [#1609](https://github.com/pbek/QOwnNotes/issues/1609))

## 20.1.17
- on Windows the application will now automatically be shown if the the update
  dialog is triggered, because otherwise Windows seems to quit the application
  when the update dialog is closed while the application is hidden
  (for [#1514](https://github.com/pbek/QOwnNotes/issues/1514))
- highlighting of markdown lists (ordered/unordered/checked/unchecked) have been
  re-written (for [#1598](https://github.com/pbek/QOwnNotes/issues/1598), thank you @Waqar144)
    - list parsing is much faster now
    - ordered lists can optionally have a `)` instead of a `.` as the character
      after the number, both these lists will be completed if you press <kbd>Enter</kbd>
- added more Russian, Bengali, Indonesian, Irish, Italian, Japanese, Korean,
  Kurdish, Latvian, Lithuanian, Macedonian, Norwegian, Persian, Polish, Bosnian,
  Portuguese, Bulgarian, Punjabi, Chinese Simplified, Romanian, Chinese Traditional,
  Serbian (Cyrillic), Slovak, Slovenian, Spanish, Swedish, Croatian, Czech, Danish,
  Dutch, Estonian, Albanian, Filipino, Arabic, Finnish, Basque, Hausa, Hindi,
  Hungarian, Icelandic, Catalan, French, Greek, Brazilian Portuguese, German
  translation (thank you unchase, Fabijenna, MGuerra, primokorn, athrouss,
  mlimacarlos, rakekniven, tiwi90)

## 20.1.16
- you now can set a custom editor width in the `Window / Editor width` menu
  (for [#1604](https://github.com/pbek/QOwnNotes/issues/1604))
- all search results in the note text edit are now highlighted while searching
  (for [#1512](https://github.com/pbek/QOwnNotes/issues/1512))
    - the speed of the search was slightly improved
    - you can now do regular expressions searches also case insensitive

## 20.1.15
- fixed stacked code block highlighting and improved non-fenced code block highlighting
  (for [#1558](https://github.com/pbek/QOwnNotes/issues/1558), [#1602](https://github.com/pbek/QOwnNotes/issues/1602),
  [#1601](https://github.com/pbek/QOwnNotes/pull/1601) and [#1598](https://github.com/pbek/QOwnNotes/issues/1598),
  thank you @Waqar144)
- there now is a new scripting command `script.noteTextEditSelectCurrentWord()`
  to select the current word in the note text edit (for [#1603](https://github.com/pbek/QOwnNotes/pull/1603), thank you @jfx2006)
    - for more information please take a look at the
      [scripting documentation](https://www.qownnotes.org/scripting/methods-and-objects.html#select-the-current-word-in-the-note-text-edit)
- added more Cebuano, Italian, Chinese Simplified, Slovak, Chinese Traditional,
  Slovenian, Croatian, Spanish, Czech, Swedish, Danish, Ukrainian, Dutch,
  Urdu (Pakistan), Uzbek, Estonian, Vietnamese, Finnish, French, Greek, Hausa,
  Hindi, Hungarian, Icelandic, Indonesian, Irish, Japanese, Korean, Kurdish,
  Latvian, Lithuanian, Macedonian, Norwegian, Persian, Albanian, Polish, Basque,
  Portuguese, Bosnian, Punjabi, Bulgarian, Romanian, Serbian (Cyrillic), Russian,
  German, Catalan translation (thank you Fabijenna, mlocati, unchase, rakekniven,
  MGuerra, tiwi90)

## 20.1.14
- you can now also **create sub-tasks** in the **Todo list dialog** by right-clicking
  on a todo-item (for [#1596](https://github.com/pbek/QOwnNotes/issues/1596))
- more improvements for the markdown highlighting were made
  (for [#1598](https://github.com/pbek/QOwnNotes/issues/1598), thank you @Waqar144)
- fixed four spaces code blocks again (for [#1558](https://github.com/pbek/QOwnNotes/issues/1558), thank you @Waqar144)
- added more Chinese Simplified, Chinese Traditional, Indonesian, Hindi, Kurdish,
  Filipino translation (thank you Fabijenna)

## 20.1.13
- fixed not detected external note changes of the current note if the event for
  the change in the current note comes after the event that the note folder was
  modified (for [#1597](https://github.com/pbek/QOwnNotes/issues/1597))
- lots of improvements for the markdown highlighting were made
  (for [#1598](https://github.com/pbek/QOwnNotes/issues/1598), thank you @Waqar144)
    - support for overlapping formats such as bold + italic, code + italic, etc.
    - em and strong highlighting in complex cases
    - inline code spans can now handle complex cases, e.g ``` ``inline  ` code`` ```
    - strike through support
    - Commonmark compliant indented code blocks
- fixed broken note folder and script moving via drag and drop on macOS
- added more Chinese Simplified, Chinese Traditional, Hindi translation (thank you Fabijenna)

## 20.1.12
- fixed a memory leak in the preview (for [#1594](https://github.com/pbek/QOwnNotes/pull/1594), thank you @Waqar144)
- you can now browse and modify your **todo list** items in a tree instead of a list
  if you are using **sub-tasks** (for [#1596](https://github.com/pbek/QOwnNotes/issues/1596))
    - the creation of sub-tasks is not yet supported in the application
    - keep in mind that sub-tasks of completed tasks or sub-tasks will not be
      shown if you don't show completed items
- added more Chinese Simplified, Uzbek, Dutch, German, Norwegian, Persian, Polish,
  Portuguese, Punjabi, Romanian, Swedish, Serbian (Cyrillic), Slovak, Slovenian,
  Turkish, Ukrainian, Urdu (Pakistan), Albanian, Arabic, Basque, Bengali, Bosnian,
  Bulgarian, Croatian, Kurdish, Czech, Danish, Estonian, Finnish, French, Greek,
  Hausa, Hebrew, Hungarian, Icelandic, Irish, Japanese, Korean, Latvian, Lithuanian,
  Macedonian, Russian, Spanish, Italian translation (thank you Fabijenna, rakekniven,
  unchase, richarson, mlocati)

## 20.1.11
- fixed an infinite loop in the preview with complex markdown tables (for [#1592](https://github.com/pbek/QOwnNotes/issues/1592), thank you @mity)
- there now is a button to open the *App Password* page of your Nextcloud / ownCloud
  server to create a new app password for the application in the *Nextcloud / ownCloud settings*
- added more Catalan, Arabic, Irish, Ukrainian, Urdu (Pakistan), Uzbek, Vietnamese,
  Portuguese, Albanian, Hebrew, Basque, Bosnian, Bulgarian, Cebuano, Croatian,
  Czech, Danish, Dutch, Estonian, Finnish, French, Greek, Hausa, Hindi, Hungarian,
  Icelandic, Japanese, Korean, Latvian, Lithuanian, Macedonian, Norwegian, Persian,
  Polish, Punjabi, Romanian, Russian, Serbian (Cyrillic), Slovak, Slovenian,
  Spanish, Swedish, Turkish, Italian, German translation (thank you MGuerra,
  Fabijenna, mlocati, rakekniven)

## 20.1.10
- it is now possible to disable the opening of the last accessed note at startup
  in the *General settings* (for [#1588](https://github.com/pbek/QOwnNotes/issues/1588))
- the mechanism to remove conflicted copies of the `notes.sqlite` database is
  now also compatible with Dropbox (for [#1589](https://github.com/pbek/QOwnNotes/issues/1589))
- added more Arabic, Irish, Hebrew translation (thank you Fabijenna)

## 20.1.9
- added support for tilde code fence highlighting (for [#1580](https://github.com/pbek/QOwnNotes/issues/1580), thank you @Waqar144)
    - the preview also works correctly now
    - you can also fence triple-tick code blocks with tilde-fences and vice versa
- fixed some small preview issues (for [#1582](https://github.com/pbek/QOwnNotes/pull/1582), thank you @Waqar144)
- added more Hebrew, Urdu (Pakistan), Spanish translation (thank you Fabijenna, DixTheDev)

## 20.1.8
- added preview code block highlighting for INI and VEX language (for [#1529](https://github.com/pbek/QOwnNotes/issues/1529), thank you @Waqar144)
- fixed numerical code block highlighting and preview issues (for [#1575](https://github.com/pbek/QOwnNotes/issues/1575), thank you @Waqar144)
- enabled `Copy code block` menu entry for code blocks with language suffix (for [#1578](https://github.com/pbek/QOwnNotes/issues/1578), thank you @Waqar144)
- added more Irish, Portuguese, Korean translation (thank you Fabijenna)

## 20.1.7
- fixed trailing spaces not being highlighted (for [#1569](https://github.com/pbek/QOwnNotes/issues/1569), thank you @Waqar144)
- fixed numeric highlighter and added more improvements (for [#1571](https://github.com/pbek/QOwnNotes/pull/1571), thank you @Waqar144)
- added underline option to preview for MD4C (for [#1567](https://github.com/pbek/QOwnNotes/issues/1567), thank you @Waqar144)
- added the main menu entry `Reload note folder` also to the context menu in the
  note subfolder panel (for [#1574](https://github.com/pbek/QOwnNotes/issues/1574)
- fixed highlighting of ordered lists (for [#1573](https://github.com/pbek/QOwnNotes/issues/1573)
- added more Portuguese, Filipino, Kurdish, Japanese, Urdu (Pakistan), Turkish,
  Hebrew translation (thank you Fabijenna)

## 20.1.6
- fixed four spaces code blocks (for [#1558](https://github.com/pbek/QOwnNotes/issues/1558), thank you @Waqar144)
- fixed a numeric literal code block highlighting problem (for [#1559](https://github.com/pbek/QOwnNotes/issues/1559), thank you @Waqar144)
- fixed multiple multiline comments in same code block causing freezes (for [#1561](https://github.com/pbek/QOwnNotes/issues/1561), thank you @Waqar144)
- the markdown to html library [MD4C](https://github.com/mity/md4c) was integrated
  to be used in the preview instead of hoedown (for [#1567](https://github.com/pbek/QOwnNotes/issues/1567), thank you @Waqar144)
- added full css highlighting and partial YAML highlighting to the preview (for [#1570](https://github.com/pbek/QOwnNotes/pull/1570), thank you @Waqar144)
- again attempted to fix AppImage build
- added more Hindi, Arabic, French, Estonian, Hausa, Swedish, Slovak, Slovenian,
  Romanian, Polish, Norwegian, Serbian (Cyrillic), Russian, Irish, Latvian
  translation (thank you Fabijenna)

## 20.1.5
- added a **new editor color schema** *Two Dark* by @ScottKillen (for [#1557](https://github.com/pbek/QOwnNotes/issues/1557))
- attempted to fix AppImage and fixed macOS build
- code block keyword styles now also match editor styles (for [#1462](https://github.com/pbek/QOwnNotes/issues/1462))

## 20.1.4
- fixed highlighting of comments in code blocks (for [#1547](https://github.com/pbek/QOwnNotes/issues/1547), thank you @Waqar144)
- improved the preview highlighting (for [#1551](https://github.com/pbek/QOwnNotes/pull/1551), thank you @Waqar144)
- fixed inline code being interpreted as code block (for [#73](https://github.com/pbek/qmarkdowntextedit/pull/73), thank you @Waqar144)
- code blocks now can have a full-width background color in the preview (for [#1526](https://github.com/pbek/QOwnNotes/issues/1526))
- increased performance of navigation parsing panel (for [#1553](https://github.com/pbek/QOwnNotes/pull/1553), thank you @Waqar144)
- fixed broken two-line headline highlighting while typing (for [#1552](https://github.com/pbek/QOwnNotes/issues/1552), thank you @Waqar144)

## 20.1.3
- fixed opening of notes in external editor under macOS if non-default application
  was chosen (for [#1543](https://github.com/pbek/QOwnNotes/issues/1543))
- did more optimizations for note editing (for [#1545](https://github.com/pbek/QOwnNotes/pull/1545), thank you @Waqar144)
- greatly improved syntax highlighting performance (for
  [#70](https://github.com/pbek/qmarkdowntextedit/pull/70) and
  [#71](https://github.com/pbek/qmarkdowntextedit/pull/71), thank you @Waqar144)
- improved Fastmail CalDav server support (for [#1544](https://github.com/pbek/QOwnNotes/issues/1544))
- improved note store triggering mechanism
- added more Indonesian translation (thank you zmni)

## 20.1.2
- again attempted to add support for the Synology CalDAV server
  (for [#1530](https://github.com/pbek/QOwnNotes/issues/1530))
- the `Export generated raw HTML` preview context menu item now exports the html
  generated to be viewed in the preview and not the already transformed html inside
  the `QTextBrowser` widget (for [#1526](https://github.com/pbek/QOwnNotes/issues/1526))
- added vex code block highlighting support (for [#1529](https://github.com/pbek/QOwnNotes/issues/1529), thank you @Waqar144)
- added tagger script code block highlighting support (for [#68](https://github.com/pbek/qmarkdowntextedit/pull/68), thank you @Gremious)
- added support for solving equations in lists (for [#1539](https://github.com/pbek/QOwnNotes/issues/1539))
- fixed some `'` preview problems in code blocks (for [#1540](https://github.com/pbek/QOwnNotes/issues/1540), thank you @Waqar144)

## 20.1.1
- added more preview syntax highlighter fixes and improvements
  (for [#1528](https://github.com/pbek/QOwnNotes/pull/1528), thank you @Waqar144)
- attempted to add support for the Synology CalDAV server
  (for [#1530](https://github.com/pbek/QOwnNotes/issues/1530))
- fixed possible crash for inline code blocks in preview highlighter
  (for [#1532](https://github.com/pbek/QOwnNotes/issues/1532), thank you @Waqar144)
- added more Persian, Hausa, Punjabi, Uzbek, Lithuanian translation (thank you Fabijenna)

## 20.1.0
- brought more variation in the headline colors of the `Light` and `Dark` schemes
- added missing headline 6 highlighting in the note editor
- attempted to fix SuSE builds
- attempted to fix builds for older Qt versions

## 19.12.17
- did some highlighting fixes and improvements (for [#1523](https://github.com/pbek/QOwnNotes/issues/1523),
  [#64](https://github.com/pbek/qmarkdowntextedit/pull/64) and
  [#65](https://github.com/pbek/qmarkdowntextedit/pull/65), thank you @Waqar144)
- there now also is **syntax highlighting in the preview**
  (for [#1526](https://github.com/pbek/QOwnNotes/issues/1526), thank you @Waqar144)
   - most of the languages already work except YAML (doesn't work fully yet),
     INI (doesn't work fully yet) and CSS
- added more Hausa, Russian, Cebuano, Hungarian, Icelandic, Croatian, Czech,
  Danish, Finnish, Galician, Greek, Dutch, Albanian, Basque, Bosnian, Bulgarian
  translation (thank you Fabijenna, unchase)

## 19.12.16
- code block backgrounds are now rendered over the full width of the editor
  (for [#1480](https://github.com/pbek/QOwnNotes/issues/1480), thank you @Waqar144)
    - the *Dark* color schema now also has a background color by default
- added a **new editor color schema** *Nord* by @salopst
  (for [#1522](https://github.com/pbek/QOwnNotes/issues/1522))
- added more keywords to the BASH highlighting support
- added more Spanish, Catalan, Brazilian Portuguese, Italian, German translation
  (thank you richarson, MGuerra, mlimacarlos, mlocati, rakekniven)

## 19.12.15
- added YAML and INI highlighting support (for [#58](https://github.com/pbek/qmarkdowntextedit/pull/58)
  and [#59](https://github.com/pbek/qmarkdowntextedit/pull/59), thank you @Waqar144)
- added more keywords to the BASH highlighting support

## 19.12.14
- checked and unckecked **checkboxes** are now **highlighted** in the note editor
  (for [#1519](https://github.com/pbek/QOwnNotes/issues/1519))
- the *Issue assistant dialog* is now scrollable to make it easier to enter issues
  on low resolution screens (for [#1515](https://github.com/pbek/QOwnNotes/issues/1515))
- made some performance improvements when changing the current note item
  (for [#327](https://github.com/pbek/QOwnNotes/issues/327), thank you @Waqar144)
- fixed total byte display while downloading dictionaries in the *Dictionary manager dialog*
  if total byte size is not transmitted by the web server
- you can now remove multiple indent-character inside of text when indenting with spaces
  (for [#1455](https://github.com/pbek/QOwnNotes/issues/1455))
- fixed a problem with showing correct notes if no subfolder was selected
  (for [#1520](https://github.com/pbek/QOwnNotes/pull/1520), thank you @Waqar144)
- added more German, Galician, French, Lithuanian, Serbian (Cyrillic), Czech,
  Uzbek translation (thank you rakekniven, Fabijenna)

## 19.12.13
- fixed build error on older Qt versions

## 19.12.12
- improved text un-indentation of list elements (for [#1455](https://github.com/pbek/QOwnNotes/issues/1455))
- improved dark mode code block highlighting in the preview if editor highlighting
  styles are used (for [#1516](https://github.com/pbek/QOwnNotes/issues/1516))
- fixed unnecessary note folder reload when the current note is tagged by
  double clicking a tag or using the *Note tags panel*
- the screen resolutions are now shown in the *Debug settings*
- fixed a possible highlighting crash and css class and id highlighting
  (for [#57](https://github.com/pbek/qmarkdowntextedit/pull/57), thank you @Waqar144)
- added more Catalan, Swedish, Bosnian, Greek, Hebrew, Danish, Croatian, Romanian,
  Finnish, Polish, Slovenian, Slovak translation (thank you MGuerra, Fabijenna)

## 19.12.11
- fixed note tagging (got broken in 19.12.9, for [#1510](https://github.com/pbek/QOwnNotes/issues/1510))
- improved text indentation of selected text blocks (for [#1455](https://github.com/pbek/QOwnNotes/issues/1455))
- added typescript highlighting support (for [#1513](https://github.com/pbek/QOwnNotes/pull/1513), thank you @Waqar144)
- added more Russian, Italian, Hungarian, Dutch, Spanish translation (thank you
  unchase, mlocati, Fabijenna, richarson)

## 19.12.10
- you now can choose if you want to use a **<kbd>Tab</kbd> or spaces** to **indent text**
  in the *Editor settings* (for [#1455](https://github.com/pbek/QOwnNotes/issues/1455))
    - you can also choose how many spaces should be used
    - this also works with indenting selected text and in lists
    - now **by default four spaces** will be used to **indent text** instead of a <kbd>Tab</kbd>
- the num literal highlighter in CSH was re-implemented, multiline comment support was added
  and more syntax highlighter refactoring and optimization was done
  (for [#52](https://github.com/pbek/qmarkdowntextedit/pull/52),
  [#53](https://github.com/pbek/qmarkdowntextedit/pull/53),
  [#54](https://github.com/pbek/qmarkdowntextedit/pull/54) and
  [#55](https://github.com/pbek/qmarkdowntextedit/pull/55) thank you @Waqar144)
- added more Spanish, Italian, Catalan, Slovenian, Polish, Macedonian, Swedish,
  Basque, Uzbek, Danish, Bulgarian, Cebuano, Croatian, Norwegian, Czech, Finnish,
  Icelandic, Punjabi, Latvian, Romanian, Lithuanian, Dutch, Hungarian, Slovak,
  Russian, German translation (thank you richarson, mlocati, MGuerra, Fabijenna,
  unchase, rakekniven, tiwi90)

## 19.12.9
- you can now choose in the *Preview settings* if you want to use the internal export
  styling for exporting (also partial) or printing of notes or if you want to use the
  styling of your preview (for [#1469](https://github.com/pbek/QOwnNotes/issues/1469))
    - by default the internal styling will be used
    - this is especially useful for the dark mode
- a lot of general performance optimizations were done (for [#1503](https://github.com/pbek/QOwnNotes/pull/1503)
  and [#1505](https://github.com/pbek/QOwnNotes/pull/1505), thank you @Waqar144)
- added more Uzbek translation (thank you Fabijenna)

## 19.12.8
- trailing and leading spaces will now be trimmed from pasted urls in the
  *Image dialog* (for [#1500](https://github.com/pbek/QOwnNotes/issues/1500))
- more general performance optimizations were done (for [#1501](https://github.com/pbek/QOwnNotes/pull/1501)
  and [#1502](https://github.com/pbek/QOwnNotes/pull/1502), thank you @Waqar144)
- when exporting the preview of a note (full note or just selected text in the note
  editor) a uniform set of styles will now be used regardless of if in dark mode or not
  (for [#1469](https://github.com/pbek/QOwnNotes/issues/1469))
    - printing the preview of a note (full note or just selected text in the note
      editor) still uses the styles of the preview, if you want to use your custom
      colors you are able to print to a PDF (if your operating system supports it)
    - the trailing newline in code blocks will now be removed when exporting so
      that there isn't an empty line at the end of each code block
- added more Basque, Macedonian, Ukrainian, Persian, Albanian, Kurdish, Finnish
  translation (thank you Fabijenna)

## 19.12.7
- hid the excessive `Unable to find any suggestion for` spellchecker warnings
- improved CSS color attribute highlighting (for [#49](https://github.com/pbek/qmarkdowntextedit/pull/49), thank you @Waqar144)
- fixed highlighting issue for punctuation after links (for [#1498](https://github.com/pbek/QOwnNotes/issues/1498))
- fixed inserting of images from urls in the *Image dialog* (for [#1500](https://github.com/pbek/QOwnNotes/issues/1500))
- improved folder switching time when tags are used (for [#1499](https://github.com/pbek/QOwnNotes/pull/1499), thank you @Waqar144)
- added more Albanian, Kurdish, Indonesian, Uzbek, Urdu (Pakistan), Bosnian,
  Irish, Persian, Bulgarian, Serbian (Cyrillic), Norwegian, Swedish, Estonian
  translation (thank you Fabijenna, zmni)

## 19.12.6
- fixed a problem when logging out of KDE while application is minimized to tray
  (for [#1496](https://github.com/pbek/QOwnNotes/issues/1496))
- fixed a problem with XML syntax highlighting (for [#47](https://github.com/pbek/qmarkdowntextedit/pull/47), thank you @Waqar144)
- improved code block colors for the *Light editor schema*
- improved application execution speed (for [#1497](https://github.com/pbek/QOwnNotes/issues/1497), thank you @Waqar144)
- added CSS syntax highlighting (for [#48](https://github.com/pbek/qmarkdowntextedit/pull/48), thank you @Waqar144)
- added more Uzbek, Urdu (Pakistan), Bosnian, Irish, Serbian (Cyrillic),
  Norwegian, Swedish, Estonian, Lithuanian, Hebrew, Finnish, Romanian, Dutch,
  Hungarian, Basque, Icelandic, Czech, Danish translation (thank you Fabijenna)

## 19.12.5
- fixed populating of the subfolder panel if you turn on subfolders for a note folder
  in the settings (for [#1493](https://github.com/pbek/QOwnNotes/issues/1493))
- lots of fixes and improvements for the code highlighting were made and support
  for JSON and XML was added (thank you @Waqar144)
- added more Lithuanian, Italian, German, Spanish, Slovenian, Croatian, Slovak,
  Catalan, Russian translation (thank you Fabijenna, mlocati, rakekniven,
  richarson, MGuerra, unchase)

## 19.12.4
- fixed a problem with loading notes if no note subfolder was selected
  (for [#1489](https://github.com/pbek/QOwnNotes/issues/1489))
- lots of syntax highlighting improvements were made (for [#40](https://github.com/pbek/qmarkdowntextedit/pull/40), thank you @Waqar144)
- fixed deselection of multi folder selection again (for [#327](https://github.com/pbek/QOwnNotes/issues/327), thank you @Waqar144)
- added more Uzbek, Lithuanian, Danish, Estonian, Albanian translation (thank you Fabijenna)

## 19.12.3
- fixed deselection of multi folder selection (for [#327](https://github.com/pbek/QOwnNotes/issues/327), thank you @Waqar144)
- dialogs like the *Todo dialog*, the *Issue assistant dialog* or the *Action dialog*
  will now raise across other windows if activated while already active
  (for [#1485](https://github.com/pbek/QOwnNotes/issues/1485))
- the Gentoo EBuilds now are using EAPI7 (for [#1484](https://github.com/pbek/QOwnNotes/issues/1484), thank you @internethering)
- added more Danish, Finnish translation (thank you Fabijenna)

## 19.12.2
- you now can select multiple note tags to filter notes
  (for [#327](https://github.com/pbek/QOwnNotes/issues/327), thank you @Waqar144)
- you can now move paragraphs up/down with <kbd>Ctrl + Shift + ↑/↓</kbd>
  (for [#569](https://github.com/pbek/QOwnNotes/issues/569), thank you @Waqar144)
- added rust, java, csharp, go, v-lang and SQL syntax highlighting support
  in code blocks in the note edit (for [#38](https://github.com/pbek/qmarkdowntextedit/pull/38), thank you @Waqar144)
- fixed SLE build process (thank you @Waqar144)
- added more German, Catalan, Hungarian, Italian, Dutch, Russian translation
  (thank you rakekniven, MGuerra, Fabijenna, mlocati, unchase, tiwi90)

## 19.12.1
- you now can select multiple note subfolders to filter notes
  (for [#327](https://github.com/pbek/QOwnNotes/issues/327), thank you @Waqar144)
- attempted to switch to Core18 for the snap release on Launchpad
  (for [#1439](https://github.com/pbek/QOwnNotes/issues/1439))
- fixed headline background highlighting (for [#1473](https://github.com/pbek/QOwnNotes/issues/1473), thank you @Waqar144)
- fixed notes not switching from system tray menu (for [#1467](https://github.com/pbek/QOwnNotes/issues/1467), thank you @Waqar144)
- added basic C, C++, JS, PHP, Bash, Python and QML syntax highlighting support (thank you @Waqar144)
- added optional aspell support (for [#1468](https://github.com/pbek/QOwnNotes/pull/1468), thank you @Waqar144)
- added more Spanish, German, Catalan translation (thank you richarson, rakekniven, MGuerra)

## 19.12.0
- improved code block styling matching (for [#1462](https://github.com/pbek/QOwnNotes/issues/1462))
- fixed link headline styling (for [#29](https://github.com/pbek/qmarkdowntextedit/pull/29), thank you @Waqar144)
- improved headline highlighting parsing speed (for [#30](https://github.com/pbek/qmarkdowntextedit/pull/30), thank you @Waqar144)
- improved broken link highlighting speed (for [#1465](https://github.com/pbek/QOwnNotes/issues/1465), thank you @Waqar144)
- added more Albanian, Latvian, Punjabi, Hungarian, Dutch, Indonesian, Russian,
  Spanish, German, Italian translation (thank you Fabijenna, zmni, unchase,
  richarson, rakekniven, mlocati)

## 19.11.23
- now most of the editor styling will be also used in the preview by default
  (for [#1462](https://github.com/pbek/QOwnNotes/issues/1462))
    - this can be turned off in the *Preview settings*
- you now change the current note folder in the tray icon
  (for [#1451](https://github.com/pbek/QOwnNotes/issues/1451))
    - the current note folder is marked with an icon (making it bold didn't seem to work)
- added more Uzbek, Japanese translation (thank you Fabijenna, shitennouji)

## 19.11.22
- more missing locales and dictionary names were fixed (for [#1459](https://github.com/pbek/QOwnNotes/issues/1459), thank you @Waqar144)
- fixed unable to switch back to last note subfolder after jumping to a subfolder
  with the `Jump to the note's subfolder` note list context menu item
  (for [#1432](https://github.com/pbek/QOwnNotes/issues/1432))
- the return key is now ignored in the *Table dialog* so you can better edit text
  in the table (for [#1446](https://github.com/pbek/QOwnNotes/issues/1446))
- fixed distraction free mode not restored properly on app restart if the app
  previously was maximized (for [#1448](https://github.com/pbek/QOwnNotes/issues/1448), thank you @Waqar144)
- added more Indonesian, Italian, German, Russian translation (thank you zmni,
  mlocati, tiwi90, rakekniven, unchase)

## 19.11.21
- added a note list context menu item to jump to the note subfolder of the
  selected note (for [#1432](https://github.com/pbek/QOwnNotes/issues/1432), thank you @Waqar144)
- did more spellchecker optimization (for [#1442](https://github.com/pbek/QOwnNotes/pull/1442), thank you @Waqar144)
- fixed autodetection for some languages that do not have trigrams (e.g. for Malayan)
  (for [#1443](https://github.com/pbek/QOwnNotes/pull/1443), thank you @Waqar144)
- fixed dictionary names not being displayed correctly (for [#1450](https://github.com/pbek/QOwnNotes/pull/1450), thank you @Waqar144)
- improved *List item* and *Trailing spaces* colors for `Mountain` highlighting color schema
- you can now also set the horizontal pixels directly in the *Image dialog*
  (for [#1445](https://github.com/pbek/QOwnNotes/issues/1445))
- fixed dark mode hover color for newer Qt versions (for [#1394](https://github.com/pbek/QOwnNotes/issues/1394))
- you can now not only select the table layout in a table widget in the *Table dialog*,
  but also **enter text directly in the fields** which will be used as table text when
  inserting the table (for [#1446](https://github.com/pbek/QOwnNotes/issues/1446))
    - if you enter text then all the spinboxes to define the table specs will
      be updated automatically
    - if you select a table layout via mouse after text was added to the table
      the spinboxes will now only update if you are selecting a larger range
      than the text entered
- added Ubuntu 20.04 (Focal Fossa) support to the Launchpad build system
- added more Catalan, Chinese Simplified, Italian, Kurdish, Dutch, Hungarian, Uzbek,
  Russian translation (thank you MGuerra, admiraldesvl, mlocati, Fabijenna, unchase)

## 19.11.20
- greatly improved performance of highlighting (for [#1440](https://github.com/pbek/QOwnNotes/pull/1440), thank you @Waqar144)
- fixed highlighting for links like `<http://hostname>` (for [#1436](https://github.com/pbek/QOwnNotes/issues/1436))
- fixed a problem with the `leave` button showing up multiple times when repeatedly
  entering and exiting full-screen mode (for [#1438](https://github.com/pbek/QOwnNotes/issues/1438))
- added additional information text to the *Layout settings*
- added more German, Uzbek, Basque, Italian, Russian, Vietnamese, Hungarian, Dutch,
  Spanish translation (thank you rakekniven, Fabijenna, mlocati, unchase, richarson)

## 19.11.19
- fixed a problem where the background of masked characters (for example `*` in italic text)
  is overwriting parts of the neighboring characters even if the masked characters
  have no background configured (for [#1434](https://github.com/pbek/QOwnNotes/issues/1434))
- a lot more dictionaries have been added to the *dictionary manager* to download
  (for [#1435](https://github.com/pbek/QOwnNotes/pull/1435), thank you @Waqar144)
    - we now have **dictionaries for 100 languages**
- added **Irish and Uzbek translation** (a big thank you to Fabijenna)
    - join us at [QOwnNotes on Crowdin](https://crowdin.com/project/qownnotes)
      to make QOwnNotes available in more languages or help with the current
      translation
- added more Vietnamese, Italian translation (thank you Fabijenna, tiwi90)

## 19.11.18
- fixed possible jumping of text on Windows 10 if the interface font size was overwritten
  (for [#1422](https://github.com/pbek/QOwnNotes/issues/1422))
- fixed custom editor font size on Windows 10 if the interface font size was overwritten
  (for [#1423](https://github.com/pbek/QOwnNotes/issues/1423))

## 19.11.17
- the redo shortcut <kbd>Ctrl + Shift + Z</kbd> was fixed in the note text edit
  (for [#1433](https://github.com/pbek/QOwnNotes/issues/1433), thank you @Waqar144)
- lots of general performance and efficiency improvements were made
  (for [#1430](https://github.com/pbek/QOwnNotes/pull/1430), thank you @Waqar144)
- added more Catalan, Vietnamese, Dutch, Spanish, Italian, Chinese Simplified,
  Russian, Greek, German translation (thank you MGuerra, Fabijenna, richarson,
  mlocati, admiraldesvl, unchase, athrouss, rakekniven)

## 19.11.16
- fixed AppVeyor Windows build (for [#1431](https://github.com/pbek/QOwnNotes/issues/1431))
- spellchecking language autodetection in KDE was improved
  (for [#1428](https://github.com/pbek/QOwnNotes/pull/1428), thank you @Waqar144)
- fixed a note history memory leak (for [#1429](https://github.com/pbek/QOwnNotes/pull/1429), thank you @Waqar144)

## 19.11.15
- a new button `Copy debug information to clipboard` was added to the
  *Debug settings* to be used when posting issues on GitHub
    - keep in mind that the preferred way to post issues is still the
      *Issue assistant*, it does most of the work for
    - the other buttons were a bit rearranged and some dialogs added
- rendering of italic or bold text and links in headings was fixed
  (for [#742](https://github.com/pbek/QOwnNotes/issues/742), thank you @Waqar144)
- spellchecking is now disabled in the log panel and the text edit in the
  *Debug settings* (for [#1423](https://github.com/pbek/QOwnNotes/issues/1423), thank you @Waqar144)
- added more Italian, Albanian, Bulgarian, Greek, Hungarian, Basque, Czech,
  Bosnian, Finnish, Icelandic, Norwegian, Croatian, Danish, Dutch, Spanish
  translation (thank you mlocati, Fabijenna, richarson)

## 19.11.14
- the spellchecker is now disabled if no dictionaries are available and language
  auto-detection enabling is fixed for multiple languages
  (for [#1418](https://github.com/pbek/QOwnNotes/issues/1418), thank you @Waqar144)
- spellchecker information was added to the *Debug settings*
- added more Dutch, Catalan, Russian, German translation (thank you Fabijenna,
  MGuerra, unchase, rakekniven)

## 19.11.13
- fixed a possible crash if a spellchecker dictionary was removed but its user
  dictionary was still present
- trailing spaces are now highlighted in the note editor
  (for [#1416](https://github.com/pbek/QOwnNotes/issues/1416))
    - you can set a custom background color in the *Edit fonts & colors settings*
- fixed multiple block undo with bracket auto close
  (for [#1317](https://github.com/pbek/QOwnNotes/issues/1317), thank you @Waqar144)

## 19.11.12
- fixed automatic switching to the default dark editor schema when opening the settings
  dialog in dark mode (for [#1408](https://github.com/pbek/QOwnNotes/issues/1408))
- added all available dictionaries to the *Manage dictionaries dialog*
  (for [#1409](https://github.com/pbek/QOwnNotes/pull/1409), thank you @Waqar144)
- added more Bulgarian, Albanian, Slovak, Romanian, Croatian, Icelandic, Czech,
  Danish, Dutch, Finnish, Hungarian, Greek, Norwegian, Basque, Slovenian, Bosnian,
  German, Punjabi, Latvian, Russian translation (thank you Fabijenna, rakekniven, unchase)

## 19.11.11
- you now can disable all external dictionaries in the *Manage dictionaries dialog*
  (for [#1399](https://github.com/pbek/QOwnNotes/issues/1399))
    - if activated only the downloaded dictionaries will be used
- the *Manage dictionaries dialog* now uses [QOwnNotes dictionaries](https://github.com/qownnotes/dictionaries)
  as source to be able to quickly fix dictionary issues or add new dictionaries
  (for [#1399](https://github.com/pbek/QOwnNotes/issues/1399), thank you @Waqar144)
    - spellchecking for some abbreviations have been fixed
    - more dictionaries were added to the dialog
- added automatic spellchecking detection for British English and Austrian German
  (for [#1402](https://github.com/pbek/QOwnNotes/pull/1402), thank you @Waqar144)
- updated the demo notes and [Markdown Cheatsheet](https://github.com/pbek/QOwnNotes/blob/develop/src/demonotes/Markdown%20Cheatsheet.md)
  with checkbox list and spellchecking information
- changed the *Readonly mode* icon to `document-edit`
- added **Latvian and Vietnamese translation** (a big thank you to Fabijenna)
    - join us at [QOwnNotes on Crowdin](https://crowdin.com/project/qownnotes)
      to make QOwnNotes available in more languages or help with the current
      translation
- added more Punjabi, Irish, Icelandic translation (thank you Fabijenna)

## 19.11.10
- if you select `Dark mode` in the *Interface settings* now a dark color schema
  will also be automatically selected in the *Editor fonts & colors settings*
    - if you un-select `Dark mode` a light color schema will be selected
- texts like `<*.txt>` or `<file.*>` with `*` will now not be converted to note
  links in the preview (for [#266](https://github.com/pbek/QOwnNotes/issues/266))
- improved special character detection in the Evernote import, the HTML import
  from the clipboard and the title detection of the link dialog
  (for [#1400](https://github.com/pbek/QOwnNotes/issues/1400))
- did some general performance optimization (thank you @Waqar144)
- fixed specific dictionary language not being setup at startup (thank you @Waqar144)
- bracket auto-closing in combination with "undo" was improved
  (for [#24](https://github.com/pbek/qmarkdowntextedit/pull/24), thank you @Waqar144)
- added more Latvian, Punjabi, Indonesian, Macedonian, Albanian, Lithuanian,
  Catalan, Serbian (Cyrillic), Galician, Slovak, Slovenian, Romanian,
  Norwegian, Swedish, French, Spanish, Russian, Bulgarian, Greek, Hungarian,
  Icelandic, Croatian, Kurdish, Czech, Danish, Basque, Estonian, Dutch, Finnish,
  Bosnian, German translation (thank you Fabijenna, zmni, MGuerra, Jeeps,
  richarson, unchase, rakekniven)

## 19.11.9
- did some optimization for spellchecking auto detect mode
  (for [#1359](https://github.com/pbek/QOwnNotes/issues/1359), thank you @Waqar144)
- fixed build process for Qt < 5.6 (for [#1379](https://github.com/pbek/QOwnNotes/issues/1379), thank you @Waqar144)
 
## 19.11.8
- there now is a new menu entry `Manage dictionaries` in the *Edit* menu to download
  and remove spellchecker dictionaries (for [#1378](https://github.com/pbek/QOwnNotes/issues/1378))
    - if you are missing a dictionary to download please look for it at
      [LibreOffice dictionaries](https://github.com/LibreOffice/dictionaries) and edit
      the [dictionary manager dialog](https://github.com/pbek/QOwnNotes/blob/develop/src/dialogs/dictionarymanagerdialog.cpp)
    - visit [QOwnNotes Spellchecking](http://docs.qownnotes.org/en/develop/spellchecking.html)
      for more information
- automatic switching of note subfolders is now prevent if the setting to show notes in
  note subfolders recursively is enabled (for [#1371](https://github.com/pbek/QOwnNotes/issues/1371))
    - this is still experimental and might have side effects
- personal dictionary will now be stored in the portable data path when using QOwnNotes
  in portable mode (for [#1378](https://github.com/pbek/QOwnNotes/issues/1378), thank you @Waqar144)
- underscores at the end of a word are now ignored for `_italic_` formatting with underscores
  (for [#1379](https://github.com/pbek/QOwnNotes/issues/1379), thank you @Waqar144)
- added more Spanish, Cebuano, Kurdish, Lithuanian, Ukrainian, Norwegian, Swedish,
  Croatian, Czech, Dutch, Danish, Estonian, Hungarian, Finnish, Galician, Bosnian,
  Bulgarian, Greek, Romanian, Serbian (Cyrillic), Slovak, Slovenian translation
  (thank you fitoschido, Fabijenna)

## 19.11.7
- fixed a possible crash on startup if markdown highlighting is disabled
  (for [#1367](https://github.com/pbek/QOwnNotes/issues/1367), thank you @Waqar144)
- the macOS version of QOwnNotes now also uses the Hunspell spellchecker for more performance
  (for [#1359](https://github.com/pbek/QOwnNotes/issues/1359), thank you @Waqar144)
    - visit [QOwnNotes Spellchecking](http://docs.qownnotes.org/en/develop/spellchecking.html)
      for more information
- added more Cebuano, Kurdish, Lithuanian, Norwegian, Swedish, Ukrainian, Croatian,
  Czech, Dutch, Danish, Estonian, Hungarian, Finnish, Galician, Bosnian, Bulgarian,
  Greek, Romanian, Serbian (Cyrillic), Slovak, Slovenian, Italian, Indonesian,
  Russian, Catalan translation (thank you Fabijenna, tiwi90, zmni, athrouss,
  unchase, mlocati, MGuerra)

## 19.11.6
- attempted to fix a possible segmentation fault on startup
  (for [#1367](https://github.com/pbek/QOwnNotes/issues/1367), thank you @Waqar144)
- attempted to improve macOS spellchecker performance
  (for [#1359](https://github.com/pbek/QOwnNotes/issues/1359), thank you @Waqar144)
- added **Kurdish** and **Lithuanian translation** (a big thank you to Fabijenna)
    - join us at [QOwnNotes on Crowdin](https://crowdin.com/project/qownnotes)
      to make QOwnNotes available in more languages or help with the current
      translation
- added more Bosnian, Estonian translation (thank you Fabijenna)

## 19.11.5
- fixed a problem with automatic setting of the dark mode color and dark mode
  icon theme checkboxes in dark mode
- fixed a problem where you can't see see a checked state of menu items in dark mode
  if the menu item has an icon (for [#1221](https://github.com/pbek/QOwnNotes/issues/1221))
- did highlighting and spellchecking speed improvements and improved initial note
  loading performance (for [#1359](https://github.com/pbek/QOwnNotes/issues/1359), thank you @Waqar144)
- fixed some memory leaks (for [#1360](https://github.com/pbek/QOwnNotes/pull/1360), thank you @Waqar144)
- fixed the right click context menu in the note list if the folder has no notes
  (for [#1361](https://github.com/pbek/QOwnNotes/issues/1361), thank you @Waqar144)
- added more Danish, Bosnian, Icelandic translation (thank you Fabijenna)

## 19.11.4
- fixed the default text edit context menu for the spellchecker
  (for [#1355](https://github.com/pbek/QOwnNotes/issues/1355), thank you @Waqar144)
- fixed a possible crash when right-clicking on the note list
  (for [#1353](https://github.com/pbek/QOwnNotes/issues/1353), thank you @Waqar144)
- added `~/hunspell` as path for spellchecker dictionaries for Snap package
  (for [#1348](https://github.com/pbek/QOwnNotes/issues/1348))
- when creating a new note folder now by default the local path of the active
  note folder will be used (for [#1354](https://github.com/pbek/QOwnNotes/issues/1354))
- added more Cebuano, German translation (thank you Fabijenna, rakekniven)

## 19.11.3
- fixed wrong note text insertion from the web companion and other parts of the
  application if the `=` headline markings were breaking into the next line in
  the note text edit (for [#1351](https://github.com/pbek/QOwnNotes/issues/1351))
- fixed many optimization regressions from [7848f4de14fd152be8a8ffc7c8d97f8f37d1a431](https://github.com/pbek/QOwnNotes/commit/7848f4de14fd152be8a8ffc7c8d97f8f37d1a431)
- attempted to fix inclusion of spellchecker dictionaries for Snap package
  (for [#1348](https://github.com/pbek/QOwnNotes/issues/1348), thank you @Waqar144)
- added more Bosnian, Romanian, Norwegian, Icelandic, Macedonian, Finnish,
  Slovenian, Czech, Croatian, Cebuano, Serbian (Cyrillic), French, Basque
  translation (thank you Fabijenna, Jeeps)

## 19.11.2
- spellchecking is now enabled in formatted markdown (for [#125](https://github.com/pbek/QOwnNotes/issues/125), thank you @Waqar144)
- fixed margin calculation in distraction free mode that was changed in 19.11.0

## 19.11.1
- improvements where made to the **note / subfolder tree mode**
  (for [#790](https://github.com/pbek/QOwnNotes/issues/790))
    - on application startup the last opened note will be loaded from the correct subfolder
- fixed highlighting of code blocks with empty lines (for [#125](https://github.com/pbek/QOwnNotes/issues/125))
- fixed highlighting of two-line headlines when typing them in (for [#125](https://github.com/pbek/QOwnNotes/issues/125))
- did some minor speed improvements
- added more Russian, Bulgarian, Croatian, Basque, Swedish, Slovak translation
  (thank you data-man, Fabijenna)

## 19.11.0
- improvements where made to the **note / subfolder tree mode**
  (for [#790](https://github.com/pbek/QOwnNotes/issues/790))
    - note search by text and filtering by tag is now working
    - if you right click on an item you will now either get the context menu
      for a note or a subfolder depending what item you have clicked
        - you can create a new note or subfolder via context menu
        - you can show a subfolder in file manager via context menu
        - you can remove selected subfolder via context menu
        - you can rename the current subfolder via context menu, by clicking it
          twice or by pressing <kbd>F2</kbd>
    - if you click a subfolder the current note subfolder will be changed to it
- attempted to fix building the application on Qt 5.3, add more dictionary paths
  for Linux and skipping highlighting and spell checking if a text block is empty
  or just has spaces (for [#125](https://github.com/pbek/QOwnNotes/issues/125), thank you @Waqar144)
    - if you need to add additional language files on Linux you can do so in `~/.local/share/hunspell`
- fixed a note naming problem with the scripting function `createNote` if it was
  called multiple times in the same second (for [#1345](https://github.com/pbek/QOwnNotes/issues/1345))
- added more Basque, German translation (thank you Fabijenna, rakekniven)

## 19.10.13
- fixed spellchecking for notes with certain types of code blocks
  (for [#125](https://github.com/pbek/QOwnNotes/issues/125), thank you @Waqar144)
- attempted to fix building the application on Qt <= 5.5
  (for [#125](https://github.com/pbek/QOwnNotes/issues/125), thank you @Waqar144)
- added more Dutch, Finnish, Hungarian, Czech, Croatian, Danish, Indonesian,
  Catalan, Spanish, Brazilian Portuguese, Russian, German, Polish translation
  (thank you Fabijenna, zmni, MGuerra, richarson, mlimacarlos, unchase, rakekniven, lucjan)

## 19.10.12
- there now is a **spellchecker** integrated into QOwnNotes
  (for [#125](https://github.com/pbek/QOwnNotes/issues/125), a big thank you to @Waqar144)
    - [Sonnet](https://github.com/KDE/sonnet) with [Hunspell](https://hunspell.github.io/) is used for spellchecking
    - under **Linux** you need to install the Hunspell dictionaries for your language
    - under **Windows** you need to copy Hunspell dictionaries for your language
      to a folder `dicts` in your QOwnNotes application folder or in `C:\Users\[username]\dicts`
        - you can download dictionaries from [LibreOffice dictionaries](https://github.com/LibreOffice/dictionaries)
    - under **macOS** the native spellchecker will be used
        - you can download more dictionaries from [LibreOffice dictionaries](https://github.com/LibreOffice/dictionaries)
          and copy them to `~/Library/Spelling`, if the directory doesn't exist
          you need to create it
    - dictionary files are `*.dic` or `*.aff` files, visit
      [QOwnNotes Spellchecking](http://docs.qownnotes.org/en/develop/spellchecking.html)
      for more information
- you now can **disable todo lists** completely in the *Todo settings*
    - no todo calendar items will be loaded in the background
    - if you open the todo dialog you will get a message that todo lists are disabled
- added more Brazilian Portuguese, Catalan, Russian, German, Polish, Greek,
  Italian translation (thank you mlimacarlos, MGuerra, unchase, rakekniven,
  lucjan, athrouss, tiwi90)

## 19.10.11
- you now can choose to **show your notes together with your subfolders in a tree**
  (for [#790](https://github.com/pbek/QOwnNotes/issues/790))
    - you can enable this in the *Panels settings*
    - this is work in progress and by far not fully implemented yet!
    - the note tree will only be shown if subfolders are enabled for your current note folder
- there now is a **new scripting hook** `preNoteToMarkdownHtmlHook(note, markdown)`
  that is **called before the markdown html of a note is generated**
  (for [#1328](https://github.com/pbek/QOwnNotes/issues/1328))
    - it allows you to modify the markdown before it is converted to html
    - it can for example be used in multiple scripts to render code (like LaTeX math or mermaid)
      to its graphical representation for the preview
    - there is an example call in
      [preview-styling.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/preview-styling.qml)
    - for more information please take a look at the
      [scripting documentation](https://www.qownnotes.org/scripting/methods-and-objects.html#prenotetomarkdownhtmlhook)
- fixed a highlighting problem for checkbox link lists like `- [ ] [QOwnNotes](https://www.qownnotes.org)`
- fixed a possible crash when logging occurs on application exit
- added **Macedonian** and **Bosnian translation** (a big thank you to Fabijenna)
    - join us at [QOwnNotes on Crowdin](https://crowdin.com/project/qownnotes)
      to make QOwnNotes available in more languages or help with the current
      translation
- added more Dutch, Polish, Russian, German, Estonian, Brazilian Portuguese,
  Catalan, Urdu (Pakistan) translation (thank you Fabijenna, lucjan, unchase,
  rakekniven, mlimacarlos, MGuerra)

## 19.10.10
- if a **frontmatter** was detected at the beginning of a note the **filename of the note**
  now will be the **first non-empty line after the frontmatter** instead of the
  first line of the note (for [#1031](https://github.com/pbek/QOwnNotes/issues/1031))
- another attempt was made to make the Botan 2.12 library work on non-x86 platforms
  (for [#1263](https://github.com/pbek/QOwnNotes/issues/1263), thank you @Waqar144)

## 19.10.9
- an attempt was made to make the Botan 2.12 library work on non-x86 platforms
  (for [#1263](https://github.com/pbek/QOwnNotes/issues/1263))
- [scribble script](https://github.com/qownnotes/scripts/tree/master/scribble)
  was updated for relative media links (for [#266](https://github.com/pbek/QOwnNotes/issues/266))

## 19.10.8
- **frontmatter blocks** are now **highlighted** as masked syntax and are also
  **ignored in the preview** (for [#1031](https://github.com/pbek/QOwnNotes/issues/1031))
    - the frontmatter block has to start and end with a `---` line, it has to
      be at the beginning of the note and only one block can exist per note
    - don't forget that you have to enable `Allow note file name to be different from headline`
      in the *General settings* if you want to use a frontmatter block
- if now you press <kbd>Return</kbd> after a `- []` in a checkbox list the
  current line will be removed just like with `- [x]` and `- [ ]`
- there now is a new method `getFileURLFromFileName()` for `NoteApi` to get the
  absolute file url from a relative file name (for [script issue #71](https://github.com/qownnotes/scripts/issues/71))
    - for more information please take a look at the
      [scripting documentation](https://www.qownnotes.org/scripting/methods-and-objects.html#note)
- there now is a new property `fullNoteFileDirPath` for `NoteApi` to get the
  absolute directory path of the a note file (for [script issue #71](https://github.com/qownnotes/scripts/issues/71))
    - for more information please take a look at the
      [scripting documentation](https://www.qownnotes.org/scripting/methods-and-objects.html#note)
- added more Russian, Polish, German, Italian, Punjabi, Estonian, Bengali,
  Icelandic, Ukrainian, Korean, Norwegian, Persian, Romanian, Slovak, Slovenian,
  Swedish, Dutch, Croatian, Serbian (Cyrillic), Czech, Danish, Finnish, Galician,
  Greek, Hausa, Hungarian, Spanish translation (thank you unchase, lucjan,
  rakekniven, mlocati, Fabijenna, DixTheDev)

## 19.10.7
- now **all supported image formats** can be **selected with the file picker**
  of the **Image dialog** (for [#1314](https://github.com/pbek/QOwnNotes/issues/1314))
    - in addition you can now also choose `Any file` in the file picker
- if you press <kbd>Return</kbd> in a **checkbox list** the **next line** will
  now be **started with an unchecked checkbox**, regardless of if the checkbox
  in the current line was checked or not
- added **Korean translation** (a big thank you to Fabijenna)
    - join us at [QOwnNotes on Crowdin](https://crowdin.com/project/qownnotes)
      to make QOwnNotes available in more languages or help with the current
      translation
- an attempt was made to make the Botan 2.12 library work on non-SSE platforms
  (for [#1263](https://github.com/pbek/QOwnNotes/issues/1263), thank you @Waqar144)
- added more Punjabi, Estonian, Bengali, Serbian (Cyrillic), Persian, Ukrainian,
  Russian, Czech, Icelandic, Croatian, Galician, Bulgarian, Romanian, Danish,
  Slovak, Finnish, Dutch, Slovenian, Greek, Hausa, Hungarian, Swedish, Hebrew,
  Spanish, German, Italian translation (thank you Fabijenna, unchase, richarson,
  rakekniven, tiwi90)

## 19.10.6
- it is now also possible to **link to directories** in the *Link dialog*
  (for [#1308](https://github.com/pbek/QOwnNotes/issues/1308))
    - your local file manager should open if you click on those links
- the Botan library for encrypting notes was updated to **Botan 2.12**
  (for [#1263](https://github.com/pbek/QOwnNotes/issues/1263), thank you @Waqar144)
- added more Indonesian, Punjabi, Spanish, Catalan, Polish, Russian, Brazilian Portuguese,
  Greek, Italian, German translation (thank you zmni, Fabijenna, DixTheDev,
  MGuerra, lucjan, unchase, mlimacarlos, athrouss, tiwi90, rakekniven)

## 19.10.5
- if you start the application but your **note folder wasn't found** you now are
  able to **select a new note folder** (for [#1307](https://github.com/pbek/QOwnNotes/issues/1307))
    - you will not get into the *Welcome dialog* any more where your settings
      will be removed if you cancel it
- a setting to disable the search result count calculation was added to improve search speed
  in note folders with a lot of notes (for [#1248](https://github.com/pbek/QOwnNotes/issues/1248), thank you @smokris)
- added more German, Spanish, Estonian, Korean, Icelandic, Bulgarian, Catalan,
  Russian, Italian translation (thank you rakekniven, richarson, Fabijenna,
  MGuerra, unchase, tiwi90)

## 19.10.4
- attempted to fix occasional desktop environment session logout interruptions
  (for [#1269](https://github.com/pbek/QOwnNotes/issues/1269))
- added **Estonian translation** (a big thank you to Fabijenna)
    - join us at [QOwnNotes on Crowdin](https://crowdin.com/project/qownnotes)
      to make QOwnNotes available in more languages or help with the current
      translation
- fixed deprecation warnings of Botan library for encrypting notes
  (for [#1304](https://github.com/pbek/QOwnNotes/issues/1304))

## 19.10.3
- fixed a Fedora 28 build problem on the Open Build Service
- fixed several more deprecation warnings to further advance the Qt 6 compatibility
  of QOwnNotes in the future (for [#1304](https://github.com/pbek/QOwnNotes/issues/1304))
- added more Russian, Icelandic, Albanian, Punjabi, Hebrew translation
  (thank you unchase, Fabijenna)

## 19.10.2
- fixed a crash when leaving the full-screen mode if the app was restarted while in
  full-screen mode and distraction free mode (for [#1302](https://github.com/pbek/QOwnNotes/issues/1302))
- fixed a crash when opening the settings dialog in the welcome dialog
  (for [#1303](https://github.com/pbek/QOwnNotes/issues/1303))
- the parameter `--clear-settings` now also removes the settings database if not in portable mode
- fixed several deprecation warnings to further advance the Qt 6 compatibility
  of QOwnNotes (for [#1304](https://github.com/pbek/QOwnNotes/issues/1304))
- added more Icelandic, Albanian, Punjabi, Hebrew, Brazilian Portuguese, Dutch,
  Hungarian, Danish, Finnish, Norwegian, Catalan, Spanish, Italian, German
  translation (thank you Fabijenna, mlimacarlos, MGuerra, richarson, tiwi90, rakekniven)

## 19.10.1
- you now can **search for note names** in the dialogs for the **remote trash**
  and **local trash**
- added **Albanian, Hebrew, Icelandic and Punjabi translation** (a big thank you to Fabijenna)
    - join us at [QOwnNotes on Crowdin](https://crowdin.com/project/qownnotes)
      to make QOwnNotes available in more languages or help with the current
      translation
- added more Persian, Dutch, Hungarian, Brazilian Portuguese, Bulgarian, Catalan,
  German, Italian, Polish, Russian translation (thank you Fabijenna, mlimacarlos,
  MGuerra, rakekniven, mlocati, tiwi90, lucjan, mikhail-khokhlov)

## 19.10.0
- after turning on the **typewriter mode** the cursor will now immediately
  be centered (for [#1296](https://github.com/pbek/QOwnNotes/issues/1296))
- if you select a text before using the **link dialog** to insert a link
  the **selected text will now be used as link text** by default
- added **Bulgarian translation** (a big thank you to Fabijenna)
    - join us at [QOwnNotes on Crowdin](https://crowdin.com/project/qownnotes)
      to make QOwnNotes available in more languages or help with the current
      translation

## 19.9.20
- the **typewriter mode** was improved (for [#1296](https://github.com/pbek/QOwnNotes/issues/1296))
    - in most cases the text will now also be centered if the cursor is at the
      bottom of the note
- if you install a script from the **script repository** now the installed script
  will be immediately selected and the **script settings** of that script will be
  **shown** if you close the script repository dialog (for [#1298](https://github.com/pbek/QOwnNotes/issues/1298))
- the **highlighting of the current line** now also works in **read-only mode**
- added more Spanish, Filipino, Turkish, Ukrainian, Urdu (Pakistan), Norwegian,
  Basque, Serbian (Cyrillic), Russian, Slovenian, Polish, Romanian, Dutch,
  Croatian, Finnish, Czech, Hungarian, Slovak, Danish, Swedish, Catalan, German
  translation (thank you richarson, Fabijenna, mikhail-khokhlov, MGuerra, rakekniven)

## 19.9.19
- there now is a **typewriter mode** that you can turn on in the *Window menu*
  (for [#1296](https://github.com/pbek/QOwnNotes/issues/1296))
- fixed a problem with entering a `/` in the server url in the cloud connection settings
- added more Italian, French translation (thank you tiwi90, Botigli)

## 19.9.18
- **cloud connection editing** was **improved** in the *Settings dialog*
    - parameters are now instantly stored
    - the cloud connection comboboxes are now updated instantly if the
      connection name was changed
- the user interface of the **cloud connection selection** in the
  **Todo list settings** was improved
    - the combobox now got its own groupbox that will be hidden if you are
      using a custom CalDAV server
- if you have an image in the clipboard while opening the *Image dialog* the
  checkbox `Don't copy image to media folder` will now be automatically disabled
- the **title detection** in the **Link dialog** was **improved**
- added more Catalan, Swedish, Italian translation (thank you MGuerra, druus, tiwi90)

## 19.9.17
- you can now **choose different cloud connections** for your **note folders**
    - this way you can use the **correct remote note versioning and trash** and
      **share your notes** with the correct account if you are using
      **multiple Nextcloud / ownCloud instances or accounts**
    - some previous connection problems when changing connection parameters were
      now also fixed
    - the cloud connections are now also represented in the debug settings
- you can now **select a cloud connection** for your **Todo list calendars**
- fixed the Nextcloud tooltip texts in window title of the *Version dialog* and
  in an error message in the *Settings dialog*
- fixed a build error on the Open Build System (OBS)
- added more Catalan, Italian, Spanish translation (thank you MGuerra, tiwi90, richarson)

## 19.9.16
- you now can **manage multiple cloud connections** in the *Nextcloud / ownCloud settings*
    - in a future release these cloud connections can be bound to different note folders  
- some **improvements** with **encrypted notes** were made
    - if an encrypted note will be edited now the note file will **not be stored immediately**
      after the password is entered
    - the note will now **jump to the top** in the **note list** (if the note list is sorted
      that way) if the text of an encrypted note was changed
        - also the tooltip of the note in the note list will be updated
- media files are now only copied if the destination path is a note folder when
  copying or moving notes to another note folder (for [#1294](https://github.com/pbek/QOwnNotes/issues/1294))
- fixed the Nextcloud tooltip texts in the buttons on the *Nextcloud / ownCloud settings page*
- added more Greek, Dutch, Hungarian, Catalan, Indonesian, Russian, German translation
  (thank you athrouss, Fabijenna, MGuerra, zmni, mikhail-khokhlov, rakekniven)

## 19.9.15
- the **attachment dialog** was **improved**
    - you are now able to **download files** directly in the dialog
        - there also is a progress bar, along with status information on how
          much data you already have downloaded and a cancel button
        - if you enter an url and press `Ok` the file will also automatically
          gets downloaded
    - you are now able to enter a **title** that will be used in the attachment link
    - the dialog will now **detect** if you have a **local file path** of a file
      that exists on your computer in the **clipboard** when it is opened and use it
    - **`file://` urls** will now automatically be **converted to local file paths**
- you now can **move and copy notes** also to **subfolders of note folders** if you have
  enabled subfolders for the note folder (for [#1294](https://github.com/pbek/QOwnNotes/issues/1294))
    - keep in mind that only the note files will be copied/moved
    - note tags, links to other notes, images and attachments will not be transferred/adapted
- the **tab order** in the **image, attachment and link dialog** was **improved**
    - the first item with focus will now be the link/url edit field also on Windows
- added more Italian, Spanish translation (thank you tiwi90, richarson)

## 19.9.14
- the **link dialog** was **improved**
    - there now a tabs for entering an URL and selecting a note to make the
      dialog less cluttered
    - if you have a **url in the clipboard** it will automatically be entered
    - if the entered url is a webpage the **title will be fetched automatically**
        - this will now happen inside the dialog and not like previously outside
          of it where you had to wait for the result
    - the automatic **title fetching** now **detects more titles** and is in many
      cases **much faster**
    - if you now double-click on a note while there still is text in the
      url line edit the note will be linked and not the url in the line edit
- there now is a new **Insert attachment dialog**
    - the dialog will **detect** if you have a **file path or url** in the
      **clipboard** when it is opened and use it
    - **file size and type** are **displayed** when selecting a file

## 19.9.13
- if you **move notes to another subfolder** you now will also be able to automatically
  **adapt** the new **relative note file links** to that notes like `[note](note.md)` or
  `<note.md>` in all other notes (for [#266](https://github.com/pbek/QOwnNotes/issues/266))
    - furthermore **relative links of images and attachments** in the moved notes will be
      **automatically adapted** to the new subfolder
- if you **copy notes to another subfolder relative links of images and attachments**
  in the copied notes will be **automatically adapted** to the new subfolder
  (for [#266](https://github.com/pbek/QOwnNotes/issues/266))
- above changes conclude the monstrous effort of the migration to more portable relative
  file links for notes, images and attachments in [#266](https://github.com/pbek/QOwnNotes/issues/266)
- **QOwnNotesAPI** is now ready for **Nextcloud 17**
    - see [QOwnNotesAPI for Nextcloud](https://apps.nextcloud.com/apps/qownnotesapi)
- added more Basque, Spanish translation (thank you izendegi, richarson)

## 19.9.12
- if you **rename a note** you now will also be able to automatically **rename** new
  **relative note file links** like `[note](note.md)` or `<note.md>` in all other
  notes if they are found (for [#266](https://github.com/pbek/QOwnNotes/issues/266))
    - this also works with links from other note subfolders
- if you are using legacy links and are linking to notes that have utf8 characters and
  numbers in it now a `@` sign will be added (e.g. `note://zx000ūaųškj@`) to prevent
  faulty urls (for [#1292](https://github.com/pbek/QOwnNotes/issues/1292))
- added more Greek translation (thank you athrouss)

## 19.9.11
- adapted the offset when the selection-rubberband is created while cropping in
  the image dialog for all user interfaces (for [#1288](https://github.com/pbek/QOwnNotes/issues/1288))
- faulty `QFileSystemWatcher::removePaths` warnings are now ignored, they seem to have caused
  troubles on openSUSE Tumbleweed (for [#1290](https://github.com/pbek/QOwnNotes/issues/1290))
- added more Italian, Korean, Greek translation (thank you tiwi90, pgrules7, athrouss)

## 19.9.10
- added support for **note links from one subfolder tree to another** like
  `../../folder1/folder2/note.md` (for [#1287](https://github.com/pbek/QOwnNotes/issues/1287))
- removed the 2px offset when the selection-rubberband is created while cropping in
  the image dialog (for [#1288](https://github.com/pbek/QOwnNotes/issues/1288))
- added more Basque, German, Italian, Spanish translation (thank you izendegi,
  rakekniven, tiwi90, richarson)

## 19.9.9
- you can now **crop images** in the **Insert image dialog** by selecting the area
  to crop to and pressing the `Crop` button (for [#1288](https://github.com/pbek/QOwnNotes/issues/1288))
- added more Brazilian Portuguese translation (thank you mlimacarlos)

## 19.9.8
- **highlighting** of **broken note links** now also works with relative note links like
  `<note file.md>` and `[note](note file.md)` (for [#266](https://github.com/pbek/QOwnNotes/issues/266))
    - links to note files like `../note file.md` and `sub folder/note file.md` also work
- the button to create a new issue in the issue assistant by clearing the dialog
  was renamed to `Clear` because it could mislead users to think it creates a new
  issue on GitHub (for [#1289](https://github.com/pbek/QOwnNotes/issues/1289))
- added more Catalan, Spanish, Italian translation (thank you MGuerra, richarson, tiwi90)

## 19.9.7
- the **Insert image dialog** was further improved (for [#1285](https://github.com/pbek/QOwnNotes/issues/1285))
    - you can now paste **remote urls** that will be **automatically downloaded**
      and previewed in the dialog
        - local `file://` urls are also supported
    - the dialog will **detect** if you have **file paths or urls** in the
      **clipboard** when it is opened
    - there now is a new checkbox `Don't copy image to media folder` that allows to
      **directly insert urls or local paths**
        - paths to files will be made relative to the current note
        - the state of the checkbox will be persisted for more convenience
    - the maximal scaling in the slider was set to 2 for more usability
- the **Orphaned image files dialog** now detects relative image tags like `![imagename](media/30933630.jpeg)`
  (for [#266](https://github.com/pbek/QOwnNotes/issues/266))
    - the dialog is now also able to insert relative image tags
- the **Orphaned attachments dialog** now detects relative attachment tags like `[956321614](attachments/956321614.pdf)`
  (for [#266](https://github.com/pbek/QOwnNotes/issues/266))
    - the dialog is now also able to insert relative attachment tags
- disabled QML caching (for the scripts of the scripting engine) in portable mode because
  the QML cache path cannot be configured (for [#1284](https://github.com/pbek/QOwnNotes/issues/1284))
- fixed preview of remote images (for [#1286](https://github.com/pbek/QOwnNotes/issues/1286))
- added more Italian, Indonesian, Catalan, Brazilian Portuguese translation
  (thank you tiwi90, zmni, fitoschido, mlimacarlos)

## 19.9.6
- there now is a new **Insert image dialog** with **preview, scaling and clipboard**
  support (for [#1285](https://github.com/pbek/QOwnNotes/issues/1285))
    - if an image was in the clipboard it will be automatically loaded and previewed
    - you can also select an image from your file system
    - you can scale the image by setting the width or a scale factor, the preview
      will be updated accordingly
- when **inserting images into your media folder** like with the **Insert image dialog**,
  the **Evernote import**, via drag and drop or from the clipboard now markdown tags
  like `![imagename](media/30933630.jpeg)` are created (for [#266](https://github.com/pbek/QOwnNotes/issues/266))
    - this also works if you are inserting images in notes in subfolders, then the
      image would look for example like `![imagename](../media/30933630.jpeg)` if
      the depth of the subfolder is one
    - the scripting functions `downloadUrlToMedia` and `insertMediaFile` will now
      also generate a markdown image relative to the path of the current note
    - you can still use the old `file://media/30933630.jpeg` links if you enable
      legacy links in the *General settings*
- when **inserting files into your attachments folder** like with the **Insert attachment dialog**,
  the **Evernote import** or via drag and drop now markdown tags like `[filename](attachments/45412706.pdf)`
  are created (for [#266](https://github.com/pbek/QOwnNotes/issues/266))
    - this also works if you are inserting attachments in notes in subfolders, then the
      file would look for example like `[filename](../attachments/45412706.pdf)` if
      the depth of the subfolder is one
    - you can still use the old `file://attachments/45412706.pdf` links if you enable
      legacy links in the *General settings*
- when **linking to notes** like with the **Insert link dialog** or when splitting notes
  now markdown tags like `[note](other note.md)` are created (for [#266](https://github.com/pbek/QOwnNotes/issues/266))
    - this also works if the linked note is in another subfolders, then the
      link would look for example like `[note](../other note.md)` or `[note](sub folder/other note.md)`
    - you can still use the old `note://other_note` links if you enable legacy links
      in the *General settings*
- added more Hungarian, Dutch, Italian, Spanish, French, Greek, German, Catalan,
  Urdu (Pakistan) translation (thank you Fabijenna, tiwi90, richarson, Botigli,
  athrouss, rakekniven, MGuerra)

## 19.9.5
- added **Hausa translation** (a big thank you to Fabijenna)
    - join us at [QOwnNotes on Crowdin](https://crowdin.com/project/qownnotes/)
      to make QOwnNotes available in more languages or help with the current 
      translation
- the **preview of html tags** now works again (for [#1283](https://github.com/pbek/QOwnNotes/issues/1283))
    - html tags are now also not highlighted as link
- fixed incorrect note share status update (for [#1280](https://github.com/pbek/QOwnNotes/issues/1280))
- added more Greek, Dutch, Urdu (Pakistan), Spanish, German, French, Italian,
  Basque, Catalan translation (thank you athrouss, Fabijenna, richarson,
  rakekniven, Botigli, tiwi90, izendegi, MGuerra)

## 19.9.4
- you can now **allow note editing** when **sharing notes** on your **Nextcloud**
  server with the share dialog (for [#1280](https://github.com/pbek/QOwnNotes/issues/1280))
    - the person who receives the share link will then be able to edit your note
      with [Nextcloud Text](https://github.com/nextcloud/text)
- fixed the real time preview of [Nextcloud Text](https://github.com/nextcloud/text)
  images (for a regression in [#266](https://github.com/pbek/QOwnNotes/issues/266))
- added more Urdu (Pakistan) translation (thank you Fabijenna)

## 19.9.3
- **relative links to notes in subfolders** like `<My subfolder/My note.md>` or
  `[Text](My subfolder/My note.md)` will now be **rendered correctly** in the **preview**
  and are also **clickable** (for [#266](https://github.com/pbek/QOwnNotes/issues/266))
- you now can **open files** from markdown links like `<My folder/My document.pdf>` or
  `[Document](My folder/My document.pdf)` by <kbd>Ctrl</kbd> + Click on the link in the **note text edit**
  or clicking the link in the **preview** (for [#266](https://github.com/pbek/QOwnNotes/issues/266))
    - the links are resolved relative to the current note
- you can now also <kbd>Ctrl</kbd> + Click on **markdown images** like `![Image](My image.jpg)`
  in the **note text edit** to open it in an external image application
  (for [#266](https://github.com/pbek/QOwnNotes/issues/266))
    - the image links are resolved relative to the current note
- you now can press <kbd>Ctrl</kbd> + <kbd>Backspace</kbd> to **remove the current word**
  and <kbd>Alt</kbd> + <kbd>Backspace</kbd> to **remove the whole text** in **line edits**
  like the note text search (for [#1278](https://github.com/pbek/QOwnNotes/issues/1278))
- you can now also move **back and forward** in the **note history** with the
  **back button and forward button** on your mouse in the **note preview**
- added more Urdu (Pakistan) translation (thank you Fabijenna)

## 19.9.2
- you can now use **relative links to markdown files** to switch to notes
  (for [#266](https://github.com/pbek/QOwnNotes/issues/266))
    - clicking `<My note.md>` or `[Text](My note.md)` jumps to the note `My note`
    - clicking `<My subfolder/My note.md>` or `[Text](My subfolder/My note.md)` jumps
      to the note `My note` in the note subfolder `My-subfolder` relative to the
      note subfolder of your current note
      - there still are a cases where links would not work in the preview (or even be shown)
        when subfolders are involved
    - this should not only work in the preview but also when you <kbd>Ctrl</kbd> + Click
      on the link in the note text edit
- the **note subfolder** will now be **correctly set when switching to notes**
  that are not in the current note subfolder
- when you **search for notes** in the note search line edit the **note list**
  will be now **scrolled to the top** so you can see the first search results
  (for [#1279](https://github.com/pbek/QOwnNotes/issues/1279))
- added more Bengali, Arabic, Portuguese, Turkish, Persian, Hindi translation (thank you Fabijenna)

## 19.9.1
- fixed a problem with **updating the current note even if it didn't change** after the
  settings of the note text edit were updated (for [#1275](https://github.com/pbek/QOwnNotes/issues/1275))
    - this happened for example after the settings dialog was closed and could have
      led to overwriting of notes
- fixed a problem with **un-selecting the current note in the note list** after the
  note list was reloaded (for [#1275](https://github.com/pbek/QOwnNotes/issues/1275))
    - this happened for example after the settings dialog was closed
- when inserting a markdown table with the table dialog now two newline characters
  will be added in front of the table to make sure that a proper table is inserted
  (for [#78](https://github.com/pbek/QOwnNotes/issues/78))

## 19.9.0
- added support for **relative image tags** like `![photo](path/to/my/photo.jpg)`
  in the preview and the PDF export (for [#266](https://github.com/pbek/QOwnNotes/issues/266))
    - if the note is in a subfolder the path counts as relative path in relation
      to the subfolder
- pressing <kbd>Escape</kbd> twice in the note text edit will now first disable the
  search widget and then enable the read-only mode instead of the other way around
- added more Croatian, Cebuano, Polish, Persian, Spanish, Serbian (Cyrillic),
  Swedish, Czech, Hungarian, Dutch, Danish, Finnish, Galician, Greek, Norwegian,
  Basque, Romanian, Slovak, Slovenian translation (thank you Fabijenna, ericjeppesen)

## 19.8.8
- there now is a **full-screen mode** that can be toggled with <kbd>F11</kbd>
  (for [#1271](https://github.com/pbek/QOwnNotes/issues/1271))
    - the shortcut is <kbd>⌘</kbd> + <kbd>F11</kbd> on macOS
- added support for **automatic numbering and indentation of ordered lists**
  (for [#1148](https://github.com/pbek/QOwnNotes/issues/1148))
    - if you press <kbd>Enter</kbd> after `1. a list item` you will get a `2. ` in the next line 
    - if you press <kbd>Enter</kbd> after `1. ` it will remove the current line 
    - if you press <kbd>Tab</kbd> after `1. ` it will indent the current line 
    - if you press <kbd>Shift</kbd> + <kbd>Tab</kbd> after `1. ` it will un-indent the current line 
- added more Spanish, Catalan, German, Serbian (Cyrillic), Persian translation
  (thank you richarson, MGuerra, rakekniven, Fabijenna)

## 19.8.7
- fixed a menu bar problem when exiting the application in distraction free mode
  (for [#1267](https://github.com/pbek/QOwnNotes/issues/1267))
- attempted to fix an occasional operating system shutdown process interruption
  (for [#1269](https://github.com/pbek/QOwnNotes/issues/1269))
- added more Persian, Serbian (Cyrillic) translation (thank you Fabijenna)

## 19.8.6
- fixed a possible segmentation fault on macOS (Qt 5.12.2) with todo lists
  (for [#1266](https://github.com/pbek/QOwnNotes/issues/1266))
    - this was a regression from 19.8.5
- added more Persian, Dutch, Hungarian translation (thank you Fabijenna)

## 19.8.5
- fixed many **memory leaks** when **receiving network data**
  (for [#1264](https://github.com/pbek/QOwnNotes/issues/1264))
- the amount of calendar items will now be showed in the output of the *Debug settings*
  (for [#1264](https://github.com/pbek/QOwnNotes/issues/1264))
- added integration test to test https download (for [#1260](https://github.com/pbek/QOwnNotes/issues/1260))
- added more Italian, Spanish translation (thank you tiwi90, richarson)

## 19.8.4
- added **Persian translation** (a big thank you to Fabijenna)
    - join us at [QOwnNotes on Crowdin](https://crowdin.com/project/qownnotes)
      to make QOwnNotes available in more languages or help with the current 
      translation
- updated OpenSSL to 1.1.1c for the Qt 5.13 release of QOwnNotes for Windows to
  prevent network problems with https on some computer (for [#1260](https://github.com/pbek/QOwnNotes/issues/1260))
    - if you had troubles with the latest few releases for Windows you need to
      download the latest release manually once or download the files from
      [OpenSSL](https://github.com/pbek/QOwnNotes/tree/develop/appveyor/OpenSSL)
      and put them into your QOwnNotes folder 
- added more German translation

## 19.8.3
- added support for a real time preview of remote images (for [#1244](https://github.com/pbek/QOwnNotes/issues/1244))
    - the image links in the image tags will be transformed to inline images
    - no caching of the image can currently be done
- added more Serbian (Cyrillic), Portuguese, Catalan, Danish, Finnish, Greek,
  Norwegian, Romanian, Slovak, Slovenian, Russian, Polish, Spanish, Dutch,
  Hungarian, Czech, German, Italian translation (thank you Fabijenna, tiwi90,
  manuelarodsilva, MGuerra, mikhail-khokhlov, lucjan, richarson, rakekniven)

## 19.8.2
- the cursor will now be positioned in the 4th line again after creating a new
  note with the note search panel (for [#1255](https://github.com/pbek/QOwnNotes/issues/1255))
- made confirmation message for conflicted database copies more affirmative
  (for [#1111](https://github.com/pbek/QOwnNotes/issues/1111))
- now it shows `-` instead of 0 in the in-note search count (for [#1247](https://github.com/pbek/QOwnNotes/issues/1247))
- added more German, Spanish, Serbian (Cyrillic), Hungarian, Romanian, Dutch,
  Basque, Cebuano, Czech, Danish, Finnish, Greek, Norwegian, Slovak,
  Slovenian translation (thank you rakekniven, richarson, Fabijenna)

## 19.8.1
- you will now see how many **search results** there are when **searching in a note**
  and at **what search result** you **currently** are
    - the search will now start from top when search parameters are changed 
    - regular expression search restart at top was fixed
    - the text cases in the note text edit search bar are now fixed and more
      tooltips are added
- when a **note** is about to get the **same filename** as an existing note now
  a **number** is **added** instead of the current date and time to prevent name
  clashes when multiple notes are created in the same second
  (for [#1252](https://github.com/pbek/QOwnNotes/issues/1252))
- in the **note list** now the **note filename with extension** is displayed in the
  **tooltips** instead of just the note name (for [#1157](https://github.com/pbek/QOwnNotes/issues/1157))
- when a note was created via shortcut or via the note search the search bar in
  the note edit will now be reset to default properties and hidden
  (for [#1255](https://github.com/pbek/QOwnNotes/issues/1255))
- when conflicted database copies were removed a status bar message is now shown
  (for [#1111](https://github.com/pbek/QOwnNotes/issues/1111))
- improved tab order in main window for panels
- added more Italian, Brazilian Portuguese, German, Serbian (Cyrillic), Dutch,
  Hungarian, Catalan, Greek, Persian, Polish translation (thank you tiwi90,
  mlimacarlos, mlocati, 2fake1987, Fabijenna, MGuerra, rakekniven, athrouss, lucjan)

## 19.8.0
- added **Serbian translation** (a big thank you to Fabijenna)
    - join us at [QOwnNotes on Crowdin](https://crowdin.com/project/qownnotes)
      to make QOwnNotes available in more languages or help with the current 
      translation
- added more Italian, Basque, Czech, Danish, Finnish, Brazilian Portuguese,
  German translation (thank you Fabijenna, tiwi90, mlimacarlos, rakekniven)

## 19.7.11
- **whitespaces** at the **beginning of the current line** are now **reproduced**
  in the next line too when pressing <kbd>Return</kbd> in the note editor
  (for [#1249](https://github.com/pbek/QOwnNotes/issues/1249))
- there now is a **progress dialog** to see the progress of **loading notes** on
  slow systems with a lot of notes
- the column size of the search count in the note list was reduced
  (for [#1248](https://github.com/pbek/QOwnNotes/issues/1248))
- `"` characters will be now cut out of the search text in the local search text
  when searching for text globally (for [#1247](https://github.com/pbek/QOwnNotes/issues/1247))
- added more Spanish, Russian, Basque, Czech, Danish, Finnish, Brazilian
  Portuguese, German, Romanian, Slovak, Slovenian, Dutch, Norwegian, Hungarian,
  Italian translation (thank you richarson, mikhail-khokhlov, Fabijenna,
  mlimacarlos, rakekniven, tiwi90)

## 19.7.10
- the search result count ([#1248](https://github.com/pbek/QOwnNotes/issues/1248))
  and the automatic search inside the current note ([#1247](https://github.com/pbek/QOwnNotes/issues/1247))
  will now also support exact searches like `"exact phrase"`
- added more German, Spanish, Brazilian Portuguese, Italian translation (thank
  you rakekniven, richarson, mlimacarlos, mlocati)

## 19.7.9
- there now is a menu entry *Riot* in the *Help / Open chat* menu to open
  a browser window with the **[QOwnNotes Riot room](https://riot.im/app/#/room/!rUzrRvrnrOsLasDdbp:matrix.org?via=matrix.org)**
- if you are **searching for text in all notes** you will now **see how many times**
  any of the words of your **search term occurred** in each note in a new column
  in the note list (for [#1248](https://github.com/pbek/QOwnNotes/issues/1248))
    - the new column will automatically be resized to be visible next to the
      note name without horizontal scrolling
- if you are browsing notes while there are **words in the note search panel**
  the **search dialog in the note** will now be **opened automatically** and
  **search for the words** entered in the note search panel for
  [#1247](https://github.com/pbek/QOwnNotes/issues/1247))
- every time the note folder is reloaded it will now be **checked** for
  **conflicting copies** of the **`notes.sqlite` database** that will block your
  Nextcloud/ownCloud sync process (for [#1111](https://github.com/pbek/QOwnNotes/issues/1111))
    - you will be asked if you want to remove the conflicting copies
- fixed some ownCloud/Nextcloud wordings and reversed the words
- added more Indonesian, Dutch, Hungarian, Czech, Danish, Finnish, Galician,
  Chinese Simplified, Spanish, Catalan, German translation (thank you zmni,
  Fabijenna, Libnux, richarson, MGuerra, rakekniven)

## 19.7.8
- you can now **turn off cursor blinking** in the *Editor settings*
  (for [#1246](https://github.com/pbek/QOwnNotes/issues/1246))
- added a package for Ubuntu 19.10 (Eoan Ermine) to the Launchpad build process
- added more Spanish, Greek, German translation (thank you richarson, athrouss, rakekniven)

## 19.7.7
- some strange cursor behavior on macOS when leaving or entering read-only mode	was fixed
    - also fixed some ui inconsistency with read-only note files
- the ready-only state of the text edits are now better checked on automatic
  bracket-closing and similar operations
- more keys were added to trigger read-only mode notification

## 19.7.6
- the **immediate focus** on the `Don't ask again!` **checkbox** for
  **message dialogs** was now **disabled**, because this would lead to
  accidentally checking the checkbox (for [#1234](https://github.com/pbek/QOwnNotes/issues/1234))

## 19.7.5
- if you are now pressing **<kbd>Escape</kbd>** in the note text edit the
  **read-only mode** will be **activated**
- added more Italian, Indonesian, Brazilian Portuguese translation (thank you
  tiwi90, zmni, mlimacarlos)

## 19.7.4
- the text in the **note text edit** is now also **selectable by keyboard** in
  **read-only mode**
- the text in the **note preview** is now also **selectable by keyboard**
- if you now edit text in read-only mode you will get a **dialog** that allows
  you to **disable** the **read-only mode** again
- you can now (beside exporting a note as HTML file) **export** the content of
  the **note preview as raw html file** in the context menu of the note preview
- added **Basque translation** (a big thank you to Fabijenna)
    - join us at [QOwnNotes on Crowdin](https://crowdin.com/project/qownnotes)
      to make QOwnNotes available in more languages or help with the current 
      translation
- added more Cebuano and Swedish translation (thank you Fabijenna)

## 19.7.3
- support for **strike out highlighting** was added (for [#1240](https://github.com/pbek/QOwnNotes/issues/1240))
    `~~this is striked out~~` will now be shown as masked text
- fixed a crash when navigating between notes with the <kbd>Alt + Up</kbd> and
  <kbd>Alt + Down</kbd> shortcut keys (for [#1238](https://github.com/pbek/QOwnNotes/issues/1238))
- disabled shortcuts for the *Note folders* menu in the *Shortcuts settings*
  because shortcuts for them are not supported (for [#1235](https://github.com/pbek/QOwnNotes/issues/1235))
- fixed wrongly detected tray icon context menu items in toolbar settings
  (for [#1239](https://github.com/pbek/QOwnNotes/issues/1239))
- there now is a software repository for **Debian Linux 10.0**
    - please visit the [Debian installation page](https://www.qownnotes.org/installation#Debian)
      for instructions
- added more Slovenian, Greek, Slovak, Galician, Finnish, Romanian translation
  (thank you Fabijenna)

## 19.7.2
- added a **new editor color schema** *MountainDebo* by @Deboart
  (for [#1232](https://github.com/pbek/QOwnNotes/issues/1232))
- the [Web Companion browser extension](https://github.com/qownnotes/web-companion)
  was updated to version 19.7.0
    - you can now **substring search** for **all parts of your search text**,
      for example `qo sto` will find `Firefox Store Page QOwnNotes`
    - added more German, Indonesian and Czech translation
- added Flatpak installation notes to the [Flatpak installation page](https://www.qownnotes.org/installation#flatpak)

## 19.7.1
- added a lot fixes for possible memory leaks
- added more Romanian, Slovak, Slovenian translation (thank you Fabijenna)

## 19.7.0
- added support for a real time preview of [Nextcloud Text](https://github.com/nextcloud/text)
  images (for [#1226](https://github.com/pbek/QOwnNotes/issues/1226))
    - the preview image tags from Nextcloud Text will be transformed to inline images
    - no caching can currently be done because images could be modified on the Nextcloud server
 
## 19.6.6
- the default shortcuts were changed from the Opt-key to the Ctrl-key on macOS to prevent
  blocking of accent characters when writing text (for [#1222](https://github.com/pbek/QOwnNotes/issues/1222))
- the storing of the main window position was re-enabled after being disabled in
  [#1015](https://github.com/pbek/QOwnNotes/issues/1015) (for [#1225](https://github.com/pbek/QOwnNotes/issues/1225))
- the QOwnNotesAPI url was updated and the Firefox Add-on link was added to the
  `Welcome to QOwnNotes.md` note file
- added more Chinese Simplified, Brazilian Portuguese, Dutch, Hungarian, Croatian,
  Czech, Danish, Finnish, Norwegian, Polish, Russian, Catalan, Spanish, German,
  Italian translation (thank you KeRouDing, mlimacarlos, Fabijenna, mikhail-khokhlov,
  MGuerra, richarson, rakekniven, tiwi90)

## 19.6.5
- fixed icon-text-overlap for checkboxes and radio groups in the main menu in dark mode
  (for [#1221](https://github.com/pbek/QOwnNotes/issues/1221))
- there now is a new checkbox `Ignore code font size to allow zooming` in the
  *Preview settings* to allow zooming of code fonts (for [#1202](https://github.com/pbek/QOwnNotes/issues/1202))
    - this checkbox is checked by default
- fixed search widget colors in the log widget in dark mode
- added more German, Indonesian translation (thank you rakekniven, zmni)

## 19.6.4
- if you right-click on a link in the preview you now can select `Copy link location`
  to copy the location of the link (for [#800](https://github.com/pbek/QOwnNotes/issues/800))
- added more Italian, Dutch, Hungarian, Indonesian, Brazilian Portuguese translation
  (thank you tiwi90, Fabijenna, zmni, mlimacarlos)

## 19.6.3
- fixed syntax highlighting of non-formatted urls
- added more Spanish, German translation (thank you richarson, rakekniven)

## 19.6.2
- fixed Debian 8.0 build problem on Qt 5.3
- updated application description
- added more Italian and German translation (thank you mlocati)

## 19.6.1
- fixed openSUSE, Fedora and Debian build problem
- added more Catalan, Dutch, Hungarian, Russian, Italian, German, Spanish translation
  (thank you MGuerra, Fabijenna, mikhail-khokhlov, mlocati, rakekniven, soysexy03)

## 19.6.0
- You are now able to distinctively tell QOwnNotes to use the **dark mode icon theme**
  without using the *dark mode* in the *Interface settings*
  (for [#1209](https://github.com/pbek/QOwnNotes/issues/1209)) and
  [#1177](https://github.com/pbek/QOwnNotes/issues/1177))
- added more Danish, Czech, Finnish, Greek, Spanish, Hungarian, Dutch translation
  (thank you Fabijenna, richarson)

## 19.5.7
- fixed unnecessary scrolling and dark mode link colors in the *Browser extension*
  settings (for [#1213](https://github.com/pbek/QOwnNotes/issues/1213))
- if the current note has a certain tag it will be now highlighted as bold in
  the tags panel (for [#1210](https://github.com/pbek/QOwnNotes/issues/1210))
- did general performance and memory consumption improvements
- added more Russian translation (thank you mikhail-khokhlov)

## 19.5.6
- fixed creation of empty notes at the note root folder when importing Evernote enex
  files into a note subfolder and improved overall import speed
  (for [#1212](https://github.com/pbek/QOwnNotes/issues/1212))
- added a more descriptive text in the browser extension security token dialog
- the [Web Companion browser extension](https://github.com/qownnotes/web-companion)
  was updated to version 19.5.5
    - the **security token** will now be sent correctly to QOwnNotes for context menu operations
      (for [#10](https://github.com/qownnotes/web-companion/issues/10))
- added more Russian, Italian, Indonesian translation (thank you mikhail-khokhlov, mlocati, zmni)

## 19.5.5
- you are now able to **manually edit the security token** for your
  [Web Companion browser extension](https://github.com/qownnotes/web-companion)
  to be able to use the same token across different devices in case you are
  syncing your browser settings
- the [Web Companion browser extension](https://github.com/qownnotes/web-companion)
  was updated to version 19.5.4
    - you are now also able to enter the **security token** in the browser extension options
    - added more translation
- added more Catalan, Spanish, Italian translation (thank you MGuerra, richarson, mlocati)

## 19.5.4
- when using the [Web Companion browser extension](https://github.com/qownnotes/web-companion)
  you now need to enter a **security token** to communicate with QOwnNotes to
  ensure no other website or application can access your bookmarks
    - you will get a new dialog that shows the token as soon as the browser
      extension accesses QOwnNotes
    - you can also change the security token later in the *Browser extension* settings
- the [Web Companion browser extension](https://github.com/qownnotes/web-companion)
  was updated to version 19.5.3
    - you are now able to enter a **security token** to communicate with QOwnNotes 
    - added a lot of translation
    - you now can **import all browser bookmarks** (for [#8](https://github.com/qownnotes/web-companion/issues/8))
    - fixed a bookmark filtering problem
- added a new editor color schema *Solarized Light* by @fairplay
  (for [#1208](https://github.com/pbek/QOwnNotes/issues/1208))
- improved Mojave dark mode support for macOS 
  (for [#1209](https://github.com/pbek/QOwnNotes/issues/1209))
- added more Indonesian, Slovenian, Swedish, Romanian, Slovak, Russian, Italian,
  Spanish, Brazilian Portuguese, Finnish, Norwegian, Danish, Greek, Polish,
  Cebuano, Hungarian, Dutch, Czech, Croatian translation (thank you zmni,
  Fabijenna, mikhail-khokhlov, mlocati, richarson, mlimacarlos, tiwi90)

## 19.5.3
- if the setting `Show all notes in a folder including their subfolders` is
  activated now the note-subfolder list will show the note count recursively for
  all children note-subfolders (for [#1206](https://github.com/pbek/QOwnNotes/issues/1206))
- added `aarch64` architecture to AUR build process
- added more Czech, Finnish, Croatian, Romanian, Slovak, Slovenian, Brazilian
  Portuguese, Russian, German, Spanish, Hungarian, Dutch, Italian translation
  (thank you Fabijenna, mlimacarlos, mikhail-khokhlov, rakekniven, richarson,
  tiwi90, mlocati)

## 19.5.2
- now more than 10 bookmarks can be imported from [Nextcloud bookmarks](https://github.com/nextcloud/bookmarks)
  in the *Notes / Import* menu (for [#1198](https://github.com/pbek/QOwnNotes/issues/1198))
- added more Romanian translation (thank you Fabijenna)

## 19.5.1
- added feedback if the note folder was switched via the websocket to the websocket api
- the [Web Companion browser extension](https://github.com/qownnotes/web-companion)
  was updated to version 19.5.0
    - you can now also **search** for the **description of bookmarks**
    - a **loading progressbar** will now be shown while bookmarks are loaded
      when the **note folder is switched** by the user 
- there now is a software repository for **Fedora 30** (for [#1199](https://github.com/pbek/QOwnNotes/issues/1199))
    - please visit the [Fedora installation page](https://www.qownnotes.org/installation#Fedora)
      for instructions
- added more Spanish, Catalan, Romanian, Dutch, Hungarian, Norwegian, Danish,
  Czech, Italian translation (thank you richarson, MGuerra, Fabijenna, mlocati)

## 19.5.0
- you are now able to **import bookmarks** from [Nextcloud bookmarks](https://github.com/nextcloud/bookmarks)
  in the *Notes / Import* menu (for [#1198](https://github.com/pbek/QOwnNotes/issues/1198))
    - the bookmarks will be imported to your bookmarks note 
    - for more information about the **bookmarks feature** please visit
      [Web Companion browser extension](https://www.qownnotes.org/contributing/get-involved.html)
- **QOwnNotesAPI** is now ready for **Nextcloud 16**
    - see [QOwnNotesAPI for Nextcloud](https://apps.nextcloud.com/apps/qownnotesapi)
- added more Italian, German, Russian, Dutch, Norwegian, Danish, Hungarian,
  Spanish, Catalan, Romanian, Brazilian Portuguese translation (thank you
  mlocati, rakekniven, mikhail-khokhlov, Fabijenna, richarson, MGuerra, mlimacarlos)

## 19.4.5
- you now will get a warning dialog if your git client couldn't be executed when
  a git command is issued (for [#1192](https://github.com/pbek/QOwnNotes/issues/1192))
    - you will be able to turn off that dialog
- more checksum generation improvements for the AUR and added checksum support
  to the build processes of Launchpad Snap, Slackware and Gentoo
  (for [#1175](https://github.com/pbek/QOwnNotes/issues/1175))
- added more German translation

## 19.4.4
- fixed renaming issue on case-insensitive file-systems
  (for [#1190](https://github.com/pbek/QOwnNotes/issues/1190))
- another attempt to build the Windows version of QOwnNotes with Qt 5.12.2
  (for AppVeyor [#2871](https://github.com/appveyor/ci/issues/2871))
- added more Spanish translation (thank you richarson)

## 19.4.3
- you can now reorder your note folders in the *Note folders settings* via drag and drop
  (for [#1181](https://github.com/pbek/QOwnNotes/issues/1181))
- more checksum generation improvements for the AUR
  (for [#1175](https://github.com/pbek/QOwnNotes/issues/1175))
- added more Romanian, Slovenian, Slovak, Dutch, Hungarian, Indonesian,
  Brazilian Portuguese, Italian, German, Russian translation (thank you
  Fabijenna, zmni, mlimacarlos, mlocati, rakekniven, mikhail-khokhlov)

## 19.4.2
- for Linux there there now is a setting in the *Interface settings* to enforce
  the system icon theme in case your desktop environment doesn't report the icon
  theme correctly (for [#1177](https://github.com/pbek/QOwnNotes/issues/1177))
    - KDE currently seems to have this problem on Qt 5.12
    - in addition if built on Qt 5.12 or newer there now also will be tried to
      check if there is a fallback icon theme in case there is no icon theme reported
- checksum generation improvement for the AUR (for [#1175](https://github.com/pbek/QOwnNotes/issues/1175))
- added more German, Russian, Catalan, Brazilian Portuguese, Dutch, Italian,
  Slovak, Slovenian, Danish, Hungarian, Norwegian, Czech, Greek, Spanish translation
  (thank you MGuerra, mlimacarlos, vistausss, tiwi90, Fabijenna, richarson, pu.sash)

## 19.4.1
- added support for **listing** the **note folders** and **changing** the
  **current note folder** to the websocket api
- the [Web Companion browser extension](https://github.com/qownnotes/web-companion)
  was updated to version 19.4.0
    - you can now **change** the **current note folder** in a select box in the
      toolbar of the **bookmarks popup**
- added more Slovak, Slovenian, Danish, Hungarian, Dutch, Norwegian, Czech, Greek,
  Russian, German translation (thank you Fabijenna, mikhail-khokhlov, rakekniven)

## 19.4.0
- added **Romanian**, **Slovak** and **Slovenian translation** (a big thank you to Fabijenna)
    - join us at [QOwnNotes on Crowdin](https://crowdin.com/project/qownnotes)
      to make QOwnNotes available in more languages or help with the current 
      translation
- added a package for Ubuntu 19.04 (Disco Dingo) to the Launchpad build process
  (for [#1169](https://github.com/pbek/QOwnNotes/issues/1169))
- added more Indonesian, Danish translation (thank you zmni, flywheel)

## 19.3.4
- fixed layout damaging when in dark mode closing the app with the `x` button or
  if getting a HUP signal (for [#1015](https://github.com/pbek/QOwnNotes/issues/1015))
- made an attempt to remedy `notes.sqlite` sync issues with Nextcloud
  (for [#1111](https://github.com/pbek/QOwnNotes/issues/1111))
- added more Slovak, Slovenian, Hungarian, German, Catalan, Spanish, Brazilian
  Portuguese, Romanian, Italian, Dutch, Russian translation (thank you Fabijenna,
  rakekniven, MGuerra, richarson, mlimacarlos, mlocati, mikhail-khokhlov)

## 19.3.3
- there now are new scripting commands `script.noteTextEditSetSelection()`,
  `script.noteTextEditSelectionStart()` and `script.noteTextEditSelectionEnd()`
  to set and get the currently selected text in the note text edit
  (for [#1167](https://github.com/pbek/QOwnNotes/issues/1167))
    - for more information please take a look at the
      [scripting documentation](https://www.qownnotes.org/scripting/methods-and-objects.html#set-the-currently-selected-text-in-the-note-text-edit)
- you now can activate the context menu of the current widget with the shortcut <kbd>F10</kbd>
  (for [#1154](https://github.com/pbek/QOwnNotes/issues/1154))
    - please visit [QOwnNotes shortcuts](https://www.qownnotes.org/getting-started/shortcuts.html)
      for a list of all shortcuts
- added more Chinese Traditional, German translation (thank you abev66, rakekniven)

## 19.3.2
- there now is a new *script settings variable* for select-boxes (for [#1167](https://github.com/pbek/QOwnNotes/issues/1167))
    - you can define items with an index and a text
    - you will receive the selected index
    - for more information please take a look at the
      [scripting documentation](https://www.qownnotes.org/scripting/methods-and-objects.html#registering-script-settings-variables)
- there are now shortcuts to jump to the *note list* (<kbd>F5</kbd>), the
  *note subfolder panel* (<kbd>F6</kbd>) and the *tags panel* (<kbd>F7</kbd>)
  (for [#1153](https://github.com/pbek/QOwnNotes/issues/1153))
    - please visit [QOwnNotes shortcuts](https://www.qownnotes.org/getting-started/shortcuts.html)
      for a list of all shortcuts
- there now is a new scripting command `script.noteTextEditSelectCurrentLine()`
  to select the current line in the note text edit (for [#1167](https://github.com/pbek/QOwnNotes/issues/1167))
    - for more information please take a look at the [scripting documentation](https://www.qownnotes.org/scripting/methods-and-objects.html#select-the-current-line-in-the-note-text-edit)
- added more Spanish, Italian, Russian, Cebuano, Hiligaynon, Dutch, Hungarian,
  Swedish, Greek translation (thank you richarson, mlocati, mikhail-khokhlov,
  Fabijenna, druus, tiwi90)

## 19.3.1
- added **Danish translation** (a big thank you to Fabijenna)
    - join us at [QOwnNotes on Crowdin](https://crowdin.com/project/qownnotes)
      to make QOwnNotes available in more languages or help with the current 
      translation
- implemented preserving of note tag links when renaming note subfolders
  (for [#1166](https://github.com/pbek/QOwnNotes/issues/1166))
- added some markdown text edit fixes and changes
  (for [QMarkdownTextEdit #21](https://github.com/pbek/qmarkdowntextedit/pull/21), thank you @sandsmark)
- added more Czech, Polish, Swedish, Greek, Catalan, Galician, Indonesian,
  Norwegian, Dutch, Hungarian, German, Italian, Portuguese, Brazilian, Spanish
  translation (thank you Fabijenna, zmni, rakekniven, mlocati, jan_sh,
  mlimacarlos, richarson)

## 19.3.0
- added **Galician translation** and **Norwegian translation** (a big thank you
  to Fabijenna)
    - join us at [QOwnNotes on Crowdin](https://crowdin.com/project/qownnotes/)
      to make QOwnNotes available in more languages or help with the current 
      translation
- added an option to disable underline rendering in the preview to the *Preview settings*
  (for [#1160](https://github.com/pbek/QOwnNotes/issues/1160))
- added more Chinese Simplified, Indonesian, Danish, Cebuano, Hungarian, German
  translation (thank you cptbl00dra1n, Fabijenna, zmni)

## 19.2.5
- fixed numpad 0 being recognized as tick character in the note text edit
  (for [#1152](https://github.com/pbek/QOwnNotes/issues/1152), thank you @liulex)
- added checkboxes support to the preview pdf export
  (for [#235](https://github.com/pbek/QOwnNotes/issues/235), thank you @liulex)
- added more Chinese Traditional translation (thank you abev66)

## 19.2.4
- you can now click checkboxes like `- []` in the preview
  (for [#235](https://github.com/pbek/QOwnNotes/issues/235), thank you @liulex)
- fixed single-line paste behavior when there is already text selection
  (for [qmarkdowntextedit #19](https://github.com/pbek/qmarkdowntextedit/pull/19), thank you @liulex)
- fixed settings lost after canceling the welcome dialog
  (for [#1040](https://github.com/pbek/QOwnNotes/issues/1040), thank you @liulex)

## 19.2.3
- fixed a crash on first start introduced with feature [#1135](https://github.com/pbek/QOwnNotes/issues/1135)
  (for [#1146](https://github.com/pbek/QOwnNotes/issues/1146))
- fixed a problem with pasting text with newline characters at the end introduced
  with [qmarkdowntextedit #15](https://github.com/pbek/qmarkdowntextedit/pull/15)
  (for [qmarkdowntextedit #18](https://github.com/pbek/qmarkdowntextedit/pull/18), thank you @liulex)
- fixed a problem with resetting layout settings if the note folder was not found
  (for [#1140](https://github.com/pbek/QOwnNotes/issues/1140), thank you @liulex)
- fixed some possible problems with finding existing note folders 
  (for [#1144](https://github.com/pbek/QOwnNotes/issues/1144), thank you @liulex)
    - this also fixes some problems upon first start in portable mode

## 19.2.2
- fixed lost note after changing its title and switching to another note
  (for [#522](https://github.com/pbek/QOwnNotes/issues/522) and
  [#1142](https://github.com/pbek/QOwnNotes/issues/1142), a big thank you to @liulex)
- added more Portuguese, Brazilian translation (thank you mlimacarlos)

## 19.2.1
- the diff colors for added and removed text in the note versioning dialog is
  now adapted for the dark mode (for [#1139](https://github.com/pbek/QOwnNotes/issues/1139))
- you can now use <kbd>Ctrl + Enter</kbd> in the note text edit to create a new
  line and jump to it even if the caret is in the middle of the line
  (for [qmarkdowntextedit #14](https://github.com/pbek/qmarkdowntextedit/pull/14), thank you @liulex)
- added auto completion for tick characters for code blocks
  (for [qmarkdowntextedit #15](https://github.com/pbek/qmarkdowntextedit/pull/15), thank you @liulex)
- you can now copy/cut the whole paragraph if no text is selected
  (for [qmarkdowntextedit #16](https://github.com/pbek/qmarkdowntextedit/pull/16), thank you @liulex)
- added more Ukrainian translation (thank you prolinuxua)

## 19.2.0
- in **portable mode** now **relative links** will be inserted by the *Link dialog*
  (for [#1136](https://github.com/pbek/QOwnNotes/issues/1136))
    - also an attempt will be made to select the selected relative file again
      when the dialog is opened next time 
- the *Custom action toolbar* can now be hidden even if scripts with custom
  actions are enabled (for [#1135](https://github.com/pbek/QOwnNotes/issues/1135))
- enabled scrolling to top and bottom in the note text edit on macOS
  (for [#1131](https://github.com/pbek/QOwnNotes/issues/1131))
- attempted to fix the brace character if automatic bracket closing is enabled
  on macOS with an US keyboard (for [#1132](https://github.com/pbek/QOwnNotes/issues/1132))
- added more German translation (thank you rakekniven)

## 19.1.11
- fixed gif animations in the preview (for [#1104](https://github.com/pbek/QOwnNotes/issues/1104), thank you @liulex)
- fixed checked state detection for scripting function `triggerMenuAction`
  (for [#1127](https://github.com/pbek/QOwnNotes/issues/1127), thank you @smokris)
- improved styling for note html export (for [#1130](https://github.com/pbek/QOwnNotes/issues/1130))
    - adapted background colors for dark mode
- the note preview is now regenerated after the settings dialog is closed or the
  scripting engine is manually reloaded in the main menu, so that you immediately
  will see effects by scripts
- the `<body>` tag of the generated html of a note will now have a css class
  `preview` for the preview and and `export` for the export, so that you can more
  easily append different styles for the preview and export
- the checking for too large images in the preview will now be done after
  scripts are modifying the html, allowing images to correctly be resized even
  if the preview html is generated by scripts like `markdown-it`
- added more Spanish, Italian, Galician, Catalan, Greek, Swedish, Russian,
  Czech, Dutch and Hungarian translation (thank you richarson, mlocati, MGuerra,
  mikhail-khokhlov and Fabijenna)

## 19.1.10
- the issue assistant will now use `<details>` tags to make the GitHub issues
  look less cluttered
- animations of embedded gif images should now animate in the preview (at least on Windows)
  (for [#1104](https://github.com/pbek/QOwnNotes/issues/1104), a big thank you to @liulex)
- disabled the application hiding of [#1098](https://github.com/pbek/QOwnNotes/issues/1098),
  because the close event is also fired when the application will be quit in
  the dock menu on macOS (for [#1113](https://github.com/pbek/QOwnNotes/issues/1113))
- added more German, Spanish, Italian, Galician, Polish, Dutch and Hungarian
  translation (thank you rakekniven, richarson, mlocati and Fabijenna)

## 19.1.9
- the websocket api for **listing bookmarks** will now try to **merge bookmarks**
  if they were found multiple times in your bookmark notes for the
  [Web Companion browser extension](https://github.com/qownnotes/web-companion)
    - creating new bookmarks will trigger a reply to the browser extension with a
      success message 
- the [Web Companion browser extension](https://github.com/qownnotes/web-companion)
  was updated to version 19.1.11
    - added support to show a success message from QOwnNotes if bookmarks were created
    - fixed restoring of previously selected tags
    - added a lot of translation
    - fixed `tabs` permissions
- the note text edit context menu can now also create block quotes
- if a tag in the *Tags panel* is double-clicked the current tag will not only
  be tagged with the clicked tag, but the tag will also be removed if the note
  was already tagged with it (for [#1116](https://github.com/pbek/QOwnNotes/issues/1116))
    - the tag tree will now also be correctly reloaded
- added more German, Spanish, Ukrainian, Chinese Traditional, Italian, Russian,
  Indonesian, Galician, Finnish, Dutch, Greek and Hungarian translation
  (thank you prolinuxua, rakekniven, richarson, tiwi90, abev66, mlocati, zmni,
  mikhail-khokhlov and Fabijenna)

## 19.1.8
- added **Greek translation** (a big thank you to Fabijenna)
    - join us at [QOwnNotes on Crowdin](https://crowdin.com/project/qownnotes)
      to make QOwnNotes available in more languages or help with the current 
      translation
- the websocket api for **adding bookmarks** can now add multiple bookmarks at
  the same time for the [Web Companion browser extension](https://github.com/qownnotes/web-companion)
- the [Web Companion browser extension](https://github.com/qownnotes/web-companion)
  was updated to version 19.1.8
    - you can now **filter bookmarks by tags**
        - use access key <kbd>T</kbd> to jump to the tag selector
        - the selected tags will be persisted and restored the next time the
          bookmarks popup is opened
    - you can now **bookmark all tabs** with a button in the **bookmark popup**
        - use access key <kbd>B</kbd> to press the button
    - changed the **default shortcut** for the **bookmarks popup** in **Firefox**
      to <kbd>Alt + Shift + B</kbd>, because <kbd>Ctrl + B</kbd> is blocked
        - these shortcuts will only work in Firefox 66 and newer 
- added more Russian translation (thank you mikhail-khokhlov)

## 19.1.7
- when **adding bookmarks** from the [Web Companion browser extension](https://github.com/qownnotes/web-companion)
  the bookmarks tag will be applied to the bookmarks note automatically if that
  note wasn't created yet
- the [Web Companion browser extension](https://github.com/qownnotes/web-companion)
  was updated to version 19.1.7
    - added **access keys** to some **bookmark popup** actions
        - access key <kbd>A</kbd> to add a new bookmarks (for example press <kbd>Alt + A</kbd> in Chrome)
        - access key <kbd>S</kbd> to search for bookmarks
        - access key <kbd>O</kbd> to open all visible bookmarks
- added more German, Catalan, Spanish, Brazilian Portuguese, Italian, Galician,
  Finnish, Dutch, Greek, Hungarian, Swedish, Czech and Swedish translation
  (thank you rakekniven, richarson, tiwi90, MGuerra, mlocati and Fabijenna)

## 19.1.6
- bookmark links with `chrome-extension://` or `chrome://` are now also detected
  as bookmarks for the [Web Companion browser extension](https://github.com/qownnotes/web-companion)
    - also more general link detection improvements were made
- added fields to add a link name and description for inserting links to the
  *Link dialog*
- added support for **adding bookmarks** to a note `Bookmarks` from the
  [Web Companion browser extension](https://github.com/qownnotes/web-companion)
    - you can change the note name in the *Browser extension* settings
    - if the note doesn't exist it will be created the first time a bookmark is added
- the [Web Companion browser extension](https://github.com/qownnotes/web-companion)
  was updated to version 19.1.6
    - you can now **add new bookmarks** with a button in the **bookmark popup**
    - you can now **open all visible bookmarks** in new tabs with a button in
      the **bookmark popup**
- added more Indonesian, Catalan, Greek, Brazilian Portuguese, Hungarian and
  Dutch translation (thank you zmni, MGuerra, mlimacarlos and Fabijenna)

## 19.1.5
- improved detection of multiple links in a text-block in the current note for
  the **bookmark feature** of the [Web Companion browser extension](https://github.com/qownnotes/web-companion)
    - added support for showing the current note folder name
- the [Web Companion browser extension](https://github.com/qownnotes/web-companion)
  was updated to version 19.1.5
    - the previously entered **search text** in the **bookmark popup** is now
      **automatically selected and focused**, so that you can overwrite it and
      start to search immediately
    - the tab-order was now adapted, so that you can **navigate the links** with <kbd>Tab</kbd> easily
    - there now is a **default shortcut** <kbd>Ctrl + B</kbd> for**opening the bookmarks popup**
    - the name of the **current note folder** will now be displayed in the top toolbar of the **bookmark popup**
- added more German, Catalan, Spanish, Chinese Traditional, Italian, Czech,
  Finnish and Greek translation (thank you rakekniven, richarson, abev66,
  MGuerra, mlocati and Fabijenna)

## 19.1.4
- added support for showing **bookmark links of all notes tagged** with `bookmarks`
  for the [Web Companion browser extension](https://github.com/qownnotes/web-companion)
    - you can change that tag name in the *Browser extension* settings
    - detection of tags and description of such links is also improved
    - links in the current note will now have the tag `current`
    - for more information about the **bookmarks feature** please visit
      [Web Companion browser extension](https://www.qownnotes.org/contributing/get-involved.html)
- the [Web Companion browser extension](https://github.com/qownnotes/web-companion)
  was updated to version 19.1.4
    - bookmarks styling changes and the **search field** can now be **reset**
    - the content of the bookmarks **search field** will now be **persisted**
- added **Finnish translation** (a big thank you to Fabijenna)
    - join us at [QOwnNotes on Crowdin](https://crowdin.com/project/qownnotes)
      to make QOwnNotes available in more languages or help with the current 
      translation
- added more Brazilian Portuguese translation (thank you mlimacarlos)

## 19.1.3
- improved detection of links in encrypted notes for the **bookmark feature**
  of the [Web Companion browser extension](https://github.com/qownnotes/web-companion)
- the [Web Companion browser extension](https://github.com/qownnotes/web-companion)
  was updated to version 19.1.3
    - preventing opening of two new tabs when clicking a bookmark
    - added support for the new **tag** and **description** attributes in **bookmarks**
        - the syntax for using this bookmark links in notes is
          `- [name](http://link) #tag1 #tag2 some description`
        - tags will be shown in the list and the description will be shown in a new tooltip
- added more Indonesian, Catalan, Spanish, Ukrainian, Greek, Chinese Traditional,
  Italian, French, Hungarian and Dutch translation (thank you zmni, MGuerra,
  richarson, abev66, prolinuxua, mlocati and Fabijenna)

## 19.1.2
- added support for the new **bookmark feature** of the [Web Companion browser extension](https://github.com/qownnotes/web-companion)
- the [Web Companion browser extension](https://github.com/qownnotes/web-companion)
  was updated to version 19.1.2
    - you can now browse a **list of all links in the current note** if you click
      on the QOwnNotes icon in the browser toolbar
        - you can **search for links** on the popup
        - clicking a line opens the url in a new browser tab
        - clicking on the headline items of the table sorts the items in the table
    - added more German translation (thank you rakekniven)
- added more Indonesian, German, Catalan, Spanish, Portuguese, Ukrainian,
  Chinese Traditional, Italian, Swedish, Croatian, Czech, French, Hungarian and
  Dutch translation (thank you zmni, fitoschido, rakekniven, druus, richarson,
  abev66, prolinuxua, tiwi90 and Fabijenna)

## 19.1.1
- the *Chrome extension settings* got renamed to *Browser extension*
- added [Firefox Add-ons page](https://addons.mozilla.org/firefox/addon/qownnotes-web-companion)
  to the *Browser extension* settings page (for [#330](https://github.com/pbek/QOwnNotes/issues/330))
- the [Web Companion browser extension](https://github.com/qownnotes/web-companion)
  was updated to version 19.1.1
    - the extension now also works with Firefox (for [#1](https://github.com/qownnotes/web-companion/issues/1),
      thank you @Offerel for your Firefox changes)
    - reimplementation of extension with webextension-toolbox, webpack and VueJS
      for Google Chrome and Mozilla Firefox
        - creating notes from the current selection, the whole page and as
          screenshot now works with Firefox and Chrome 
    - there now is a browser option page to set and reset the server socket port
    - the text of the extension can now be translated
        - visit [QOwnNotes translations](https://crowdin.com/project/qownnotes)
          if you want to help to translate the interface to your language
    - download the latest version on [Web Companion releases](https://github.com/qownnotes/web-companion/releases)
- changed the tooltip text for the `Allow note file name to be different from headline`
  setting to be more clear (for [#1110](https://github.com/pbek/QOwnNotes/issues/1110))
- fixed some typos in the *Trash dialog* (for [#1107](https://github.com/pbek/QOwnNotes/issues/1107), thank you @smokris)
- you can now disable the socket server in the *Browser extension settings*
  (for [#1105](https://github.com/pbek/QOwnNotes/issues/1105))
- added more German, Catalan, Spanish, Portuguese, Chinese Traditional,
  Ukrainian, Italian, Swedish, Croatian, Czech, French, Hungarian and Dutch
  translation (thank you rakekniven, fitoschido, zmni, druus, richarson, abev66,
  prolinuxua, tiwi90 and Fabijenna)

## 19.1.0
- you can now change the socket server port for connecting with the
  [Web Companion browser extension](https://github.com/qownnotes/web-companion)
  in the new *Chrome extension settings* (for [#330](https://github.com/pbek/QOwnNotes/issues/330))
    - the Chrome extension is now also available in the
      [Chrome Web Store](https://chrome.google.com/webstore/detail/qownnotes-web-companion/pkgkfnampapjbopomdpnkckbjdnpkbkp)
- fixed some possible crashes if a socket client disconnects from the socket server
  (for [#330](https://github.com/pbek/QOwnNotes/issues/330))
- you can now enable note subfolders in the welcome dialog if you are running
  QOwnNotes for the first time
- fixed some typos in dialogs
- added more German, Italian, Czech, Swedish, Croatian, French, Hungarian and
  Dutch translation (thank you druus, rakekniven, tiwi90 and Fabijenna)

## 18.12.9
- on macOS the application will now be minimized when closed instead of quit to
  more mimic the default behavior of applications on macOS (for [#1098](https://github.com/pbek/QOwnNotes/issues/1098))
- added missing `libqt5websockets5` dependency to Travis CI Debian package
  (for [#1099](https://github.com/pbek/QOwnNotes/issues/1099))
- "dangerous" characters will now be converted to whitespaces instead of `_` in
  filenames and headlines to prevent underline formatting in headlines if two `_`
  characters are present (for [#330](https://github.com/pbek/QOwnNotes/issues/330))
- added support for converting html to markdown when a page is created as new
  note for the [Web Companion browser extension](https://github.com/qownnotes/web-companion)
  (for [#330](https://github.com/pbek/QOwnNotes/issues/330))
- inline images will now be automatically transformed to real images if html is
  converted to markdown (for [#330](https://github.com/pbek/QOwnNotes/issues/330))
- the [Web Companion browser extension](https://github.com/qownnotes/web-companion)
  was updated to version 18.12.2
    - added support to import pages as new note
        - the html will be converted to markdown and images of the page will be
          downloaded (might take a while)
    - added support to add a screenshot of the visible area as new note
    - selected text will now be created as new note with newlines if possible
    - download the latest version on [Web Companion releases](https://github.com/qownnotes/web-companion/releases)
- added more Hungarian and Dutch translation (thank you Fabijenna)

## 18.12.8
- improved note name filtering to prevent troubles on Windows for the
  [Web Companion browser extension](https://github.com/qownnotes/web-companion)
  (for [#330](https://github.com/pbek/QOwnNotes/issues/330))
    - the name and note text will be trimmed to prevent wrong newlines
    - the Web Companion browser extension was updated to version 18.12.1
        - fixed the default port
        - the url of the page will now also be written to the note
        - restructured the context menu
        - added an context menu entry to reset the settings
        - download the latest version on [Web Companion releases](https://github.com/qownnotes/web-companion/releases)
- the amount of `=` characters added for the headlines of new notes are now
  limited to 40 to prevent too long headline lines
    - the name will be trimmed to prevent wrong newlines in the headline
- added more Spanish translation (thank you richarson)

## 18.12.7
- added support for the new [Web Companion browser extension](https://github.com/qownnotes/web-companion)
  to add selected text as new note (for [#330](https://github.com/pbek/QOwnNotes/issues/330))
    - you can download the current release of the extension at
      [Web Companion 18.12.0](https://github.com/qownnotes/web-companion/releases/tag/18.12.0)
    - then visit `chrome://extensions` and drop the extension file into Chrome to install the extension
- added more Polish, German, Chinese Traditional and Italian (thank you lucjan,
  rakekniven, abev66 and mlocati)

## 18.12.6
- start of QOwnNotes websocket server integration for implementing a
  **Web Scraper** browser extension (for [#330](https://github.com/pbek/QOwnNotes/issues/330))
    - you should already be able to "talk" to your QOwnNotes application by
      connecting to `ws://127.0.0.1:22222`, for example with [Echo Test](https://www.websocket.org/echo.html?location=ws://127.0.0.1:22222)
- the automatically generated context menus for floating panels are now disabled
  to prevent enabling of the note subfolder toolbar if subfolders are disabled
  for the current note folder (for [#1093](https://github.com/pbek/QOwnNotes/issues/1093))
- the binary **Arch Linux** repository on the *openSUSE Build Service* is working
  again (for OBS issue [#6692](https://github.com/openSUSE/open-build-service/issues/6692))
- added more German, Spanish, Chinese Traditional, Ukrainian, Italian, Polish
  and French translation (thank you lucjan, rakekniven, richarson, tiwi90,
  abev66, prolinuxua and dev2a)

## 18.12.5
- you can now **search** for items in the **navigation panel**
  (for [#1090](https://github.com/pbek/QOwnNotes/issues/1090))
    - the search bar acts as new panel headline, the previous headline of the
      navigation tree widget is now disabled
	- there is an option to hide the search bar of the navigation panel in 
	  the *Panel settings*
- the script `Markdown-it markdown parser` in the *Script repository* now 
  optionally supports *definition lists* (for script issue [#48](https://github.com/qownnotes/scripts/issues/48))
- added `qt5-xmlpatterns` to the Arch Linux build depend list
- the automatically generated toolbar context menus are now disabled to prevent 
  enabling of the note subfolder toolbar if subfolders are disabled for the
  current note folder (for [#1093](https://github.com/pbek/QOwnNotes/issues/1093))
    - the *note subfolder* menu entry is now disabled in the *Panels menu*
      instead of hidden if subfolders are not enabled for current note folder
- added more German, Catalan, Spanish, Portuguese, Chinese Traditional,
  Ukrainian, Italian, Hungarian and Dutch translation (thank you jan_sh,
  MGuerra, richarson, manuelarodsilva, abev66, prolinuxua, mlocati, tiwi90 
  and Fabijenna)

## 18.12.4
- if single application mode is enabled the app will now attempt to check if 
  the mode is supported on your system and turn it off if it isn't supported
  (for [#1089](https://github.com/pbek/QOwnNotes/issues/1089))
- you can now disable the auto-completion of previous searches in the search 
  panel by turning it off in the *Panel settings* (for [#1087](https://github.com/pbek/QOwnNotes/issues/1087))
    - also some memory optimization when adding new searches was done
- added more Chinese Traditional, Ukrainian, Italian, Hungarian and Dutch
  translation (thank you abev66, prolinuxua, mlocati and Fabijenna)

## 18.12.3
- the Windows version of QOwnNotes is now built with Qt 5.11.2
- the Qt 5.7 build for Windows XP is now working again (for [#828](https://github.com/pbek/QOwnNotes/issues/828))
    - seek out the latest master build from [AppVeyor](https://ci.appveyor.com/project/pbek/qownnotes/history),
      choose the *Qt 5.7 environment* and download the *artifact*
- an attempt was made to fix icon scaling problems especially on high dpi *Windows* and *macOS* systems
  (for [#1005](https://github.com/pbek/QOwnNotes/issues/1005) and [#1085](https://github.com/pbek/QOwnNotes/issues/1085))
- added more Ukrainian, Spanish and German translation (thank you prolinuxua and richarson)

## 18.12.2
- there now is a software repository for **Fedora 29**
    - please visit the [Fedora installation page](https://www.qownnotes.org/installation#Fedora)
      for instructions
- fixed build process for Qt below 5.6
- added more German, Brazilian Portuguese, Ukrainian, French, Catalan, Italian,
  Hungarian and Dutch translation (thank you rakekniven, tiwi90, prolinuxua,
  MGuerra, Botigli, mlimacarlos and Fabijenna)

## 18.12.1
- there now is a new **command line parameter** `--allow-multiple-instances`
  that allows **multiple instances** of QOwnNotes to be started even if
  disallowed in the settings
- the command line parameter `-h` now also works in single application mode
  if a second instance of QOwnNotes is started
- the **Log panel** now has a **minimum height** of 20px to prevent that it 
  can't be seen when enabled
- a warning now will be printed out if QOwnNotes is started a second time in
  single application mode
- added more German, Brazilian Portuguese, Chinese Traditional, Spanish,
  Italian, Hungarian and Dutch translation (thank you rakekniven, jan_sh,
  richarson, abev66, mlimacarlos, mlocati, tiwi90 and Fabijenna)

## 18.12.0
- you now can **copy the text from code blocks** to the clipboard by right
  clicking on it and selecting `Copy code block` (for [#1077](https://github.com/pbek/QOwnNotes/issues/1077))
- the command line parameters `--help` and `--dump-settings` now also work in
  single application mode if a second instance of QOwnNotes is started
- a warning now will be printed out in the first application if QOwnNotes is 
  started a second time in single application mode
- **QOwnNotesAPI** is now ready for **Nextcloud 15**
    - see [QOwnNotesAPI for Nextcloud](https://apps.nextcloud.com/apps/qownnotesapi)
- added more Catalan, Spanish, Italian, Hungarian and Dutch translation (thank
  you MGuerra, richarson, mlocati and Fabijenna)

## 18.11.6
- you can now use pre-made QOwnNotes layouts in the new **Layout settings**
  (for [#1069](https://github.com/pbek/QOwnNotes/issues/1069))
- fixed a possible crash when pasting images into the issue assistant
  (for [#1072](https://github.com/pbek/QOwnNotes/issues/1072))
- added more German, Spanish, Brazilian Portuguese, Italian, Hungarian and 
  Dutch translation (thank you jan_sh, rakekniven, richarson, tiwi90, mlocati,
  mlimacarlos and Fabijenna)

## 18.11.5
- you now can **override the interface font size** in the *Interface settings*
  (for [#1071](https://github.com/pbek/QOwnNotes/issues/1071))
- high DPI scaling is now enabled (if built with Qt 5.6 or above)
  (for [#1071](https://github.com/pbek/QOwnNotes/issues/1071))
- added more German, Italian and Dutch translation (thank you jan_sh, mlocati
  and Fabijenna)

## 18.11.4
- the **selection colors** in **dark mode** are now adapted to not get removed
  by blue filters like *f.lux* (for [#1067](https://github.com/pbek/QOwnNotes/issues/1067))
- there now is a description text for each layout in the *Welcome dialog*
  (for [#1069](https://github.com/pbek/QOwnNotes/issues/1069))
    - the name of the layouts and the description texts will now be translated
- added more German, French, Hungarian and Dutch translation (thank you jan_sh,
  Botigli and PurplePlums)

## 18.11.3
- the `Cancel` button of the note encryption password dialog is now always enabled
  (for [#1065](https://github.com/pbek/QOwnNotes/issues/1065))
- increased top and bottom padding of table cells in the note preview
  (for [#1018](https://github.com/pbek/QOwnNotes/issues/1018))
- after a panel layout was chosen in the welcome dialog the application 
  window will now be centered and resized initially (for [#1069](https://github.com/pbek/QOwnNotes/issues/1069))
- the size of the welcome dialog is now adapted to more fit the layout 
  selector and the background color of the layout preview is adapted to the 
  dialog background color (for [#1069](https://github.com/pbek/QOwnNotes/issues/1069))
- interface languages now work in the snap version of QOwnNotes
- added more Chinese Traditional, Spanish, Italian and Catalan translation
  (thank you abev66, richarson, tiwi90 and MGuerra)

## 18.11.2
- you can now select the **initial panel layout** in the *Welcome dialog*
  (for [#1069](https://github.com/pbek/QOwnNotes/issues/1069))
    - `Minimal`, `Full`, `Full vertical` and `Single column` are currently available
    - a screenshot of the layout will be viewed when selecting one 
- the **note preview** will now view **table borders** (for [#1018](https://github.com/pbek/QOwnNotes/issues/1018))
    - the script `Markdown-it markdown parser` now also supports table borders
    - to disable this feature you can use `table {border-width: 0;}` in the
      `Custom note preview styling` script
- the SSL certificate for <https://docs.qownnotes.org> now works properly 
- if the `Cancel` button is clicked in the *Welcome dialog* now all settings 
  will be cleared before the application quits
- added more German, Spanish, Italian, French, Catalan, Brazilian Portuguese, 
  Hungarian and Dutch translation (thank you rakekniven, richarson, mlocati, 
  Botigli, MGuerra, mlimacarlos and Fabijenna)

## 18.11.1
- the **auto formatter** of **markdown tables** will now also automatically increase
  the size of the headline separator (for [#1052](https://github.com/pbek/QOwnNotes/issues/1052))
- you can now **ignore certain note subfolders** by defining them as list of
  regular expressions (separated by `;`) in the *Panels settings*
  (for [#1054](https://github.com/pbek/QOwnNotes/issues/1054))
    - by default folders starting with a `.` now will be ignored (this was
      already the case on Linux and macOS)
- added more Italian, German, Catalan, Spanish, Chinese Traditional and French
  translation (thank you mlocati, rakekniven, MGuerra, richarson, abev66 and Botigli)

## 18.11.0
- improved code block import of the **Evernote importer**
  (for [#1047](https://github.com/pbek/QOwnNotes/issues/1047))
- you can now **auto format** spaces in a **markdown table** at the current cursor
  position with *Edit / Edit / Auto format table* (for [#1052](https://github.com/pbek/QOwnNotes/issues/1052))
    - the auto complete shortcut <kbd>Ctrl</kbd> + <kbd>Space</kbd> also will
      attempt to auto format the table
- added more Italian, German, Catalan, Brazilian Portuguese, Chinese Traditional
  and Turkish translation (thank you mlocati, rakekniven, MGuerra, mlimacarlos,
  abev66 and ardakilicdagi)

## 18.10.6
- improved code block import of the **Evernote importer**
  (for [#1047](https://github.com/pbek/QOwnNotes/issues/1047))
    - some other issues were also fixes
- added more information to the note sharing dialog and the sharing notification
  (for [#1049](https://github.com/pbek/QOwnNotes/issues/1049))
- added more Spanish, Italian, German, Catalan and Brazilian Portuguese translation
 (thank you richarson, mlocati, rakekniven, MGuerra and mlimacarlos)

## 18.10.5
- added a notification that you have to sync your notes yourself when sharing
  notes (for [#1049](https://github.com/pbek/QOwnNotes/issues/1049))
- did more `ownCloud` to `ownCloud / Nextcloud` text replacing
- the **Evernote importer** is now able to import **code blocks**
  (for [#1047](https://github.com/pbek/QOwnNotes/issues/1047))
- added more Italian, Chinese Traditional, Catalan, Hungarian and Dutch
  translation (thank you mlocati, abev66, MGuerra and Fabijenna)

## 18.10.4
- the file open dialogs that can select multiple files now also remember the 
  previous directory correctly
- **Importing notes from text files** was improved
    - you will now see what file is imported in a progress bar
    - you can now cancel the import process
    - speed improvement when importing many files
- the *Copy note headline* menu entry was moved to the *Edit / Extra* menu

## 18.10.3
- a status message for the `Copy note headline` feature was added
- the debug output in the *Debug settings* now supports more data-types for 
  setting entries and will also show the name of the data-types
- the last 200 entries of the note history will now be stored when you switch 
  note folders or quit the application and will be restored accordingly
  (for [#1044](https://github.com/pbek/QOwnNotes/issues/1044))
    - please visit [QOwnNotes shortcuts](https://www.qownnotes.org/getting-started/shortcuts.html)
      for a list of all shortcuts to navigate the note history
    - if you have enabled the setting `Restore cursor position when opening a note`
      in the *General settings* your position in notes will be restored if 
      you restart the application or switch between note folders
- fixed QOwnNotes favorite button in Gnome dash
  (for [#1019](https://github.com/pbek/QOwnNotes/issues/1019))
- added more Spanish, Italian, Chinese Traditional, Catalan, German, Brazilian
  Portuguese and Hungarian translation (thank you richarson, mlocati, abev66,
  MGuerra, rakekniven, mlimacarlos and Fabijenna)

## 18.10.2
- you can now **copy** the **headline** of the current note with the shortcut 
  <kbd>Alt</kbd> + <kbd>⇧</kbd> + <kbd>H</kbd>
    - multiple types of headlines are supported
    - please visit [QOwnNotes shortcuts](https://www.qownnotes.org/getting-started/shortcuts.html)
      for a list of all shortcuts
- added more Spanish, Italian, Brazilian Portuguese and German translation
  (thank you richarson, mlimacarlos and rakekniven)

## 18.10.1
- changes of the current note outside of QOwnNotes will now not trigger the
  `The current note was modified outside of this application!` dialog while 
  editing the current note if the content of the note file wasn't really
  modified (for [#1041](https://github.com/pbek/QOwnNotes/issues/1041))
    - this was a problem because the Nextcloud / ownCloud sync client seems 
      to write to the note files on some systems even if there were no actual
      changes to be made, this spawned the dialog if the user was actively
      editing the note
- there now is a *Telegram* menu entry in the *Help / Open chat* menu to open
  a browser window with the **QOwnNotes Telegram group**
- there is a community-created QOwnNotes package for **FreeBSD** by *Matthew Stobbs*
  at [QOwnNotes FreeBSD port](https://www.qownnotes.org/installation#FreeBSD)
  (for [#969](https://github.com/pbek/QOwnNotes/issues/969), a big thank you
  to Matthew!)
- added more German translation

## 18.10.0
- on Linux and Windows now the setting to only run one app instance at the same 
  time will be used by default
    - macOS only runs one instance of apps, so we don't need this setting there
    - you need to restart the app a second time to let this setting take effect
    - you can still turn this off in the settings, but keep in mind that this
      will cause problems with settings written by the multiple instances
    - use the `--session` parameter to run QOwnNotes multiple times with 
      different settings
        - you can also export your settings from your first instance and import
          them in your second instance
- the macOS app file now has a version number that can be viewed with Finder
  (for [#1038](https://github.com/pbek/QOwnNotes/issues/1038))
- a new community maintained repository for CentOS by Jörg Woll is now back again
    - visit [CentOS repository](https://www.qownnotes.org/installation#CentOS)
      for more information
- added more French translation (thank you adrientaudiere)

## 18.09.5
- you can now create markdown new-lines (with two spaces at the end) by pressing
  <kbd>Shift</kbd> + <kbd>Return</kbd> in the note editor (for [#1032](https://github.com/pbek/QOwnNotes/issues/1032))
- note folder names in the *Note folders* settings will now be truncated at 50
  characters and new-line characters will be removed (for [#1027](https://github.com/pbek/QOwnNotes/issues/1027))

## 18.09.4
- fixed selecting editor and preview fonts in macOS under Qt 5.11.2
  (for [#1033](https://github.com/pbek/QOwnNotes/issues/1033))
- added more Catalan, Dutch and German translation (thank you MGuerra, Fabijenna
  and dgirke)

## 18.09.3
- attempted to improve the *single application mode* (for [#1025](https://github.com/pbek/QOwnNotes/issues/1025))
    - on certain occasions it was possible that the app wouldn't start a 2nd 
      time even if it was closed, this should be fixed now
- the **AppImage** version of QOwnNotes is working again
  (for [#1002](https://github.com/pbek/QOwnNotes/issues/1002), a big thank you
  to @fodrice for the patch)
- improved automatic note renaming for manually added note links that end with a `@`
- replaced `ownCloud` by `ownCloud / Nextcloud` in the *Welcome dialog*
- added more Spanish, Italian, Chinese Traditional, Greek and German translation
  (thank you richarson, mlocati, abev66, Soulrain and rakekniven)

## 18.09.2
- you can now **import text files as notes** from the *Note / Import* menu
    - if no markdown headline was found in the file the basename of the file 
      will be used as headline
- the list of note subfolders in the note list when moving notes to note
  subfolders is now sorted alphabetically (for [#1022](https://github.com/pbek/QOwnNotes/issues/1022))

## 18.09.1
- prevented possible misbehavior when moving multiple tags to another tag
  (for [#1014](https://github.com/pbek/QOwnNotes/issues/1014))
- **note links** will now also jump to notes in **other note subfolders**
  (for [#478](https://github.com/pbek/QOwnNotes/issues/478))
    - first only the current note subfolder will be searched, if the note
      wasn't found there all note subfolders will be searched
    - if the linked note was in another note subfolder the current note
      subfolder will be changed to that note subfolder
    - the note link dialog now shows notes of all note subfolders
    - highlighting of note links if they are broken also works over all note subfolders
    - automatically updating links when notes are renamed should now also work
      across note-subfolders 
- fixed possibly duplicate entries in the script list of the *Script repository*
- changed a lot of links to QOwnNotes webpage in the application and
  documentation from `http` to `https`
- improved support for alternative links ending with `@` when automatically 
  updating links when notes are renamed
- added more Spanish, Italian, Chinese Traditional, Catalan, German, Brazilian
  Portuguese, Hungarian and Dutch translation (thank you richarson, mlocati,
  abev66, MGuerra, rakekniven, mlimacarlos and Fabijenna)

## 18.09.0
- there now is a new button in the *Update dialog* to don't show the dialog 
  automatically any more (for [#813](https://github.com/pbek/QOwnNotes/issues/813))
    - this button does the same as the checkbox to disable the update dialog in
      the *General settings*
- fixed opening of links like `[https://www.qownnotes.org](https://www.qownnotes.org)`
  when <kbd>Ctrl</kbd>-clicked in the note edit panel
- `qownnotes` snap was fixed for systems with wayland (for [#1004](https://github.com/pbek/QOwnNotes/issues/1004),
  a big thank you to @pachulo)
- added more Spanish, Italian, Chinese Traditional, Catalan, Arabic, Hungarian
  and Dutch translation (thank you richarson, mlocati, abev66, MGuerra,
  sosozozo and Fabijenna)

## 18.08.11
- the **Find action** dialog now searches for every entered word in `and`-fashion
  (for [#1007](https://github.com/pbek/QOwnNotes/issues/1007))
    - all menu items that have all of the entered words (or parts of them) in 
      the name or tooltip will be shown
- attempted to fix a reloading of the notes list problem on some installations
  after importing notes from Evernote (for [#1006](https://github.com/pbek/QOwnNotes/issues/1006))
- when selecting multiple notes clicking on the headlines of the selected 
  notes in the note preview will now jump to the note when using subfolders and
  `All notes` was selected, even if the note wasn't in the same note subfolder
  (for [#949](https://github.com/pbek/QOwnNotes/issues/949))
- added more Czech, Japanese, Indonesian and German translation (thank you
  hruban, piyo and zmni)

## 18.08.10
- some restructuring of the **Issue assistant** was made and some additional
  information to help the user was provided
- added a new cli parameter `--dump-settings` that prints out a dump of the 
  settings and other information about the application and environment in GitHub
  Markdown and exits the application
    - the documentation page [Command line interface parameters](http://docs.qownnotes.org/en/develop/cli-parameters.html)
      was updated
- the **Find action** dialog now searches for every entered word in an `or`-fashion
  (for [#1007](https://github.com/pbek/QOwnNotes/issues/1007))
    - all menu items that have one of the words in them will be shown
- added more Spanish, Italian, Chinese Traditional, Catalan and German
  translation (thank you richarson, mlocati, abev66, MGuerra and rakekniven)

## 18.08.9
- there now is a new **Issue assistant** to post questions, feature requests or problems
    - you will find it in the *Help menu*
    - it will guide you to the steps to enter information and gathers all 
      needed data to post an issue on the [Issue page](https://github.com/pbek/QOwnNotes/issues/new)
- added more German, Hungarian and Dutch translation (thank you rakekniven and Fabijenna)

## 18.08.8
- improved preview of multiple notes (for [#949](https://github.com/pbek/QOwnNotes/issues/949))
    - implemented correct breaking of lines
    - fixed preview of note text that was interpreted as html
    - removed empty lines
    - showing 5 lines instead of 3
    - lines are truncated at 80 characters
- more **Evernote import** improvements (for [#998](https://github.com/pbek/QOwnNotes/issues/998))
    - fixed importing of notes if user enabled that the filename can be
      different from the note name in the settings
    - added importing of the note attribute metadata of version 3 of the
      Evernote export file
- added more Brazilian Portuguese, Chinese Traditional, Chinese Simplified,
  and German translation (thank you richarson, abev66 and wenzhuoz)

## 18.08.7
- improved the user interface of the **Evernote import** (for [#998](https://github.com/pbek/QOwnNotes/issues/998))
- if you now **select multiple notes** a **small preview** of those notes will be 
  viewed in the preview panel (for [#949](https://github.com/pbek/QOwnNotes/issues/949))
    - you can click on the headlines of those notes to jump to the note
    - only the first 40 selected notes will be shown to not decrease the 
      performance when selecting a lot of notes
- added more Italian, Indonesian, German, Hungarian and Dutch translation (thank
  you mlocati, zmni, kuik, rakekniven and Fabijenna)

## 18.08.6
- when importing from **Evernote** you can now also **import metadata** from 
  your Evernote notes (for [#998](https://github.com/pbek/QOwnNotes/issues/998))
    - you can select what metadata to import in the import dialog from a 
      tree widget
    - the dialog will remember which items you want to import
    - the selected metadata will be imported as markdown table if available 
      in the Evernote note 
- the Evernote import dialog will now also remember if you want to import 
  images or attachments
- a lot triple dots were changed to ellipsis in the user interface

## 18.08.5
- linking to notes with long filenames now works
    - this was a problem because of STD 3 restrictions limiting the hostname 
      to 63 characters
    - now we are using the username of an url for the note filename, instead of 
      the hostname for long filenames
    - for existing note-links just add a `@` at the end of the url to make it work
- fixed some possible issues with the note encryption (thank you @sdicke)
- if you select multiple notes you now can use the new `Note operations...` 
  button in in the area where the note edit normally is displayed to do 
  operations on multiple notes (for [#949](https://github.com/pbek/QOwnNotes/issues/949))
- added more Spanish, Italian, Chinese Traditional, Chinese Simplified,
  Indonesian, German, Hungarian and Dutch translation (thank you richarson,
  mlocati, abev66, wenzhuoz, zmni, rakekniven and Fabijenna)

## 18.08.4
- you can now **export and import your settings** (for [#989](https://github.com/pbek/QOwnNotes/issues/989))
    - keep in mind that when importing your settings may get removed and not 
      every setting will get restored, like the note folder settings and which
      scripts you were using
    - you also will need to adjust some settings, especially across platforms,
      but your notes will stay intact
- added a small fix for highlighting comment blocks (thank you @sdicke)
- added documentation for command line parameters at [Command line interface parameters](http://docs.qownnotes.org/en/develop/cli-parameters.html)
- added more Spanish, Italian, Chinese Traditional, Chinese Simplified,
  Indonesian, German, Hungarian and Dutch translation (thank you richarson,
  mlocati, abev66, wenzhuoz, zmni, rakekniven and Fabijenna)

## 18.08.3
- fixed a problem with automatically storing modified notes (for [#987](https://github.com/pbek/QOwnNotes/issues/987))
- updated QOwnNotesApi to be used with Nextcloud 14 (for [#20](https://github.com/pbek/qownnotesapi/issues/20))
- added more Catalan and Hungarian translation (thank you MGuerra and Fabijenna)

## 18.08.2
- significantly **reduced application startup time** (for [#985](https://github.com/pbek/QOwnNotes/issues/985),
  a big thank you to @frsfnrrg)
- when clicking a link to a note the currently selected notes will now be 
  cleared to avoid that multiple notes are selected and thus the current note 
  will not be shown
- added many improvements to the new **Vim mode** (for [#205](https://github.com/pbek/QOwnNotes/issues/205))
    - visible commands in the status bar
    - support for more commands like `:w`, `:q` and `:wq`
    - searching and replacing with more feedback
    - visual mode also works now with more feedback
    - text indenting
- added more Polish and Spanish translation (thank you lucjan and richarson)

## 18.08.1
- if automatic bracket closing is enabled and if you are entering double quotes 
  and the next character is already a double quote then the character will not
  be entered, but the cursor will be moved to the right (for [#982](https://github.com/pbek/QOwnNotes/issues/982))
- there now is a new scripting function `script.triggerMenuAction()` to trigger
  a menu action
    - for more information please take a look at the [scripting documentation](https://www.qownnotes.org/scripting/methods-and-objects.html#triggering-a-menu-action)
- you can now focus the note text edit in a script with `mainWindow.focusNoteTextEdit()`
- there now is a new script `Disable read-only mode` in the *Scripting repository*
  to disable the read-only mode if a note is double clicked
    - the note text edit will also be focused afterwards
- added more Polish, Chinese Traditional, Brazilian Portuguese, German and
  Italian translation (thank you lucjan, abev66, mlimacarlos, rakekniven and mlocati)

## 18.08.0
- you can now enable a new **Vim mode** in the *Editor settings*
  (for [#205](https://github.com/pbek/QOwnNotes/issues/205))
    - keep in mind that some QOwnNotes shortcuts will not work correctly in that mode
- added more Catalan and Polish translation (thank you MGuerra and lucjan)

## 18.07.8
- if note editing was disabled when you create a new note it is now turned on automatically
- when selecting multiple notes the note edit widget will now be hidden and 
  the amount of selected notes will be shown (for [#949](https://github.com/pbek/QOwnNotes/issues/949))
- added more Polish and German translation (thank you lucjan and rakekniven)

## 18.07.7
- attempted to fix a problem with Chinese and Japanese input methods
  (for [#976](https://github.com/pbek/QOwnNotes/issues/976))
- added a script `Solar (Jalaali) calendar integration` to the *Script repository*
  to attempt to integrate the solar (Jalaali) calendar into QOwnNotes
  (for [#977](https://github.com/pbek/QOwnNotes/issues/977))
    - it currently supports creating a headline for new notes with the current
      Jalaali date and time
    - take a look at [solar-calendar-integration](https://github.com/qownnotes/scripts/tree/master/solar-calendar-integration)
      if you want to improve it
- added more Brazilian Portuguese and Portuguese translation (thank you
  mlimacarlos and mtbf99)

## 18.07.6
- fixed attachment detection and link generation in the new dialog to manage
  orphaned attachments (for [#895](https://github.com/pbek/QOwnNotes/issues/895))
- did some improvements on updating the current note in the note list if the 
  note list preview is turned on (for [#949](https://github.com/pbek/QOwnNotes/issues/949))
- added more Spanish, Chinese Traditional, Russian, Dutch and Hungarian
  translation (thank you richarson, abev66, pu.sash and Fabijenna)

## 18.07.5
- if you open the dialog to manage orphaned images the first item will now
  automatically be selected
- there now is a new menu entry to **manage orphaned attachments** in the
  `Edit / Extra` menu (for [#895](https://github.com/pbek/QOwnNotes/issues/895))
    - you can view information about the attachment, add the attachment to the
      current note, remove the attachment, open the attachment with an 
      associated application or open the folder of the attachment
- added more Catalan, Chinese Traditional, Brazilian Portuguese, Indonesian, 
  German and Russian translation (thank you MGuerra, abev66, mlimacarlos, zmni,
  rakekniven and pu.sash)

## 18.07.4
- under Windows it is now checked if user chose a different drive for the note 
  folder in the welcome dialog in portable mode (for [#971](https://github.com/pbek/QOwnNotes/issues/971))
- the error dialog for errors while talking to your ownCloud server will not 
  be shown a 2nd time if it is already present on the screen (for [#950](https://github.com/pbek/QOwnNotes/issues/950)) 
- attempted to fix builds for older Qt versions
- added more Spanish translation (thank you richarson)

## 18.07.3
- there now is a new checkbox in the *Panel settings* to show the notes in 
  in the note list with a preview (for [#949](https://github.com/pbek/QOwnNotes/issues/949))
    - this feature is still experimental and implementation is not finished yet
    - manual renaming of the note name via the context menu is already implemented
- attempted to fix OBS builds for Debian 8, xUbuntu_15.10 and xUbuntu_15.04
- added more Catalan, Chinese Traditional, Brazilian Portuguese, Indonesian, 
  German, Spanish, Italian, Dutch and Hungarian translation (thank you MGuerra,
  abev66, mlimacarlos, zmni, rakekniven, richarson, mlocati and Fabijenna)

## 18.07.2
- added more debug output on the terminal and checks when using the welcome
  dialog to setup the note folder to check for troubles on some Windows machines
  (for [#951](https://github.com/pbek/QOwnNotes/issues/951))
- on Windows there now is a new experimental settings in the *General settings*
  to automatically close the note folder database to prevent problems with sync 
  tools (for [#926](https://github.com/pbek/QOwnNotes/issues/926))
- added more Brazilian Portuguese translation (thank you mlimacarlos)

## 18.07.1
- fixed displaying of tags when navigating notes with the arrow keys and when
  switching note folders (for [#958](https://github.com/pbek/QOwnNotes/issues/958))
- if the settings dialog is opened from the welcome dialog now only the one 
  settings page requested is visible to prevent problems with a not finished 
  setup process
- added more Catalan, Dutch and Hungarian translation (thank you MGuerra and Fabijenna)

## 18.07.0
- fixed updating the *note tags panel* after multiple notes were selected
  (for [#946](https://github.com/pbek/QOwnNotes/issues/946))
- added some more debug messages for finding a problem under Windows
  (for [#951](https://github.com/pbek/QOwnNotes/issues/951))
- added more Chinese Traditional, Italian, Spanish and German translation 
  (thank you abev66, mlocati, richarson and rakekniven)

## 18.06.7
- tags in the `Move tag to...` context menu of the tag list are now sorted by name
 (for [#947](https://github.com/pbek/QOwnNotes/issues/947))
- if a code block is inserted in an empty line then automatically a multi-line
  code block will be inserted instead of an inline code block
  (for [#912](https://github.com/pbek/QOwnNotes/issues/912))
- the *Note tags* panel now shows the tags of all selected notes and is also
  able to remove tags from or add tags to all those notes
  (for [#946](https://github.com/pbek/QOwnNotes/issues/946))
    - if notes are selected there will be a symbol in front of the tags to 
      show you how many notes are selected
- added more Chinese Traditional, Brazilian Portuguese, Italian, Catalan, 
  Dutch and Hungarian translation (thank you abev66, mlimacarlos, mlocati, 
  MGuerra and Fabijenna)

## 18.06.6
- improved setting the size of the settings dialog if it was opened for the 
  first time
- tags in the `tag selected notes with...` context menu of the note list are
  now sorted by name (for [#947](https://github.com/pbek/QOwnNotes/issues/947))
- the search line edits in the note-, tag- and subfolder-lists can now be 
  cleared with the <kbd>Escape</kbd> key (for [#948](https://github.com/pbek/QOwnNotes/issues/948))

## 18.06.5
- disabled note folder database closing feature temporarily, since it breaks 
  the application under Windows for new note folders (for [#926](https://github.com/pbek/QOwnNotes/issues/926))

## 18.06.4
- the pages in the *Settings dialog* are now scrollable to assure that all
  settings can be viewed on very low screen resolutions like on netbooks
  (for [#940](https://github.com/pbek/QOwnNotes/issues/940))

## 18.06.3
- fixed the menu bar visibility after leaving the *distraction free mode* if it 
  was invisible when entering it (for [#941](https://github.com/pbek/QOwnNotes/issues/941))
- the note folder database will now be explicitly closed for Windows after each
  operation to prevent synchronization problems (for [#926](https://github.com/pbek/QOwnNotes/issues/926))
- added more Chinese Traditional, Brazilian Portuguese, Italian, Catalan, 
  Spanish, German, Dutch and Hungarian translation (thank you abev66,
  mlimacarlos, mlocati, MGuerra, richarson, rakekniven and Fabijenna)

## 18.06.2
- added `cups-control` to he snap version of QOwnNotes to possibly enable 
  support for printing to printers (for [#938](https://github.com/pbek/QOwnNotes/issues/938))
- added tagging support for subfolders (for [#884](https://github.com/pbek/QOwnNotes/issues/884),
  a big thank you to @cal2195)
- added new **Chinese Traditional translation** (for [#934](https://github.com/pbek/QOwnNotes/issues/934),
  a big thank you to @abev66)
    - the current translation `Chinese` was renamed to `Chinese Simplified`
- the desktop file `QOwnNotes.desktop` was renamed to `PBE.QOwnNotes.desktop`
  for the Wayland compositor (for [#933](https://github.com/pbek/QOwnNotes/issues/933))

## 18.06.1
- when the application gets restarted from the *Settings dialog* a possible
  `--clear-settings` parameter will now not be appended as parameter, so the 
  settings will not get removed a second time after the restart
- added a new scripting hook `handleNoteNameHook(note)` that is called when the
  note name is determined for a note
  (for [#929](https://github.com/pbek/QOwnNotes/issues/929))
    - take a look at the
      [handleNoteNameHook documentation](https://www.qownnotes.org/scripting/methods-and-objects.html#handlenotenamehook)
      for more information
    - the user interface will currently not be updated when the note text (that 
      could result in a note name change with the new hook) is stored

## 18.06.0
- encrypted notes are now read-only in the editor unless you are using
  *Edit encrypted note* or you are decrypting it (for [#815](https://github.com/pbek/QOwnNotes/issues/815))
- fixed storing of keyboard shortcuts in the *Shortcuts settings*
  (for [#912](https://github.com/pbek/QOwnNotes/issues/912))

## 18.05.7
- despite users having problems in the past with fetching the update request 
  via SSL are we trying to use https for the update request again
  (for [#918](https://github.com/pbek/QOwnNotes/issues/918))
    - the QOwnNotes webpage <http://www.qownnotes.org> will now also be 
      automatically redirected to <https://www.qownnotes.org>
- it now will not be attempted to store a new note text if the note file is read-only
  (for [#916](https://github.com/pbek/QOwnNotes/issues/916))
    - this was especially a problem for the `noteTaggingHook` scripting 
      method, because the note text is modified there most of the time
        - but the user interface still might think that a tag was added to or
          removed from a note, because the `noteTaggingHook` scripting method 
          might not necessarily modify the note text, but use for example an 
          external database
- when pasting HTML code with <kbd>Ctrl</kbd> + <kbd>⇧</kbd> + <kbd>V</kbd>
  the escaped characters `&gt;`,`&lt;` and `&amp;` will now be unescaped to 
  `>`, `<` and `&` (for [#922](https://github.com/pbek/QOwnNotes/issues/922))
- did some Qt 5.11 build fixing

## 18.05.6
- fixed a Segmentation fault when opening the settings dialog from the welcome dialog
  (for [#914](https://github.com/pbek/QOwnNotes/issues/914))

## 18.05.5
- a conflicting keyboard shortcut will now be highlighted if you jump to it 
  while assigning a shortcut in the *Shortcuts settings*
  (for [#912](https://github.com/pbek/QOwnNotes/issues/912))
- all export methods in the *Export menu* and the note text-edit context menu
  will now open the folder of the exported file after exporting
- added more Brazilian Portuguese, Italian, Catalan, Spanish and German
  translation (thank you mlimacarlos, mlocati, MGuerra, richarson and 
  rakekniven)

## 18.05.4
- you are now able to jump to the conflicting keyboard shortcut if you assign a
  shortcut that is already used by another menu item in the *Shortcuts settings*
  (for [#912](https://github.com/pbek/QOwnNotes/issues/912))
    - the duplicate dialog if assigning a conflicting shortcut if no shortcut
      was defined before was fixed
- added a new scripting hook `noteDoubleClickedHook(note)` that is called every
  time a note is double clicked in the note list (for [#913](https://github.com/pbek/QOwnNotes/issues/913))
    - take a look at the
      [noteDoubleClickedHook documentation](https://www.qownnotes.org/scripting/methods-and-objects.html#noteDoubleClickedHook)
      for more information
    - there is an example script at
      [external-note-open.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/external-note-open.qml)
    - you will also find a script `External note open` in the script repository
      that uses the new hook
- added more Brazilian Portuguese, Italian, Dutch and Hungarian translation
  (thank you mlimacarlos, mlocati and Fabijenna)

## 18.05.3
- you will now get a notification if you assign a shortcut that is already 
  used by another menu item in the *Shortcuts settings*
  (for [#912](https://github.com/pbek/QOwnNotes/issues/912))
- added more Catalan and German translation (thank you MGuerra and rakekniven)

## 18.05.2
- the todo calendar list in the *Settings dialog* will now only be reloaded 
  if it was empty or was reloaded manually to prevent a clearing of the list 
  when the server is not reachable
    - the display names of the CalDav calendars will now be stored too
- `QXcbConnection: XCB error:` warnings are now suppressed

## 18.05.1
- there now is a new scripting command `script.fetchNoteIdsByNoteTextPart()` to 
  fetch the ids of all notes with a certain text in the note text
  (for [#904](https://github.com/pbek/QOwnNotes/issues/904))
    - for more information please take a look at the [scripting documentation](https://www.qownnotes.org/scripting/methods-and-objects.html#search-for-notes-by-note-text)
    - there now is a new script `Unique note id` in the *Script repository*
      that generates a 10 character alphanumeric id for the current note and
      also allows to jump to the note by it
- there now is a software repository for **Fedora 28**
    - please visit the [Fedora installation page](https://www.qownnotes.org/installation#Fedora)
      for instructions
- added more Brazilian Portuguese, Indonesian, German, Spanish, Italian, Dutch
  and Hungarian translation (thank you mlimacarlos, zmni, rakekniven, richarson,
  mlocati and Fabijenna)

## 18.05.0
- there now is a new scripting command `script.noteTextEditSelectAll()` to 
  select all text in the note text edit (for [#902](https://github.com/pbek/QOwnNotes/issues/902))
    - you can use this together with `script.noteTextEditWrite()` to overwrite
      the whole text of the current note
    - for more information please take a look at the [scripting documentation](https://www.qownnotes.org/scripting/methods-and-objects.html#select-all-text-in-the-note-text-edit)
- the dialog to delete orphaned images isn't modal any more and you can now 
  also use it to insert those images into the current note
- added more Indonesian, Catalan and German translation (thank you zmni, MGuerra
  and rakekniven)

## 18.04.5
- if note editing was disabled now a notification dialog will open if you start 
  typing in the note edit (for [#900](https://github.com/pbek/QOwnNotes/issues/900))
    - in addition a button with the text `Read-only` will be viewed in the 
      status bar if note editing is disabled
    - you can click this button to enable note editing again
- double-clicking on a tag now assigns the tag to the current note
  (for [#896](https://github.com/pbek/QOwnNotes/issues/896))
- `QXcbConnection: XCB error: 8` warnings are now suppressed
- added more Brazilian Portuguese, Indonesian, Catalan, German, Spanish,
  Italian, Dutch and Hungarian translation (thank you mlimacarlos, zmni,
  MGuerra, rakekniven, richarson, mlocati and Fabijenna)

## 18.04.4
- fixed a warning if you are un-indenting text at the start or end of the note
- you can now scroll the note content up and down with <kbd>Ctrl</kbd> + <kbd>Up</kbd>
  and <kbd>Ctrl</kbd> + <kbd>Down</kbd> (for [#872](https://github.com/pbek/QOwnNotes/issues/872))
    - please visit [QOwnNotes shortcuts](https://www.qownnotes.org/getting-started/shortcuts.html)
      for a list of all shortcuts
- you can now use the shortcut <kbd>F4</kbd> to jump to the *Note edit panel*
  (for [#877](https://github.com/pbek/QOwnNotes/issues/877))
    - if the *Note edit panel* was disabled it will be enabled first
    - please visit [QOwnNotes shortcuts](https://www.qownnotes.org/getting-started/shortcuts.html)
      for a list of all shortcuts

## 18.04.3
- `libpng warning: iCCP: known incorrect sRGB profile` warnings are now suppressed 
- parts of the logging-handling was rewritten to even more prevent crashes 
  when logging to the log widget while the app is shutting down
- added more German translation (thank you rakekniven)

## 18.04.2
- added a small note edit styling change for Windows and macOS in the
  note edit is configured as central widget (for [#420](https://github.com/pbek/QOwnNotes/issues/420))
- fixed links to files with spaces in the file-path when creating the
  link with the *Link dialog*
- in the *Link dialog* the the last selected file will now be remembered
  in the open-file dialog
- attempted to fix a non-reproducible crash in Windows 7 when a note is
  stored by clicking the close button (for [#886](https://github.com/pbek/QOwnNotes/issues/886))
- added more Brazilian Portuguese, Indonesian, Catalan, French, Spanish,
  Italian, Dutch and Hungarian translation (thank you mlimacarlos, zmni,
  MGuerra, bepolymathe, asoliverez, mlocati and Fabijenna)

## 18.04.1
- fixed mistakenly questions about creating a new note when <kbd>Ctrl</kbd> +
  clicking on a note or pressing <kbd>Ctrl</kbd> + <kbd>Space</kbd> in a note
- there now is a new checkbox in the *Panels settings* to use the note edit
  panel as fixed central widget to be **resized** if the **window** or other
  **panels** are **resized** (for [#420](https://github.com/pbek/QOwnNotes/issues/420))
    - for new installations of QOwnNotes this will be enabled by default
    - for running sessions of QOwnNotes this will be disabled by default
    - if this is disabled you are able to create more complex panel layouts,
      but all panels will be resized if the window is resized
- added more Brazilian Portuguese, Spanish and Indonesian translation
  (thank you mlimacarlos, richarson, zmni)

## 18.04.0
- fixed a problem with the highlighting of headlines in a non-default
  highlighting schema on Windows 10 (for [#883](https://github.com/pbek/QOwnNotes/issues/883))
- description text of script options in the *Script settings* is now selectable
  and links are clickable (for [#882](https://github.com/pbek/QOwnNotes/issues/882))
- fixed a problem in the *Script repository* where searching for a script 
  would also trigger the loading of more items
- fixed possible `setPointSizeF` warnings
- added more Polish, Italian, Catalan, Spanish, German, Dutch and Hungarian
  translation (thank you lucjan, mlocati, MGuerra, richarson, rakekniven and Fabijenna)

## 18.03.10
- **broken note-links** are now **highlighted** in the note edit (for [#736](https://github.com/pbek/QOwnNotes/issues/736))
    - there is a new entry `Broken link` in the *Color schema* settings where
      you can change the styling
- added repositories for **openSUSE Leap 15**, **SUSE Linux Enterprise 15** and
  **SUSE Linux Enterprise 12 SP3**
    - please visit the [openSUSE installation page](https://www.qownnotes.org/installation#openSUSE)
      for instructions
- years were bumped to 2018
- added more Brazilian Portuguese, Italian, Catalan, Spanish, German, French,
  Dutch and Hungarian translation (thank you mlimacarlos, mlocati, MGuerra,
  richarson, rakekniven, bepolymathe and Fabijenna)

## 18.03.9
- you can now choose to create a new note when clicking on a "broken" note-link
  in the note edit or the preview (for [#736](https://github.com/pbek/QOwnNotes/issues/736))
- added more Czech translation (thank you hruban)

## 18.03.8
- the scripting command `script.writeToFile()` now writes text to a file
  in the UTF-8 character set (for [script issue #25](https://github.com/qownnotes/scripts/issues/25))
    - for more information about the command please take a look at
      [scripting documentation](https://www.qownnotes.org/scripting/methods-and-objects.html#writing-text-to-a-file)
- added the plug `opengl` to the snapped version of QOwnNotes to prevent 
  OpenGL AppArmor errors (for [#875](https://github.com/pbek/QOwnNotes/issues/875))
- added the plug `removable-media` to the snapped version of QOwnNotes to
  allow access to mounted folders (for [#876](https://github.com/pbek/QOwnNotes/issues/876))
- added more Japanese, Dutch and Hungarian translation (thank you piyo and Fabijenna)

## 18.03.7
- there now is a new scripting command `script.fetchNoteById()` to fetch a note
  by its id (for [#871](https://github.com/pbek/QOwnNotes/issues/871))
    - for more information please take a look at the [scripting documentation](https://www.qownnotes.org/scripting/methods-and-objects.html#fetching-a-note-by-its-id)
- there now is a new scripting command `script.getSaveFileName()` to show a 
  save file dialog (for [#871](https://github.com/pbek/QOwnNotes/issues/871))
    - for more information please take a look at the
      [scripting documentation](https://www.qownnotes.org/scripting/methods-and-objects.html#showing-a-save-file-dialog)
- there now is a new scripting command `script.selectedNotesIds()` to return a
  list of the ids of all selected notes (for [#871](https://github.com/pbek/QOwnNotes/issues/871))
    - for more information please take a look at the
      [scripting documentation](https://www.qownnotes.org/scripting/methods-and-objects.html#getting-a-list-of-the-ids-of-all-selected-notes)
- there now is a new scripting command `script.writeToFile()` to write text 
  to a file (for [#871](https://github.com/pbek/QOwnNotes/issues/871))
    - for more information please take a look at the
      [scripting documentation](https://www.qownnotes.org/scripting/methods-and-objects.html#writing-text-to-a-file)
- there now is a new method `toMarkdownHtml()` for `NoteApi` to get the
  generated html of a note (for [#871](https://github.com/pbek/QOwnNotes/issues/871))
    - for more information please take a look at
      [scripting documentation](https://www.qownnotes.org/scripting/methods-and-objects.html#note)
- there now is a new script `Export notes as one HTML file` in the
  *Script repository* to export multiple notes as one HTML file (for [#871](https://github.com/pbek/QOwnNotes/issues/871))
- the text case toggling feature now automatically selects the word under the 
  cursor if no text was selected (thank you @schittli)
    - additionally, the original cursor- and text-selection will be restored
- added more Czech, Dutch, Catalan, German, Indonesian, Brazilian Portuguese,
  Italian and Spanish translation (thank you hruban, rizki271197, rakekniven,
  Fabijenna, richarson, MGuerra, mlimacarlos and mlocati)

## 18.03.6
- the `Allow note editing` menu item was moved to the `Note` menu
  (for [#866](https://github.com/pbek/QOwnNotes/issues/866))
    - the text was changed to `Allow all note editing` if note editing is 
      disabled and `Disallow all note editing` if note editing is enabled
- fixed possible duplicate list entries in the *Script repository*
- added more Czech, French, Catalan, German, Tagalog, Brazilian Portuguese,
  Italian and Spanish translation (thank you hruban, bepolymathe, rakekniven,
  deletescape, richarson, Janelbrian, MGuerra, mlimacarlos and mlocati)

## 18.03.5
- added **Hiligaynon translation** and **Urdu translation** (a big thank you to
  BaTz and mr_aamirijaz)
    - join us at [QOwnNotes on Crowdin](https://crowdin.com/project/qownnotes/)
      to make QOwnNotes available in more languages or help with the current 
      translation
- there now is a button `Load more scripts` in the *Script repository* that 
  will be shown if there are more scripts available to load from the GitHub
  script repository
- added more Brazilian Portuguese, Spanish, Italian, German and Bengali
  translation (thank you AnikHasan, mlocati, richarson and mlimacarlos)

## 18.03.4
- if you scroll to the bottom of the script list in the *Script repository* now
  the next 30 scripts will be loaded
- if you clear the text in the search bar of the *Script repository* the 
  script list will be reloaded with the first 30 of all scripts

## 18.03.3
- the menu entries to autocomplete, solve equations or open URLs and the menu 
  entry to split a note at the cursor position of the *Edit/Extra* menu were 
  also added to the context menu of the note text edit (for [#866](https://github.com/pbek/QOwnNotes/issues/866))
- the context menu options of the note text edit now also works for encrypted
  notes while editing them
- added more Bengali, French, Catalan, Tagalog, German and Spanish translation 
  (thank you AnikHasan, nizam8089, bepolymathe, MGuerra, sageneil and UgoYak)

## 18.03.2
- there now is a new scripting function `script.insertMediaFile()` to insert a
  media file into the media folder (for [#869](https://github.com/pbek/QOwnNotes/issues/869))
    - for more information please take a look at the [scripting documentation](https://www.qownnotes.org/scripting/methods-and-objects.html#inserting-a-media-file-into-the-media-folder)
- there now is a new scripting function `script.regenerateNotePreview()` to
  regenerate the note preview (for [#869](https://github.com/pbek/QOwnNotes/issues/869))
    - for more information please take a look at the [scripting documentation](https://www.qownnotes.org/scripting/methods-and-objects.html#regenerating-the-note-preview)
- the *checkbox* properties for the *script settings variables* did get a `text`
  attribute for the checkbox text along with the `description` now used for the
  description text (for [#869](https://github.com/pbek/QOwnNotes/issues/869))
    - for more information please take a look at the [scripting documentation](https://www.qownnotes.org/scripting/methods-and-objects.html#registering-script-settings-variables)
- there now is a new script `Scribble` in the *Scripting repository* to insert a
  scribble image to the media-folder, that will be edited by an external image
  editor (for [#869](https://github.com/pbek/QOwnNotes/issues/869))
- switched from SourceForge to TuxFamily for the hosting of the sourcecode 
  archives for the releases of AUR, Snap, Slackware and Gentoo
    - you will find the archives on [QOwnNotes archives](https://download.tuxfamily.org/qownnotes/src)
- added more Hindi, Tagalog, Bengali, Italian, German, Cebuano, Brazilian
  Portuguese, French, Dutch and Hungarian translation (thank you 
  cryptoexplorer7, sageneil, nizam8089, mlocati, rakekniven, cyyy1998, dev2a,
  mlimacarlos and Fabijenna)

## 18.03.1
- fixed translation files

## 18.03.0
- added **Cebuano translation** and **Hindi translation** (a big thank you to
  sageneil, valcarsido, loydjayme25, jbeguna04 and cryptoexplorer7)
    - join us at [QOwnNotes on Crowdin](https://crowdin.com/project/qownnotes/)
      to make QOwnNotes available in more languages or help with the current 
      translation
- when copying or moving notes to another note-folder the embedded media files 
  will now automatically be copied to the media folder of the note-folder
  (for [#865](https://github.com/pbek/QOwnNotes/issues/865))
- added more Tagalog, Filipino, Indonesian, French, Czech, Chinese, Arabic,
  German, Spanish, Swedish, French, Dutch and Hungarian translation (thank you
  sageneil, Janelbrian, saducasjohncarl, rizki271197, bepolymathe, echo511,
  hermitwhite, wael87, deletescape, Fabijenna, richarson, ekblom and dev2a)

## 18.02.5
- the options in the *Log panel* can now be closed with the <kbd>Esc</kbd> key 
- added **Filipino translation** and **Tagalog translation** (a big thank you to
  sageneil, deboyz18, saducasjohncarl and franzancot)
    - join us at [QOwnNotes on Crowdin](https://crowdin.com/project/qownnotes/)
      to make QOwnNotes available in more languages or help with the current 
      translation
- added more French, German, Indonesian, Catalan and Chinese translation (thank
  you dev2a, rakekniven, rizki271197, MGuerra and hermitwhite)

## 18.02.4
- fixed a problem with downloading media files on Arch Linux
  (for [#857](https://github.com/pbek/QOwnNotes/issues/857))
- added more Italian, French, Japanese, Swedish, Brazilian Portuguese, Spanish,
  German, Indonesian, Catalan, Dutch and Hungarian translation (thank you 
  mlocati, bepolymathe, piyo, ekblom, mlimacarlos, richarson, rakekniven,
  rizki271197, MGuerra and Fabijenna)

## 18.02.3
- the *Local trash* button will now by default be shown in the main toolbar
  (for [#674](https://github.com/pbek/QOwnNotes/issues/674))
    - you have to reset your toolbar in the *Toolbar settings* if you are 
      already using the app
- added a workaround for certain translations of the word `ownCloud`
  (for [#850](https://github.com/pbek/QOwnNotes/issues/850))
- most settings of the page setup dialog will now be stored when you 
  confirm the printing- and PDF-export-dialog and will be restored if you visit 
  the dialog again (for [#704](https://github.com/pbek/QOwnNotes/issues/704))
- added more German, Italian, Japanese, Brazilian Portuguese and Spanish
  translation (thank you rakekniven, mlocati, piyo, mlimacarlos and richarson)

## 18.02.2
- there now is a new **scripting command** to show a **question message box**
  `script.questionMessageBox()` (for [#847](https://github.com/pbek/QOwnNotes/issues/847))
    - for more information please take a look at
      [Showing a question message box](https://www.qownnotes.org/scripting/methods-and-objects.html#showing-a-question-message-box)
- added more local trash features (for [#674](https://github.com/pbek/QOwnNotes/issues/674))
    - files that are missing in the local trash-folder will be now be marked in
      the *Local trash* dialog so that you know that they can't be restored
    - the old note is now also added to the local trash if it was renamed 
      manually in the note list
    - sorting by trashing date in the trash dialog now works correctly
    - **trashed items** are now by default **expired after 30 days**
        - you can turn that off in the *Local trash* settings and also set 
          the amount of days there
- when **dropping a markdown file** into QOwnNotes it will now be **copied** to
  the active **note subfolder** and not just to the current note folder
- added more Italian, French, Japanese, Swedish, Brazilian Portuguese, Dutch
  and Hungarian translation (thank you mlocati, bepolymathe, piyo, ekblom,
  mlimacarlos and Fabijenna)

## 18.02.1
- fixed autocompleter user interface offset, especially in the distraction free mode
  (for [#842](https://github.com/pbek/QOwnNotes/issues/842))
- there now is a **local trash** to gather your removed notes
  (for [#674](https://github.com/pbek/QOwnNotes/issues/674))
    - this also works when a note gets renamed because you are modifying the 
      headline of the note
    - there is a new panel *Local trash* in the setting where you can 
      turn the local trash off
    - there is a new menu entry for the *Local trash* that opens a dialog to 
      view your trashed notes and to restore or remove them
    - trashed files are stored per note folder in a new folder `trash`
        - additional information about the trashed files is stored in the 
          sqlite database of the note folder
- added more Italian, German, French, Indonesian, Swedish, Brazilian Portuguese,
  Dutch and Hungarian translation (thank you mlocati, rakekniven, bepolymathe,
  zmni, ekblom, mlimacarlos and Fabijenna)

## 18.02.0
- there now is a new scripting hook `autocompletionHook()` to return a list of
  strings to be added to the autocompletion list when the autocompletion is
  invoked by pressing <kbd>Ctrl</kbd> + <kbd>Space</kbd>
  (for [#842](https://github.com/pbek/QOwnNotes/issues/842))
    - for more information please take a look at the [scripting documentation](https://www.qownnotes.org/scripting/methods-and-objects.html#autocompletionhook)
- there now is a new scripting function `script.noteTextEditCurrentWord()` to 
  read the current word in the note text edit (for [#842](https://github.com/pbek/QOwnNotes/issues/842))
    - for more information please take a look at the [scripting documentation](https://www.qownnotes.org/scripting/methods-and-objects.html#read-the-current-word-from-the-note-text-edit)
- there now is a new scripting function `script.searchTagsByName()` to 
  fetches all tags by doing a substring search on the name field
  (for [#842](https://github.com/pbek/QOwnNotes/issues/842))
    - for more information please take a look at the [scripting documentation](https://www.qownnotes.org/scripting/methods-and-objects.html#search-for-tags-by-name)
- the script `@tag tagging in note text` was updated to use autocompletion
  (for [#842](https://github.com/pbek/QOwnNotes/issues/842))
- the Windows version of QOwnNotes is now built with Qt 5.10.0
- added more Czech, Italian, German, French, Catalan, Indonesian, Spanish,
  Dutch and Hungarian translation (thank you milazajicek, mlocati, rakekniven,
  bepolymathe, MGuerra, zmni, richarson and Fabijenna)

## 18.01.4
- you can now **print the selected text** in the note text edit as highlighted 
  text or as rendered markdown html (like in the preview)
- you can now **export the selected text** in the note text edit as highlighted 
  text or as rendered markdown html (like in the preview)
- added more German and Chinese translation (thank you bsapwindows)

## 18.01.3
- there now also is set a foreground color for code-blocks and inline-code in the
  preview so the code will be visible if the user uses a dark desktop theme but
  doesn't use the dark-mode preview colors yet (for [#830](https://github.com/pbek/QOwnNotes/issues/830))
- fixed the disabling of the markdown highlighting
- attempted to fix drawing problems in the note edit widget and possible 
  crashes in Windows (for [#826](https://github.com/pbek/QOwnNotes/issues/826)
  and [#717](https://github.com/pbek/QOwnNotes/issues/717))
- attempted to fix translations for the QOwnNotes snap
- QOwnNotes is now also built with Qt 5.7 to allow Windows XP users to 
  still use it (for [#828](https://github.com/pbek/QOwnNotes/issues/828))
    - although you have to seek out the latest `master` build from
      [AppVeyor](https://ci.appveyor.com/project/pbek/qownnotes/history) 
      yourself, choose the Qt 5.7 environment and download the artifact
- added more Chinese translation (thank you bsapwindows)

## 18.01.2
- attempted to fix drawing problems in the note edit widget and possible 
  crashes in Windows (for [#826](https://github.com/pbek/QOwnNotes/issues/826)
  and [#717](https://github.com/pbek/QOwnNotes/issues/717))
- the Windows release is now built with Qt 5.9.2
- attempted to fix some character-set issues with snaps (thank you @popey)
- added more Indonesian, French, German, Italian, Spanish, Brazilian Portuguese, 
  Catalan, Hungarian and Dutch translation (thank you zmni, bepolymathe,
  rakekniven, mlocati, UgoYak, mlimacarlos, MGuerra and Fabijenna)

## 18.01.1
- there now is a new checkbox in the *Editor settings* to fully highlight blockquotes
  instead of just the `>` (for [#822](https://github.com/pbek/QOwnNotes/issues/822))
- added more Indonesian, French, Spanish, Catalan, Hungarian and Dutch
  translation (thank you zmni, bepolymathe, richarson, MGuerra and Fabijenna)

## 18.01.0
- there now is a new setting `Only use editor width setting in distraction free mode`
  in the *Editor settings* that you can turn off so you can use the *Editor width*
  setting from the *Window menu* also if you are not in the distraction free mode
  (for [#814](https://github.com/pbek/QOwnNotes/issues/814))
- added more Indonesian, Spanish, Brazilian Portuguese, Catalan and Italian
  translation (thank you zmni, richarson, mlimacarlos, MGuerra and mlocati)

## 17.12.6
- added **Turkish translation** (a big thank you to omeratagun, realinfo,
  ihtiht and Apsimati)
    - join us at [QOwnNotes on Crowdin](https://crowdin.com/project/qownnotes/)
      to make QOwnNotes available in more languages or help with the current 
      translation
- added some preset text when opening a GitHub issue to share a new editor schema
- added more Indonesian, French, Spanish, Brazilian Portuguese, Polish, German,
  Catalan, Italian, Hungarian and Dutch translation (thank you zikraa,
  bepolymathe, richarson, mlimacarlos, lucjan, rakekniven, MGuerra, mlocati 
  and Fabijenna)

## 17.12.5
- there now is a new menu entry `Check for script updates` in the *Scripting*
  menu and the *Scripting settings* to open a dialog where you can update
  scripts that need an update (for [#792](https://github.com/pbek/QOwnNotes/issues/792))
- added more Spanish, Italian, Japanese, Catalan, German, Dutch and Hungarian
  translation (thank you richarson, mlocati, piyo, MGuerra, rakekniven and Fabijenna)

## 17.12.4
- fixed a possible problem with the settings dialog size for new installations
- moved the setting `Show all notes of a tag including the notes of their children-tags`
  in the *Panel settings* to the settings for the *Note panel*

## 17.12.3
- you now can choose to show all notes of a tag including the notes of their 
  children-tags (for [#137](https://github.com/pbek/QOwnNotes/issues/137))
    - there is a new setting `Show all notes of a tag including the notes of their children-tags`
      in the *Panel settings*
- a `width` attribute is now always added to images in the preview to make the 
  life of script developers easier (for [#808](https://github.com/pbek/QOwnNotes/issues/808))
- added more Spanish, Italian, Swedish, Dutch, Hungarian and French translation
  (thank you richarson, mlocati, druus, Fabijenna and bepolymathe)

## 17.12.2
- when formatting text with strikeout now leading and trailing spaces in the 
  selected text will be placed before and after the formatting markers to prevent 
  malformed markdown code (for [#767](https://github.com/pbek/QOwnNotes/issues/767))
- bold, italic or strikeout formatting can now be undone if the selected 
  text is surrounded by the corresponding formatter characters
  (for [#756](https://github.com/pbek/QOwnNotes/issues/756))
- encrypted notes are now shown decrypted when using `Open note in different window`
  if you previously entered the password (for [#805](https://github.com/pbek/QOwnNotes/issues/805))
- added more Catalan translation (thank you MGuerra)

## 17.12.1
- fixed the confirmation for removing the current workspace
  (for [#802](https://github.com/pbek/QOwnNotes/issues/802))
- changed the default note sort order back to "by date" after it was set to 
  "alphabetically" in a pull request for [#369](https://github.com/pbek/QOwnNotes/issues/369)
- the welcome dialog now detects if an *ownCloud* or *Nextcloud* sync folder is 
  already existing and suggests it for placing the notes folder in it
    - if none was present the default *Nextcloud* folder will be suggested
- added more German, Bengali, Brazilian Portuguese and French translation
  (thank you rakekniven, mrrifat1, mlimacarlos and bepolymathe)

## 17.12.0
- enabled text wrapping for the script settings (for [#792](https://github.com/pbek/QOwnNotes/issues/792))
- you can now also search for text in the note-preview with <kbd>Ctrl</kbd> + <kbd>F</kbd>
    - please visit [QOwnNotes shortcuts](https://www.qownnotes.org/getting-started/shortcuts.html)
      for a list of all shortcuts
- there is now a checkbox `Don't ask again!` for most confirmation message 
  boxes and a checkbox `Don't show again!` for most information message boxes
  to override the answer for all future prompts of that message boxes
  (for [#485](https://github.com/pbek/QOwnNotes/issues/485) and
  [#706](https://github.com/pbek/QOwnNotes/issues/706))
    - the message boxes will not be shown again until you reset the override
    - you can reset all overrides in the *General settings*
- added more German, Bengali, Spanish and French translation (thank you
  mrrifat1, richarson and bepolymathe)

## 17.11.6
- you can now enable debug output in release-builds of QOwnNotes
  (for [#759](https://github.com/pbek/QOwnNotes/issues/759))
- for headlines a whitespace after the last `#` is now mandatory
- you can now use the new scripting function `script.jumpToNoteSubFolder()`
  to jump to a note-subfolder in scripts (for [#792](https://github.com/pbek/QOwnNotes/issues/792))
    - for more information please take a look at the
      [scripting documentation](https://www.qownnotes.org/scripting/methods-and-objects.html#jumping-to-a-note-subfolder)
- added more Catalan, Italian, Spanish, Brazilian Portuguese and French translation
  (thank you MGuerra, mlocati, richarson, mlimacarlos and bepolymathe)

## 17.11.5
- changed `System tray` text in the *Interface settings* on macOS to `Menu bar`
  (for [#769](https://github.com/pbek/QOwnNotes/issues/769))
- expanded the valid list-characters by `- [ ] `, `- [x] `, `* [ ] `, `* [x] `,
  `+ [ ] ` and `+ [x] ` (for [#789](https://github.com/pbek/QOwnNotes/issues/789), thank you @sanderboom)
- auto-completion of list-items now also works when soft-wraps are enabled
  (for [#791](https://github.com/pbek/QOwnNotes/issues/791), thank you @sanderboom)
- there now is a new item in the *Edit menu* to allow note editing (on by default) 
  (for [#787](https://github.com/pbek/QOwnNotes/issues/787))
    - it can be toggled with a keyboard shortcut <kbd>Alt</kbd> + <kbd>⇧</kbd> + <kbd>E</kbd>
    - if turned off you cannot enter text into the note text edit and a lot 
      of menu- and toolbar-items to edit the note are turned off too
    - there also in a new button in the *Main toolbar* if you reset your toolbars 
- added more German, Catalan, Arabic, Spanish, French, Hungarian and Dutch
  translation (thank you MGuerra, sosozozo, richarson, bepolymathe and Fabijenna)

## 17.11.4
- the settings to show the system tray icon and to start the application hidden 
  if the system tray icon is enabled were moved from the main menu to the 
  *Interface settings* (for [#769](https://github.com/pbek/QOwnNotes/issues/769))
    - if the user enables the system tray icon the setting to only start one 
      instance of the application will also be enabled automatically if you are
      on Linux or Windows (macOS does that by itself)
- added more German, Catalan, Italian, Spanish, Brazilian Portuguese, French,
  Hungarian and Dutch translation (thank you rakekniven, MGuerra, mlocati,
  richarson, mlimacarlos, mcsa, bepolymathe and Fabijenna)
- there now is a software repository for **Fedora 27**
    - please visit the [Fedora installation page](https://www.qownnotes.org/installation#Fedora)
      for instructions

## 17.11.3
- added **Bengali translation** (a big thank you to mrrifat1!)
    - join us at [QOwnNotes on Crowdin](https://crowdin.com/project/qownnotes/)
      to make QOwnNotes available in more languages or help with the current 
      translation
- lists with `+` will now also continued when pressing <kbd>Return</kbd>
  (for [#782](https://github.com/pbek/QOwnNotes/issues/782), thank you @sanderboom)
- the default foreground and background colors for the note text-edit, that was
  set in the in the *Editor settings* now also work for other Markdown
  text-edits, like the *Debug panel*, the separate note-views and the
  *Debug settings* (for [#778](https://github.com/pbek/QOwnNotes/issues/778))
- the disabling of the setting `Show with full path` in the *Panel settings* now 
  also works under *Windows* (for [#777](https://github.com/pbek/QOwnNotes/issues/777))
- added more German, Catalan, Italian, Spanish, Brazilian Portuguese and
  Croatian translation (thank you rakekniven, MGuerra, soysexy03, mlocati,
  richarson, mlimacarlos and xpertmind)

## 17.11.2
- when formatting bold or italic now leading and trailing spaces in the selected
  text will be placed before and after the formatting markers to prevent 
  malformed markdown code (for [#767](https://github.com/pbek/QOwnNotes/issues/767))
- made some text changes in menus
- added a warning in the update service
- added more German, French, Italian, Spanish, Brazilian Portuguese, Hungarian
  and Dutch translation (thank you rakekniven, bepolymathe, mlocati, richarson,
  mlimacarlos and Fabijenna)

## 17.11.1
- in dark mode the background color of the search bar in the note text edit 
  will now also be adapted (if the desktop environment didn't already do it)
  (for [#768](https://github.com/pbek/QOwnNotes/issues/768))
    - there now also are new, darker colors for the *found* and *not found* 
      states when doing text searches in dark mode
- fixed a lot of link colors in the user interface in dark mode for desktop 
  environments that are forcing a to dark link color
  (for [#597](https://github.com/pbek/QOwnNotes/issues/597))
- if the setting to use a dark-mode system-tray icon is activated the 
  application-icon will also be set to the dark-mode icon
  (for [#770](https://github.com/pbek/QOwnNotes/issues/770))
    - the setting was renamed to `Enable dark mode application icon and tray icon`
- added more German, Catalan, Portuguese, Indonesian, Chinese, Italian, Spanish,
  Brazilian Portuguese, Hungarian and Dutch translation (thank you rakekniven,
  MGuerra, another1, zmni, rosatravels, mlocati, richarson, mlimacarlos and Fabijenna)

## 17.11.0
- list symbols like `*` and `-` will now only be removed in an empty line when
  pressing <kbd>Return</kbd> if there was a space-character after them
  (for [#763](https://github.com/pbek/QOwnNotes/issues/763))
- added another layer of error handling to the *Update checker* and more 
  error messages in case something went wrong (for [#759](https://github.com/pbek/QOwnNotes/issues/759))
    - be sure to take a look at the warnings in the *Log panel* if you want 
      more information
- added some warnings to the `startSynchronousProcess` scripting function and
  the 3rd parameter is now optional
- added some more information to the *External git log command* settings
- you can now use the new scripting function `script.getApplicationSettingsVariable()`
  to load application settings variables in scripts
    - for more information please take a look at the
      [scripting documentation](https://www.qownnotes.org/scripting/methods-and-objects.html#loading-application-settings-variables)
- added more French, Catalan, Portuguese, Hungarian and Dutch translation
  (thank you bepolymathe, MGuerra, manuelarodsilva and Fabijenna)

## 17.10.10
- prevention of focus-stealing of the update dialog is now only done on periodic
  checks instead of also on manual checks (for [#751](https://github.com/pbek/QOwnNotes/issues/751))
- the setting for note-subfolders to show all notes in a folder including their
  subfolders now also makes the note-search find items in all folders 
  (for [#757](https://github.com/pbek/QOwnNotes/issues/757))
- added more French, Spanish, Italian, Brazilian Portuguese, Hungarian, Dutch
  and German translation (thank you bepolymathe, richarson, mlocati, DevilLorde,
  Fabijenna and rakekniven)

## 17.10.9
- fixed the *New note* icon
- fixed the *Application arguments* debug dump
- there now is a new parameter `--session <name>` to start QOwnNotes in a
  different context for settings and internal files
- there now is a new setting for note-subfolders to show all notes in a folder 
  including their subfolders in the *Panels settings* (for [#757](https://github.com/pbek/QOwnNotes/issues/757))
    - if turned on the *All notes* item in the *Note subfolder* panel is 
      hidden because the root folder already shows all notes
- added a package for Ubuntu 18.04 (Bionic Beaver) to the Launchpad build process
- added more Italian, Brazilian Portuguese, French and German translation (thank
  you mlocati, mlimacarlos, bepolymathe and rakekniven)

## 17.10.8
- restructured the main menu to make it less cluttered
- there now is a new checkbox in the *Todo settings* to close the todo list 
  after saving an item (for [#753](https://github.com/pbek/QOwnNotes/issues/753))
- added more Catalan, Spanish, Italian, Brazilian Portuguese, Hungarian, Dutch
  and German translation (thank you MGuerra, richarson, mlocati, mlimacarlos,
  Fabijenna and rakekniven)

## 17.10.7
- added new search engines *Qwant* and *Startpage* and reordered the list in 
  the *General settings* (for [#661](https://github.com/pbek/QOwnNotes/issues/661))
- tried to prevent some occasions of focus stealing of the automatic update 
  dialog (for [#751](https://github.com/pbek/QOwnNotes/issues/751))
- added more Catalan, Swedish, Dutch, Hungarian and French translation (thank
  you davidse, druus, Fabijenna and bepolymathe)

## 17.10.6
- you can now select different search engines for searching text in notes in the
  *General settings* (for [#661](https://github.com/pbek/QOwnNotes/issues/661), 
  thank you @ron-popov)
- added an information text to the Windows update dialog to remind the user that
  QOwnNotes needs to run from a location where the user has write access to for 
  the automatic update to work (for [#750](https://github.com/pbek/QOwnNotes/issues/750))
- added more French and German translation (thank you bepolymathe)

## 17.10.5
- highlighting of ordered lists in the note edit panel now works above `9.`
  (for [#746](https://github.com/pbek/QOwnNotes/issues/746))
- fixed links to notes whose names only consist of numbers when `Ctrl`-clicking
  them in the note text edit, in the preview there is currently no way to make
  them work (for [#748](https://github.com/pbek/QOwnNotes/issues/748))
- did some dialog-text changes
- added more Polish, Catalan, Italian, Brazilian Portuguese, Dutch, Hungarian
  and German translation (thank you lucjan, MGuerra, mlocati, mlimacarlos,
  Fabijenna and rakekniven)

## 17.10.4
- fixed that a 2nd note is created if the non-automatic headlines for new notes
  are enabled (for [#740](https://github.com/pbek/QOwnNotes/issues/740))
- added more Spanish translation (thank you richarson)

## 17.10.3
- you now can enable in the *General settings* that the shortcut to create new 
  notes asks for a headline of the note instead of generating the headline 
  automatically (for [#740](https://github.com/pbek/QOwnNotes/issues/740))
- there now is a new scripting command `script.inputDialogGetText` to open an
  input dialog with a line edit (for [#740](https://github.com/pbek/QOwnNotes/issues/740))
    - for more information please take a look at
      [Opening an input dialog with a line edit](https://www.qownnotes.org/scripting/methods-and-objects.html#opening-an-input-dialog-with-a-line-edit)
- added more French and German translation (thank you bepolymathe and rakekniven)

## 17.10.2
- fixed that the automatic closing of brackets is only allowed if the cursor was
  at the end of a block (was removed by a pull request)
- the `updater.bat` in Windows now kills the `QOwnNotes.exe` process when the
  application is updated because it doesn't seem to end in portable mode
  (for [#376](https://github.com/pbek/QOwnNotes/issues/376))
- added more French and Spanish translation (thank you bepolymathe and richarson)

## 17.10.1
- the automatic update dialog will now also be disable per default for 
  repositories and self-builds if the settings dialog was opened in the 
  welcome dialog at first application startup
  (for [#732](https://github.com/pbek/QOwnNotes/issues/732))
- there now is a `--clear-settings` parameter for the application that clears
  the settings before running the application
- there now is a `--help` parameter for the application that show the 
  available command line arguments of QOwnNotes 

## 17.10.0
- fixed a crash with empty note-subfolders and manual reloading of the note
  folder (for [#731](https://github.com/pbek/QOwnNotes/issues/731))
- tried to remedy some inconsistency in the margins of the *distraction free mode*
  (for [#727](https://github.com/pbek/QOwnNotes/issues/727))
- made more text selectable and changed the sort-order in the
  *Script repository* widget
- added the experimental script `@tag tagging in note text` to the script repository
  to make it easier to play around with the external note tagging support via the
  scripting engine (for [#530](https://github.com/pbek/QOwnNotes/issues/530))

## 17.09.9
- the note text edit margins will now be updated when the font size increased
  or decreased in the *distraction free mode*
  (for [#727](https://github.com/pbek/QOwnNotes/issues/727))
- improvements of the external note tagging support via the scripting engine
  (for [#530](https://github.com/pbek/QOwnNotes/issues/530))
    - fixed crash when removing more than one tag that have notes linked to them
- when inserting a link into a note some more broken characters from the 
  webpage's html-title will be replaced
- added more German translation (thank you rakekniven)

## 17.09.8
- fixed the upstream RTL bug in the note text edit
  (for [#714](https://github.com/pbek/QOwnNotes/issues/714))
- improvements of the external note tagging support via the scripting engine
  (for [#530](https://github.com/pbek/QOwnNotes/issues/530))
    - fixed the problem where notes were overwritten by other notes in bulk operations
    - fixed a lot of occasions where not all notes were handled in bulk operations
    - fixed some random crashes with bulk operations and when the note tag tree reloads
    - fixed some random crashes, updating of the current note and occasions 
      where not all notes were handled when renaming tags
- fixed increasing and decreasing of code font-size via keyboard shortcut 
  (for [#724](https://github.com/pbek/QOwnNotes/issues/724))
- when inserting a link into a note some more broken characters from the 
  webpage's html-title will be replaced
- added more Hungarian and French translation (thank you Fabijenna and sagoum.kakoum)

## 17.09.7
- underline bracket closing is temporary disabled because underlines are used
  in different contexts
- improvements of the external note tagging support via the scripting engine
  (for [#530](https://github.com/pbek/QOwnNotes/issues/530))
	- improved ui feedback when adding and removing tags in the current note
	- tagging of multiple notes works now but is still a bit wacky because of
	  race conditions with files that are getting changed all around (notes 
	  and note database)
		- the same goes for removing multiple notes
- added more Hungarian translation (thank you Fabijenna)

## 17.09.6
- added the first alpha version of external note tagging support via the 
  scripting engine (for [#323](https://github.com/pbek/QOwnNotes/issues/323),
  [#338](https://github.com/pbek/QOwnNotes/issues/338),
  [#379](https://github.com/pbek/QOwnNotes/issues/379),
  [#472](https://github.com/pbek/QOwnNotes/issues/472),
  [#530](https://github.com/pbek/QOwnNotes/issues/530) and
  [#603](https://github.com/pbek/QOwnNotes/issues/603))
    - as soon as a script is activated that implements the new function
      `noteTaggingHook` note tagging will be handled by that function
    - please install the script [note-tagging.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/note-tagging.qml)
      to test the basic functionality and report in [#530](https://github.com/pbek/QOwnNotes/issues/530).
        - your internal tag database and your notes might get modified 
        - don't use this script on your production note folder, please make a 
          copy of it and test it there!
    - following features should already work via the QOwnNotes user interface
        - initially importing tags like `@tag` from your notes and overwriting
          your current tag assignment
            - you will not loose your tags tree, just the former assignment to notes
            - you can still move tags into other tags
            - if more than one tag has the same name in your tag tree the 
              first hit will be assigned
        - adding a tag to a note will add the tag to the note text        
        - removing a tag from a note will remove the tag from the note text
        - removing of tags in the tag list will remove those tags from your notes
        - renaming of tags in the tag list will rename those tags in your notes
        - bulk tagging of notes in the note list will add those tags to your notes
        - bulk removing of tags from notes in the note list will remove those 
          tags from your notes
    - for more information please take a look at the [scripting documentation](https://www.qownnotes.org/scripting/methods-and-objects.html#notetagginghook)

## 17.09.5
- added support for entering a bracket-character that surrounds the currently 
  selected text (for [#719](https://github.com/pbek/QOwnNotes/issues/719), thank you @sanderboom)
- added more Catalan and Hungarian translation (thank you MGuerra and Fabijenna)
- added two new scripting-methods `addTag` and `removeTag` to `NoteApi` to 
  add and remove tags from notes (for [#530](https://github.com/pbek/QOwnNotes/issues/530))
    - custom tagging could now be implemented by parsing the note text for 
      custom tag handlers and adding/removing tags from the note
    - for more information please take a look at the [scripting documentation](https://www.qownnotes.org/scripting/methods-and-objects.html#note)

## 17.09.4
- fixed a possible crash when selecting a lot of notes quickly in succession 
  and improved speed of selecting notes in quick in succession
  (for [#717](https://github.com/pbek/QOwnNotes/issues/717))
- apostrophe bracket closing is temporary disabled because apostrophes are used
  in different contexts (for [#708](https://github.com/pbek/QOwnNotes/issues/708))
- added more French, Indonesian, Dutch and Hungarian translation (thank you 
  bepolymathe, zmni and Fabijenna)

## 17.09.3
- added improvements to not start new list items when not in a list
  (for [#712](https://github.com/pbek/QOwnNotes/issues/712), thank you @sanderboom)
- added improvements for the auto-completion of `~` and `_`
  (for [#713](https://github.com/pbek/QOwnNotes/issues/713), thank you @sanderboom)
- moved the setting of the *width of the cursor in the note-edit* from the 
  *Interface settings* to the *Editor settings*
- added more Brazilian Portuguese, Russian, Italian, Chinese, German and Spanish
  translation (thank you mlimacarlos, Maboroshy, mlocati, Libnux, rakekniven and 
  richarson)

## 17.09.2
- the scroll position of the note-preview is now also synced when clicking on
  headlines in the navigation panel (for [#710](https://github.com/pbek/QOwnNotes/issues/710))
- you can now set the width of the cursor in the note-edit in the *Interface settings*
  (for [#710](https://github.com/pbek/QOwnNotes/issues/710))
- the automatic closing of brackets now also works for `_` and `~`
  (for [#708](https://github.com/pbek/QOwnNotes/issues/708), thank you @sanderboom)
- added more Brazilian Portuguese, Catalan, Italian, Japanese and Spanish
  translation (thank you mlimacarlos, MGuerra, mlocati, piyo and richarson)

## 17.09.1
- the `QMarkdownTextedit`, which is used in all markdown text-edits is now based
  on`QPlainTextEdit` instead of `QTextEdit`, which should result in **improved
  rendering speed** for large notes
    - this will improve the note text edit, log panel, version dialog, trash 
      dialog, todo dialog, settings dialog, note dialog and note history dialog
- you can now choose different **interface styles** in the *Interface settings* 
  to change the look of the application's interface and its controls
    - the style will be previewed instantly if you select another style
    - if you select `Automatic` you will be prompted to restart the application
- the snap-packaged version of QOwnNotes will now automatically use the `Fusion`
  interface style, which dramatically improves the styling of the user interface
- the automatic closing of brackets now also works for `'`
  (for [#708](https://github.com/pbek/QOwnNotes/issues/708))
- added more Brazilian Portuguese, Catalan, German and Spanish translation 
  (thank you mlimacarlos, MGuerra and richarson)

## 17.09.0
- margins for headlines in the note editor are now disabled, because Qt bugs 
  are causing multiple problems with them
    - they prevent "undo" in headlines
      (for [#520](https://github.com/pbek/QOwnNotes/issues/520))
    - they generate invisible lines at the end of a note
      (for [#667](https://github.com/pbek/QOwnNotes/issues/667))
    - the cause a crash when reaching the invisible lines when highlighting of
      the current line is turned on (for [#701](https://github.com/pbek/QOwnNotes/issues/701))
- improved background color of current line highlighting for the schemas *Dark*,
  *AnalyticalYellow*, *Bépolymathe Colorize* and *Mountain*
  (for [#701](https://github.com/pbek/QOwnNotes/issues/701))
- added more Russian, Catalan, German, Italian and French translation (thank 
  you Maboroshy, MGuerra, rakekniven, mlocati and bepolymathe)

## 17.08.11
- the shortcuts for restoring workspaces will now be correctly restored after restart
  (for [#697](https://github.com/pbek/QOwnNotes/issues/697))
- the **current line** will now **highlighted** in the editor
  (for [#701](https://github.com/pbek/QOwnNotes/issues/701))
	- you can turn that feature off in the *Editor settings*
	- the background color can be selected as part of the theming in the
	 *Editor fonts & colors settings*
		- a sane default color was chosen for each of the current schemes
- added more Russian, Spanish, Croatian, Indonesian, Brazilian Portuguese,
  Swedish, Catalan and Chinese translation (thank you Maboroshy, richarson,
  davidsraba2, zmni, mlimacarlos, druus, MGuerra and chonghua)

## 17.08.10
- fixed a problem with the note history when notes with the same name were 
  present in different note subfolders
- added a fix by @SmartBlug for the issue of jumping cursors for some users
  (for [#689](https://github.com/pbek/QOwnNotes/issues/689))
- added more French, Indonesian, German and Italian translation (thank you 
  bepolymathe, zmni, rakekniven and mlocati)

## 17.08.9
- added **Indonesian translation** (a big thank you to zmni!)
    - join us at [QOwnNotes on Crowdin](https://crowdin.com/project/qownnotes/)
      to make QOwnNotes available in more languages or help with the current 
      translation
- improved highlighting for Rmarkdown comments like `[Comment]: # (text)`
  (for [#690](https://github.com/pbek/QOwnNotes/issues/690))
- some text changes in the settings dialog were done
- added more German translation (thank you rakekniven)

## 17.08.8
- fixed some highlighting-issues with HTML comments
- added highlighting for Rmarkdown comments like `[Comment]: # (text)`
  (for [#690](https://github.com/pbek/QOwnNotes/issues/690))
- added a fix by @SmartBlug for the sporadic cursor-jumping to the top in the
  note edit (for [#689](https://github.com/pbek/QOwnNotes/issues/689))
- added more Italian and Chinese translation (thank you mlocati, Libnux and enyie)

## 17.08.7
- old notes that were automatically renamed will now only be removed if the new
  note was stored successfully (for [#682](https://github.com/pbek/QOwnNotes/issues/682))
- if a note gets renamed automatically because the note title changed it will
  now be checked if the note can actually be stored to the note folder before
  really storing it to be able to attempt to replace additional characters in
  the filename before storing it again in case the note is stored on a
  filesystem with harsher filename restrictions
  (for [#682](https://github.com/pbek/QOwnNotes/issues/682) and
  [#683](https://github.com/pbek/QOwnNotes/issues/683))
    - if the note still cannot be stored to disk the 2nd time the error will be 
      logged to the log panel like it already was before
- added a new editor color schema *Mountain* by @bepolymathe
  (for [#680](https://github.com/pbek/QOwnNotes/issues/680))
- added a new editor color schema *Arc* by @johanson
  (for [#687](https://github.com/pbek/QOwnNotes/issues/687))
- added more French, Italian, German, Chinese and Spanish translation
  (thank you bepolymathe, mlocati, rakekniven, enyie and richarson)
- the `Delete line` function now *pulls the text from the bottom up*
  (for [#432](https://github.com/pbek/QOwnNotes/issues/432))

## 17.08.6
- if you want to remote control QOwnNotes with sockets you can use `WebSocketServer` in a script
    - for more information please take a look at the [scripting documentation](https://www.qownnotes.org/scripting/methods-and-objects.html#working-with-websockets)
- you can now also search for filenames of notes in the note search line-edit, 
  instead of just for note-text (for [#678](https://github.com/pbek/QOwnNotes/issues/678))
- you now can search for notes in all tags / subfolders with the new main menu 
  entry `Find notes in all tags / subfolders` (shortcut <kbd>Alt + F</kbd>)
  (for [#671](https://github.com/pbek/QOwnNotes/issues/671))
    - the menu entry `Find notes in all subfolders` was removed as a consequence
    - please visit [QOwnNotes shortcuts](https://www.qownnotes.org/getting-started/shortcuts.html)
      for a list of all shortcuts
- added more French and Italian translation (thank you bepolymathe and mlocati)

## 17.08.5
- added a new editor color schema *Bepolymathe Colorize*
  (for [#675](https://github.com/pbek/QOwnNotes/issues/675), thank you @bepolymathe)
- double clicking on items in the *note subfolder tree* and *tag tree* now 
  causes the tree items to expand and collapse instead of editing them
  (for [#669](https://github.com/pbek/QOwnNotes/issues/669))
    - there now is a new menu entry `Rename subfolder` in the note subfolder
      tree to rename subfolders
- you can now use the new scripting functions `script.setPersistentVariable()`
  and `script.getPersistentVariable()` to store and load persistent variables in 
  scripts (for [#677](https://github.com/pbek/QOwnNotes/issues/677))
    - for more information please take a look at the
      [scripting documentation](https://www.qownnotes.org/scripting/methods-and-objects.html#storing-and-loading-persistent-variables)
- added more French and Swedish translation (thank you bepolymathe and druus)

## 17.08.4
- the order of the scripts in the *Script settings* now really adjusts which
  script is executed first (for [#663](https://github.com/pbek/QOwnNotes/issues/663))
- a warning with error code and error message will now be printed to the log 
  panel if a network error occurs when talking to your ownCloud / Nextcloud 
  server (for [#664](https://github.com/pbek/QOwnNotes/issues/664))
- attempted to fix the reminder date in todo list items for newer versions of
  Nextcloud (for [#664](https://github.com/pbek/QOwnNotes/issues/664))

## 17.08.3
- you can now reorder your scripts in the *Script settings* via drag and drop
  to adjust which scripts should be executed first
  (for [#663](https://github.com/pbek/QOwnNotes/issues/663))
- the scripting function `noteToMarkdownHtmlHook` can now be used in multiple
  scripts to modify the html of the preview
  (for [#663](https://github.com/pbek/QOwnNotes/issues/663))
- added more French, Italian, Brazilian Portuguese and Spanish translation
  (thank you bepolymathe, mlocati, mlimacarlos and richarson)

## 17.08.2
- added **Swedish translation** (a big thank you to druus!)
    - join us at [QOwnNotes on Crowdin](https://crowdin.com/project/qownnotes/)
      to make QOwnNotes available in more languages or help with the current 
      translation
- the `Delete line` function now only deletes the text to the beginning of 
  the line and not the whole line to mimic the behavior of other text editors
  (for [#432](https://github.com/pbek/QOwnNotes/issues/432))
- if <kbd>Tab</kbd> is pressed while adding a tag to the current note the tag 
  that starts with the currently entered text will now be added
  (for [#658](https://github.com/pbek/QOwnNotes/issues/658))
- when `Show in system tray` will be checked in the *Window menu* then the 
  user will now receive a notification if `Allow only one app instance` isn't 
  enabled in the settings (for [#660](https://github.com/pbek/QOwnNotes/issues/660))
- added more Catalan and German translation (thank you MGuerra)

## 17.08.1
- fixed linking of notes in the link dialog
  (for [#646](https://github.com/pbek/QOwnNotes/issues/646))
- fixed the colors in the editor color schema *Bépolymathe Light* 
  (for [#648](https://github.com/pbek/QOwnNotes/issues/648))
- added more Catalan translation (thank you MGuerra)

## 17.08.0
- added a new editor color schema *Bepolymathe Light* (thank you @bepolymathe)
  (for [#648](https://github.com/pbek/QOwnNotes/issues/648))
- when pasting html from the clipboard as markdown null characters and Windows
  line breaks will now be removed as well as all tags will be interpreted over
  multiple lines (for [#645](https://github.com/pbek/QOwnNotes/issues/645))
- added more Catalan translation (thank you MGuerra)

## 17.07.9
- fixed a possible crash when using scripts with custom actions in the note
  list context menu when the script engine is reloaded and the context menu
  is opened (for [#490](https://github.com/pbek/QOwnNotes/issues/490))
- there now is a new scripting command `script.inputDialogGetItem` to open an
  input dialog with a select box (for [#490](https://github.com/pbek/QOwnNotes/issues/490))
    - for more information please take a look at
      [Opening an input dialog with a select box](https://www.qownnotes.org/scripting/methods-and-objects.html#opening-an-input-dialog-with-a-select-box)
- when entering an url in the link-dialog with <kbd>Ctrl + L</kbd> that is 
  missing a`://` then `http://` will automatically added in front of the url
  (for [#646](https://github.com/pbek/QOwnNotes/issues/646))
- added more Catalan and Spanish translation (thank you MGuerra and richarson)

## 17.07.8
- there now is a new scripting command `script.selectedNotesPaths()` to return a
  list of the paths of all selected notes 
  (for [#490](https://github.com/pbek/QOwnNotes/issues/490))
    - for more information please take a look at
      [Getting a list of the paths of all selected notes](https://www.qownnotes.org/scripting/methods-and-objects.html#getting-a-list-of-the-paths-of-all-selected-notes)
- scripting function `registerCustomAction` now has a new parameter to add 
  a menu entry to the note list context menu
    - for more information please take a look at the 
      [registerCustomAction documentation](https://www.qownnotes.org/scripting/methods-and-objects.html#registering-a-custom-action)
- there is a new script *External note diff* in the script repository, that 
  uses above new features and adds adds a menu entry to the context menu of the
  note list to **diff selected notes** in an external diff program 
    - the path of the diff program can be selected in the script settings of 
      the script
- added more Catalan, French, Italian, Brazilian Portuguese and German
  translation (thank you MGuerra, bepolymathe, mlocati and mlimacarlos)

## 17.07.7
- added **Catalan translation** (a big thank you to MGuerra!)
    - join us at [QOwnNotes on Crowdin](https://crowdin.com/project/qownnotes/)
      to make QOwnNotes available in more languages or help with the current 
      translation
- fixed more problems with resetting the mouse cursor after pressing <kbd>Ctrl</kbd> 
  for Windows (for [#637](https://github.com/pbek/QOwnNotes/issues/637))
- fixed an issue with the highlighting of special cases of the references of 
  reference links (for [#643](https://github.com/pbek/QOwnNotes/issues/643))
- added paragraph recognition for the pasting of html-code
  (for [#643](https://github.com/pbek/QOwnNotes/issues/643), thank you @vanleo2001)
- added more Dutch and German translation (thank you Fabijenna and rakekniven)

## 17.07.6
- fixed resizing of too large images under Windows in the preview and the 
  PDF export (for [#639](https://github.com/pbek/QOwnNotes/issues/639))
- the mouse-cursor will now be set to a beam-cursor when pressing <kbd>Ctrl + R</kbd> 
  to reset the pointing-hand-cursor that was set by pressing <kbd>Ctrl</kbd>
  (for [#637](https://github.com/pbek/QOwnNotes/issues/637))
- *Show root folder name* and *Display note subfolders as full tree* in the
  *Panels settings* are now set by default
  (for [#612](https://github.com/pbek/QOwnNotes/issues/612))
- replacing multiple line breaks when pasting html into a note with
  `Ctrl + Shift + V` (for [#634](https://github.com/pbek/QOwnNotes/issues/634))
- added more French and German translation (thank you Loelo and rakekniven)

## 17.07.5
- fixed a problem with correctly storing the note text when special note text
  changes were made between the note save intervals
  (for [#635](https://github.com/pbek/QOwnNotes/issues/635))

## 17.07.4
- the mouse-cursor will now be set to a beam-cursor when pressing <kbd>Ctrl + F</kbd> 
  to reset the pointing-hand-cursor that was set by pressing <kbd>Ctrl</kbd>
  (for [#637](https://github.com/pbek/QOwnNotes/issues/637))
- under Windows the selection background color in the note text edit is now set 
  to a light blue if not in dark mode to prevent visibility problems when
  searching text (for [#636](https://github.com/pbek/QOwnNotes/issues/636))
- added more Polish, Spanish, Italian, Brazilian Portuguese, Hungarian and
  German translation (thank you lucjan, polkillas, smart2128, mlimacarlos,
  vaganyik and jmbm)

## 17.07.3
- when opening the todo list dialog from the system tray the main window of 
  QOwnNotes will be brought to the foreground too to prevent crashes under
  Windows (for [#630](https://github.com/pbek/QOwnNotes/issues/630))
- dialog windows are now maximized if it looks like that they don't fit on
  the current screen the first time they were opened
  (for [#631](https://github.com/pbek/QOwnNotes/issues/631))
- you now get a warning if you try to rename a note in the note list to enable
  the option to allow the note filename to be different from the headline if 
  you haven't allowed it yet (for [#613](https://github.com/pbek/QOwnNotes/issues/613))
    - there is a button to directly open the *General settings* in the 
      warning dialog
- files are now only downloaded if the status code of the request was `success`
  to prevent downloaded files with error pages
  (for [#627](https://github.com/pbek/QOwnNotes/issues/627))
- when downloading files a proper filename suffix will be set by detecting the
  mime type of the file (for [#627](https://github.com/pbek/QOwnNotes/issues/627))
- added more German translation (thank you rakekniven)

## 17.07.2
- more problematic characters will be removed from the filename-suffix of 
  downloaded media files to make sure the file can be stored locally
  (for [#627](https://github.com/pbek/QOwnNotes/issues/627))
- added more French translation (thank you lepascalou)

## 17.07.1
- **QOwnNotesAPI**, the ownCloud / Nextcloud API application for QOwnNotes
  is now available in the ownCloud Marketplace
    - you can find it at the [QOwnNotesAPI Marketplace page](https://marketplace.owncloud.com/apps/qownnotesapi)
- there now is a new *Editor color schema* **Material Dark** by @UltimateRandom
  (for [#626](https://github.com/pbek/QOwnNotes/issues/626), a big thank you)
- added more Italian translation (thank you smart2128)

## 17.07.0
- there now is a repository with QOwnNotes built as **AppImage** for 32 and 64 
  bit systems (for [#534](https://github.com/pbek/QOwnNotes/issues/534))
    - please visit the [AppImage installation page](https://www.qownnotes.org/installation#AppImage)
      for instructions
    - if you run QOwnNotes from the AppImage the automatic updater will be 
      enabled by default and you will be led the the AppImage download page if 
      there is a new version of the application available
- fixed a problem in the desktop file for all openSUSE builds
- added more Polish, Spanish, Croatian, Italian, Brazilian Portuguese, Hungarian
  and German translation (thank you lucjan, asoliverez, richarson, xpertmind,
  mlocati, mlimacarlos, vaganyik, jmbm and rakekniven)

## 17.06.6
- there are now new context menu entries in the note list
  (for [#613](https://github.com/pbek/QOwnNotes/issues/613))
    - a `New note` menu entry to create a new note
    - a `Rename note` menu entry to rename the note if you allowed the note 
      filename to be different than the headline in the *General settings*
- the `Edit tag` context menu entry in the tag tree was renamed to `Rename tag`
- there now is a software repository for **Debian Linux 9.0**
    - please visit the [Debian installation page](https://www.qownnotes.org/installation#Debian)
      for instructions
- there now is a software repository for **Fedora 26**
    - please visit the [Fedora installation page](https://www.qownnotes.org/installation#Fedora)
      for instructions
- there now is a software repository for **openSUSE Leap 42.3**
    - please visit the [openSUSE installation page](https://www.qownnotes.org/installation#openSUSE)
      for instructions
- added more Japanese and German translation (thank you piyo and jmbm)

## 17.06.5
- the first line of the note is now not modified when the note file gets 
  automatically renamed because of harmful or unwanted characters
  (for [#619](https://github.com/pbek/QOwnNotes/issues/619))
    - this doesn't seem to trouble ownCloud / Nextcloud notes a lot, but it
      renames the notes to its own liking
    - so you could use `# ` at the beginning of your note and it will only be
      removed in the note file name (unless you allow the note file name to 
      be different from the headline in the *General settings*)
    - if you want more control over your note file name you can achieve that 
      by writing a script, please take a look at
      [handleNoteTextFileNameHook](https://www.qownnotes.org/scripting/methods-and-objects.html#handlenotetextfilenamehook)
- added more Polish, Spanish and German translation (thank you lucjan,
  richarson and rakekniven)

## 17.06.4
- if you now add a local script in the *Scripting settings* you immediately get
  the dialog to select the script
- there now is a new script variables datatype `boolean`
    - to get information about how to use it please take a look at the example
      [variables.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/variables.qml)
    - there is a new script `Weather stats` in the *Script repository* that 
      is using this new setting
- added more French translation (thank you lepascalou)

## 17.06.3
- you can now open the *Script repository dialog* with the Shortcut
  <kbd>Alt</kbd> + <kbd>⇧</kbd> + <kbd>I</kbd>
- if you now try to install a script in the *Script repository* that doesn't 
  support your platform you will get a dialog to cancel the installation
  (for [Script repository issue #3](https://github.com/qownnotes/scripts/issues/3))
- if you now try to install a script in the *Script repository* that needs a 
  newer version of QOwnNotes you will get a notification to update the application
- a problem with storing script settings if there is more than one setting was
  fixed (for [#605](https://github.com/pbek/QOwnNotes/issues/605))
- the MainWindow is now exposed for scripting as variable `mainWindow`
  (for [#604](https://github.com/pbek/QOwnNotes/issues/604))
    - you can use `mainWindow.buildNotesIndexAndLoadNoteDirectoryList(true, true)`
      to force a reload of the note list
    - for more information please take a look at the
      [scripting documentation](https://www.qownnotes.org/scripting/methods-and-objects.html#mainwindow)
- added more Italian translation (thank you mlocati)

## 17.06.2
- there are now new properties for notes in the scripting engine `fileCreated`
  and `fileLastModified` (for [#599](https://github.com/pbek/QOwnNotes/issues/599))
    - you can use the methods from [Date](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Date)
      to work with these date properties
    - for more information please take a look at the
      [scripting documentation](https://www.qownnotes.org/scripting/methods-and-objects.html#note)
- when posting a script to the *Script repository* you can now define which 
  platforms are supported by the script with the new `platforms` parameter
  (for [Script repository issue #3](https://github.com/qownnotes/scripts/issues/3))
    - for an example please take a look at
      [info.json](https://github.com/qownnotes/scripts/blob/master/example-script/info.json)
    - this information will be viewed in the *Script repository* dialog
    - if the platform you are running QOwnNotes on is not supported by the script
      the item will be colored grey in the script list of the *Script repository*
- added more Spanish and Brazilian Portuguese translation (thank you
  richarson and mlimacarlos)

## 17.06.1
- the Evernote import now supports importing file attachments with the 
  correct placement in the note (for [#552](https://github.com/pbek/QOwnNotes/issues/552))
    - the placement of imported images was also improved
- added more Italian and German translation (thank you mlocati and bigant)

## 17.06.0
- you can now cycle through text cases of the selected text instead of just 
  toggling uppercase and lowercase with the shortcut <kbd>Ctrl</kbd> +
  <kbd>⇧</kbd> + <kbd>U</kbd> (for [#396](https://github.com/pbek/QOwnNotes/issues/396))
    - you now can also use *proper case* and *sentence case*
    - this was superbly done by @quailiff, a big thank you
- more styling for links and `<kbd>` tags in various labels and text-browsers 
  was now done to fix problems with operating systems that don't set default
  colors fitting to be used in dark mode
  (for [#597](https://github.com/pbek/QOwnNotes/issues/597))
- added more Italian, Brazilian Portuguese and French translation (thank you
  mlocati, mlimacarlos and lepascalou)
    
## 17.05.14
- when moving or copying notes to another note subfolder tagging information
  will now be restored (for [#595](https://github.com/pbek/QOwnNotes/issues/595))
- you can now use the shortcut <kbd>Ctrl</kbd> + <kbd>⇧</kbd> + <kbd>E</kbd> to 
  insert a file attachment into the current note
  (for [#419](https://github.com/pbek/QOwnNotes/issues/419))
    - the file will be copied to an `attachments` folder in the current note 
      folder (note subfolders don't have their own `attachments` folder) and 
      a text link will be created in the note
    - the new `attachments` folder will be ignored in the note subfolder tree
    - you can also drag and drop files into the note, they will be inserted as
      attachment if they are not an image or a note file
    - a space character will be added in front of the attachment link if 
      there was no space or new line in front of the current cursor position
- the Evernote import now optionally supports importing file attachments
  (for [#552](https://github.com/pbek/QOwnNotes/issues/552))
    - attachments in enex files that don't contain correct information about 
      where they are placed inside the text are imported at the end of the text

## 17.05.13
- the Evernote import now supports tagging
  (for [#592](https://github.com/pbek/QOwnNotes/issues/592))
- added more Italian, Brazilian Portuguese and Spanish translation (thank you
  mlocati, mlimacarlos and richarson)

## 17.05.12
- a lot of work was done by @derboblan for the note list, tag tree and note
  subfolder panel, a big thank you for that
  (for [#369](https://github.com/pbek/QOwnNotes/issues/369))
	- there now is a new page *Panels* in the *Settings dialog*
	- you can configure the sort order of the note list, tag tree and note
	  subfolder panel
	- sorting tags by last use is now possible
	- counting used tags in sub folders was also fixed
	- there are options to hide the search bar for tags and note subfolders
	- for note subfolders there are options to show the root folder name, to
	  display the note subfolders as tree with a root element and to show the
	  full path in the tree

## 17.05.11
- the full integer range of values can now be selected in the number selector
  in script variables in the *Scripting settings*
  (for [#536](https://github.com/pbek/QOwnNotes/issues/536))
- fixes in the `QOwnNotes.desktop` file were made by @chronitis (thanks a lot)
- added more Hungarian translation (thank you vaganyik)

## 17.05.10
- fixed duplication of script settings variables in the in the *Scripting
  settings* (for [#536](https://github.com/pbek/QOwnNotes/issues/536))
- moved the qml path settings above the script variable settings in the
  *Scripting settings*
- added more Italian, Brazilian Portuguese and Dutch translation (thank you
  mlocati, mlimacarlos and dmattijssen)

## 17.05.9
- added information about enabled scripts to the *Debug settings*
- now information about the currently installed version of the script will be 
  shown in the *Script repository* if the script is already installed
- you can now also reinstall or update your scripts in the *Script repository*
- the user interface of the *Scripting settings* was adapted to make the 
  entering of script settings more clear
- fixed a problem with automatically setting the name of a local script in 
  the *Scripting settings*
- added more German, Brazilian Portuguese and Spanish translation (thank you
  bigant, mlimacarlos and richarson)

## 17.05.8
- instead of importing `com.qownnotes.noteapi` and `com.qownnotes.tagapi` in 
  your scripts you now only need to import `QOwnNotesTypes`
- there now are new scripting functions `fromNativeDirSeparators`,
  `toNativeDirSeparators` and `dirSeparator` to work with native file paths
    - for more information please take a look at the
      [scripting documentation](https://www.qownnotes.org/scripting/methods-and-objects.html)
- you can now use the property list `resources` in your `info.json` in the
  *Script repository* to specify further files that are present in your script's
  directory and that should be downloaded to be used by your script
    - you can use this for example for other scripts you want to run or for 
      icons in custom actions
    - for example code please take a look at
      [selected-markdown-to-bbcode](https://github.com/qownnotes/scripts/tree/master/selected-markdown-to-bbcode)
- if you register a `property string scriptDirPath;` in your script the 
  property will be filled with the path to the directory where your script is 
  located
    - you can use this for example to access other files that are located in 
      your script's directory
- added more Italian and French translation (thank you mlocati and lepascalou)

## 17.05.7
- tried to fix a design glitch with the `Add script` button menu in the 
  script settings on non-highdpi displays
- you can now **register settings variables** in **scripts**
    - this allows you to let the user configure your script in the script 
      settings
    - currently there are settings types for `string`, `text`, `integer` and 
      `file`
    - take a look at the example at
      [variables.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/variables.qml)
      to understand how this all will work
    - a lot of new useful scripts were added to the script repository, which 
      now can be configured with the script settings variables
- scripts in the script repository can now use the `minAppVersion` property 
  to state what the minimum version of QOwnNotes has to be for the script to run
    - for an example please take a look at
      [info.json](https://github.com/qownnotes/scripts/blob/master/example-script/info.json)
- there now are two new menu entries in the main menu to open the script 
  settings and to search for scripts in the script repository
- you can now print your note preview or export it as pdf even if the preview
  is turned off (for [#587](https://github.com/pbek/QOwnNotes/issues/587))
- added more Italian and Spanish translation (thank you mlocati and richarson)

## 17.05.6
- the `Add script` button in the *Scripting settings* is now a button group
  where you can add local scripts and scripts from the *Script repository*
- if the script in the *Scripting settings* was from the *Script repository*
  additional information from the repository will be shown
- the *example script* will now be ignored in the *Script repository*
- the authors of the scripts in the script repository are now shown in the
  *Script repository dialog* and the *Scripting settings* (if the information
  was available at the time of script installation)
- added more Italian and Brazilian Portuguese translation (thank you mlocati and
  mlimacarlos)

## 17.05.5
- you can now access the new [script repository](https://github.com/qownnotes/scripts)
  from a new button *Search script repository* in the *scripting settings*
    - in the new *Script repository* dialog you can search for scripts from 
      the repository and install them
    - more features and scripts will be added in the coming releases
- added more Chinese, Dutch and Hungarian translation (thank you chonghua and
  Fabijenna)

## 17.05.4
- scripting function `registerCustomAction` now got a new parameter to hide 
  the button of the custom action in the toolbar
    - for more information please take a look at the 
      [registerCustomAction documentation](https://www.qownnotes.org/scripting/methods-and-objects.html#registering-a-custom-action)
- fixed a problem with tag cases for cyrillic tags
  (for [#579](https://github.com/pbek/QOwnNotes/issues/579))
- fixed a warning when tagging a note with the same tag twice
- added more Czech and French translation (thank you jkalousek and lepascalou)

## 17.05.3
- removed a gray area under the note edit widget (glitch from version 17.05.2)
- you can now reduced the height of the scripting widget and the log widget 
  down to 10 pixel
- you can now reduced the height of the note tag widget until the buttons are
  reached
- added more Italian, Brazilian Portuguese, Polish, Spanish and German
  translation (thank you mlocati, mlimacarlos, lucjan and richarson)

## 17.05.2
- the shortcut of custom actions will now be remembered after the app is 
  restarted (for [#576](https://github.com/pbek/QOwnNotes/issues/576))
- there now is a new scripting command to show an open file dialog
  `script.getOpenFileName()` (for [#536](https://github.com/pbek/QOwnNotes/issues/536))
    - for more information please take a look at
      [Showing an open file dialog](https://www.qownnotes.org/scripting/methods-and-objects.html#showing-an-open-file-dialog)
- there now are new scripting commands to register labels with
  `script.registerLabel("my-label")` and write to them with
  `script.setLabelText("my-label", "my custom text")`
  (for [#536](https://github.com/pbek/QOwnNotes/issues/536))
    - the labels will be visible in the new scripting dock widget
    - you can use both plain text or html in the labels
    - the text will be selectable and links can be clicked
    - for more information please take a look at
      [Registering a label](https://www.qownnotes.org/scripting/methods-and-objects.html#registering-a-label)
      and [Setting the text of a registered label](https://www.qownnotes.org/scripting/methods-and-objects.html#setting-the-text-of-a-registered-label)

## 17.05.1
- the restoring of the expanded state of the items in the tag tree widget now
  works across multiple note folders
- QOwnNotes now also remembers when you select *All notes* in the note 
  subfolder tree widget when you restart the app
  (for [#572](https://github.com/pbek/QOwnNotes/issues/572))
- there now is a new scripting command to show an information message box
  `script.informationMessageBox()` (for [#536](https://github.com/pbek/QOwnNotes/issues/536))
    - for more information please take a look at
      [Showing an information message box](https://www.qownnotes.org/scripting/methods-and-objects.html#showing-an-information-message-box)
- added more Italian, French, Polish and Spanish translation (thank you mlocati,
  lepascalou, lucjan and richarson)

## 17.05.0
- the expanded state of the items in the tag tree widget is now restored 
  when the tree is reloaded
- if the version of your QOwnNotesAPI app on your ownCloud / Nextcloud server
  is too low a warning will now be shown when the app starts
  (for [#564](https://github.com/pbek/QOwnNotes/issues/564))
- added more Italian, Brazilian Portuguese, Polish, German and Spanish
  translation (thank you mlocati, mlimacarlos, lucjan, bigant and richarson)

## 17.04.4
- a user agent and content type header was added to requests to your ownCloud / 
  Nextcloud server to prevent troubles with some server hosting providers
  (for [#541](https://github.com/pbek/QOwnNotes/issues/541))
- added more information text to the git settings
- added more Italian, Brazilian Portuguese, Polish, Portuguese, French and
  Spanish translation (thank you mlocati, mlimacarlos, lucjan, manuelarodsilva,
  lepascalou and richarson)

## 17.04.3
- the preview will now be updated correctly if it was previously disabled
  (for [#559](https://github.com/pbek/QOwnNotes/issues/559))
    - when selecting another workspace or switching to the previous workspace
    - when `Show all panels` is clicked in the `Windows` menu
    - when a panel is selected in the `Windows / Panels` menu
- you can now enter a custom command to show the git log of a note in the
  `Git versioning` settings
    - you can optionally use `%notePath%` to enter the note path if you are 
      using more complex parameters
        - for example: `gitk %notePath%`
    - you'll find a new menu item in the `Edit` menu and the note list 
      context menu to show the note versions if you entered a git log command
- added more Italian, Brazilian Portuguese, Polish and Spanish translation 
  (thank you mlocati, mlimacarlos, lucjan and richarson)

## 17.04.2
- more git versioning implementation
    - added Windows support
    - you can now select the path of your git executable in the new
      `Git versioning` settings
    - you can set the git commit interval time, default is 30 sec 
    - changes are now also committed when changing the note folder (for both 
      the old and new note folder) or quitting the application
- added more Brazilian Portuguese translation (thank you mlimacarlos)

## 17.04.1
- more git versioning implementation
    - fixed committing if note path was not initialized by git yet
    - added macOS support 
- added more Spanish translation (thank you richarson)
 
## 17.04.0
- the note preview will now only be updated if the note text has changed
  to improve performance (especially when viewing large notes) and prevent
  a flickering of the scrollbars when no editing is done
- fixed an url highlighting problem
  (for [#556](https://github.com/pbek/QOwnNotes/issues/556))
- added more Italian, German, Polish, French translation (thank you mlocati,
  xpertmind, lucjan and lepascalou)

## 17.03.9
- added experimental git support to keep a local history of files in a note
  folder under Linux
    - you can enable this feature in the note folder settings
    - all changes in your active note folder will be committed to git every 30 sec
- added more Italian, Croatian, German, Polish, Spanish and Brazilian Portuguese
  translation (thank you mlocati, xpertmind, lucjan, richarson and mlimacarlos)

## 17.03.8
- added **Portuguese translation** (`pt_PT`) by manuelarodsilva, thank you very much!
    - the current Portuguese translation (`pt_BR`) got renamed to `Português do Brasil`
    - the current setting will be migrated to `Português do Brasil` when you start the app
- added **Croatian translation** by xpertmind, thank you very much!
- added more Italian, German, Polish and Spanish translation (thank you mlocati,
  xpertmind, lucjan and richarson)

## 17.03.7
- fixed loading of trashed notes in the trash dialog
- improved error messages for the version and trash dialog
- added more Italian, French and Spanish translation (thank you mlocati,
  lepascalou and richarson)

## 17.03.6
- when configuring a new note folder path in the settings the server-path 
  will now be suggested
  (for [QOwnNotesAPI Issue #11](https://github.com/pbek/qownnotesapi/issues/11))
    - the placeholder text was updated to be more informative
- broken links between notes and tags are now automatically removed when the 
  tag tree is reloaded (for [#548](https://github.com/pbek/QOwnNotes/issues/548))
- added more German, Italian, Polish and Portuguese translation (thank you
  mlocati, lucjan and mlimacarlos)

## 17.03.5
- fixed more url highlighting and clicking issues
- you can now copy the file path of an image in the preview by right-clicking
  on it and selecting `Copy image file path` from the context menu
- added more German translation

## 17.03.4
- fixed highlighting and opening of links with `/` at the end

## 17.03.3
- after starting QOwnNotes now the last note along with the previous cursor 
  and scrollbar position will be restored
  (for [#537](https://github.com/pbek/QOwnNotes/issues/537))
    - if that fails the first note in the note list will be opened

## 17.03.2
- fixed a scrollbar position warning
- when pressing <kbd>Tab</kbd> in the note list to switch to the note text 
  edit the previous position will now also be restored
- the generated preview html will now be cached to increase performance when 
  editing large notes (for [#537](https://github.com/pbek/QOwnNotes/issues/537))
- the preview html will now only be generated if the preview panel is visible
  to increase performance when editing large notes

## 17.03.1
- the current note in the scripting function `script.currentNote()` will now 
  be update correctly when when text is written to it by the user 
  (for [#536](https://github.com/pbek/QOwnNotes/issues/536))
- the owncloud / Nextcloud application QOwnNotesAPI was updated to version 17.3.0
    - fixed the time-output of the trashed notes api
    - fixed some PHP warnings in the log
- when restoring the position in a note not only the cursor position will now
  be restored, but also the scroll bar position to make the position 
  restoration feel more natural
    - this is now implemented in the following scenarios
        - switching between note folders
        - selecting a previous selected note (if enabled in the settings)
        - moving forward or back in the note history
        - jumping to note bookmarks

## 17.03.0
- you can now assign different tag colors in dark mode
    - this is handy because the colors you may have defined on another, synced
      computer, that did not use dark mode may not look good on this computer
      in dark mode
    - with this update the currently set non-dark mode colors will be used as
      dark mode colors 
- added more Dutch, Hungarian and Spanish translation (thank you Fabijenna and
  richarson)

## 17.02.6
- the maximum tag nesting level of the html preview was increased from 16 to 32
  to allow more levels of list items than six
  (for [#473](https://github.com/pbek/QOwnNotes/issues/473))
- added more French, Italian, Polish and Portuguese translation (thank you
  lepascalou, mlocati, lucjan and mlimacarlos)

## 17.02.5
- you can now show a custom action in the note text edit context menu
    - for more information please take a look at the
      [custom action documentation](https://www.qownnotes.org/scripting/methods-and-objects.html#registering-a-custom-action)
- the last directory path will now be selected if you select a script file in 
  the *Scripting settings* or select an external editor in the *General settings*
- the script name will now be set from the file name if no script name was set
  yet in the *Scripting settings*
- added more German translation

## 17.02.4
- you can now copy text to the clipboard in a script with the new scripting
  method `script.setClipboardText()`
    - there is an example at
      [selected-markdown-to-bbcode.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/selected-markdown-to-bbcode.qml)
    - for more information please take a look at the
      [scripting documentation](https://www.qownnotes.org/scripting/methods-and-objects.html#copying-text-into-the-clipboard)
- the unicode line endings from the result of the scripting method
  `script.noteTextEditSelectedText()` will now be converted to `\n` 
- you can now jump to a note with the new scripting method `script.setCurrentNote()`
  if it is visible in the note list (for [#510](https://github.com/pbek/QOwnNotes/issues/510))
    - there is an example at
      [journal-entry.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/journal-entry.qml)
    - for more information please take a look at the
      [scripting documentation](https://www.qownnotes.org/scripting/methods-and-objects.html#jumping-to-a-note)

## 17.02.3
- the bracket closing feature now also just moves the cursor to the right if a
  closing bracket is typed at the position of a closing bracket if there is a
  matching opening bracket (for [#514](https://github.com/pbek/QOwnNotes/issues/514))
    - this works for `)`, `]` and `}` 
- you can now fetch a note by its file name in a script with the new scripting
  method `script.fetchNoteByFileName(fileName)`
  (for [#510](https://github.com/pbek/QOwnNotes/issues/510))
    - for more information please take a look at the
      [scripting documentation](https://www.qownnotes.org/scripting/methods-and-objects.html#fetching-a-note-by-its-file-name)
- you can now check if a note exists by its file name in a script with the 
  new scripting method `script.noteExistsByFileName(fileName)`
  (for [#510](https://github.com/pbek/QOwnNotes/issues/510))
    - for more information please take a look at the
      [scripting documentation](https://www.qownnotes.org/scripting/methods-and-objects.html#checking-if-a-note-exists-by-its-file-name)

## 17.02.2
- the restoring of the cursor position in notes is now disabled by default 
  for macOS because of troubles it causes for some users
  (for [#500](https://github.com/pbek/QOwnNotes/issues/500))
- if the shortcuts for deleting the current line or word isn't used in the 
  note text edit now the default shortcuts will be delegated to the widget 
  with the focus (for [#476](https://github.com/pbek/QOwnNotes/issues/476))
- added more Russian and Portuguese translation (thank you Maboroshy and mlimacarlos)

## 17.02.1
- QOwnNotes for macOS is finally deployed with Qt 5.8, solving some pesky Qt bugs
  (for [#503](https://github.com/pbek/QOwnNotes/issues/503))
- fixed the preview of images if there are more than one in the same line
  (for [#509](https://github.com/pbek/QOwnNotes/issues/509))
- added more Spanish, Hungarian, Italian, Russian, Polish and Portuguese
  translation (thank you richarson, rommel4444, mlocati, azar9610, lucjan and 
  mlimacarlos)

## 17.02.0
- you now can sort or notes also alphabetically descending
  (for [#504](https://github.com/pbek/QOwnNotes/issues/504))
- disabled the `Copy` button in the Note editor font schema settings 
  under macOS because of a Qt bug for Qt versions lower than 5.8
  (for [#503](https://github.com/pbek/QOwnNotes/issues/503))
- added more Spanish, Dutch, Italian, German and French translation (thank
  you richarson, Leime, mlocati, bigant and lepascalou)

## 17.01.14
- you can now turn off the ownCloud / Nextcloud support in the settings to 
  prevent some checks
    - the check whether todo lists will used was also improved
- fixed a problem with showing a copied Editor color schema under macOS
  (for [#503](https://github.com/pbek/QOwnNotes/issues/503))
- added more Portuguese, Dutch, Italian, Czech and Polish translation (thank
  you mlimacarlos, Leime, mlocati, jkalousek and lucjan)

## 17.01.13
- the directory of the previously selected file will now be remembered for a
  lot of file dialogs (for [#502](https://github.com/pbek/QOwnNotes/issues/502))
    - the last directory of each dialog will be remembered separately
    - if no directory was set the last directory path of any dialog will be used
    - if there still is no readable path the home directory will be chosen
- fixed the image link that is inserted when using the *Insert image* menu entry
  (for [#501](https://github.com/pbek/QOwnNotes/issues/501))
- fixed the base64 encoded images in the note HTML export for images with 
  spaces in the path
- added more Portuguese, Ukrainian, Chinese, Czech and Polish translation (thank
  you mlimacarlos, Borisvit, chonghua, jkalousek and lucjan)

## 17.01.12
- there now is a new script function `script.downloadUrlToMedia("http://image.url");`
  to allow you to download an url to the media folder and return the media url
  or the markdown image text (for [#499](https://github.com/pbek/QOwnNotes/issues/499))
    - there is an example at
      [paste-latex-image.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/paste-latex-image.qml)
    - for more information please take a look at the
      [scripting documentation](https://www.qownnotes.org/scripting/methods-and-objects.html#downloading-an-url-to-the-media-folder)
- added more fixes for a warning that might trigger when notes are moved to an
  other note subfolder in macOS
  (for [#498](https://github.com/pbek/QOwnNotes/issues/498))
- added more Hungarian, Ukrainian, Spanish and Italian translation (thank you
  Fabijenna, Borisvit, richarson and mlocati)

## 17.01.11
- added Czech translation (a big thank you to jkalousek!)
- fixed a warning that might trigger when notes are moved to another note subfolder
  (for [#498](https://github.com/pbek/QOwnNotes/issues/498))
- you now can disable the restoring of the cursor position in the *General settings*
  (for [#500](https://github.com/pbek/QOwnNotes/issues/500))
- added more French and Chinese translation (thank you lepascalou and chonghua)
 
## 17.01.10
- fixed a crash that can happen with long notes with malformed urls
  (for [#494](https://github.com/pbek/QOwnNotes/issues/494))
- you can now save modified notes also manually with the shortcut <kbd>Ctrl</kbd> +
  <kbd>S</kbd> (for [#484](https://github.com/pbek/QOwnNotes/issues/484))
- fixed a problem with <kbd>Ctrl</kbd> - clicking urls with a `)` in it
- when opening notes the cursor position inside the note will now be restored
  to the position when the note was last visited in the current session
- added more Dutch, Portuguese and Italian translation (thank you Leime,
  mlimacarlos and mlocati)

## 17.01.9
- when exporting a note as html the images are now base64 encoded and included
  inside the html (for [#491](https://github.com/pbek/QOwnNotes/issues/491))
- added more Dutch, French, Italian and Polish translation (thank you Leime,
  lepascalou, mlocati and lucjan)

## 17.01.8
- improved log and status messages when a note could not be saved for example
  when the file system doesn't allow certain characters in the filename
  (for [#486](https://github.com/pbek/QOwnNotes/issues/486))
- you can now set that notes are stored with UNIX newline characters instead 
  of the native newline characters in the *General settings*
  (for [#487](https://github.com/pbek/QOwnNotes/issues/487))
    - you can use this for example under Windows if you have troubles with 
      newlines in the ownCloud / Nextcloud notes app
- added more Russian and Chinese translation (thank you azar9610 and chonghua)

## 17.01.7
- fixed a problem with duplicate custom note file extensions in the general 
  settings (for [#483](https://github.com/pbek/QOwnNotes/issues/483))
    - now the same file extension also can't be added twice
- under Windows some more characters that are asking for troubles are now 
  removed from the title at the Evernote import
  (for [#465](https://github.com/pbek/QOwnNotes/issues/465))
- added more German, Polish, Spanish and Chinese translation (thank you bigant,
  lucjan, richarson and chonghua)

## 17.01.6
- you can now use the panel toggle menu items in your toolbars
  (for [#481](https://github.com/pbek/QOwnNotes/issues/481))
- under Windows some characters that are asking for troubles are now removed 
  from the title at the Evernote import
  (for [#465](https://github.com/pbek/QOwnNotes/issues/465))
- added more Dutch, Chinese, Portuguese, Italian and German translation (thank
  you lenkadubois, chonghua, mlimacarlos and mlocati)

## 17.01.5
- fixed a problem with opening notes with special characters in its file name in
  an external editor if no editor was defined in the settings
- you can now use the shortcut <kbd>Alt</kbd> + <kbd>⇧</kbd> + <kbd>O</kbd> 
  to open the current note in a readonly view
    - you can use that view to leave a note open to look at while writing on 
      another note
    - markdown highlighting will be used in that view and you also can search
      for text in it
- added more Dutch, Chinese and German translation (thank you Leime, chonghua
  and bigant)

## 17.01.4
- fixed two database errors when using QOwnNotes for the first time
  (for [#467](https://github.com/pbek/QOwnNotes/issues/467))
- fixed a highlighting issue when using a grave accent character in `<>` links
- fixed an issue with the note search getting stuck when searching for `^ ` or
  `  ` (just spaces) (for [#469](https://github.com/pbek/QOwnNotes/issues/469))
- fixed a crash under Windows when entering an opening `<` with a lot of text
  after it, but no closing `>`
  (for [#461](https://github.com/pbek/QOwnNotes/issues/461) and
  [#471](https://github.com/pbek/QOwnNotes/issues/471))
- added more Italian, Polish, Russian, Dutch, Spanish, Chinese and French
  translation (thank you mlocati, lucjan, frostyland, Leime, richarson, chonghua
  and lepascalou)

## 17.01.3
- after installing QOwnNotes now only a minimal set of panels will be shown 
  to reduce complexity in the beginning
    - a second workspace with a full set of panels can be chosen in the 
      workspace selector
    - if a tag will be added to a note by the user the panel to add tags to a 
      note will be shown automatically and on the first time also the tag list
      panel will enabled  
- image links like `[![name](image)](link)` and `[![](image)](link)` will now
  also be highlighted (for [#463](https://github.com/pbek/QOwnNotes/issues/463))
- added more Dutch, Hungarian, Italian, Spanish and Portuguese translation 
  (thank you Leime, Fabijenna, mlocati, richarson and mlimacarlos)

## 17.01.2
- years were bumped to 2017
- since the support for legacy **CalDAV** URLs in **Nextcloud 11** is still 
  broken support for the new URLs was added and will now used by default (if the 
  application is re-installed)
    - the legacy URLs will be supported as well
    - switch to the new setting in the *Todo setting* if you have troubles 
      with Nextcloud 11
    - ownCloud 9 seems to support the new CalDAV URLs as well 
- added more Portuguese, Polish and Polish translation (thank you mlimacarlos, 
  mlocati and lucjan)

## 17.01.1
- you can now enter a custom time format when inserting the current time into
  a note in the *Editor settings*
    - take a look at the [time format documentation](https://www.qownnotes.org/editor/time-formats.html)
      for more information about the available time formats
- the menu entry for showing the note subfolder panel is now invisible if
  note subfolders are not enabled for the current note folder

## 17.01.0
- notes in the note list are now resorted if they were sorted alphabetically 
  and a note was manually renamed in the note list
  (for [#450](https://github.com/pbek/QOwnNotes/issues/450), thank you @dwu)
- improved on setting the note text edit to be readonly if the last note from a 
  note folder or note subfolder was removed or moved somewhere else or when
  there is no visible note to view
  (for [#446](https://github.com/pbek/QOwnNotes/issues/446))
- the *Delete word* functionality now matches that of other editors more closely
  (for [#453](https://github.com/pbek/QOwnNotes/issues/453), thank you @dwu)
- implemented the ability to set shortcuts for panels
  (for [#452](https://github.com/pbek/QOwnNotes/issues/452))
- added more Spanish and Polish translation (thank you richarson and lucjan)

## 16.12.16
- when the current note is moved to a different note subfolder the current note
  will now be unset, like when it is moved to a different note folder
  (for [#446](https://github.com/pbek/QOwnNotes/issues/446))
- added more French, Dutch, Hungarian and Italian translation (thank you
  lepascalou, Fabijenna and mlocati)

## 16.12.15
- fixed a highlighting problem with italic tags at the start of an ordered 
  list, that starts with a `*`, like `* *italic*`
  (for [#431](https://github.com/pbek/QOwnNotes/issues/431))
- you will now get a dialog to restart the application if you have modified a 
  setting in the settings dialog that requests a restart
- added the application arguments to the output in the debug settings
- added more Dutch, Hungarian, Italian, Spanish and Polish translation 
  (thank you Fabijenna, mlocati, richarson and lucjan)

## 16.12.14
- fixed a highlighting problem with normal text between bold formatted markdown 
  texts (for [#431](https://github.com/pbek/QOwnNotes/issues/431))
- now a `¶` character, followed by a new line is used instead of a `\n` to
  signal a line break in the diffs of the note versions dialog
  (for [#435](https://github.com/pbek/QOwnNotes/issues/435))
- you can now delete the current line in the note text edit with <kbd>Alt</kbd> +
  <kbd>Backspace</kbd> (or <kbd>⌘</kbd> + <kbd>Backspace</kbd> under macOS)
  (for [#434](https://github.com/pbek/QOwnNotes/issues/434) and
  [#432](https://github.com/pbek/QOwnNotes/issues/432))
- you can now delete the current word in the note text edit with <kbd>Ctrl</kbd> +
  <kbd>Backspace</kbd> (or <kbd>Alt</kbd> + <kbd>Backspace</kbd> under macOS)
  (for [#432](https://github.com/pbek/QOwnNotes/issues/432))

## 16.12.13
- note links like `<my note.md>` will now only be translated to real note 
  links in the preview if the linked note was found in the current note folder
  (for [#433](https://github.com/pbek/QOwnNotes/issues/433))
- bold markdown tags will now also be closed at the beginning of a line after
  the 2nd `*` was entered (because after the first `*` it could still be a list)
  (for [#428](https://github.com/pbek/QOwnNotes/issues/428))
- added more Spanish translation (thank you richarson)
 
## 16.12.12
- the foreground and background color of the note text edit will now be taken
  from the *Text preset* highlighting item, so you can use dark highlighting 
  schemas even if you are not in dark mode
  (for [#429](https://github.com/pbek/QOwnNotes/issues/429))
    - it's now also possible to override these colors in a script if you 
      specify the css selector, for example with
      `script.addStyleSheet("QTextEdit#noteTextEdit {background-color: red;}");`
- the automatic bracket-closing of the `*` character now also works if used in
  markdown lists that start with a `* ` and when used in normal text at the 
  start of the line after the 2nd `*`
  (for [#428](https://github.com/pbek/QOwnNotes/issues/428))
- added more Dutch, Hungarian, Italian and Polish translation (thank you
  Fabijenna, Leime, mlocati and lucjan)

## 16.12.11
- added a link to the **new documentation** at
  [docs.qownnotes.org](https://docs.qownnotes.org) to the scripting settings
- the problem with accessing your todo lists on **Nextcloud 11** will be 
  fixed in Nextcloud 11.0.1
  (see [Nextcloud Issue #2552](https://github.com/nextcloud/server/issues/2552))
- added a warning that the ownCloud notes app doesn't support subfolders in
  the note folder settings
- there now is a repository for **Fedora 25**
    - please visit the [QOwnNotes installation page](https://www.qownnotes.org/installation#Fedora)
      for instructions
- added more markdown highlighting support
    - **multiple block quote characters** like `> > deeper level`
    - support for **headlines without space** after the `#` like `##no space`,
      because Beebom was (mis-)using headlines that way in his YouTube video
      [10 Must Have Linux Apps You Should Use](https://youtu.be/oSSyLNb8jA8?t=2m19s) ;)
- you can now also **clear the calendar cache** and **reload the tasks** from 
  your server in the *Todo dialog*
- you now can see the **current line number**, the position in the line and the 
  amount of selected characters in the status bar
- added more Dutch, Hungarian, Italian, Spanish and Polish translation 
  (thank you Fabijenna, mlocati, richarson and lucjan)

## 16.12.10
- when linking notes now the title of the note will be used as link-text
- decreased the saturation of some background colors of the `Light` color 
  schema to increase readability
- tried to fix a problem with wrong reminder times of tasks in the todo list
- fixed a problem with duplicate shortcut items in the shortcut settings 
  after opening the settings dialog multiple times
  (for [#424](https://github.com/pbek/QOwnNotes/issues/424))
- added more Dutch, Chinese and Japanese translation (thank you Leime,
  Fabijenna, chonghua and MoeMoePig)

## 16.12.9
- fixed a problem where duplicate actions would be shown in the *find action* 
  dialog
- added experimental support for **regular file links** like `<my-note.md>` or 
  `[my note](my-note.md)` in the note edit pane and the note preview
   (for [#266](https://github.com/pbek/QOwnNotes/issues/266))
    - no paths are allowed
    - only note files are supported
    - when `Ctrl` - clicked the note will be opened
    - the file link is internally converted to a `note://my_note` url, file 
      extensions will be stripped
    - highlighting for such links was also added
    - in the preview these file links are replaced with note links, that also
      takes place when such a link was found in a code block
- added more Dutch, Hungarian, Spanish, Italian and Portuguese translation 
  (thank you Fabijenna, richarson, mlocati and mlimacarlos)

## 16.12.8
- added a new shortcut <kbd>Alt</kbd> + <kbd>⇧</kbd> + <kbd>W</kbd> to 
  **search** for the **selected text** on the **web**
- urls with empty title like `[](file://link)` are now also highlighted
  (for [#415](https://github.com/pbek/QOwnNotes/issues/415))
- added more Dutch, Hungarian, Japanese, Italian, German and Polish translation 
  (thank you Fabijenna, piyo, mlocati, bigant and lucjan)

## 16.12.7
- headlines now have a small top and bottom margin in the note edit panel
- replaced the text `ownCloud` by `ownCloud / Nextcloud` in the settings 
  dialog to make it more clear that Nextcloud is also supported
- there is a new shortcut <kbd>Ctrl</kbd> + <kbd>⇧</kbd> + <kbd>B</kbd> that
  **inserts a block quote** character or formats the selected text as block quote
- added more Dutch, Hungarian, French, Portuguese and Ukrainian translation 
  (thank you Fabijenna, lepascalou, mlimacarlos and Borisvit)

## 16.12.6
- a lot of **markdown syntax** is now **masked in gray** in the markdown 
  highlighting
    - there is a new highlighting state `Masked syntax` in the Editor fonts &
      color settings to choose a styling for masked markdown syntax
- the font size of the headlines were increased in the `Light` and `Dark` color 
  schemas
- QOwnNotesAPI 16.12.0 was released in the Nextcloud store to fix signature
  checks (for [#414](https://github.com/pbek/QOwnNotes/issues/414))
- added more Spanish, Portuguese and Russian translation (thank you richarson,
  mlimacarlos and frostyland)

## 16.12.5
- updated the OpenSSL DLLs to 1.0.2j to prevent problems on some Windows
 installations (for [#413](https://github.com/pbek/QOwnNotes/issues/413))
- added more Dutch, Hungarian, Italian, Polish, Arabic and Ukrainian translation 
  (thank you Fabijenna, mlocati, lucjan, yousufinternet and Borisvit)

## 16.12.4
- all three settings about external modifications of the current note in the 
  general settings are now mutual exclusive
  (for [#405](https://github.com/pbek/QOwnNotes/issues/405))
- the `version 0` problem at fresh installations of QOwnNotesAPI from the store 
  was now fixed in Nextcloud 11
  (for [#262](https://github.com/pbek/QOwnNotes/issues/262))
- **scripts** now also work with a path relative to the data directory in 
  **portable mode** (for [#412](https://github.com/pbek/QOwnNotes/issues/412))
- if you select a new script path for a script in the script settings the 
  directory of your current script will be selected
    - in portable mode the data directory will be selected if no script path 
      was set yet
- the **path to an external editor** will now also be stored relative to the 
  data directory in **portable mode**
    - if you select a path and no path was set yet the data directory will
      now be selected
- added more Dutch and Hungarian translation (thank you Fabijenna)

## 16.12.3
- fixed a problem with saving tasks in Nextcloud 11.0 RC 1
  (for [#406](https://github.com/pbek/QOwnNotes/issues/406))
- fixed two warnings about no matching signals when opening the todo dialog
- trying to enable the network access again when the network has gone away to
  fix occasional problems where the app remained without network
- you can now choose `Always accept external changes in the future` when your
  current note got modified externally and the diff dialog pops up
  (for [#405](https://github.com/pbek/QOwnNotes/issues/405))
    - you can reset that checkbox in the general settings

## 16.12.2
- fixed a problem with line feeds under Windows where notes will move up to 
  the top in the note list even when not actively edited
  (for [#408](https://github.com/pbek/QOwnNotes/issues/408))
- took some more safety measures to prevent crashes when the application is 
  quitting while log output was still sent to the the log panel
  (for [#406](https://github.com/pbek/QOwnNotes/issues/406))

## 16.12.1
- markdown reference links with text as reference id are now also highlighted
  and can now also be opened with <kbd>Ctrl</kbd> + `Click`
- fixed a problem with exporting font color schemas to a file that already 
  exists (for [#407](https://github.com/pbek/QOwnNotes/issues/407))
- added the new dark font color schema *AnalyticalYellow* (thank you, @Omnicraft)
  (for [#407](https://github.com/pbek/QOwnNotes/issues/407))
- fixed a crash that could occur when closing the settings dialog immediately 
  after opening it, while the ownCloud connection test is still running 
  (for [#406](https://github.com/pbek/QOwnNotes/issues/406))
- fixed problems with note sharing, note versions and the note trash with 
  Nextcloud 11.0 beta 2
  (for [#406](https://github.com/pbek/QOwnNotes/issues/406))
- added more Dutch, Hungarian, Russian, Spanish and Ukrainian translation 
  (thank you Fabijenna, frostyland, richarson and Borisvit)

## 16.12.0
- you can now **import csv files** with the `Insert table` dialog
  (for [#78](https://github.com/pbek/QOwnNotes/issues/78))
    - you'll get a preview of the content of your csv file
    - you can choose the separator, text-delimiter and whether the
      first line is the table heading
- more highlighting fixes
  (for [#78](https://github.com/pbek/QOwnNotes/issues/78))
    - fixed an issue with other highlighting inside an inline code block
    - table highlighting support for tables, that don't start with a `|`
      was dropped, that's far to messy to deal with
- fixed a problem where the shortcuts were not viewed in the settings
- made a special allowance for allowing entering two `*` characters in a row 
  and closing both of them when the first `*` was at the end of the line
  (for [#386](https://github.com/pbek/QOwnNotes/issues/386))
- markdown reference urls can now also be opened with <kbd>Ctrl</kbd> + `Click`
- added more Japanese, Italian, Polish and Portuguese translation (thank
  you MoeMoePig, Manu-sh, lucjan and mlimacarlos)

## 16.11.18
- fixed highlighting issues with images and tables
  (for [#78](https://github.com/pbek/QOwnNotes/issues/78))
- tried to fix a package dependency in openSUSE Tumbleweed
  (for [#367](https://github.com/pbek/QOwnNotes/issues/367))
- there now is a new scripting command `script.reloadScriptingEngine()`
  to reload the scripting engine
- dramatically reduced the loading time of large notes
    - the navigation tree in the navigation panel was rebuilt too often
      with the new highlighter

## 16.11.17
- lots of memory leak fixes, memory related optimizations and speed
  improvements, mainly in conjunction with the dialogs and the
  ownCloud service
- added more German, Italian, Polish and Portuguese translation (thank
  you mlocati, lucjan and mlimacarlos)

## 16.11.16
- **markdown** syntax **highlighting improvements**
  (for [#78](https://github.com/pbek/QOwnNotes/issues/78))
    - better multi-line html comment detection
    - image links like `![](file://)` work now
    - it's now possible to have links and italic/bold formatting in a headline
    - urls without any other markup are now highlighted too
        - they can now also be opened with <kbd>Ctrl</kbd> + `Click`
    - formatting bold and italic with `__bold__` and `_italic_` now also works
    - fixed a wrongly detected table in a link
- you can now **insert** a **markdown table** with the new shortcut
  <kbd>Alt</kbd> + <kbd>⇧</kbd> + <kbd>T</kbd>
  (for [#78](https://github.com/pbek/QOwnNotes/issues/78))
    - a markdown table wizard will be opened to create an empty table
      to work with
- added more Spanish, French and Russian translation (thank you
  richarson, lepascalou and frostyland)

## 16.11.15
- a **new markdown parser and highlighter** was now implemented from
  scratch to allow a higher parsing speed and more flexibility in
  the future (for [#78](https://github.com/pbek/QOwnNotes/issues/78))
    - text will now be instantly highlighted, there is no
      highlighting-interval setting any more
    - now you also can change the styling in the settings or
      enable/disable the highlighting without re-starting the application
    - please report issues with the new highlighter if you find any
- **markdown tables** are now supported by the new markdown highlighter
  (for [#78](https://github.com/pbek/QOwnNotes/issues/78))
    - the mono-space code font is used for it
    - you can change the highlighting color and other styles in the settings
- there now is a **Solus package** for QOwnNotes in the Solus main repository
    - see [Solus package for QOwnNotes](https://www.qownnotes.org/Blog/Solus-package-for-QOwnNotes)
      for more information
- Nextcloud support is now mentioned a bit more often
- to save memory the log output will now only be written to the log
  panel if the panel is visible
- added more Italian, Dutch, Hungarian, Polish, Spanish, Portuguese,
  Ukrainian, German and Japanese translation (thank you mlocati,
  Fabijenna, lucjan, richarson, mlimacarlos, Borisvit, bigant and MoeMoePig)

## 16.11.14
- added **Ukrainian translation** (a big thank you to Borisvit!)
- reduced the margins in the todo dialog to save space on small displays
- added more Italian, Dutch, Hungarian, Polish, German, Portuguese and
  French translation (thank you Manu-sh, Fabijenna, lucjan, bigant,
  mlimacarlos and lepascalou)

## 16.11.13
- QOwnNotes is now also available for Ubuntu 17.04
    - see [Ubuntu installation](https://www.qownnotes.org/installation#Linux)
      for more information
- fixed a problem with inserting images if application language is Italian
  (for [#391](https://github.com/pbek/QOwnNotes/issues/391))
    - potential file open dialog filter problems in other languages were
      also fixed in the process
- fixed searching of settings that are not at the root level of the
  settings page tree
- now a button menu is used in the todo dialog to improve it for small
  screen sizes
- the path to the settings file (or the key if the settings are stored
  in the Windows registry) and the path to the application database file
  are now shown in the debug settings
    - the database file path of the note folders is shown in the note
      folder list 
- added more Italian, Dutch, Hungarian, Polish, Spanish, Portuguese,
  French and Japanese translation (thank you Manu-sh, Fabijenna, lucjan,
  richarson, mlimacarlos, lepascalou and piyo)

## 16.11.12
- added a new scripting hook `noteOpenedHook(note)` that is called every
  time a note is opened
  (for [#389](https://github.com/pbek/QOwnNotes/issues/389))
    - take a look at the
      [noteOpenedHook documentation](https://www.qownnotes.org/scripting/methods-and-objects.html#noteopenedhook)
      for more information
    - there is an example script at
      [on-note-opened.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/on-note-opened.qml)
- added some documentation for all scripting hooks to the
  [scripting documentation](https://www.qownnotes.org/scripting/methods-and-objects.html)
- the closing of brackets and markdown formatting characters is now only
  allowed if the cursor was at the end of a block
  (for [#386](https://github.com/pbek/QOwnNotes/issues/386))
- closing brackets and markdown formatting characters are now
  automatically removed if the opening character is removed with
  <kbd>Backspace</kbd>
  (for [#386](https://github.com/pbek/QOwnNotes/issues/386))
   - opening characters can be `(`, `[`, `{`, `<`, `*` or `"`
   - you can disable this feature in the `Editor` settings
   - if text was selected closing characters will not be removed
- added more Italian, Polish and French translation (thank you Manu-sh,
  lucjan and lepascalou)

## 16.11.11
- the `Markdown highlighting` options moved to the new `Editor` settings
- more changes to closing of brackets and markdown formatting characters
  (for [#386](https://github.com/pbek/QOwnNotes/issues/386))
    - you can now disable the feature in the `Editor` settings
    - added closing of `<` and removed closing of `'`
    - `*` will now only be closed if not used in lists
        - it will be checked if it is on the beginning of a block or if
          it has just whitespaces in front of it
- added more Italian, Dutch, Hungarian, Polish, Spanish, Portuguese,
  German and Japanese translation (thank you mlocati, Manu-sh,
  Fabijenna, lucjan, richarson, mlimacarlos, bigant and piyo)

## 16.11.10
- implemented automatic entering of **matching characters** like closing
  parenthesis (for [#386](https://github.com/pbek/QOwnNotes/issues/386))
    - characters like `"`, `'`, `*`, `(`, `[` or `{` are automatically closed
    - `"`, `'` and `*` are only closed if there isn't already an opening
      character in the current text block
- there now is a new button `Import as note` in the todo dialog that
  **imports** the current **todo item as new note**
  (for [#387](https://github.com/pbek/QOwnNotes/issues/387))
- improved the initial sizes of the panels on the first application start
- added more Chinese, Dutch, Portuguese, Russian, German and Japanese
  translation (thank you nkzk, Fabijenna, mlimacarlos, frostyland,
  guijan and piyo)

## 16.11.9
- fixed the shortcut display in the action dialog on macOS
  (and filed a Qt documentation bug 
  [QTWEBSITE-744](https://bugreports.qt.io/browse/QTWEBSITE-744))
- tried to improve the restoring of the workspace in full-screen and
  maximized mode
- added more Dutch and Italian translation (thank you Fabijenna and mlocati)

## 16.11.8
- you can now **search for actions from the menu** with `Find action`
  in the *Help* menu or by pressing <kbd>Ctrl</kbd> + <kbd>⇧</kbd> +
  <kbd>A</kbd>
- you can now set **custom stylesheets** to **change the look of QOwnNotes**
  with the help of the new scripting method `script.addStyleSheet()`
  (for [#381](https://github.com/pbek/QOwnNotes/issues/381))
    - there is an example for adding custom stylesheets at
      [custom-stylesheet.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-stylesheet.qml)
    - take a look at the
      [scripting documentation](https://www.qownnotes.org/scripting/methods-and-objects.html#add-a-custom-stylesheet)
      for more information
    - you can get the object names from the `*.ui` files, for example
      [mainwindow.ui](https://github.com/pbek/QOwnNotes/blob/develop/src/mainwindow.ui)
    - take a look at [Style Sheet Reference](http://doc.qt.io/qt-5/stylesheet-reference.html)
      for a reference of what styles are available
- you can now also **log to a log file** by checking
 `Enable logging to file` in the *Debug options*
    - if you enable it the path to the log file will be viewed
    - all log messages will be appended to that file
    - there also is a button to clear the log file
- added information whether the notes path exists on the server to the
  debug information
- fixed storing of position and size of non-modal dialogs
- added more Dutch translation (thank you Fabijenna)

## 16.11.7
- the tag color of the note will now be updated correctly if a tag is
  removed from the current note
  (for [#378](https://github.com/pbek/QOwnNotes/issues/378))
- when creating a new tag in the tag tree by right-clicking on a tag and
  clicking `Add tag` the tag will now be shown in the tag tree
- you can now also create tags in the tag tree by right-clicking on the
  `All notes` or `Untagged notes` items and selecting `Add tag`
- added more Polish translation (thank you lucjan)

## 16.11.6
- fixed tag creation (was broken in version 16.11.5) 
  (for [#378](https://github.com/pbek/QOwnNotes/issues/378))

## 16.11.5
- fixed a problem when QOwnNotes didn't detect the macOS updater script
  for some users and added removing of the temporary updater script
  after the update
  (for [#102](https://github.com/pbek/QOwnNotes/issues/102))
- you can now **assign a color to tags**
  (for [#378](https://github.com/pbek/QOwnNotes/issues/378))
    - multiple tags can be selected to assign a color to or to disable
      the color
    - the color will be used as background color of the tag items in the
      tag tree and of the notes in the note list
    - if multiple tags with colors are assigned to a note then the color
      of the first tag with a color is used for the note 
- created a workaround for a sporadic crash that occurs when a network
  connection goes away
- added more Italian, Dutch, Hungarian, French, Spanish and Japanese
  translation (thank you mlocati, Fabijenna, lepascalou, richarson and piyo)

## 16.11.4
- the process for the automatic updates in macOS was improved because of
  problems of some users
  (for [#102](https://github.com/pbek/QOwnNotes/issues/102))
- the `Lock panels` menu entry was renamed to `Unlock panels`, got a new
  icon and now acts accordingly
  (for [#368](https://github.com/pbek/QOwnNotes/issues/368))
- the icon for the `Reload scripting engine` menu entry was changed to
  make it different from the icon of `Reload note folder`
  (for [#368](https://github.com/pbek/QOwnNotes/issues/368))
- when the current note was modified externally the note diff dialog now
  just asks whether to `Accept external changes?`
  (for [#365](https://github.com/pbek/QOwnNotes/issues/365))
    - selecting `No` now really causes external changes to be
      overwritten by the text in the note text edit 
- added more Spanish and Polish translation (thank you richarson and lucjan)

## 16.11.3
- there are now **automatic updates** for **macOS**
  (for [#102](https://github.com/pbek/QOwnNotes/issues/102))
    - in the update dialog you will see a new `Update` button
    - if you press it a script will be loaded after a confirmation to 
      download the latest version of QOwnNotes and update your current
      installation
    - after the update QOwnNotes will be started again
    - please use the checkbox `Fake old version number to trigger update`
      in the `Debug options` and click `Check for updates` in the `Help`
      main menu to test the update process and provide feedback
- added more Italian, Dutch, Hungarian, Portuguese and Japanese
  translation (thank you mlocati, Fabijenna, mlimacarlos and piyo)

## 16.11.2
- QOwnNotes is now built with Qt 5.5 under macOS again because of
  troubles with the Qt 5.7 brewmaster deployment
- changed the download button for Windows in the update dialog to make
  it more clear that it just downloads
- added more Spanish, Italian, French and Japanese translation (thank
  you richarson, mlocati, lepascalou and piyo)

## 16.11.1
- fixed build problems with Qt 5.7 under Windows and Mac OS
  (for [#376](https://github.com/pbek/QOwnNotes/issues/376))
- enabled `HOEDOWN_EXT_MATH_EXPLICIT` in hoedown
  (for [#377](https://github.com/pbek/QOwnNotes/issues/377))

## 16.11.0
- QOwnNotes is now built with Qt 5.7 under OS X and Windows instead of Qt 5.5
- added information about the system environment to the debug settings dump
- there are now **automatic updates** for **Windows**
  (for [#376](https://github.com/pbek/QOwnNotes/issues/376))
    - in the update dialog you will see a new `Update` button
    - if you press it the latest version of QOwnNotes will be downloaded
      and an updater script will uncompress the ZIP file to the folder
      where you were running QOwnNotes from
    - all temporary files for the update will be deleted
    - after the update QOwnNotes will be started again
        - QOwnNotes will be started in portable mode again if it was in
          portable mode before
- there is a new `Debug options` page in the settings
  (for [#376](https://github.com/pbek/QOwnNotes/issues/376))
    - use the checkbox `Fake old version number to trigger update` and
      click `Check for updates` in the `Help` main menu to test the
      update process so you can provide feedback
- added more Spanish, Italian, Dutch, Hungarian, Polish, French
  and Japanese translation (thank you richarson, mlocati, Fabijenna,
  lucjan, lepascalou and piyo)

## 16.10.10
- the controls in the log panel are now hidden by default and can be
  accessed via a context menu
    - you can also clear the log from the context menu
- added an information text about color schemas to the dark mode settings
- improved the search for key sequences in the shortcut settings
- the todo dialog is not modal any more and doesn't hinder you from 
  using the application main window 
  (for [#275](https://github.com/pbek/QOwnNotes/issues/275))
    - you can also open as many todo dialogs as you want now
- added more Russian, Spanish, Italian, Portuguese, Dutch, Hungarian
  and Japanese translation (thank you frostyland, richarson, mlocati,
  mlimacarlos, Fabijenna and piyo)

## 16.10.9
- you can now choose to **scale images down** when inserted into notes
  in the general settings to save disk space
  (for [#268](https://github.com/pbek/QOwnNotes/issues/268))
    - you can set a maximum height and width of the images
- you can now **search for text** in the **settings dialog** to find the
  page you need more easily
- reloading the script engine in the script settings now also stores the
  enabled state of the script engine 
  (for [#371](https://github.com/pbek/QOwnNotes/issues/368))
- the custom actions will be loaded correctly to the custom action
  toolbar when leaving the settings dialog with the `Ok` button 
  (for [#371](https://github.com/pbek/QOwnNotes/issues/368))
- tried to fix a database folder problem under Windows
  (for [#373](https://github.com/pbek/QOwnNotes/issues/373))
- the note folder menu and several menu actions are now disabled in the
  toolbar editor because they will not work in the toolbars
  (for [#371](https://github.com/pbek/QOwnNotes/issues/368))
- added more Russian, Spanish, German, French, Italian, Dutch, Hungarian
  and Japanese translation (thank you frostyland, richarson, bigant,
  lepascalou, mlocati, Fabijenna and piyo)

## 16.10.8
- tried to fix some logging problems that could cause a crash when the
  app quits under OS X
  (for [#371](https://github.com/pbek/QOwnNotes/issues/371))
- added a new `Panels` menu to the windows menu for showing and hiding
  panels (for [#366](https://github.com/pbek/QOwnNotes/issues/366))
- added a new `Toolbars` menu to the windows menu for showing and hiding
  toolbars (for [#366](https://github.com/pbek/QOwnNotes/issues/366))
- the note subfolder panel now remembers if the user originally wanted
  it to be visible if the visibility was set with the new Panels menu
  (for [#370](https://github.com/pbek/QOwnNotes/issues/370))
    - this works even if the note subfolder panel was turned off because
      the current note folder has subfolders disabled
- removed custom toolbars will now not be present again after restart of
  the application
- the `Show toolbar` menu entry now also shows and hides custom toolbars
- added more Italian, Arabic, Dutch, Hungarian and Japanese translation
  (thank you mlocati, yousufinternet, Fabijenna and piyo)

## 16.10.7
- more toolbar editing fixes and improvements
    - fixed a problem with recurring custom toolbar names when creating
      a new one in the settings
    - editing of the window and custom action toolbars is now disabled
      because they are modified by the application itself too often
    - double-clicking on the menu item list now adds the current item
    - double-clicking on the toolbar item list now removes the current item
    - now the user is asked for permission to remove a toolbar
    - the tool buttons are now push buttons to look less crappy under OS X
- added more Italian, Portuguese, Dutch, Hungarian, Spanish and Japanese
  translation (thank you mlocati, mlimacarlos, Fabijenna, richarson and piyo)

## 16.10.6
- the **buttons** in the **toolbars** can now be **customized**
  (for [#343](https://github.com/pbek/QOwnNotes/issues/343))
    - there is a new `Toolbars` page in the settings to add buttons from
      the main menu to toolbars, move buttons around or remove buttons
    - you can as well **add custom toolbars**
    - if you have messed the toolbars up you can also reset the toolbars
      to the default
    - there are still some issues, like that the window toolbar can't be
      modified because of the magic that is happening there
- tried to fix the openSUSE Tumbleweed repository package by using
  `libqt5-qtdeclarative` instead of (the missing) `libQt5Declarative5`
    - feedback is welcome
- added more Italian, French, Portuguese, Polish, Dutch, Russian,
  Japanese and Spanish translation (thank you mlocati, bdouxx,
  mlimacarlos, lucjan, Fabijenna, frostyland, piyo and richarson)

## 16.10.5
- added a new scripting method `tagCurrentNote()` to tag the current note
  (for [#351](https://github.com/pbek/QOwnNotes/issues/351))
    - there is an example for creating a custom action to add a tag
      `favorite` to the current note at
      [favorite-note.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/favorite-note.qml)
- the status messages now all start with an uppercase letter
- there now is a checkbox for ignoring of non-VTODO calendars in the
  todo settings that is turned on by default (for @binerf on GitHub)
- added more Italian, French, Portuguese, Polish, German, Hungarian and
  Dutch translation (thank you mlocati, lepascalou, mlimacarlos, lucjan,
  bigant and Fabijenna)

## 16.10.4
- fixed the left and right margin of the note tag panel
- fixed the restoring of the focus after switching workspaces
  (for [#343](https://github.com/pbek/QOwnNotes/issues/343))
- added an example for custom styling of html in the note preview at
  [preview-styling.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/preview-styling.qml)
  (for [#356](https://github.com/pbek/QOwnNotes/issues/356))
- the log dialog is now a dockable panel that can be placed everywhere
- fixed a problem with showing the title bar of floating panels after
  application start
- fixed a problem where the note folder select box was hidden
- fixed an image preview problem and problem with the notes path under
  Windows (for [#357](https://github.com/pbek/QOwnNotes/issues/357))
- the subfolder dock widget is now turned on or off according to whether
  the subfolders are enabled or not when the workspaces are switched
  (for [#343](https://github.com/pbek/QOwnNotes/issues/343))
- attempted to fix a tagging portability problems between Windows and
  other operating systems
  (for [#358](https://github.com/pbek/QOwnNotes/issues/358))
- added more Arabic, Japanese, Italian, French, Portuguese, Polish,
  Spanish, German, Hungarian and Dutch translation (thank you
  yousufinternet, piyo, mlocati, lepascalou, mlimacarlos, lucjan, guijan
  and Fabijenna)

## 16.10.3
- fixed a problem when the note edit panel was hidden before entering
  the distraction free mode
  (for [#343](https://github.com/pbek/QOwnNotes/issues/343))
- fixed a problem with the restoring of the last workspace correctly
  when starting the app maximized
- the note folder combobox and the note search line edit now have their
  own movable panels
  (for [#343](https://github.com/pbek/QOwnNotes/issues/343))
    - as a consequence the `Show note folders in main area` checkbox in
      the interface settings was removed
    - if the note folder combobox was hidden while the `Select note folder`
      shortcut in the main menu was pressed the panel will be hidden
      again after the combobox was closed
    - you have to move the two new panels to their proper places if you
      have already used version 16.10.2
- decreased the possible minimal height of the note tag panel
- added more Japanese, Italian, French and Polish translation (thank you
  piyo, mlocati, lepascalou and lucjan)

## 16.10.2
- all **panels** are now **dockable**
  (for [#343](https://github.com/pbek/QOwnNotes/issues/343))
    - if you turn off panel locking you can **rearrange panels** as
      you like, they can even **float**, stack or be turned off
    - there is a button to show all panels in case you lost some
        - you can also right click on the toolbar to turn on and off
          panels individually
    - you can create, remove and rename different **workspaces** and 
      switch between them
    - there is a button to **switch between the last two workspaces**
      for example to toggle between edit and preview if you don't want
      to show both panels
    - as a consequence of the new workspace feature the panel visibility
      checkboxes and the different panel placement settings were removed
- added more Arabic, Japanese, Italian, French, Portuguese, Russian,
  Polish and Spanish translation (thank you yousufinternet, piyo,
  mlocati, lepascalou, mlimacarlos, frostyland, lucjan and richarson)

## 16.10.1
- QOwnNotes is now available in **Arabic language** (a big thank you to
  yousufinternet)
- moved the preview font settings to a new page in the settings to make
  more room in the interface settings
- there now is a new checkbox `Use internal icon theme instead of system
  icon theme` in the interface settings
  (for [#331](https://github.com/pbek/QOwnNotes/issues/331))
- there now is a software repository for **openSUSE Leap 42.2**
    - please visit the
      [openSUSE installation page](https://www.qownnotes.org/installation#openSUSE)
      for instructions
- the Ubuntu Linux 14.04 package is now built with Qt 5.6.1
    - please visit the
      [Ubuntu Linux installation page](https://www.qownnotes.org/installation#Linux)
      for more information
- the preview will now only show the decrypted content of a note if the
  `edit encrypted note` button was pressed
- added more Portuguese, Italian, French, Polish and German translation
  (thank you mlimacarlos, mlocati, lepascalou, lucjan and Sqashtrash)

## 16.10.0
- you now can use your **custom method** to **encrypt and decrypt notes**
  via the scripting engine
  (for [#334](https://github.com/pbek/QOwnNotes/issues/334))
    - you can use the new scripting hook `encryptionHook()` do implement
      your own encryption
    - there are examples to encrypt notes with **[Keybase.io](https://keybase.io)**
      ([encryption-keybase.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/encryption-keybase.qml)),
      **PGP** ([encryption-pgp.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/encryption-pgp.qml)) or
      do a trivial "encryption" with ROT13
      ([encryption-rot13.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/encryption-rot13.qml))
    - you can disable the password dialog with `script.encryptionDisablePassword()`
    - to start an external synchronous process you can use `script.startSynchronousProcess()`
    - to check on which platform the script is running you can use
      `script.platformIsLinux()`, `script.platformIsOSX()` and `script.platformIsWindows()` 
    - for more information on the new commands please take a look at the 
      [scripting documentation](https://www.qownnotes.org/scripting/methods-and-objects.html)
- added some fixes to the demo script
  [insert-headline-with-link-from-github-url.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/insert-headline-with-link-from-github-url.qml)
- the menu entry `Show tag pane under navigation pane` was replaced by
  `Show note list under tag panel` where note list and tag pane are swapped
  (for [#340](https://github.com/pbek/QOwnNotes/issues/340))
- added more Portuguese, Russian, Japanese, Spanish, Italian, French and
  German translation (thank you mlimacarlos, Maboroshy, piyo,
  richarson, mlocati, lepascalou and bigant)

## 16.09.17
- changed the IRC channel link to a fancier web client and added a link
  to the [qownnotes Gitter room](https://gitter.im/qownnotes/qownnotes)
- fixed a problem with special characters when auto-completing text
  with `Ctrl + Space`
- there a now is a new scripting method `script.noteTextEditWrite(text)`
  to write text to the note text edit
  (for [#339](https://github.com/pbek/QOwnNotes/issues/339))
    - for more information please take a look at the 
      [noteTextEditWrite documentation](https://www.qownnotes.org/scripting/methods-and-objects.html#write-text-to-the-note-text-edit)
- there a now is a new scripting method `script.noteTextEditSelectedText()`
  to read the selected text from the note text edit
  (for [#339](https://github.com/pbek/QOwnNotes/issues/339))
    - for more information please take a look at the 
      [noteTextEditSelectedText documentation](https://www.qownnotes.org/scripting/methods-and-objects.html#read-the-selected-text-in-the-note-text-edit)
- added more Portuguese, Polish, Italian and Russian translation (thank
  you mlimacarlos, lucjan, mlocati and frostyland)

## 16.09.16
- added RTL support for the markdown preview
  (for [#335](https://github.com/pbek/QOwnNotes/issues/335))
    - you can turn it on in the interface settings
- you now can **delete orphaned images** in a dialog with an image preview
  (for [#204](https://github.com/pbek/QOwnNotes/issues/204))
    - you will find a new menu entry in the edit menu to open the dialog 
- added more Portuguese, Polish, Italian and German translation (thank
  you mlimacarlos, lucjan, mlocati and bigant)

## 16.09.15
- you can now not only solve equations with `Ctrl + Space` but also 
  **autocomplete** the **currently written word** if it was already
  used in the note
- added more French and German translation (thank you lepascalou and bigant)

## 16.09.14
- added some missing translations
  (for [#333](https://github.com/pbek/QOwnNotes/issues/333))
- more **Evernote note import** features
  (for [#332](https://github.com/pbek/QOwnNotes/issues/332))
    - some instances where images weren't detected were fixed
    - images that are not used in the note text can now also be imported
    - code block import
- added more Portuguese, Russian, Polish, Spanish, Italian, Hungarian
  and Dutch translation (thank you mlimacarlos, frostyland, lucjan,
  richarson, asoliverez, mlocati and Fabijenna)

## 16.09.13
- more **Evernote note import** features
  (for [#332](https://github.com/pbek/QOwnNotes/issues/332))
    - the note folder is now reloaded when closing the import dialog
      if notes were imported
    - you can now also import the images of your notes
    - some line break improvements were made
- added more Portuguese, Japanese, Italian, Hungarian and Dutch
  translation (thank you mlimacarlos, piyo, vpistis, mlocati and
  Fabijenna)

## 16.09.12
- the export menu entries in the note menu were moved to a `Export`
  sub-menu to declutter the main menu
- first alpha version of the **Evernote note import**
  (for [#332](https://github.com/pbek/QOwnNotes/issues/332))
    - export an `enex` file (export notes) from your Evernote desktop client
    - click `Import / Import notes from Evernote` in the QOwnNotes main menu
    - please give feedback on how it worked out in above issue
- added more Portuguese, French, Japanese, Polish, German and Italian
  translation (thank you mlimacarlos, lepascalou, piyo, lucjan, bigant
  and mlocati)

## 16.09.11
- disabled the icon of the the share menu entry in OS X 
- `QOwnNotesPortable.exe` was removed from the Windows release of
   v16.09.10, because it wasn't clear if the BAT to EXE "converter" it
   was created with added malware
  (for [#236](https://github.com/pbek/QOwnNotes/issues/236))
   - please use `QOwnNotesPortable.bat` instead
- the item height of the navigation tree widget will now also be set by
  the `List and tree item height` setting in interface settings
  (for [#322](https://github.com/pbek/QOwnNotes/issues/322))
- added access keys for the missing main menu items
  (for [#329](https://github.com/pbek/QOwnNotes/issues/329))
- now a better job is done at syncing the scrollbars after toggling
  between edit and preview
  (for [#328](https://github.com/pbek/QOwnNotes/issues/328))
- fixed setting of wrong preview font on for start of QOwnNotes if the
  settings dialog is used in the welcome dialog
  (for [#325](https://github.com/pbek/QOwnNotes/issues/325))
- added more Portuguese, Russian, Japanese, Polish, German, Hungarian
  and Dutch translation (thank you mlimacarlos, frostyland, piyo,
  lucjan and Fabijenna)

## 16.09.10
- updated the monochromatic tray icon (thanks a lot @Maboroshy)
- in **portable mode** the note folders will now be automatically stored
  relative to the `Data` folder so that the correct note folders will be
  loaded regardless where your QOwnNotes installation is currently located
  (for [#236](https://github.com/pbek/QOwnNotes/issues/236))
    - for Windows there now is a `QOwnNotesPortable.exe` in your release
      path to start QOwnNotes in portable mode
    - there now is a new page in the settings to show more information
      about the portable mode
- added more Italian, Portuguese, Russian, French, Polish, Hungarian and
  Dutch translation (thank you mlocati, mlimacarlos, Maboroshy,
  lepascalou, lucjan and Fabijenna)

## 16.09.9
- you can now set and reset the item height of the tree widgets and
  lists of the main window in the interface settings
  (for [#322](https://github.com/pbek/QOwnNotes/issues/322))
    - this is handy if you are using QOwnNotes on a touch display
- there now is a button to reset the toolbar size in the interface settings
- there now is a **portable mode**, for example to carry around
  QOwnNotes on an USB stick (work in progress)
  (for [#236](https://github.com/pbek/QOwnNotes/issues/236))
    - it will be activated if you run QOwnNotes with the parameter
      `--portable`
    - the internal sqlite database and the settings will be stored
      inside a `Data` folder at the binary's location
    - the settings will be stored in an ini file
    - TODO
        - special handling for note folders, so that they always are
          read from the `Data` folder
        - a batch file for Windows to run QOwnNotes in portable mode
- added more Polish translation (thank you lucjan)

## 16.09.8
- added a button to share a color schema you created in the settings
- took better care that no settings are getting stored after the
  `Clear app data and exit` button was pressed in the settings and
  fixed a problem where the database file wasn't deleted under Windows
  (for [#318](https://github.com/pbek/QOwnNotes/issues/318))
- added more Portuguese, Russian, Chinese, Polish, Hungarian and Dutch
  translation (thank you mlimacarlos, frostyland, lucjan, nkzk and
  Fabijenna)

## 16.09.7
- a lot of text case changes have been made
- added more debug output to the debug settings
- fixed a problem with note subfolders when changing the path of the
  currently active note folder in the settings
  (for [#318](https://github.com/pbek/QOwnNotes/issues/318))
- added more Italian, Portuguese, Russian, Spanish, French, German,
  Chinese, Japanese, Polish, Hungarian and Dutch translation (thank you
  mlocati, mlimacarlos, frostyland, richarson, lepascalou, nkzk, piyo,
  lucjan and Fabijenna)

## 16.09.6
- fixed a possible crash if the option to use a custom CalDAV server was
  selected in the todo settings, but no server url was entered
- only calendars that can hold todo items will now be shown in the
  calendar list in the todo settings
  (for [#312](https://github.com/pbek/QOwnNotes/issues/312))
- the completed status of ical items will now also be taken into account
  then checking for completed todo items on your CalDAV server
  (for [#312](https://github.com/pbek/QOwnNotes/issues/312))
- because of the changes made to the viewing of calendar names you now
  have to select them again in the todo settings
- now an error message is shown if the username or password to your
  CalDAV server is incorrect
- when automatically checking for application updates the update dialog
  will now be refreshed even if it is already open if there is an even
  newer version available
  (for [#102](https://github.com/pbek/QOwnNotes/issues/102))
    - this should prevent the user from downloading an old version and
      getting a new update dialog after updating and starting the
      application again
- added more Italian, Portuguese, Russian, Hungarian and Dutch
  translation (thank you mlocati, mlimacarlos, frostyland and Fabijenna)

## 16.09.5
- more todo list features
  (for [#312](https://github.com/pbek/QOwnNotes/issues/312))
    - storing of additional information to a task, such as description,
      priority and reminder date for custom CalDAV servers (this was
      tested with [Fruux](https://fruux.com))
    - the display name of the calendar will now be shown instead of the
      hash of the calendar
    - there now is a button to clear your calendar item cache in the
      todo settings, so that all todo list items will be reloaded from
      your server the next time you open your todo dialog
    - fixed a problem where an error message is shown every time you
      open your settings dialog when you don't have entered a ownCloud
      server name or when you switch to a the custom CalDAV server
      and have no server name entered yet in the todo settings
- added more Japanese, Portuguese and Polish translation (thank you
  piyo, mlimacarlos and lucjan)

## 16.09.4
- you can now also manage **todo lists** from a **custom CalDAV server**
  (for [#312](https://github.com/pbek/QOwnNotes/issues/312))
    - this should already work
        - listing of all calendars in the settings dialog
        - listing of all tasks of a calendar in the todo dialog
        - creating new tasks
        - removing tasks
        - setting tasks to completed / uncompleted
    - work in progress
        - showing the calendar names instead of the hashes from the server
        - not showing an error message when switching to CalDAV in the
          settings if no server url is entered
        - storing additional information to a task, such as description,
          priority and reminder date
        - showing a better warning dialog if CalDAV username / password
          are wrong, currently there only is a warning in the log dialog
- the automatic note link url renaming method of v16.09.3 will now not
  just search for the url but trying to detect markdown links to prevent
  the wrong links from being renamed
  (for [#313](https://github.com/pbek/QOwnNotes/issues/313))
- added a new monochromatic tray icon
  (for [#306](https://github.com/pbek/QOwnNotes/issues/306),
  thanks a lot @Maboroshy)
- renamed the `tag pane` to `tag / subfolder pane` in the menu for more
  clarity
- added more Italian, Portuguese, Polish, German, Spanish, Hungarian
  and Dutch translation (thank you mlocati, mlimacarlos, kamilko,
  richarson and Fabijenna)

## 16.09.3
- the tag tree will now be reloaded if a tag is moved to another location
  (for [#311](https://github.com/pbek/QOwnNotes/issues/311))
- when renaming a note in QOwnNotes and a link to that note was found in
  other notes there now a dialog will be opened to ask if you want to
  rename the urls to the note in all found notes
  (for [#313](https://github.com/pbek/QOwnNotes/issues/313))
    - this works if the note was renamed by modifying the first line of
      the note or (if you turned that on) by changing the note name in
      the note list
- added more Spanish translation (thank you richarson)

## 16.09.2
- fixed the tag tree reloading if a tag is added to or removed from a
  note in the note edit or via bulk tagging 
  (for [#311](https://github.com/pbek/QOwnNotes/issues/311))
- attempted to fix the start problems of the snap version of QOwnNotes
  (popey, thank you for mentioning) 
- added more Italian, Portuguese, Polish and Russian translation
  (thank you vpistis, mlimacarlos, lucjan, kamilko and frostyland)

## 16.09.1
- there now is an option in the windows menu to start the application 
  hidden if the tray icon is shown
  (for [#307](https://github.com/pbek/QOwnNotes/issues/307))
- updated Hoedown (markdown to html library) to version 3.0.7

## 16.09.0
- fixed a possible crash when opening the settings dialog from a
  message dialog
- QOwnNotesAPI was updated to version 16.09.0
    - fixed a problem when the ownCloud or Nextcloud server didn't
      provide the path of a trashed note and thus no note was shown in
      the trash dialog of QOwnNotes
    - increased `max-version` to `9.2`
    - switched to rolling release version numbers
- several fixes in the snap version of QOwnNotes
- tried to make the tag button area a tiny bit smaller
- added more Italian, Chinese, Portuguese, Polish, Russian, German,
  Spanish, Hungarian and Dutch translation (thank you vpistis,
  a514917345, mlimacarlos, lucjan, frostyland, bigant, richarson
  and Fabijenna)

## 16.08.20
- if you have note subfolders activated for a note folder there now is
  a new menu entry `Find notes in all subfolders` in the note menu
  (for [#305](https://github.com/pbek/QOwnNotes/issues/305))
    - triggering it jumps to `All notes` in the note subfolder tree 
      and triggers a `Find note`
- if there are more tags assigned to a note than the width of the edit
  pane allows there now will be used a scrollbar to scroll through the 
  tags, so that the width of the edit pane can still be small
  (for [#308](https://github.com/pbek/QOwnNotes/issues/308))
- more **editor color scheme improvements**
  (for [#280](https://github.com/pbek/QOwnNotes/issues/280))
    - you are now able the adapt the font size for each highlighting item
        - you will be able to see the adapted font size in the item preview
    - the highlighting items were reordered
    - the schema `Default` was renamed to `Light`
- you can now enable a dark mode system tray icon in the interface settings
  (for [#306](https://github.com/pbek/QOwnNotes/issues/306))
- added more French, Italian, Japanese, Portuguese, Polish, Hungarian
  and Dutch translation (thank you Loelo, mlocati, piyo, mlimacarlos,
  Snipirise and Fabijenna)

## 16.08.19
- finally there are configurable **editor color schemes**
  (for [#280](https://github.com/pbek/QOwnNotes/issues/280))
    - there are two default schemes called `Light` and `Dark` that
      are mimicking the current default mode and the dark mode colors
    - you can create as many schemes as you like by copying and
      modifying them
    - you can change the foreground color, background color, bold state,
      italic state and underline state
    - you can export schemes to files and import them again from 
      these files
        - if you come up with a good and proven color theme, that should
          be included by default, please let me know 
- added more Spanish, Russian and German translation (thank you
  richarson, frostyland and bigant)

## 16.08.18
- lots of text corrections in the main window and the settings dialog
- the navigation pane can now also show unrecognized headlines, like
  `# Header [link](http://url)`
  (for [#302](https://github.com/pbek/QOwnNotes/issues/302))
- the settings page was redone to get more structure in it
    - there now is a list on the left to select the proper settings page
- added a donate menu item to the help menu
    - since I've been repeatably asked where to deposit donations there
      now is a donation page at [donations](https://www.qownnotes.org/donate)
- added more Italian, Portuguese, Polish, Dutch and Hungarian
  translation (thank you mlocati, mlimacarlos, lucjan and Fabijenna)

## 16.08.17
- when creating custom action buttons you now can define a button icon
  from your freedesktop theme icons or a local file
  (for [#297](https://github.com/pbek/QOwnNotes/issues/297))
    - for more information please take a look at the 
      [registerCustomAction documentation](https://www.qownnotes.org/scripting/methods-and-objects.html#register-a-custom-action)
    - you will find a list of icons at
      [freedesktop icons](https://specifications.freedesktop.org/icon-naming-spec/icon-naming-spec-latest.html)
- fixed a problem with selecting text
  (for [#300](https://github.com/pbek/QOwnNotes/issues/300))
- fixed a problem with the one column mode while in vertical preview
  layout mode (for [#295](https://github.com/pbek/QOwnNotes/issues/295))
- you can now turn off the refreshing of the note list on external note
  folder changes completely in the general settings if for example you
  have so many notes, that even the checking for file-time and file-size
  changes is taking too long
  (for [#291](https://github.com/pbek/QOwnNotes/issues/291))
    - use this at your own risk!
- added more Italian, Spanish and Polish translation (thank you mlocati,
  richarson and lucjan)

## 16.08.16
- you are now able to define **custom actions** that will be shown as
  **menu entry** in the scripting menu and optionally in the toolbar
  as **button** (for [#296](https://github.com/pbek/QOwnNotes/issues/296)
  and for [#297](https://github.com/pbek/QOwnNotes/issues/297))
    - there is an example for opening the current note with KWrite
      and to create a new note with a custom text at
      [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-actions.qml)
    - there is a small documentation about the function
      `registerCustomAction` and other functions in the
      [scripting documentation](https://www.qownnotes.org/scripting/methods-and-objects.html#register-a-custom-action)
    - you can also assign shortcuts for your custom actions in the 
      shortcut settings
- added new methods to the scripting engine
    - you can now also get the full file path of the current note
      with `script.currentNote().fullNoteFilePath`
    - you can now register a custom action with `script.registerCustomAction()`
    - you can now create a new note with `script.createNote()`
    - you can now access the clipboard with `script.clipboard()`
    - take a look at the [scripting documentation](https://www.qownnotes.org/scripting/methods-and-objects.html)
      for more information
- there now is a new example to create a note from a 5pm email with a 
  custom action at
  [note-from-5pm-mail.qml](https://github.com/pbek/QOwnNotes/tree/develop/docs/scripting/examples/note-from-5pm-mail.qml)
- the warning about tags when copying or moving notes to a note
  subfolder will now be only displayed if one of the selected notes has
  a tag assigned (for [#289](https://github.com/pbek/QOwnNotes/issues/289))
- added more Italian, Portuguese, Dutch and Hungarian translation
  (thank you mlocati, mlimacarlos and Fabijenna)

## 16.08.15
- fixed a problem under OSX where the shortcuts will not be viewed
  in the menu and the settings
- fixed a problem where the note list and the note subfolder list where
  not refreshed when a note or a note subfolder was removed
  (for [#289](https://github.com/pbek/QOwnNotes/issues/289))
- you are now able to show the tag pane under the navigation pane with
  a checkbox in the window menu (for nothinbuttherain on 
  [reddit](https://www.reddit.com/r/linux/comments/4xto4y/4_open_source_alternatives_to_evernote/#thing_t1_d6jlppu))

## 16.08.14
- when the notes are reloaded the note list and the note subfolder tree
  will now only be refreshed in the user interface if a note or a note
  subfolder was really modified
  (for [#291](https://github.com/pbek/QOwnNotes/issues/291))
- fixed a problem with copying/moving notes from a note subfolder to the
  main note folder where not all changes were viewed in the user interface 
  (for [#289](https://github.com/pbek/QOwnNotes/issues/289))
- added more Spanish, Italian, German and Portuguese translation
  (thank you richarson, mlocati, bigant and mlimacarlos)

## 16.08.13
- the **reloading speed** of the **note folder** and note subfolders has
  been **dramatically improved**
  (for [#291](https://github.com/pbek/QOwnNotes/issues/291))
    - when a reload is triggered now only modified notes and folders 
      will be loaded again
    - a reload happens for example if a file, that wasn't watched by 
      QOwnNotes was modified outside the application
    - when the note folder is reloaded manually with <kbd>Ctrl</kbd> +
      <kbd>⇧</kbd> + <kbd>R</kbd> every note and note folder will be reloaded
- it's now possible to copy/move notes from a note subfolder to the note 
  folder (for [#289](https://github.com/pbek/QOwnNotes/issues/289))
- when in *one column mode* the other panes will now not be hidden when 
  QOwnNotes is restarted
  (for [#295](https://github.com/pbek/QOwnNotes/issues/295)
    - they still will be hidden when the mode is manually entered 
- added more Spanish, Italian, Russian, Portuguese, French, Hungarian 
  and Dutch translation (thank you eemantsal, mlocati, frostyland,
  mlimacarlos, Loelo and Fabijenna)

## 16.08.12
- there now is a new item **`All notes`** in the **note subfolder** 
  tree that you can use to search for notes in all subfolders
  (for [#186](https://github.com/pbek/QOwnNotes/issues/186))
    - if you create a new note while having that item selected the note 
      will be created in the root note folder
- when splitting notes now a link to the new note will be added at the 
  end of the old note 
  (for [#290](https://github.com/pbek/QOwnNotes/issues/290))
- in a note subfolder now links to notes in the same subfolder work
    - when linking to notes only notes from the current subfolder will 
      be viewed in the link dialog
- if you are in the last line and press cursor down the cursor will now
  jump to the end of the line (for jd6661 on the IRC channel)
    - if you are in the first line and press cursor up the cursor will 
      now jump to the start of the line
- added more Spanish, Italian, Japanese, Portuguese, Hungarian and Dutch
  translation (thank you richarson, vpistis, piyo, mlimacarlos and Fabijenna)

## 16.08.11
- there now is a new shortcut <kbd>Alt</kbd> + <kbd>⇧</kbd> + <kbd>X</kbd>
  to **split the current note** into two notes at the current cursor position
  (for [#290](https://github.com/pbek/QOwnNotes/issues/290))
    - the name of the current note plus the current timestamp will be
      used as name for the new note
        - the new note will be tagged with the same tags as the old note
- note subfolders are now turned off by default when a new note folder 
  is created
- fixed a build problem with MSVC19 (thank you @rwiesenfarth)
  (for [#292](https://github.com/pbek/QOwnNotes/issues/292))
- added more Spanish, Italian, Japanese, Hungarian and Dutch translation
  (thank you richarson, mlocati, kamilko, piyo and Fabijenna)

## 16.08.10
- now you can **copy and move notes to a note subfolder** from the note 
  context menu (for [#289](https://github.com/pbek/QOwnNotes/issues/289))
- there now is a checkbox in the interface settings where you can 
  **disable the markdown highlighting**
  (for [#197](https://github.com/pbek/QOwnNotes/issues/197))
- added more Spanish, Polish, Hungarian and Dutch translation
  (thank you richarson, kamilko and Fabijenna)

## 16.08.9
- all **shortcuts** from the main menu should now also **work** if your
  desktop environment doesn't show the main menu bar (like **Unity** does)
  (for [#288](https://github.com/pbek/QOwnNotes/issues/288))
- there now is a new shortcut <kbd>Ctrl</kbd> + <kbd>⇧</kbd> + 
  <kbd>M</kbd> to **hide the main menu bar** 
  (for [#287](https://github.com/pbek/QOwnNotes/issues/287))
    - if the menu bar is hidden a new button will be added to the window 
      toolbar to show the main menu bar again
- fixed an UI glitch under OS X where the `add tag` button stuck to 
  the right corner
- added more Spanish and Hungarian translation (thank you richarson 
  and Fabijenna)

## 16.08.8
- when the one column mode is turned on we now try to make sure the 
  note edit pane is visible
  (for [#281](https://github.com/pbek/QOwnNotes/issues/281))
    - when the one column mode is turned off we now try to make sure 
      the navigation pane is visible again
- fixed a problem where notes were filtered when switching note folders 
  while there was a text in the note search line edit
- added a missing demo markdown file
- added more Russian, Dutch and Italian translation (thank you 
  frostyland, Fabijenna and mlocati)

## 16.08.7
- the one column mode is now turned off by default, it was
  accidentally turned on by default
  (for [#281](https://github.com/pbek/QOwnNotes/issues/281))
	- the note edit pane will now be automatically turned on
	  when entering the one column mode

## 16.08.6
- there now is a **one column minimal mode**, like NVAlt has
  (for [#281](https://github.com/pbek/QOwnNotes/issues/281))
    - you can activate it in the window menu
    - it turns off the navigation pane, the tag pane and the preview 
      pane and puts the edit pane beyond the note list
    - the setting will be remembered when you exit the app
    - entering the distraction free mode turns the one column mode off
- some dynamically set shortcuts are now set at the right time
  so they don't show up as custom shortcuts
  (for [#194](https://github.com/pbek/QOwnNotes/issues/194))
    - the note edit context menu shortcuts now also load the custom
      shortcut settings
- changed some icons in the font settings
- the `Add tag` button now sicks to the right of the note edit pane
  (for jd6661 on the IRC channel) 
- added more Italian translation (thank you mlocati)

## 16.08.5
- fixed a crash when opening the link dialog
- resetting of shortcuts works now
  (for [#282](https://github.com/pbek/QOwnNotes/issues/282))
- added more Portuguese, German and Japanese translation (thank you
  mlimacarlos, bigant and piyo)

## 16.08.4
- fixed an issue with the positioning of dialogs if there was
  no stored geometry for them
  (for [#279](https://github.com/pbek/QOwnNotes/issues/279))
    - the geometry (along will the dialog position) will now also be 
      stored if the dialog was moved

## 16.08.3
- there now is a new **Shortcuts** tab in the settings where you can 
  assign custom shortcuts for menu items of the main window
  (for [#194](https://github.com/pbek/QOwnNotes/issues/194))
    - you can also search for shortcuts
- fixed a problem with XFCE where the settings dialog had a geometry
  of 1x1 pixels
  (for [#279](https://github.com/pbek/QOwnNotes/issues/279))
    - additionally now the geometry of all dialogs will be saved when 
      the window is resized and restored when the dialog is opened
- added more Portuguese, Russian, Hungarian, Dutch and Italian
  translation (thank you mlimacarlos, frostyland, Fabijenna, vpistis
  and mlocati)

## 16.08.2
- you can now **share notes** and **remove note shares** on your 
  ownCloud server in the note share dialog
- added more Hungarian, Dutch and Japanese translation (thank you 
  Fabijenna and piyo)

## 16.08.1
- you can now toggle between the note edit mode and the preview with
  <kbd>Alt</kbd> + <kbd>⇧</kbd> + <kbd>P</kbd>
  (for [#269](https://github.com/pbek/QOwnNotes/issues/269))
- there now is a new button to **share your notes** on your ownCloud server
    - currently you can only see if your note is shared publicly and 
      copy the share url from the share dialog
    - actual sharing of notes is work in progress
- added more Portuguese, Russian, Hungarian, Dutch and Japanese
  translation (thank you mlimacarlos, frostyland, Fabijenna and piyo)

## 16.08.0
- tagging, removing tags, moving and copying multiple notes in note 
  subfolders now works correctly if notes with the same name exist 
  in other note subfolders
  (for [#263](https://github.com/pbek/QOwnNotes/issues/263))
- <kbd>Ctrl</kbd> + <kbd>⇧</kbd> + <kbd>U</kbd> now toggles the case 
  of the selected text
    - the shortcut for checking for updates now moved to 
      <kbd>Alt</kbd> + <kbd>⇧</kbd> + <kbd>U</kbd> 
- added a link to the QOwnNotes markdown cheatsheet to the help menu
    - the markdown file will also be created as a note the first time 
      QOwnNotes starts with an empty note folder
- the scripting hook `handleNewNoteHeadlineHook()` now has a parameter
  to pass the text that would be used to create the headline
  (for [#272](https://github.com/pbek/QOwnNotes/issues/272))
    - the hook will now also be called when using the search line edit 
    - if you are using the hook you have to add the parameter to the
      function in your script, so that the scripting engine will find
      your function call
    - if the headline text was modified by the hook no underline 
      characters `===` will be added automatically, you have to do that
      in your script
    - take a look at the updated example at
      [custom-new-note-headline.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-new-note-headline.qml)
    - you now can use the hook to create your own headline styles, 
      for example using `# headline` instead of `headline`, followed by
      `==========`
        - if you are doing this don't forget to allow the note
          headline to be different from the note filename in the
          general settings
- <kbd>Alt</kbd> + <kbd>⇧</kbd> + <kbd>S</kbd> now adds the `~~`
  characters to symbolize a strikeout of the selected text
  (for [#274](https://github.com/pbek/QOwnNotes/issues/274))
    - there also is a new button in the formatting toolbar for strikeout
    - strikeout now works in the markdown preview

## 16.07.15
- tagging of notes in non-subfolder mode is working again 
  (for [#263](https://github.com/pbek/QOwnNotes/issues/263))
- if showing of note subfolders is turned off in the settings the 
  active note subfolder will now be reset to prevent problems

## 16.07.14
- did a lot of improvements for support of same note names in different 
  note subfolders and general note storing, loading, tagging and
  renaming
  (for [#263](https://github.com/pbek/QOwnNotes/issues/263))
- added support for tagging in note subfolders
  (for [#263](https://github.com/pbek/QOwnNotes/issues/263))
- added more Portuguese, German, Russian, Hungarian, Dutch translation
  (thank you mlimacarlos, bigant, frostyland, Maboroshy and Fabijenna)

## 16.07.13
- quotes will now be shown in the markdown preview
  (for [#261](https://github.com/pbek/QOwnNotes/issues/261))
- no network proxy will now be used by default to prevent issues with 
  the networking stack on some operating systems
  (for [#258](https://github.com/pbek/QOwnNotes/issues/258))
- some more SSL errors are now ignored
  (for [#264](https://github.com/pbek/QOwnNotes/issues/264))
- added more support for same note names in different note subfolders
  (for [#263](https://github.com/pbek/QOwnNotes/issues/263))
    - there still has to be done more here

## 16.07.12
- the expand/collapse state of the items in the note subfolder tree will not 
  be stored while searching and restored when the search field is cleared
  (for [#254](https://github.com/pbek/QOwnNotes/issues/254))
    - after a subfolder was created by the search field it will be cleared
- added more German and Dutch translation (thank you bigant and Telesight)

## 16.07.11
- fixed a character set problem when storing tasks in the todo list dialog
  (for [#256](https://github.com/pbek/QOwnNotes/issues/256))
- improved the demo script `insert-headline-with-link-from-github-url.qml` to
  decode html entities
- added an option to use GitHub line breaks for the debug output in the 
  settings dialog 
- fixed a problem where a subfolder will not be viewed in the note subfolder 
  tree after it was created
- OSX: added a new shortcut <kbd>⌘</kbd> + <kbd>Opt</kbd> + <kbd>V</kbd> to 
  paste notes, images and html from the clipboard
- added more Portuguese translation (thank you mlimacarlos)

## 16.07.10
- added more information to the settings debug tab
- there now are error messages when something went wrong while loading your 
  ownCloud todo lists in the `Todo` tab of the settings dialog
  (for [#229](https://github.com/pbek/QOwnNotes/issues/229))
    - for example keep in mind that the ownCloud calendar service uses case
    sensitive usernames and the ownCloud server itself uses case insensitive
    usernames, so please use the right case in when typing in your username
    in the ownCloud settings tab of QOwnNotes
- when opening a note subfolder in the file manager the folder will be opened
  instead of the parent folder with the subfolder selected
  (for [#246](https://github.com/pbek/QOwnNotes/issues/246))
- fixed troubles with deleting the last note in a note folder or note 
  subfolder (for [#253](https://github.com/pbek/QOwnNotes/issues/253))
- removed the snap part from the default notes folder path to access the 
  notes in the real home directory and not in the snap home directory for 
  Snapcraft builds 
  (for [Snappy playpen issue #145](https://github.com/ubuntu/snappy-playpen/issues/145))
- added more Dutch and German translation (thank you hartwin and bigant)

## 16.07.9
- some debug output was added when the calendar list is reloaded in the 
  settings dialog, because there were some people who had some troubles to 
  connect to their ownCloud calendars
    - please read [My calendars cannot be fetched from my ownCloud server](https://www.qownnotes.org/Knowledge-base/My-calendars-cannot-be-fetched-from-my-ownCloud-server)
      for instructions
- there are now QOwnNotes snap packages available for `amd64`, `i386`, `arm64` 
  and `armhf`
    - please visit the
      [QOwnNotes installation page](https://www.qownnotes.org/installation#snap)
      for instructions
- added more Dutch, Portuguese and Russian translation (thank you hartwin,
  mlimacarlos and Maboroshy)

## 16.07.8
- added another fix to a duplicate note subfolder listing problem
  (for [#243](https://github.com/pbek/QOwnNotes/issues/243))
- you can now also create a note subfolder by typing it into the search field 
  and pressing <kbd>Return</kbd>
- note subfolders deeper than 2nd level can now be used
  (for [#246](https://github.com/pbek/QOwnNotes/issues/246))
- note subfolders can now be renamed in the note subfolder tree
- now the root note folder cannot be removed in the note subfolder tree
  (for [#246](https://github.com/pbek/QOwnNotes/issues/246))
- the folders that are about to be deleted when removing a note subfolder 
  will now be viewed in the confirmation dialog to make it more clear what 
  will happen (for [#246](https://github.com/pbek/QOwnNotes/issues/246))
- the informational text-block with the links to the examples is now always 
  accessible in the scripting settings tab
  (for [#251](https://github.com/pbek/QOwnNotes/issues/251))
- viewing and restoring of note versions from your ownCloud server now works 
  for notes in note subfolders
- viewing trashed notes on your ownCloud server now works in note subfolders,
  as well as restoring them on your server or downloading them directly
- added more Hungarian and Japanese translation (thank you Fabijenna and piyo)

## 16.07.7
- fixed a possible crash after removing the last script in the settings dialog
  (for [#247](https://github.com/pbek/QOwnNotes/issues/247))
- tried to fix an optimization crash that happened sporadically when building 
  the tag tree
- the build issues with snapcraft 2.12.1 are now fixed
  (for [Snappy playpen issue #145](https://github.com/ubuntu/snappy-playpen/issues/145))
- tried to fix the duplicate file and subfolder listings problem under Windows
  (for [#243](https://github.com/pbek/QOwnNotes/issues/243))
- creating a note in a note sub folder will now not trigger a full rebuild of
  the note index (for [#248](https://github.com/pbek/QOwnNotes/issues/248))
   - on some occasions it only works after a first note was created after 
     QOwnNotes was started
- there now is a context menu in the note subfolder tree where you can
  (for [#246](https://github.com/pbek/QOwnNotes/issues/246))
    - create new notes
    - create new subfolders
    - remove selected subfolders
    - show the subfolder in the file manager

## 16.07.6
- more fixes for the duplicate file and subfolder listings problem under OS X
  (for [#242](https://github.com/pbek/QOwnNotes/issues/242))
- the expand/collapsed state settings for the note subfolder tree will now be
  removed if the note folder is removed
- renaming notes in note subfolders should now work without the "externally 
  removed" error message
  (for [#183](https://github.com/pbek/QOwnNotes/issues/183))

## 16.07.5
- fixed a duplicate file and subfolder listings problem
  (for [#242](https://github.com/pbek/QOwnNotes/issues/242))
- the columns in the note subfolder tree will now be resized when a folder was 
  expanded or collapsed
  (for [#183](https://github.com/pbek/QOwnNotes/issues/183))

## 16.07.4
- the **responsiveness of the app's user interface** while loading many notes, 
  note subfolders, tags or tasks was **significantly improved**
- the **expand/collapsed state** in the **note subfolder tree** is now stored 
  and will be restored when the note subfolders are loaded
  (for [#183](https://github.com/pbek/QOwnNotes/issues/183))
- fixed the theme icon of the down button in the search widget of the 
  markdown editor
- there is now is a **snap** `qownnotes` publicly available for QOwnNotes
  (thanks a lot to @dholbach and @kyrofa for the help)
    - snaps are working on many Linux distributions like Arch Linux, Debian, 
      Fedora, Gentoo Linux and Ubuntu
    - please visit the
      [QOwnNotes installation page](https://www.qownnotes.org/installation#snap)
      for instructions
- there now are community-created packages for **CentOS 7** for QOwnNotes 
  (thanks a lot to Jörg Woll)
    - please visit the
      [QOwnNotes installation page](https://www.qownnotes.org/installation#CentOS)
      for instructions
- added more Russian, Italian, Hungarian and German translation (thank you 
  Maboroshy, mlocati, Fabijenna and bigant)

## 16.07.3
- fixed a bug with searching for multiple words like `"more words"` where the
  app didn't respond any more
- the default code font for the note editor and the markdown preview is now the 
  system fixed font instead of Courier
- fixed the install process in the Slackware repository
  (for [#226](https://github.com/pbek/QOwnNotes/issues/226))
- the package signature of the ownCloud app QOwnNotesAPI was fixed
  (for [#238](https://github.com/pbek/QOwnNotes/issues/238))

## 16.07.2
- added more features to the **note sub folder tree**
  (for [#183](https://github.com/pbek/QOwnNotes/issues/183))
    - the tree decoration was removed in the note list since it should not be
      a tree any more
    - improved the column width setup of the note sub folder tree 
- on Linux a `make install` should now install the application after it was 
  built (thank you to Crazy for the improvements on the qmake project file)
- added more Russian, Dutch, Hungarian and German translation (thank you 
  frostyland, Fabijenna and bigant)

## 16.07.1
- added `Letter` as page size for PDF exports under Windows and OS X
  (for [#234](https://github.com/pbek/QOwnNotes/issues/234))
- added more features to the **note sub folder tree**
  (for [#183](https://github.com/pbek/QOwnNotes/issues/183))
    - there are now tooltips and a note count for note sub folder tree items
    - new notes are now created in the correct note sub folder
    - the current note sub folder will now be stored for each note folder, so
      that it gets remembered when note folders are switched
    - you can now search for note sub folders in the note sub folder line edit
- added more Dutch translation (thank you Telesight)

## 16.07.0
- there now is a new experimental **note sub folder tree** instead of the 
  folders and notes in the same note tree
   - notes are filtered by note sub folder when clicked on a note sub folder
   - searching for note sub folders is not implemented yet
- there now is a community maintained **KaOS repository** for QOwnNotes 
  (thanks a lot to @Adurol)
    - please visit the
      [QOwnNotes installation page](https://www.qownnotes.org/installation#KaOS)
      for instructions
- there now is a repository for **SUSE Linux Enterprise 12 SP1 Backports**
    - please visit the
      [QOwnNotes installation page](https://www.qownnotes.org/installation#openSUSE)
      for instructions
- added more Portuguese and Russian translation (thank you mlimacarlos and 
  Maboroshy)

## 16.06.14
- updated the installation instruction for **Linux Mint** and 
  **Ubuntu Linux 14.04**
  (for [#231](https://github.com/pbek/QOwnNotes/issues/231))
	- please visit the 
	  [QOwnNotes installation](https://www.qownnotes.org/installation#Ubuntu) 
	  page for more information
- first pre-alpha release of the **note subfolder tree integration**
  (for [#183](https://github.com/pbek/QOwnNotes/issues/183))
	- per default notes will be shown as a list in a new note tree widget
	- all functionality of the old note list was integrated into the new note 
	  tree and should work as it used to
	- for each configured note folder you can now check a checkbox 
	  `Show subfolders in note list` to show the subfolders of the note folder
	- if the checkbox is checked notes of a note folder and the subfolders are 
	  shown as tree instead of a list
	- editing notes in subfolders should already work in most cases, but you 
	  cannot create new notes in subfolders yet
	- a lot more work has to be done to be able to use tags, media files, 
	  note versioning and trash, moving notes in subfolders, updating the 
	  position of edited notes in the tree and so on
- added more Portuguese and Russian translation (thank you mlimacarlos and 
  frostyland)

## 16.06.13
- there now is a checkbox to disable the status bar in the windows menu
  (for [#230](https://github.com/pbek/QOwnNotes/issues/230))
    - if you have disabled the status bar use the <kbd>Esc</kbd> key to leave 
      the distraction free mode, because the `leave` button is in the status bar
- there now is a **Slackware repository** for QOwnNotes (thanks a lot to 
  @richarson for the help)
  (for [#226](https://github.com/pbek/QOwnNotes/issues/226))
    - please visit the [QOwnNotes installation page](https://www.qownnotes.org/installation#Slackware) for instructions
- fixed some `database not open` errors and possible problems with the database
  (for [#226](https://github.com/pbek/QOwnNotes/issues/226))
- added more Dutch translation (thank you Telesight)
- fixed a wrong string in the settings dialog

## 16.06.12
- a problem with detecting the ownCloud server if it just returns a server 
  code 302 when requesting the server url was fixed in the settings dialog 
- there now is a new scripting hook `handleNewNoteHeadlineHook()`, that is 
  called before a new note is created
  (for [#218](https://github.com/pbek/QOwnNotes/issues/218))
    - it allows you to **modify the headline of a note before it is created**
    - note that you have to take care about a unique note name, otherwise
      the new note will not be created, it will just be found in the note list
    - there is an example at
      [custom-new-note-headline.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/custom-new-note-headline.qml)
- added more Hungarian and Russian translation (thank you Fabijenna and 
  frostyland)

## 16.06.11
- the 100% cpu problem when disabling the markdown highlighter in the settings 
  is now fixed (for [#217](https://github.com/pbek/QOwnNotes/issues/217))
- there are now more dependencies required in the openSUSE packages: 
  `libQt5Sql5-sqlite libQt5Gui5 libQt5Network5 libQt5Widgets5 libQt5Xml5`
  (for [#220](https://github.com/pbek/QOwnNotes/issues/220))
- there now are repositories for **Fedora 24**
    - please visit the [QOwnNotes installation page](https://www.qownnotes.org/installation#Fedora) for instructions
- relative file urls like `[manual](file://../ownCloudUserManual.pdf)` and 
  `<file://../ownCloudUserManual.pdf>` now work in the note text edit and the
   note preview (for [#221](https://github.com/pbek/QOwnNotes/issues/221))
 
## 16.06.10
- if there were no note folders found when QOwnNotes starts they now will 
  generated from the settings at the very beginning, this should prevent some 
  errors (for [#220](https://github.com/pbek/QOwnNotes/issues/220))
- the packages `libQt5Sql5` and `libQt5PrintSupport5` are now required in the 
  QOwnNotes openSUSE packages
  (for [#220](https://github.com/pbek/QOwnNotes/issues/220))

## 16.06.9
- there now is note folder information in the debug dump in the settings dialog
- a fallback for loading the note folder path to prevent `Can't write 
  to "/notes.sqlite"` errors was added
  (for [#219](https://github.com/pbek/QOwnNotes/issues/219) and 
  [#220](https://github.com/pbek/QOwnNotes/issues/220))
- the layout direction for the `next` button in the welcome dialog was
  changed to `left to right` again, because some operating systems showed 
  strange behaviors with `right to left`
  (for [#220](https://github.com/pbek/QOwnNotes/issues/220))

## 16.06.8
- the menu entry `Show in system tray` was renamed to `Show menu bar item` on 
  OS X (for [#214](https://github.com/pbek/QOwnNotes/issues/214))
- the `Show toolbar` menu entry is now checked as long as there is one toolbar
  activated (for [#213](https://github.com/pbek/QOwnNotes/issues/213))
    - if the menu entry it is checked by a user all toolbars will be activated 
      again
- under OS X now a lot of settings keys, that get added by Qt under OS X 
  will be ignored in the debug output of the settings dialog
  (for [#217](https://github.com/pbek/QOwnNotes/issues/217))
- added more Portuguese, Japanese, French, Hungarian and Spanish translation
  (thank you mlimacarlos, piyo, Loelo, Fabijenna and kender)

## 16.06.7
- when doing a text search in your note the selected text is now preset as 
  search text if there is any and there is no other search text filled in
- added more information to the debug tab in the settings
- the todo tab in the settings is now disabled if there are no ownCloud settings
  (for [#208](https://github.com/pbek/QOwnNotes/issues/208))
- the app instance settings are now hidden on OS X since we don't need them 
  there (for [#208](https://github.com/pbek/QOwnNotes/issues/208))
- added more Portuguese and Russian translation (thank you mlimacarlos and 
  frostyland)

## 16.06.6
- you now can use a real **dark mode** on every operating system
  (for [#208](https://github.com/pbek/QOwnNotes/issues/208))
    - turn it on in the interface settings
- very long tag names will now be shortened in the tag remove button
  (for [#210](https://github.com/pbek/QOwnNotes/issues/210))
- added more Hungarian and Spanish translation (thank you Fabijenna and kender)

## 16.06.5
- <kbd>Ctrl</kbd> + <kbd>Space</kbd> to solve simple equations and open links
  now also works under OS X
  (for [#203](https://github.com/pbek/QOwnNotes/issues/203))
- menu icons are now hidden under OSX to be more "conform" 
  (for [#198](https://github.com/pbek/QOwnNotes/issues/198))
- you now can disable the soft wrapping in the note editor and preview in the
  window menu (for [#197](https://github.com/pbek/QOwnNotes/issues/197))
- the creation of empty tags is now disabled
  (for [#197](https://github.com/pbek/QOwnNotes/issues/197))
- added more Portuguese, Russian, Dutch, Japanese and German translation
  (thank you mlimacarlos, frostyland, Telesight, Fabijenna, piyo and bigant)

## 16.06.4
- file names with multiple dots are now supported when inserting a headline 
  from the note filename with <kbd>Ctrl</kbd> + <kbd>⇧</kbd> + <kbd>H</kbd>  
  (for [#206](https://github.com/pbek/QOwnNotes/issues/206))
- you now can open a link at the current cursor position with 
  <kbd>Ctrl</kbd> + <kbd>Space</kbd> 
  (for [#203](https://github.com/pbek/QOwnNotes/issues/203))
- added more Portuguese, Russian, Dutch, Japanese and German translation
  (thank you mlimacarlos, frostyland, Telesight and piyo)

## 16.06.3
- the **highlighting** for text found by the **search for multiple words** 
  feature from version 16.06.2 now also works for those patterns
  (for [#195](https://github.com/pbek/QOwnNotes/issues/195))
- <kbd>Ctrl</kbd> + <kbd>⇧</kbd> + <kbd>H</kbd> now inserts a headline from
  the note filename at the top of the note
  (for [#201](https://github.com/pbek/QOwnNotes/issues/201))
- there now is a new toolbar for the quit button right of the other toolbars, 
  so this button can be disabled
  (for [#197](https://github.com/pbek/QOwnNotes/issues/197))
- you can now set the markdown highlighting interval in the interface settings
  (for [#104](https://github.com/pbek/QOwnNotes/issues/104))
    - setting it to `0` will disable the highlighting
      (for [#197](https://github.com/pbek/QOwnNotes/issues/197))

## 16.06.2
- you now can **search for multiple words** in notes
  (for [#195](https://github.com/pbek/QOwnNotes/issues/195))
    - by default notes that contain every single word will be found, `word1 
      word2` will find all notes that are containing `word1` and `word2`
    - you can search for longer texts by using quotes, `"this word1" word2` 
      will find all notes that are containing `this word1` and `word2`
- there now is a **search history**
  (for [#138](https://github.com/pbek/QOwnNotes/issues/138))
    - your last 100 note searches will be stored and you can access them as 
      completer in the note search line edit
    - for every note folder a different search history will be kept
    - your whole search history will be shown if you press the <kbd>Down</kbd> 
      button in the note search line edit
- added more Chinese translation (thank you hoilc)

## 16.06.1
- the window title will now be updated correctly if the note is renamed 
- added more Spanish translation (thank you guijan)

## 16.06.0
- now the tag list context menu isn't opened if no tags are selected
  (for [#166](https://github.com/pbek/QOwnNotes/issues/166))
- fixed a wrong note count for `All notes` in the tag list
- the item `Untagged notes` in the tag list will now only be viewed if there 
  are untagged notes
- instead of the notification dialog after bulk tagging notes now a status 
  message will be shown
  (for [#166](https://github.com/pbek/QOwnNotes/issues/166))
- added more Portuguese, Russian, German and Japanese translation (thank you 
  mlimacarlos, frostyland, bigant and piyo)

## 16.05.24
- you can now `Show note in file manager` and `Open note in external editor` 
  from the context menu in the note list
  (for [#188](https://github.com/pbek/QOwnNotes/issues/188))
- added tag name auto-completion when adding a tag to the current note
  (for [#166](https://github.com/pbek/QOwnNotes/issues/166))
- there now is a new item in the tag list that allows you to view all untagged 
  notes (for [#166](https://github.com/pbek/QOwnNotes/issues/166))
- added more Portuguese and Japanese translation (thank you mlimacarlos and
  piyo)

## 16.05.23
- it is now possible to add **custom note file extensions** in the settings
  (for [#185](https://github.com/pbek/QOwnNotes/issues/185))
    - these file extensions will be recognized in the note list and you can 
      create new notes with them
    - they will also work with the ownCloud note versions and the trash feature 
      (for the latter you have to update QOwnNotesAPI on your ownCloud 
      server to version 0.4.4)
- fixed a possible crash in the ownCloud trash dialog
- added a menu entry to the `#qownnotes` **IRC channel** on freenode
    - join the fun at [#qownnotes IRC channel](https://webchat.freenode.net/?channels=qownnotes)
- added more Portuguese, Dutch and Hungarian translation (thank you 
  mlimacarlos and Fabijenna)

## 16.05.22
- more fixes for a problem with the interface language under Windows
  (for [#184](https://github.com/pbek/QOwnNotes/issues/184))

## 16.05.21
- attempted to fix a problem with the interface language
  (for [#184](https://github.com/pbek/QOwnNotes/issues/184))
- added frames for the search box for the task description edit, the 
  note version view, the log dialog and the settings debug output edit so that 
  the search widget doesn't get placed inside the text edits
- the note text view in the trash dialog now also supports markdown 
  highlighting and text searching
- replacing in markdown text edits is now prohibited if they are readonly
- when creating a code block from a selected multi-line text now a newline 
  character will be added at the end of the selected text if there wasn't 
  already one there
- added more information to the debug settings tab

## 16.05.20
- the `insert code block` command will now automatically use a multi-line 
  code block instead of an inline code block if a newline character is found in 
  the selected text (for [#180](https://github.com/pbek/QOwnNotes/issues/180))
- the `deb` package should now use the 2016 version of the application icon
- added more Italian and Russian translation (thank you alexl and frostyland)

## 16.05.19
- added more information to the debug settings tab
- added a hot-fix for a crash in conjunction with the system tray under 
  Ubuntu 14.04 (for [#172](https://github.com/pbek/QOwnNotes/issues/172))
- added more Japanese, German and Portuguese translation (thank you piyo, 
  bigant and mlimacarlos)

## 16.05.18
- in the tray icon there now is a new context menu entry for opening QOwnNotes
  (for [#176](https://github.com/pbek/QOwnNotes/issues/176))
- left clicking on the tray icon now doesn't show or hide the app under OS X 
  because this will also also trigger the context menu
- closing the app from the title bar should now hide the app more reliably if
  the system tray icon was enabled
- opening QOwnNotes with the tray icon or the context menu entry will also 
  un-minimize the window and bring it to the front
  (for [#176](https://github.com/pbek/QOwnNotes/issues/176))
- trying to fix duplicate menu entries in the context menu under Ubuntu 14.04
  (for [#172](https://github.com/pbek/QOwnNotes/issues/172))
- the pane sizes in the main window are now saved and restored for all 
  combinations of tag pane, edit pane, markdown pane and vertical mode 
  toggling possibilities to assure a better pane sizes experience when 
  toggling different panes
  (for [#175](https://github.com/pbek/QOwnNotes/issues/175))

## 16.05.17
- the log entries in the log window now have different colors depending on 
  their log type
- fixed a crash when showing the log window after opening QOwnNotes from the 
  system tray
- from the QOwnNotes tray icon you can now create new notes, open the ten top 
  most notes, open the todo lists, open the 10 most recent tasks or quit the app
  (for [#172](https://github.com/pbek/QOwnNotes/issues/172))
- the tasks of your todo lists will now automatically be updated from your
  ownCloud server every 10 minutes to allow better reminding of tasks
  (for [#172](https://github.com/pbek/QOwnNotes/issues/172))
- the `Show in system tray` checkbox was moved to the `Window` menu
- added more Japanese and Russian translation (thank you piyo and frostyland)

## 16.05.16
- added improvements to the HTML export by @dwu
- assigned tags are now preserved when manually renaming a note
- many improvements with note file renaming and preserving tags in 
  conjunction with scripting and automatic filename generation has been made
  (for [#170](https://github.com/pbek/QOwnNotes/issues/170))
    - the scripting hook `handleNoteTextFileNameHook(note)` now needs to return
      a filename without file-extension
- added more German, Japanese and Russian translation (thank you bigant, piyo
  and frostyland)

## 16.05.15
- the note folder select box is now hidden correctly every time, if it should
  not be viewed and was triggered by the shortcut
  (for [#169](https://github.com/pbek/QOwnNotes/issues/169))
- fixed a database warning
- now also the cursor position in the note text will be restored when 
  switching note folders
- when replacing text in a note with the `Replace` button the cursor will now
  stay with the last search hit if no more text can be replaced instead of 
  moving to the top of the note
- the note version dialog now also lets you also view the full text of the 
  versioned note (beside only the diff between the version on the server and 
  the current version)
    - you can also search in the note text with <kbd>Ctrl</kbd> + <kbd>F</kbd>
- there now is a new scripting hook `noteToMarkdownHtmlHook(note, html)`
  that is called when the markdown html of a note is generated
  (for [#164](https://github.com/pbek/QOwnNotes/issues/164))
    - it allows you to modify this html
    - this is for example called before by the note preview
    - there is an example call in
      [example.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/example.qml)
- added more Japanese and Portuguese translation (thank you piyo and mlimacarlos)

## 16.05.14
- there are several new features in the **log dialog**
    - you can now select different log types you want to log:
      `debug`, `info`, `warning`, `critical`, `fatal`, `status` and `scripting`
      (for [#170](https://github.com/pbek/QOwnNotes/issues/170))
    - the adding of log entries to the text area was improved to handle many 
      log entries
    - the log text area will now only scroll automatically to the bottom if 
      it was scrolled to the bottom before the log entry was added
    - you can search for text in the text area with <kbd>Ctrl</kbd> + <kbd>F</kbd>
    - there is a new button to clear the log
- there are now icons for each dialog
- fixed two possible crashes in the versioning dialog
- you are now able to ignore all external modifications of your note file 
  while you are editing your note in the note diff dialog
  (for [#171](https://github.com/pbek/QOwnNotes/issues/171))
    - you can turn the ignore off in the settings 
- added more Japanese and Portuguese translation (thank you piyo and mlimacarlos)

## 16.05.13
- there now is a new shortcut <kbd>Ctrl</kbd> + <kbd>Alt</kbd> + <kbd>F</kbd>
  that lets you select a different note folder
  (for [#169](https://github.com/pbek/QOwnNotes/issues/169))
- you now can now reload the scripting engine with <kbd>Ctrl</kbd> + 
  <kbd>⇧</kbd> + <kbd>S</kbd>
    - you need to do that when you modify a script
    - you can also do that with a button in the scripting settings
        - it will be done automatically when you set a script file, delete a 
          script or store the settings
- you can now open a dialog to view past status messages and the log output 
  from your scripts (for [#166](https://github.com/pbek/QOwnNotes/issues/166))
    - use the new script command `script.log()` to log to that dialog
- there now is a new menu entry for exporting the HTML of your note preview
  (for [#168](https://github.com/pbek/QOwnNotes/issues/168))
- downloading media files, web pages for links, calendar files for the todo 
  list and files in QML scripting will now support following redirects from 
  Qt 5.6 on
- there now is a new script function `script.downloadUrlToString("http://the-webpage");`
    - it allows you to download an url to a string
    - there is an example at
      [insert-headline-with-link-from-github-url.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/insert-headline-with-link-from-github-url.qml)
- added more German and Russian translation (thank you bigant and frostyland)

## 16.05.12
- tags and notes can now be exposed to the QML scripting engine
- beside the note name, the file name and the note text you can also get a 
  list of all assigned tag names for notes or a list of assigned tag objects
    - see [examples](https://github.com/pbek/QOwnNotes/tree/develop/docs/content/scripting/examples)
      for a full list of all note properties and functions 
- you can get the current note with `script.currentNote()` from QML
- `function onNoteStored(note)` now gets a note object as parameter instead of 
  just the file name and the note text
- there now is a new scripting hook `function handleNoteTextFileNameHook(note)`
  that is called when a note gets stored to disk (if *Allow note file name to 
  be different from headline* is enabled in the settings)
  (for [#166](https://github.com/pbek/QOwnNotes/issues/166))
    - it allows you to modify the name of the note file
    - there is an example at
      [use-tag-names-in-filename.qml](https://github.com/pbek/QOwnNotes/blob/develop/docs/scripting/examples/use-tag-names-in-filename.qml)
- added more Japanese, Portuguese, German and Dutch translation (thank you 
  piyo, mlimacarlos, bigant and Telesight)

## 16.05.11
- fixed the QOwnNotesAPI test in the settings dialog if no network connection
  is present
- made the ownCloud server error text in the settings dialog copyable by mouse
- clarified the the meaning of the setting `notify about all external 
  modifications` (for [#161](https://github.com/pbek/QOwnNotes/issues/161))
- generally increased the time a status message is visible for messages that 
  were viewed too short (for [#161](https://github.com/pbek/QOwnNotes/issues/161))
- scripts are now automatically validated when the script file is set in the 
  settings dialog
- a new script hook `insertingFromMimeDataHook` was added
    - this function is called when html or media files are pasted to a note 
      with `Ctrl + Shift + V`
    - you can intercept the media paste mechanism with it
    - there is an example at
      [note-text-from-5pm-mail.qml](https://github.com/pbek/QOwnNotes/tree/develop/docs/scripting/examples/note-text-from-5pm-mail.qml)
- the script function `modifyMediaMarkdown` was be renamed to `insertMediaHook`
  for more consistency
- added a `Paste HTML or media` context menu entry for notes
- added more Japanese translation (thank you piyo)

## 16.05.10
- QOwnNotes now has experimental **scripting support**
    - by using the power of the QQmlEngine and QML files you are now able
      to script some behaviors in the application yourself
    - you will find a new *Scripting* tab in the settings dialog where you can 
      add as many scripts to QOwnNotes as you like
    - it is possible to disable or remove scripts
    - you may need to restart QOwnNotes to let changes in a script take action 
    - I prepared some
      [examples](https://github.com/pbek/QOwnNotes/tree/develop/docs/content/scripting/examples)
      to play around with
    - if you need access to a certain functionality in QOwnNotes or have 
      questions or ideas please open an issue on the 
      [QOwnNotes issue page](https://github.com/pbek/QOwnNotes/issues)

## 16.05.9
- added a small remedy for the problem of not detected external note changes 
  under Linux
- fixed the `undefined` list entry in the version dialog and the trash dialog
- fixed a problem with the automatic text selection when replacing text in notes

## 16.05.8
- fixed the the jumping to headlines when clicking on them in the navigation 
  bar if the note edit pane was disabled at app startup
  (for [#165](https://github.com/pbek/QOwnNotes/issues/165))
- fixed the distraction free mode if the note edit pane was disabled while 
  entering the distraction free mode
- added more Japanese, Portuguese, Italian and Russian translation (thank you 
  piyo, mlimacarlos, rogepix and frostyland)

## 16.05.7
- made a workaround for the not working page setup dialog for the PDF 
  export under OS X and Windows 
  (for [#156](https://github.com/pbek/QOwnNotes/issues/156))
    - you can select page sizes from A0 to A9
    - you can select the page orientation (portrait and landscape)
- added more German translation

## 16.05.6
- SHA256 hashes were added to the AUR repository
- you can now select the page format when exporting notes as PDF
  (for [#156](https://github.com/pbek/QOwnNotes/issues/156))
- added more German, Japanese and French translation (thank you piyo and Loelo)

## 16.05.5
- fixed a problem with the Gentoo Linux overlay
  (for [#163](https://github.com/pbek/QOwnNotes/issues/163))
- did some styling fixes for the text replace widget on OS X
- you can new use a new setting in the general settings to **allow that the 
  note file name is different from the note headline**
  (for [#157](https://github.com/pbek/QOwnNotes/issues/157))
    - this allows you to keep your old note files regardless of what the content
      of them is
    - the note name will also be found in the note search
    - you are able to **rename your note files** by double-clicking on the note 
      name in the note list
        - renaming will fail if there already exists a note with the same name
        - some characters are also not allowed in the file name, they will be 
          removed automatically
    - be aware of that ownCloud notes needs the note file name to be the same 
      as the note headline
- added more Japanese translation (thank you piyo)

## 16.05.4
- the last open note will now be remembered when switching between note folders
- you can now use the new **regular expression** and **whole word** modes to 
  search for text inside of notes
    - when replacing with regular expressions containing capturing parentheses, 
      occurrences of `\1`, `\2`, ... can be used in the replace text
- fixed a possible search and replace infinite loop
- added support for case sensitive search
- added more Portuguese and German translation (thank you mlimacarlos and dgirke)

## 16.05.3
- QOwnNotes will now be built from a **source archive** (like it should be) in 
  the AUR and in the Gentoo overlay (and not from git repositories)
    - visit [Building QOwnNotes](https://www.qownnotes.org/installation#building) 
    for more information on building QOwnNotes and source archives
- the text is now also selected in the text replace dialog
- added more Japanese translation (thank you piyo)

## 16.05.2
- the **active tag** will now be **remembered across different note folders** 
  when switching between note folders
- the note preview is now regenerated once a second when the preview 
  pane is resized to readjust the width of embedded images
  (for [#154](https://github.com/pbek/QOwnNotes/issues/154))
- when the text replace functionality is activated now the currently selected 
  text will be entered in the search line edit
- when clicking a headline in the navigation bar the text beneath the headline 
  will be now made fully visible by scrolling more intelligently
  (for [#153](https://github.com/pbek/QOwnNotes/issues/153))
    - in addition the text of the headline will be selected
- if a note was created by hitting `Return` in the note search line edit now
  the line edit will be cleared so all notes will be viewed again
- <kbd>Ctrl</kbd> + <kbd>Space</kbd> now tries to **solve simple equations** 
  like `(4+5)*3=` in the current line in front of the cursor 
  (for [#152](https://github.com/pbek/QOwnNotes/issues/152))
    - the result will be put into the note if the cursor is after the `=` 
- fixed a problem where opening the settings dialog from another dialog 
  would remove some settings

## 16.05.1
- fixed the automatic image resizing in the preview under Windows
  (for [#154](https://github.com/pbek/QOwnNotes/issues/154))
- improved the automatic image width calculation for the image resizing in the
  preview (for [#154](https://github.com/pbek/QOwnNotes/issues/154))
- fixed a sporadic crash that occurred when switching the note folder
- in the navigation tree now a suitable parent headline will be searched if 
  the headline order was not correct
- the preview view now also scrolls to the position of the headline if 
  clicked in the navigation bar
- the navigation bar now also works while editing encrypted notes
- added more Portuguese, Japanese and Dutch translation (thank you 
  mlimacarlos, piyo and hartwin)

## 16.05.0
- a navigation bar that shows the headline structure of the current note was 
  implemented (for [#153](https://github.com/pbek/QOwnNotes/issues/153))
    - click on a headline to jump to it in the note
    - you can hide the navigation bar by dragging it to the bottom with the 
      splitter handle between the note list and the navigation bar 
- added more German and French translation (thank you Loelo)

## 16.04.18
- added **Italian translation** (a big thank you to alexl and rogepix!)
- added some styling fixes for the text-replace widget in OS X 
- fixed the size of the todo dialog
- the width of images in the preview will now be reduced to the width of the 
  preview pane if they are larger than that
  (for [#154](https://github.com/pbek/QOwnNotes/issues/154))
- added more German, Portuguese, French, Russian, Japanese, Dutch and Hungarian
  translation (thank you mlimacarlos, Loelo, frostyland, piyo and Fabijenna)

## 16.04.17
- decreased the width of *add tag* dialog
- if a note is created while a tag was selected in the tag tree the note will
  now get tagged with that tag
- when creating a new note folder in the settings dialog the folder name will
  now be selected and gets the focus
- you can now replace found text in the current note with <kbd>Ctrl</kbd> +
  <kbd>R</kbd>
  (for [#149](https://github.com/pbek/QOwnNotes/issues/149))
    - The shortcut for removing the current note went to <kbd>Alt</kbd> + 
    <kbd>R</kbd>
    - please visit [QOwnNotes shortcuts](https://www.qownnotes.org/getting-started/shortcuts.html)
      for a list of all shortcuts
- added more Portuguese, French, Russian, German and Japanese translation 
  (thank you mlimacarlos, Loelo, frostyland and piyo)

## 16.04.16
- did some fixes to the settings dialog to support small screen widths
  (for [#136](https://github.com/pbek/QOwnNotes/issues/136))
- the tag tree will now be expanded automatically when the tags are loaded 
  and the columns are sized to the contained content
- you can now add a new tag beneath another tag by right-clicking on it and 
  select *Add tag*
- added a new QOwnNotes desktop icon (a big thank you to Alex L from the KDE 
  V Design Group)
- added more Dutch, Hungarian, Portuguese, French, Spanish, Russian and Japanese 
  translation (thank you Fabijenna, mlimacarlos, Loelo, fitoschido, frostyland
  and piyo)

## 16.04.15
- **tags can now have a hierarchical structure**
  (for [#137](https://github.com/pbek/QOwnNotes/issues/137))
	- there is a number right of each tag name in the tag tree that indicates
	  how many notes are tagged with the tag
	- you are able to search for tags in your tag tree with the `find or create 
	  tag` line edit by typing in it
		- if you press enter a new tag with the content of the line edit is 
		  created at the root level
	- you are able to move selected tags to another tag by right-clicking on 
	  them and select *Move tags to...*
	- you are able to remove selected tags by right-clicking on them and select
	  *Remove tags*
	- you are able to edit the name of selected tags by right-clicking on them
	  and select *Edit tag* or double-clicking on it
	- you are able to tag selected notes by right-clicking on the notes and 
	  select *Tag selected notes with...*
	- you are able to remove a tag from selected notes by right-clicking on the 
	  notes and select *Remove tag from selected notes...*
	- the name of a tag has to be unique for its parent
		- so you can create tags with the same name under different tags
		- you cannot move a tag to another tag if a tag with the same name 
		  already exists there
- improved the code background color in the markdown view in dark mode
- when in single application mode the app will now come up again if it was 
  minimized and started a second time
  (for [#136](https://github.com/pbek/QOwnNotes/issues/136))
- added more Portuguese, French, Spanish and Japanese translation (thank you 
  mlimacarlos, Loelo, fitoschido and piyo)

## 16.04.14
- you can now also delete tags, notes and tasks with the <kbd>Backspace</kbd>
  key (for the OS X users) instead of only with the <kbd>Del</kbd> key
- fixed some problems with remembering the correct sizes of the panes in the 
  main window after an app restart
- you can now set in the general settings that you only want to **allow one 
  application instance running at the same time**
  (for [#136](https://github.com/pbek/QOwnNotes/issues/136))
- the tag list has now a context menu for deleting and editing tags
- the structure of the settings dialog was changed to decrease the height for
  smaller screens
  (for [#141](https://github.com/pbek/QOwnNotes/issues/141))
- added more Russian and Japanese translation (thank you Valerian and piyo)
- improved the meaningfulness of some error messages

## 16.04.13
- added better support for full width numbers in note names when linking notes 
  with unicode characters
  (for [#144](https://github.com/pbek/QOwnNotes/issues/144))
- the in-text search in the todo summary edit and debug text-edit were fixed
- collapsing was disabled for all panels in the main window, so they can't be 
  hidden by accident
  (for [#148](https://github.com/pbek/QOwnNotes/issues/148))
- you can now also **split** the note edit pane and note markdown preview 
  **vertically** instead of horizontally to gain more space on screens with 
  lower horizontal resolution
  (for [#145](https://github.com/pbek/QOwnNotes/issues/145))
- added more Portuguese and Japanese translation (thank you mlimacarlos and piyo)

## 16.04.12
- **switched QOwnNotes release versioning** to a more *rolling release 
  style*, so it doesn't get confused with semantic versioning
    - `<year of release>.<month of release>.<release number in the month>` 
- added support for full width numbers in note names when linking notes with 
  unicode characters
  (for [#144](https://github.com/pbek/QOwnNotes/issues/144))
- added a workaround to keep the note count in the tag name and still being 
  able to edit it without having to remove the note count from the name
- it is now possible to toggle the visibility of the note edit pane
  (for [#145](https://github.com/pbek/QOwnNotes/issues/145))
- the main toolbar is now split into multiple toolbars that you can rearrange
  or hide by themselves
  (for [#141](https://github.com/pbek/QOwnNotes/issues/141))
    - keep in mind that you can also drag toolbars to the left, right or 
      bottom of the window
- added more Japanese translation (thank you piyo)

## 1.4.3
- added **Japanese translation** (a big thank you to piyo!)
- opening notes in the default editor was fixed for Windows
  (for [#142](https://github.com/pbek/QOwnNotes/issues/142))
- opening notes with multi-byte file names in the an custom external editor was 
  fixed for Windows and OS X
  (for [#143](https://github.com/pbek/QOwnNotes/issues/143))
- SSL warnings on OS X are not logged to the console any more
- support for **linking notes with unicode characters** in their names was added
  (for [#144](https://github.com/pbek/QOwnNotes/issues/144))
- fixed a problem with a jumping text cursor when the note folder is modified
  externally (for [#146](https://github.com/pbek/QOwnNotes/issues/146))
- you can now **remove a tag from multiple notes** by selecting them, then 
  right-clicking on the notes and selecting *Remove tag from selected notes...*  
  (for [#139](https://github.com/pbek/QOwnNotes/issues/139))
- added more Russian, German, Hungarian and Dutch translation (thank you 
  Valerian, calis2002 and Fabijenna)

## 1.4.2
- some improvements were made to the detection whether the <kbd>Ctrl</kbd> 
  key is pressed in the note text edit or not
- the lines between frames on the main screen were made a bit stronger under 
  Linux  
- the `Reload note folder` menu entry also reloads the current note now
- fixed some displaying problems on the main screen
  (for [#140](https://github.com/pbek/QOwnNotes/issues/140))
- the note edit and note markdown view scrollbars are now linked together
- the markdown preview now also works when editing encrypted notes (although 
  the update interval is the note storage interval)
- fixed an issue with the note order in the note list in alphabetical sorting 
  mode when editing a note
  (for [#141](https://github.com/pbek/QOwnNotes/issues/141))
- some visual improvements in the html of the markdown preview has been made
  (for [#141](https://github.com/pbek/QOwnNotes/issues/141))
- inserted buttons for *bold* and *italic* in the main menu
  (for [#141](https://github.com/pbek/QOwnNotes/issues/141))
- added more Portuguese and Russian translation (thank you mlimacarlos and Valerian)

## 1.4.1
- you can now **tag multiple notes** by selecting them, then right-clicking on 
  the notes and selecting *Tag selected notes with...*  
  (for [#135](https://github.com/pbek/QOwnNotes/issues/135))
- added **live markdown preview**
  (for [#119](https://github.com/pbek/QOwnNotes/issues/119))
    - the **markdown note view** is now optionally **visible all the time**
    - <kbd>Ctrl</kbd> + <kbd>E</kbd> now toggles the markdown view mode
    - the markdown preview will be updated every 2 sec 
    - todo: linked scrolling between note edit and markdown view
    - todo: markdown preview update when editing encrypted notes 
- some main splitter user interface fixes for OS X were done
- did some note editing user interface improvements and fixes for the 
  distraction free mode and the dark mode
- added more Portuguese and Russian translation (thank you mlimacarlos and Valerian)

## 1.4.0
- a lot of **design changes** were done in the **main interface**
- the amount of all notes is now also shown in the tag list
- the size of the settings screen was decreased so it fits on smaller displays
- the distraction free mode was fixed for when tagging is turned on
- the distraction free mode was fixed for when dark mode is turned on
- added more Portuguese and Russian translation (thank you mlimacarlos and Valerian)

## 1.3.9
- there now is a menu entry to reload the current note folder
    - use <kbd>Ctrl</kbd> + <kbd>⇧</kbd> + <kbd>R</kbd> as a shortcut
- on Linux now only the 200 most recent notes will be watched for external 
  changes to prevent that nothing is watched at all because of too many open 
  files
- the notes in the note list now have an icon to look more fancy and the last
  modified date is shown in the tooltip
- the amount of notes for each tag is now shown in the tag list 
- added more Portuguese and Russian translation (thank you mlimacarlos and Valerian)

## 1.3.8
- some user interface styling and naming changes for tagging 
- when a tag is clicked in the tag list widget the note search line edit now 
  will be cleared
- when a new tag is created for a note the previously entered text will now 
  be selected
- fixed a possible crash when selecting a remote note folder in the note 
  folder settings

## 1.3.7
- you now can **tag your notes**
	- you can turn on tagging with <kbd>Alt</kbd> + <kbd>⇧</kbd> + <kbd>T</kbd>
	- tags and their associations to note files are stored in each note folder
	  individually in a SQLite database
	- if notes are renamed by QOwnNotes the tags will be moved to the renamed
	  notes
	- if notes are move to another folder their associated tags are lost 
	- <kbd>Alt</kbd> + <kbd>⇧</kbd> + <kbd>A</kbd> adds a new tag to a note

## 1.3.6
- fixed a remote folder selection issue with deeper levels of folders in the 
  note folders settings
- added more Russian translation (thank you Valerian)

## 1.3.5
- fixed a remote folder selection issue with special characters and 
  whitespaces in the folder names
- implemented a new symbol in the note search/create line edit with an  
  additional dark mode theme version of the symbol
- improved the note font preview in the settings for dark mode themes
- improved the links and some texts in the settings dialog for dark mode themes
- added more Portuguese and Russian translation (thank you mlimacarlos and Valerian)

## 1.3.4
- in the *note folder* settings you can now select your remote ownCloud note 
  path directly on your server 
   - the folders will be fetched via WebDAV from your ownCloud server 
- added more Russian translation (thank you Valerian)
- characters like `:`, `/` and `\`, as well as multiple whitespace characters
  will now be removed from the note file name if the first line of the note 
  is containing them
  (for [#126](https://github.com/pbek/QOwnNotes/issues/126))

## 1.3.3
- fixed a possible problem with an empty default note file extension setting
  (for [#123](https://github.com/pbek/QOwnNotes/issues/123))
- added more Spanish and Russian translation (thank you asoliverez and Valerian)

## 1.3.2
- fixed a problem with the output of ownCloud error messages in the settings 
  dialog
- fixed a potential crash when opening the note folder tab in the settings 
  from the welcome screen as long there was no note folder configured
- removed an obsolete button from the ownCloud settings tab
- <kbd>⇧</kbd> + <kbd>Tab</kbd> will now also un-indent spaces in front of text
- added more Portuguese and Spanish translation (thank you mlimacarlos and asoliverez)

## 1.3.1
- fixed the background color in the note folder text edit on the welcome 
  screen for dark mode themes 

## 1.3.0
- moving and copying of notes now works with note folders, like it should
- fixed the position of the context menu in the note text edit
  (for [#124](https://github.com/pbek/QOwnNotes/issues/124))
- added a setting that will optimize the colors in dark desktop themes a bit
    - more ideas for dark mode theme colors are welcome
- added more Portuguese translation (thank you mlimacarlos)

## 1.2.9
- fixed that the note list didn't get reloaded after you set a different active 
  note folder in the settings
- fixed that notes weren't stopped from being watched for external changes 
  after the note folder got switched
- if you configured an editor for opening your note files with it the editor 
  will now be executed in the background, so you can continue to work with 
  QOwnNotes
- strings like `?b=16068071000` will now be removed from the suffix of 
  downloaded media files

## 1.2.8
- you are now able to **configure your note folders** in the settings
  (for [#36](https://github.com/pbek/QOwnNotes/issues/36))
	- as a consequence the *recent note folders* will not be used any more to 
	  switch between note folders
		- everything that was using the *recent note folders* will now use the 
		  **note folders**
	- you can also configure a path on your ownCloud server for each folder
	- the field *main ownCloud sync folder* was removed from the ownCloud 
	  settings since this will now be done be the *note folders*
	- keep in mind that you also can turn on a **select box** with all your 
	  **note folders** over your note list in the settings 
- the default horizontal stretch of the notes list and the note view was 
  improved
- some more information was added to the welcome screen
- added more Polish translation (thank you @Olhur)

## 1.2.7
- fixed a problem after moving notes to another folder
  (for [#117](https://github.com/pbek/QOwnNotes/issues/117))
- added more Portuguese translation (thank you mlimacarlos)

## 1.2.6
- fixed building of QOwnNotes under ARM by patching the Botan library
  (for [#116](https://github.com/pbek/QOwnNotes/issues/116))
- there now is a setup-wizard-like welcome dialog that opens if no note path is
  set (for example at first start of QOwnNotes)
    - there you can setup your notes path
    - reach the ownCloud settings to setup access to your ownCloud server
    - reach the general settings to disable app metrics
- added more Portuguese translation (thank you mlimacarlos)

## 1.2.5
- lots of todo dialog improvements
    - the *save and insert* button will be disabled correctly now
    - did some tab order improvements
    - the description edit now gets the focus after creating a new task
    - when editing a reminder date and pressing enter, the task 
      now gets stored and the description edit gets the focus
    - you can now remove tasks with the <kbd>Del</kbd> key
    - fixed the shortcuts of the *Store*, *Store and insert* and *Remove* 
      buttons
        - please visit [QOwnNotes shortcuts](https://www.qownnotes.org/getting-started/shortcuts.html)
          for a list of all shortcuts
- added more German and Portuguese translation (thank you calis2002 and mlimacarlos)

## 1.2.4
- the selected text in a note will be inserted as summary in the todo dialog
  to create a new tasks more easily
- you now can insert a link to a task in a note from the todo dialog
    - tasks from these links get viewed in the todo dialog when clicked
      if they exist  
- added some information about [Let's encrypt](https://letsencrypt.org/) to 
  the settings
- added some status messages about externally modified notes
- added some missing SSL error checks

## 1.2.3
- added some alternate shortcuts for moving between notes
    - <kbd>Alt</kbd> + <kbd>Up</kbd> jumps to the previous visible note
    - <kbd>Alt</kbd> + <kbd>Down</kbd> jumps to the next visible note
- you can now close the distraction free mode with <kbd>Esc</kbd>
- you can now open the settings with <kbd>Ctrl</kbd> + <kbd>,</kbd>
- there now is a new setting for **ignoring SSL errors** when connecting to 
  your ownCloud server
    - by default this setting is turned on, to mimic recent behaviour of 
      QOwnNotes
    - get free SSL certificates at [Let's encrypt](https://letsencrypt.org/) 
      for your own server and turn off the *ignoring SSL errors* checkbox!
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
  (for [#113](https://github.com/pbek/QOwnNotes/issues/113))
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
  (for [#106](https://github.com/pbek/QOwnNotes/issues/106))
    - use <kbd>Alt</kbd> + <kbd>⇧</kbd> + <kbd>F</kbd> as shortcut
    - under OS X the folder of the note is opened in Finder, but the note is 
      not selected
- added more German and Spanish translation (thank you calis2002 and 
  moblesmarti)

## 1.1.8
- added rich text pasting fixes for Internet Explorer
  (for [#112](https://github.com/pbek/QOwnNotes/issues/112))

## 1.1.7
- you can now also drop images from websites directly to a note
  (for [#105](https://github.com/pbek/QOwnNotes/issues/105))
- if there is no plain text to paste with <kbd>Ctrl</kbd> + <kbd>V</kbd> then
  it will be checked if there html, notes or images to paste automatically
  (for [#105](https://github.com/pbek/QOwnNotes/issues/105))
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
  <kbd>V</kbd> (for [#112](https://github.com/pbek/QOwnNotes/issues/112))
    - html tags like `strong`, `b`, `em`, `i`, `a`, `h1`, `h2`, `h3`, `h4`, 
      `h5`, `br` will be translated to markdown
    - images from `img` tags will be downloaded and inserted as markdown images

## 1.1.5
- added more German, Hungarian and Dutch translation (thank you Fabijenna)
    - join us at [QOwnNotes on Crowdin](https://crowdin.com/project/qownnotes/invite)
      to make QOwnNotes available in more languages
- fixed some typos
- added several fixes for Windows
  (for [#105](https://github.com/pbek/QOwnNotes/issues/105))
    - dragging and dropping of images into notes
    - the image preview in markdown mode
    - opening of links to local files

## 1.1.4
- added support for copying notes from a file manager to QOwnNotes via drag and 
  drop (for [#100](https://github.com/pbek/QOwnNotes/issues/100))
- added support for dragging and dropping images into notes
  (for [#105](https://github.com/pbek/QOwnNotes/issues/105))
- added support for copying and pasting images into notes from a file manager
  (for [#105](https://github.com/pbek/QOwnNotes/issues/105))
    - use <kbd>Ctrl</kbd> + <kbd>⇧</kbd> + <kbd>V</kbd> as shortcut for 
    pasting this way
- added support for copying and pasting images from an image manipulation 
  software into a note
  (for [#105](https://github.com/pbek/QOwnNotes/issues/105))
    - a new png image will be created in the `media` folder of your notes folder 
    - use <kbd>Ctrl</kbd> + <kbd>⇧</kbd> + <kbd>V</kbd> as shortcut for 
    pasting image data
    - please visit [QOwnNotes shortcuts](https://www.qownnotes.org/getting-started/shortcuts.html)
      for a list of all shortcuts
- added more Hungarian translation (thank you Fabijenna)

## 1.1.3
- added **Hungarian translation** (a big thank you to Fabijenna!)
    - join us at [QOwnNotes on Crowdin](https://crowdin.com/project/qownnotes/)
      to make QOwnNotes available in more languages
- added shortcuts for note printing
  (for [#99](https://github.com/pbek/QOwnNotes/issues/99))
    - <kbd>Ctrl</kbd> + <kbd>P</kbd> prints the current note as text
    - <kbd>Ctrl</kbd> + <kbd>⇧</kbd> + <kbd>P</kbd> prints the current note 
    as markdown
- you now get a feedback while note versions or trashed notes are loaded and 
  the buttons are disabled while you are loading them 
  (for [#97](https://github.com/pbek/QOwnNotes/issues/97))
- added more German translation

## 1.1.2
- added more Dutch translation (thank you Fabijenna)
- fixed the *leave distraction free mode* button icon
- the notes viewed in the distraction free mode now have an adjustable viewport 
  width (for [#86](https://github.com/pbek/QOwnNotes/issues/86))
    - the viewport will resize automatically if the width of the window changes
    - you can select between four width modes in the *Window* menu 
- you can now toggle visibility of the tool bar in the *Windows* menu
  (for [#103](https://github.com/pbek/QOwnNotes/issues/103))
- added more German translation

## 1.1.1
- added **Dutch translation** (a big thank you to Fabijenna!)
- pressing <kbd>Ctrl</kbd> + <kbd>⇧</kbd> + <kbd>F</kbd> to do a note search 
  now leaves the distraction free mode
- the notes in the distraction free mode now have a padding 
  (for [#86](https://github.com/pbek/QOwnNotes/issues/86))
- added more German translation
- there now is a **Gentoo Linux overlay** for QOwnNotes (thanks a lot to 
  @internethering and @suhr for the help)
    - please visit the [QOwnNotes installation page](https://www.qownnotes.org/installation#Gentoo) for instructions

## 1.1
- QOwnNotes now allows the usage of `# ` markdown headlines as note title
  (for [#87](https://github.com/pbek/QOwnNotes/issues/87))
    - the characters will be removed in the note name
    - keep in mind that this behaviour may not be supported by other note 
      clients, that may add the `# ` back again into the file name 
- added more Russian translation (thank you SpacePotato)

## 1.0.9
- improved the in-note search, so that it will not be able to block the note 
  text any more

## 1.0.8
- you can now enter a distraction free mode with a new button in the toolbar
  (for [#86](https://github.com/pbek/QOwnNotes/issues/86))
    - you can also toggle it with <kbd>Ctrl</kbd> + <kbd>⇧</kbd> + <kbd>D</kbd> 

## 1.0.7
- the Chinese language got renamed internally for Crowdin, it's possible that 
  you have to re-select it in the settings if you were using it 
  (for [#95](https://github.com/pbek/QOwnNotes/issues/95))
    - join us on [QOwnNotes on Crowdin](https://crowdin.com/project/qownnotes/)
      to make QOwnNotes available in your language
- **fixed note encryption** that got broken in version 1.0.6
    - if you have encrypted notes in version 1.0.6 you have to first **decrypt 
      them with version 1.0.6** before using them in version 1.0.7! 

## 1.0.6
- you can now enter **network proxy settings** in the new network settings 
  tab to make QOwnNotes work over network proxies
  (for [#79](https://github.com/pbek/QOwnNotes/issues/79))
- added more German translation
- use <kbd>Ctrl</kbd> + <kbd>PgUp</kbd> and <kbd>Ctrl</kbd> + <kbd>PgDn</kbd>
  to **jump between visible notes**
  (for [#80](https://github.com/pbek/QOwnNotes/issues/80))
    - please visit [QOwnNotes shortcuts](https://www.qownnotes.org/getting-started/shortcuts.html)
      for a list of all shortcuts
    
## 1.0.5
- Google Analytics got removed in favor of Piwik
- code blocks will now wrap around selected text when inserted
  (for [#91](https://github.com/pbek/QOwnNotes/issues/91))
- increased the color contrast in the markdown highlighting for better 
  readability
- added more Russian translation (thank you @elnull)

## 1.0.4
- added a button to insert a markdown code block at the current cursor position
  (for [#82](https://github.com/pbek/QOwnNotes/issues/82))
    - you can use 'Ctrl + Shift + C` as shortcut
- fixed a display problem with the update notification button under Windows
  (for [#90](https://github.com/pbek/QOwnNotes/issues/90))
- added more German translation

## 1.0.3
- added **Portuguese translation** by @mlimacarlos, thank you a lot!
- added more German, French and Russian translation (thank you @Dalan94 and @elnull)
- you can now select an external editor in the settings that will be used
  instead of the default editor when you open a note file with `Open note in 
  external editor`
  (for [#77](https://github.com/pbek/QOwnNotes/issues/77))
- added an option to disabled the app-heartbeat in the settings
  (for [#84](https://github.com/pbek/QOwnNotes/issues/84))

## 1.0.2
- you can now set a note bookmark with `Ctrl + ⇧ + 0..9` on slot 0..9
    - use `Ctrl + 0..9` jump to the bookmark on slot 0..9
- fixed a markdown highlighting problem with certain characters like `»` and `·`
  (for [#73](https://github.com/pbek/QOwnNotes/issues/73))
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
  (for [#71](https://github.com/pbek/QOwnNotes/issues/71))

## 1.0
- yippee, **version 1.0**, so there were already 100 releases!?
    - want to get involved with QOwnNotes? read:
    [Getting involved](https://www.qownnotes.org/contributing/get-involved.html)
- updated German and Russian translation (thanks for your help, @elnull)
- fixed the visual blocking of the note search bar by the scroll bar
- the code font in the note Markdown view can now be configured in the settings
  (for [#59](https://github.com/pbek/QOwnNotes/issues/59))

## 0.99
- implemented plural translations
  (for [#65](https://github.com/pbek/QOwnNotes/issues/65))
- updated German translation
- added missing icons in dialogs
- added plural translations for English and German
- added **Russian translation** (a big thank you to @elnull!)
  (for [#67](https://github.com/pbek/QOwnNotes/issues/67))
- the languages are now selectable in their native language
  (for [#67](https://github.com/pbek/QOwnNotes/issues/67))
- add some links to knowledge base articles in the settings dialog

## 0.98
- made a workaround for the lacking OS X theming support, icons should work 
  again
- the search bar in the text edits now also uses themes

## 0.97
- I adopted the **AUR package** **`qownnotes`**, the repository 
  `qownnotes-pbek` will be discontinued (a big thank you to Midov for 
  managing `qownnotes` until now)
- QOwnNotes now uses default **freedesktop theme icons**
  (for [#39](https://github.com/pbek/QOwnNotes/issues/39))
    - if the operating system hasn't a theme set (like for Windows and OS X)
      there is a bundled fallback theme with Breeze icons that will be used 
    - this allows the usage of QOwnNotes with **dark system themes**

## 0.96
- fixed the debug output in the *Debug* settings tab
- you can now select a font for the code in the markdown enabled text edits
- you can now reset the fonts to the operating system's default font

## 0.95
- code in the Markdown edit now uses the `Courier` font
  (for [#59](https://github.com/pbek/QOwnNotes/issues/59))
- the update checker was modified to fix a crash on OS X
- there now is a new *Interface* tab in the settings dialog to make room for 
  more configuration items
- there now is a new button in the *general settings*, that allows you to clear 
  all settings, remove the local todo database and exit QOwnNotes
  (for [#56](https://github.com/pbek/QOwnNotes/issues/56))

## 0.94
- pressing `Delete` in the note list widget now attempts to remove all 
  selected notes, not just the current one
  (for [#63](https://github.com/pbek/QOwnNotes/issues/63))
- canceling the update dialog now really cancels the update process like it 
  used to
- fixed a problem with a wrong note showing up after you change your note folder
  (for [#60](https://github.com/pbek/QOwnNotes/issues/60))
  
## 0.93
- added more German translation by @calis2002
- attempted to fix a problem with removing multiple notes
- fixed a problem with two notes with the same name (but different extension) 
  showing up in the note list
- fixed a problem with empty entries in the recent note folders list 

## 0.92
- you can now choose to use `.md` instead of `.txt` as your default 
  file-extension for your notes in the settings
  (for [#25](https://github.com/pbek/QOwnNotes/issues/25))
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
- if you feel like translating QOwnNotes to another language please read
 [How can I help to translate QOwnNotes?](https://www.qownnotes.org/contributing/translation.html)
- fixed a character in link titles that was disorienting the markdown 
  highlighter

## 0.88
- more app internationalization implementation and integration in deployment 
  systems (for [#40](https://github.com/pbek/QOwnNotes/issues/40))
- start of German translation of QOwnNotes
    - please leave a message at the
    [QOwnNotes issues page](https://github.com/pbek/QOwnNotes/issues)
    if you want to help out with translation
- fixed a file watcher error on app startup
- the demo notes are now only created once instead of every time a notes folder
  is empty (for [#41](https://github.com/pbek/QOwnNotes/issues/41))
- fixed a problem with an empty note in the note list
- fixed a problem with the note text after the last note was deleted in the 
  note list

## 0.87
- start of app internationalization implementation
  (for [#40](https://github.com/pbek/QOwnNotes/issues/40))
- fixed a problem with the crypto key expiration of encrypted messages where one
  had to enter the password every minute while editing the note
  (for [#42](https://github.com/pbek/QOwnNotes/issues/42))
    - now the crypto key will expire after 10min without editing the note
- did some fixes and layout changes in the settings dialog
- added basic app metrics
    - anonymous usage data helps to decide what parts of QOwnNotes to 
    improve next and to find and fix bugs
    - you can disable the app metrics in the settings

## 0.86
- tried to fix a sporadic crash on OS X
- there now is a software repository for **Debian Linux 8.0**
    - please visit the [QOwnNotes installation page](https://www.qownnotes.org/installation) for instructions

## 0.85
- you can now export the current note as markdown file
- beside the regular QOwnNotes software repository for Arch Linux there now 
  also is an official package on the **Arch User Repository** called 
 `qownnotes-pbek`

## 0.84
- now non Travis CI or AppVeyor builds can perform an update check again, but 
  will open the [QOwnNotes installation page](https://www.qownnotes.org/installation) 
  instead of downloading the app
- there is a new software repository for **Arch Linux**
    - please visit the [QOwnNotes installation page](https://www.qownnotes.org/installation) for instructions

## 0.83
- switched two buttons in the menu bar
- fixed a problem with updating and creating the todo list item database (for [#35](https://github.com/pbek/QOwnNotes/issues/35))

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
- you can now jump to all visited note folders with a select box above the notes list (for [#31](https://github.com/pbek/QOwnNotes/issues/31))
    - this select box is turned off by default and can be turned on in the general settings
- small margin tuning on the main screen

## 0.77
- the icon size in the main toolbar was set to a fixed `38x38` to try to prevent high-dpi problems in KDE Plasma
- `libqt5sql5-sqlite` was added to the dependencies for the Ubuntu PPA build and the Ubuntu deb-package (for [#30](https://github.com/pbek/QOwnNotes/issues/30))
- if you don't want to miss a single version of QOwnNotes you might want to take a look at [New release version RSS feed](https://www.qownnotes.org/Blog/New-release-version-RSS-feed)

## 0.76
- the Windows version of QOwnNotes now also includes the OpenSSL library, please report if you have any problems on the [QOwnNotes issue page](https://github.com/pbek/QOwnNotes/issues))
- clicking links in all text edit boxes with markdown highlighting should work in the same way now

## 0.75
- added markdown highlighting to the debug information text box
- added link clicking support, 4 characters tab stops, custom font support and block indentation to all markdown highlighting editors

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
- you can now clear the recent note folders history in the settings (for [#26](https://github.com/pbek/QOwnNotes/issues/26))
- you can now insert images in notes with `Ctrl+I`
    - the image will be copied to the `media` folder in the current notes folder
    - the image will be shown in the markdown view (maximum width is 980px)
    - the image is clickable in the edit view
- there now is a new debug tab in the settings dialog where you can find a dump of data you can use in an Issue on the [QOwnNotes issue page](https://github.com/pbek/QOwnNotes/issues))

## 0.70
- beside the new software repositories for **Ubuntu Linux** and **openSUSE Linux** there are now also repositories for **Fedora Linux**
    - please visit the [QOwnNotes installation page](https://www.qownnotes.org/installation) for instructions
- tried to fix a crash that sometimes occurs on OSX after waking from sleep
- you can now also export the text of notes as PDF

## 0.69
- if you are using **Ubuntu Linux** or **openSUSE Linux** you can now take advantage of the brand new repositories for both distributions
    - take a look at the [QOwnNotes installation page](https://www.qownnotes.org/installation) for instructions
    - the update check for these releases have been removed from the menu and you will not get a report for new software versions, since updates are done by your operating system
    - you can always visit the [QOwnNotes changelog](https://www.qownnotes.org/changelog.html) to see what has changed in the versions
- you can now export notes as PDF (for [#23](https://github.com/pbek/QOwnNotes/issues/23))

## 0.68
- fixed a problem with creating new todo items after changing the enabled todo lists
- fixed a problem with newly created todo items that are set to completed
- fixed another problem with storing todo items with a reminder and an empty description
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
- the markdown view also supports tables now (for [#20](https://github.com/pbek/QOwnNotes/issues/20))

## 0.58
- fixed a problem with linking notes with special characters in their name (everything but charactes and numbers)
- html entities from the titles of inserted links are now decoded
- whitespaces are now trimmed from the start and end of the title of inserted links
- you can now select a local file to link to in the link dialog
- added a new shortcut `Ctrl + T` to insert the current date in ISO 8601 format

## 0.57
- `Ctrl + Click` in edit mode now works on the whole portion of links like `[QOwnNotes - cross-platform open source notepad](https://www.qownnotes.org/)`, not just on the URL alone
- added a menu entry to the new knowledge base in the help menu
- you can now change the font of the note text edit and the note markdown view in the settings (for [#19](https://github.com/pbek/QOwnNotes/issues/19))

## 0.56
- added a shortcut list to the help menu and to the startup notes
- you can now also link to URLs with `Ctrl + L` (or `Cmd + L` on OSX), if possible the title of the webpage will be fetched and used as link text

## 0.55
- added some different shortcuts for the note history on the mac
- tried to fix the duplicate line shortcut for the Ubuntu Unity desktop
- the OSX package is now code signed (but there are still some troubles with the signature)

## 0.54
- fixed a problem with canceling the notes folder selection dialog
- the note history is now cleared when switching to another notes directory
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
- you can open your ownCloud server Apps page from the settings dialog

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
- fixed some troubles with the Windows package
- added a hotkey to create new note with date in name
- turned off pasting of rich text in the note text edit box to avoid troubles
