# QOwnNotes Changelog

## 16.06.5
- <kbd>Ctrl</kbd> + <kbd>Space</kbd> to solve simple equations and open links
  now also works under OS X
  (for [Issue #203](https://github.com/pbek/QOwnNotes/issues/203))
- menu icons are now hidden under OSX to be more "conform" 
  (for [Issue #198](https://github.com/pbek/QOwnNotes/issues/198))
- added more Portuguese, Russian, Dutch, Japanese and German translation
  (thank you mlimacarlos, frostyland, Telesight, Fabijenna, piyo and bigant)

## 16.06.4
- file names with multiple dots are now supported when inserting a headline 
  from the note filename with <kbd>Ctrl</kbd> + <kbd>⇧</kbd> + <kbd>H</kbd>  
  (for [Issue #206](https://github.com/pbek/QOwnNotes/issues/206))
- you now can open a link at the current cursor position with 
  <kbd>Ctrl</kbd> + <kbd>Space</kbd> 
  (for [Issue #203](https://github.com/pbek/QOwnNotes/issues/203))
- added more Portuguese, Russian, Dutch, Japanese and German translation
  (thank you mlimacarlos, frostyland, Telesight and piyo)

## 16.06.3
- the **highlighting** for text found by the **search for multiple words** 
  feature from version 16.06.2 now also works for those patterns
  (for [Issue #195](https://github.com/pbek/QOwnNotes/issues/195))
- <kbd>Ctrl</kbd> + <kbd>⇧</kbd> + <kbd>H</kbd> now inserts a headline from
  the note filename at the top of the note
  (for [Issue #201](https://github.com/pbek/QOwnNotes/issues/201))
- there now is a new toolbar for the quit button right of the other toolbars, 
  so this button can be disabled
  (for [Issue #197](https://github.com/pbek/QOwnNotes/issues/197))
- you can now set the markdown highlighting interval in the interface settings
  (for [Issue #104](https://github.com/pbek/QOwnNotes/issues/104))
    - setting it to `0` will disable the highlighting
      (for [Issue #197](https://github.com/pbek/QOwnNotes/issues/197))

## 16.06.2
- you now can **search for multiple words** in notes
  (for [Issue #195](https://github.com/pbek/QOwnNotes/issues/195))
    - by default notes that contain every single word will be found, `word1 
      word2` will find all notes that are containing `word1` and `word2`
    - you can search for longer texts by using quotes, `"this word1" word2` 
      will find all notes that are containing `this word1` and `word2`
- there now is a **search history**
  (for [Issue #138](https://github.com/pbek/QOwnNotes/issues/138))
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
  (for [Issue #166](https://github.com/pbek/QOwnNotes/issues/166))
- fixed a wrong note count for `All notes` in the tag list
- the item `Untagged notes` in the tag list will now only be viewed if there 
  are untagged notes
- instead of the notification dialog after bulk tagging notes now a status 
  message will be shown
  (for [Issue #166](https://github.com/pbek/QOwnNotes/issues/166))
- added more Portuguese, Russian, German and Japanese translation (thank you 
  mlimacarlos, frostyland, bigant and piyo)

## 16.05.24
- you can now `Show note in file manager` and `Open note in external editor` 
  from the context menu in the note list
  (for [Issue #188](https://github.com/pbek/QOwnNotes/issues/188))
- added tag name auto-completion when adding a tag to the current note
  (for [Issue #166](https://github.com/pbek/QOwnNotes/issues/166))
- there now is a new item in the tag list that allows you to view all untagged 
  notes (for [Issue #166](https://github.com/pbek/QOwnNotes/issues/166))
- added more Portuguese and Japanese translation (thank you mlimacarlos and
  piyo)

## 16.05.23
- it is now possible to add **custom note file extensions** in the settings
  (for [Issue #185](https://github.com/pbek/QOwnNotes/issues/185))
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
  (for [Issue #184](https://github.com/pbek/QOwnNotes/issues/184))

## 16.05.21
- attempted to fix a problem with the interface language
  (for [Issue #184](https://github.com/pbek/QOwnNotes/issues/184))
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
  the selected text (for [Issue #180](https://github.com/pbek/QOwnNotes/issues/180))
- the `deb` package should now use the 2016 version of the application icon
- added more Italian and Russian translation (thank you alexl and frostyland)

## 16.05.19
- added more information to the debug settings tab
- added a hot-fix for a crash in conjunction with the system tray under 
  Ubuntu 14.04 (for [Issue #172](https://github.com/pbek/QOwnNotes/issues/172))
- added more Japanese, German and Portuguese translation (thank you piyo, 
  bigant and mlimacarlos)

## 16.05.18
- in the tray icon there now is a new context menu entry for opening QOwnNotes
  (for [Issue #176](https://github.com/pbek/QOwnNotes/issues/176))
- left clicking on the tray icon now doesn't show or hide the app under OS X 
  because this will also also trigger the context menu
- closing the app from the title bar should now hide the app more reliably if
  the system tray icon was enabled
- opening QOwnNotes with the tray icon or the context menu entry will also 
  un-minimize the window and bring it to the front
  (for [Issue #176](https://github.com/pbek/QOwnNotes/issues/176))
- trying to fix duplicate menu entries in the context menu under Ubuntu 14.04
  (for [Issue #172](https://github.com/pbek/QOwnNotes/issues/172))
- the pane sizes in the main window are now saved and restored for all 
  combinations of tag pane, edit pane, markdown pane and vertical mode 
  toggling possibilities to assure a better pane sizes experience when 
  toggling different panes
  (for [Issue #175](https://github.com/pbek/QOwnNotes/issues/175))

## 16.05.17
- the log entries in the log window now have different colors depending on 
  their log type
- fixed a crash when showing the log window after opening QOwnNotes from the 
  system tray
- from the QOwnNotes tray icon you can now create new notes, open the ten top 
  most notes, open the todo lists, open the 10 most recent tasks or quit the app
  (for [Issue #172](https://github.com/pbek/QOwnNotes/issues/172))
- the tasks of your todo lists will now automatically be updated from your
  ownCloud server every 10 minutes to allow better reminding of tasks
  (for [Issue #172](https://github.com/pbek/QOwnNotes/issues/172))
- the `Show in system tray` checkbox was moved to the `Window` menu
- added more Japanese and Russian translation (thank you piyo and frostyland)

## 16.05.16
- added improvements to the HTML export by @dwu
- assigned tags are now preserved when manually renaming a note
- many improvements with note file renaming and preserving tags in 
  conjunction with scripting and automatic filename generation has been made
  (for [Issue #170](https://github.com/pbek/QOwnNotes/issues/170))
    - the scripting hook `handleNoteTextFileNameHook(note)` now needs to return
      a filename without file-extension
- added more German, Japanese and Russian translation (thank you bigant, piyo
  and frostyland)

## 16.05.15
- the note folder select box is now hidden correctly every time, if it should
  not be viewed and was triggered by the shortcut
  (for [Issue #169](https://github.com/pbek/QOwnNotes/issues/169))
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
  (for [Issue #164](https://github.com/pbek/QOwnNotes/issues/164))
    - it allows you to modify this html
    - this is for example called before by the note preview
    - there is an example call in
      [example.qml](https://github.com/pbek/QOwnNotes/blob/develop/doc/scripting/example.qml)
- added more Japanese and Portuguese translation (thank you piyo and mlimacarlos)

## 16.05.14
- there are several new features in the **log dialog**
    - you can now select different log types you want to log:
      `debug`, `info`, `warning`, `critical`, `fatal`, `status` and `scripting`
      (for [Issue #170](https://github.com/pbek/QOwnNotes/issues/170))
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
  (for [Issue #171](https://github.com/pbek/QOwnNotes/issues/171))
    - you can turn the ignore off in the settings 
- added more Japanese and Portuguese translation (thank you piyo and mlimacarlos)

## 16.05.13
- there now is a new shortcut <kbd>Ctrl</kbd> + <kbd>Alt</kbd> + <kbd>F</kbd>
  that lets you select a different note folder
  (for [Issue #169](https://github.com/pbek/QOwnNotes/issues/169))
- you now can now reload the scripting engine with <kbd>Ctrl</kbd> + 
  <kbd>⇧</kbd> + <kbd>S</kbd>
    - you need to do that when you modify a script
    - you can also do that with a button in the scripting settings
        - it will be done automatically when you set a script file, delete a 
          script or store the settings
- you can now open a dialog to view past status messages and the log output 
  from your scripts (for [Issue #166](https://github.com/pbek/QOwnNotes/issues/166))
    - use the new script command `script.log()` to log to that dialog
- there now is a new menu entry for exporting the HTML of your note preview
  (for [Issue #168](https://github.com/pbek/QOwnNotes/issues/168))
- downloading media files, web pages for links, calendar files for the todo 
  list and files in QML scripting will now support following redirects from 
  Qt 5.6 on
- there now is a new script function `script.downloadUrlToString("http://the-webpage");`
    - it allows you to download an url to a string
    - there is an example at
      [insert-headline-with-link-from-github-url.qml](https://github.com/pbek/QOwnNotes/blob/develop/doc/scripting/insert-headline-with-link-from-github-url.qml)
- added more German and Russian translation (thank you bigant and frostyland)

## 16.05.12
- tags and notes can now be exposed to the QML scripting engine
- beside the note name, the file name and the note text you can also get a 
  list of all assigned tag names for notes or a list of assigned tag objects
    - see [examples](https://github.com/pbek/QOwnNotes/tree/develop/doc/scripting)
      for a full list of all note properties and functions 
- you can get the current note with `script.currentNote()` from QML
- `function onNoteStored(note)` now gets a note object as parameter instead of 
  just the file name and the note text
- there now is a new scripting hook `function handleNoteTextFileNameHook(note)`
  that is called when a note gets stored to disk (if *Allow note file name to 
  be different from headline* is enabled in the settings)
  (for [Issue #166](https://github.com/pbek/QOwnNotes/issues/166))
    - it allows you to modify the name of the note file
    - there is an example at
      [use-tag-names-in-filename.qml](https://github.com/pbek/QOwnNotes/blob/develop/doc/scripting/use-tag-names-in-filename.qml)
- added more Japanese, Portuguese, German and Dutch translation (thank you 
  piyo, mlimacarlos, bigant and Telesight)

## 16.05.11
- fixed the QOwnNotesAPI test in the settings dialog if no network connection
  is present
- made the ownCloud server error text in the settings dialog copyable by mouse
- clarified the the meaning of the setting `notify about all external 
  modifications` (for [Issue #161](https://github.com/pbek/QOwnNotes/issues/161))
- generally increased the time a status message is visible for messages that 
  were viewed too short (for [Issue #161](https://github.com/pbek/QOwnNotes/issues/161))
- scripts are now automatically validated when the script file is set in the 
  settings dialog
- a new script hook `insertingFromMimeDataHook` was added
    - this function is called when html or media files are pasted to a note 
      with `Ctrl + Shift + V`
    - you can intercept the media paste mechanism with it
    - there is an example at
      [note-text-from-5pm-mail.qml](https://github.com/pbek/QOwnNotes/tree/develop/doc/scripting/note-text-from-5pm-mail.qml)
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
      [examples](https://github.com/pbek/QOwnNotes/tree/develop/doc/scripting)
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
  (for [Issue #165](https://github.com/pbek/QOwnNotes/issues/165))
- fixed the distraction free mode if the note edit pane was disabled while 
  entering the distraction free mode
- added more Japanese, Portuguese, Italian and Russian translation (thank you 
  piyo, mlimacarlos, rogepix and frostyland)

## 16.05.7
- made a workaround for the not working page setup dialog for the PDF 
  export under OS X and Windows 
  (for [Issue #156](https://github.com/pbek/QOwnNotes/issues/156))
    - you can select page sizes from A0 to A9
    - you can select the page orientation (portrait and landscape)
- added more German translation

## 16.05.6
- SHA256 hashes were added to the AUR repository
- you can now select the page format when exporting notes as PDF
  (for [Issue #156](https://github.com/pbek/QOwnNotes/issues/156))
- added more German, Japanese and French translation (thank you piyo and Loelo)

## 16.05.5
- fixed a problem with the Gentoo Linux overlay
  (for [Issue #163](https://github.com/pbek/QOwnNotes/issues/163))
- did some styling fixes for the text replace widget on OS X
- you can new use a new setting in the general settings to **allow that the 
  note file name is different from the note headline**
  (for [Issue #157](https://github.com/pbek/QOwnNotes/issues/157))
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
  - visit [Building QOwnNotes](http://www.qownnotes.org/installation#building) 
  for more information on building QOwnNotes and source archives
- the text is now also selected in the text replace dialog
- added more Japanese translation (thank you piyo)

## 16.05.2
- the **active tag** will now be **remembered across different note folders** 
  when switching between note folders
- the note preview is now regenerated once a second when the preview 
  pane is resized to readjust the width of embedded images
  (for [Issue #154](https://github.com/pbek/QOwnNotes/issues/154))
- when the text replace functionality is activated now the currently selected 
  text will be entered in the search line edit
- when clicking a headline in the navigation bar the text beneath the headline 
  will be now made fully visible by scrolling more intelligently
  (for [Issue #153](https://github.com/pbek/QOwnNotes/issues/153))
    - in addition the text of the headline will be selected
- if a note was created by hitting `Return` in the note search line edit now
  the line edit will be cleared so all notes will be viewed again
- <kbd>Ctrl</kbd> + <kbd>Space</kbd> now tries to **solve simple equations** 
  like `(4+5)*3=` in the current line in front of the cursor 
  (for [Issue #152](https://github.com/pbek/QOwnNotes/issues/152))
    - the result will be put into the note if the cursor is after the `=` 
- fixed a problem where opening the settings dialog from an other dialog 
  would remove some settings

## 16.05.1
- fixed the automatic image resizing in the preview under Windows
  (for [Issue #154](https://github.com/pbek/QOwnNotes/issues/154))
- improved the automatic image width calculation for the image resizing in the
  preview (for [Issue #154](https://github.com/pbek/QOwnNotes/issues/154))
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
  implemented (for [Issue #153](https://github.com/pbek/QOwnNotes/issues/153))
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
  (for [Issue #154](https://github.com/pbek/QOwnNotes/issues/154))
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
  (for [Issue #149](https://github.com/pbek/QOwnNotes/issues/149))
    - The shortcut for removing the current note went to <kbd>Alt</kbd> + 
    <kbd>R</kbd>
    - please visit [QOwnNotes shortcuts](http://www.qownnotes.org/shortcuts/QOwnNotes)
      for a list of all shortcuts
- added more Portuguese, French, Russian, German and Japanese translation 
  (thank you mlimacarlos, Loelo, frostyland and piyo)

## 16.04.16
- did some fixes to the settings dialog to support small screen widths
  (for [Issue #136](https://github.com/pbek/QOwnNotes/issues/136))
- the tag tree will now be expanded automatically when the tags are loaded 
  and the columns are sized to the contained content
- you can now add a new tag beneath an other tag by right-clicking on it and 
  select *Add tag*
- added a new QOwnNotes desktop icon (a big thank you to Alex L from the KDE 
  V Design Group)
- added more Dutch, Hungarian, Portuguese, French, Spanish, Russian and Japanese 
  translation (thank you Fabijenna, mlimacarlos, Loelo, fitoschido, frostyland
  and piyo)

## 16.04.15
- **tags can now have a hierarchical structure**
  (for [Issue #137](https://github.com/pbek/QOwnNotes/issues/137))
	- there is a number right of each tag name in the tag tree that indicates
	  how many notes are tagged with the tag
	- you are able to search for tags in your tag tree with the `find or create 
	  tag` line edit by typing in it
		- if you press enter a new tag with the content of the line edit is 
		  created at the root level
	- you are able to move selected tags to an other tag by right-clicking on 
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
		- you cannot move a tag to an other tag if a tag with the same name 
		  already exists there
- improved the code background color in the markdown view in dark mode
- when in single application mode the app will now come up again if it was 
  minimized and started a second time
  (for [Issue #136](https://github.com/pbek/QOwnNotes/issues/136))
- added more Portuguese, French, Spanish and Japanese translation (thank you 
  mlimacarlos, Loelo, fitoschido and piyo)

## 16.04.14
- you can now also delete tags, notes and tasks with the <kbd>Backspace</kbd>
  key (for the OS X users) instead of only with the <kbd>Del</kbd> key
- fixed some problems with remembering the correct sizes of the panes in the 
  main window after an app restart
- you can now set in the general settings that you only want to **allow one 
  application instance running at the same time**
  (for [Issue #136](https://github.com/pbek/QOwnNotes/issues/136))
- the tag list has now a context menu for deleting and editing tags
- the structure of the settings dialog was changed to decrease the height for
  smaller screens
  (for [Issue #141](https://github.com/pbek/QOwnNotes/issues/141))
- added more Russian and Japanese translation (thank you Valerian and piyo)
- improved the meaningfulness of some error messages

## 16.04.13
- added better support for full width numbers in note names when linking notes 
  with unicode characters
  (for [Issue #144](https://github.com/pbek/QOwnNotes/issues/144))
- the in-text search in the todo summary edit and debug text-edit were fixed
- collapsing was disabled for all panels in the main window, so they can't be 
  hidden by accident
  (for [Issue #148](https://github.com/pbek/QOwnNotes/issues/148))
- you can now also **split** the note edit pane and note markdown preview 
  **vertically** instead of horizontally to gain more space on screens with 
  lower horizontal resolution
  (for [Issue #145](https://github.com/pbek/QOwnNotes/issues/145))
- added more Portuguese and Japanese translation (thank you mlimacarlos and piyo)

## 16.04.12
- **switched QOwnNotes release versioning** to a more *rolling release 
  style*, so it doesn't get confused with semantic versioning
    - `<year of release>.<month of release>.<release number in the month>` 
- added support for full width numbers in note names when linking notes with 
  unicode characters
  (for [Issue #144](https://github.com/pbek/QOwnNotes/issues/144))
- added a workaround to keep the note count in the tag name and still being 
  able to edit it without having to remove the note count from the name
- it is now possible to toggle the visibility of the note edit pane
  (for [Issue #145](https://github.com/pbek/QOwnNotes/issues/145))
- the main toolbar is now split into multiple toolbars that you can rearrange
  or hide by themselves
  (for [Issue #141](https://github.com/pbek/QOwnNotes/issues/141))
    - keep in mind that you can also drag toolbars to the left, right or 
      bottom of the window
- added more Japanese translation (thank you piyo)

## 1.4.3
- added **Japanese translation** (a big thank you to piyo!)
- opening notes in the default editor was fixed for Windows
  (for [Issue #142](https://github.com/pbek/QOwnNotes/issues/142))
- opening notes with multi-byte file names in the an custom external editor was 
  fixed for Windows and OS X
  (for [Issue #143](https://github.com/pbek/QOwnNotes/issues/143))
- SSL warnings on OS X are not logged to the console any more
- support for **linking notes with unicode characters** in their names was added
  (for [Issue #144](https://github.com/pbek/QOwnNotes/issues/144))
- fixed a problem with a jumping text cursor when the note folder is modified
  externally (for [Issue #146](https://github.com/pbek/QOwnNotes/issues/146))
- you can now **remove a tag from multiple notes** by selecting them, then 
  right-clicking on the notes and selecting *Remove tag from selected notes...*  
  (for [Issue #139](https://github.com/pbek/QOwnNotes/issues/139))
- added more Russian, German, Hungarian and Dutch translation (thank you 
  Valerian, calis2002 and Fabijenna)

## 1.4.2
- some improvements were made to the detection whether the <kbd>Ctrl</kbd> 
  key is pressed in the note text edit or not
- the lines between frames on the main screen were made a bit stronger under 
  Linux  
- the `Reload note folder` menu entry also reloads the current note now
- fixed some displaying problems on the main screen
  (for [Issue #140](https://github.com/pbek/QOwnNotes/issues/140))
- the note edit and note markdown view scrollbars are now linked together
- the markdown preview now also works when editing encrypted notes (although 
  the update interval is the note storage interval)
- fixed an issue with the note order in the note list in alphabetical sorting 
  mode when editing a note
  (for [Issue #141](https://github.com/pbek/QOwnNotes/issues/141))
- some visual improvements in the html of the markdown preview has been made
  (for [Issue #141](https://github.com/pbek/QOwnNotes/issues/141))
- inserted buttons for *bold* and *italic* in the main menu
  (for [Issue #141](https://github.com/pbek/QOwnNotes/issues/141))
- added more Portuguese and Russian translation (thank you mlimacarlos and Valerian)

## 1.4.1
- you can now **tag multiple notes** by selecting them, then right-clicking on 
  the notes and selecting *Tag selected notes with...*  
  (for [Issue #135](https://github.com/pbek/QOwnNotes/issues/135))
- added **live markdown preview**
  (for [Issue #119](https://github.com/pbek/QOwnNotes/issues/119))
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
	- if notes are move to an other folder their associated tags are lost 
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
  (for [Issue #126](https://github.com/pbek/QOwnNotes/issues/126))

## 1.3.3
- fixed a possible problem with an empty default note file extension setting
  (for [Issue #123](https://github.com/pbek/QOwnNotes/issues/123))
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
  (for [Issue #124](https://github.com/pbek/QOwnNotes/issues/124))
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
  (for [Issue #36](https://github.com/pbek/QOwnNotes/issues/36))
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
- fixed a problem after moving notes to an other folder
  (for [Issue #117](https://github.com/pbek/QOwnNotes/issues/117))
- added more Portuguese translation (thank you mlimacarlos)

## 1.2.6
- fixed building of QOwnNotes under ARM by patching the Botan library
  (for [Issue #116](https://github.com/pbek/QOwnNotes/issues/116))
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
        - please visit [QOwnNotes shortcuts](http://www.qownnotes.org/shortcuts/QOwnNotes)
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
