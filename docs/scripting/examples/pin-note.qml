import QtQml 2.0
import QOwnNotesTypes 1.0

/* This script is tested only on GNU/Linux, it's not tested on OSX and won't work on Windows.
 * If you know how to make it properly rename note files in Windows feel free to make a pull request.
 * 
 * This script implements custom action buttons to prepend current note name with a special (or any other) 
 * unicode symbols so the note will be "pinned" on top of the list with "alphabetical sorting" on.
 * You can use different symbols, one at a time, to "pin" notes. 
 * That allows to make kind of hierarchy of pinned notes.
 * 
 * Notes can be "unpinned" by using same symbol second time.
 * 
 * Since the script actually creates a new note with the same content assigned tags won't be saved.
 * 
 * This script should be used with "Allow note file name to be different from headline", 
 * "Accept all external modification of current note", "Sort by -> Alphabetical" options on. */

QtObject {
    
    /* Put different symbols to prepend note name here, as much of them as you like.
     * You can get more unicode symbols at https://unicode-table.com/en/#dingbats
     * Some unicode symbols can be unavailable in font set used in your system. */
    property var symbol:["★", "⚑", "✔", "☎"]
        
    /// Each symbol will get own button to add it to selected note name.
    function init() {
        for (var n = 0; n < symbol.length; n++) {
        script.registerCustomAction(n, "Mark note with " + symbol[n], symbol[n])
        }
    }
    
    /// Each symbol uses single algorithm to prepend note name with it.
    function customActionInvoked(n) {
        
        if (symbol.indexOf(symbol[n]) == -1) {
            exit()
        }
        
        /// Get note data
        var note = script.currentNote()
        var fullFileName = note.fullNoteFilePath
        var fileName = note.fileName
        var filePath = fullFileName.replace(fileName,"")
        
        script.log("Note: " + note + fullFileName)
        
        /* Delete symbol if note is "pinned" by it.
         * Or prepend "unpinned" note with that symbol. 
         * Or "repin" note with the new symbol. */
        if (fileName.substring(0,2) == symbol[n] + " ") {
           var newFileName = fileName.substring(2)
           script.log("Note " + fileName + " unmarked")
        } 
        else if (symbol.indexOf(fileName.substring(0,1)) != -1 && fileName.substring(1,2) == " ") {
            var newFileName = symbol[n] + " " + fileName.substring(2)
            script.log("Note " + fileName + " marked with " + symbol[n])
        }
        else {
            var newFileName = symbol[n] + " " + fileName
            script.log("Note " + fileName + " marked with " + symbol[n])
        }
        
        /* Since QOwnNotes doesn't allow to delete notes with scripting yet, 
         * rename the note file by a system's native command with a new "prepended" name. 
         * Use different commands for Windows and POSIX systems. */
        if (script.platformIsWindows()) {
            script.log("This script currently doesn't work in Windows. Sorry for inconvenience." )
        } else {
            var moveCommand = "mv"
            var arguments = [fullFileName,filePath + newFileName]
        }
        
        script.startSynchronousProcess(moveCommand, arguments, "")
    }
}
