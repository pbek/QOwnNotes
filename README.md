# QOwnNotes
 
[QOwnNotes](http://www.bekerle.com/QOwnNotes) is a **note editor**, written in Qt 5, that works together with the **notes application** of [ownCloud](http://owncloud.org/). So far it works in Linux, Mac OS and Windows.

The notes are stored as **plain text files** and are **synced with ownCloud's file sync** functionality. Of course other software, like [Dropbox](https://www.dropbox.com) can be used too.

I like the concept of having notes accessible in plain text files, like it is done in the **ownCloud notes app**, but I was not able to find a decent desktop note taking tool or a text editor, that handles them well. Out of this need **QOwnNotes** was born.

This application is still under development, so there will be added new features constantly.

Visit the project page here: [QOwnNotes project page](http://www.bekerle.com/QOwnNotes)

## Screenshot
![Screenhot](screenshot.png)

## Current features
- notes can be created, edited, viewed and removed
- list of notes is showed, sorted by modification date (descending)
- a notes folder can be chosen
- sub-string searching of notes is possible and search results are highlighted in the notes
- application can be operated with keyboard shortcuts.
- external changes of note files are watched (notes or note list are reloaded)
- markdown highlighting of notes
- notes are getting their name from the first line of the note text (just like in the ownCloud notes web-application) and the note text files are automatically renamed, if the the first line changes
- showing differences between current note and externally changed note

## Building QOwnNotes
To get the most current features you can build the application from the source code. Download the latest source here: [QOwnNotes Source on GitHub as ZIP](https://github.com/pbek/QOwnNotes/archive/develop.zip)

Then download [Qt Creator](http://www.qt.io/download-open-source), open the project file `src/QOwnNotes.pro` and clicking on *Build / Build Project QOwnNotes*.

Please feel free to contribute source code to this project, make suggestions or report troubles!

You can also visit [QOwnNotes on GitHub](https://github.com/pbek/QOwnNotes).

## Binaries
So far I created a deb package for **Linux**, and a **Mac OS X** and **Windows** build. You can get them here: [QOwnNotes project page](http://www.bekerle.com/QOwnNotes).

Although the binaries will **not always have the latest features** and you need to have **Qt 5 installed** to make them work. Grab Qt from their webpage: [Download Qt](http://www.qt.io/download-open-source)

And I still have to get some desktop icons into the Windows and OS X applications. ;)

## Disclaimer
This SOFTWARE PRODUCT is provided by THE PROVIDER "as is" and "with all faults." THE PROVIDER makes no representations or warranties of any kind concerning the safety, suitability, lack of viruses, inaccuracies, typographical errors, or other harmful components of this SOFTWARE PRODUCT. 

There are inherent dangers in the use of any software, and you are solely responsible for determining whether this SOFTWARE PRODUCT is compatible with your equipment and other software installed on your equipment. You are also solely responsible for the protection of your equipment and backup of your data, and THE PROVIDER will not be liable for any damages you may suffer in connection with using, modifying, or distributing this SOFTWARE PRODUCT.
