# QOwnNotes Changelog

## 0.42
- improvements in the settings connection troubleshooter

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
