import QtQml 2.0

/**
 * This script will execute an external program every time a note was written to the disk
 */
QtObject {
    /**
     * This function is called when a note gets stored to disk
     * You cannot modify stored notes, that would be a mess since 
     * you are most likely editing them by hand at the same time
     * 
     * @param fileName string the file path of the note
     * @param noteText string the note text
     */
    function onNoteStored(fileName, noteText) {
        // execute an external program
        // try not to alter the note file or QOwnNotes will try to update 
        // the file every time you change the note
        script.startDetachedProcess("/path/to/my/program", [fileName]);
        
        console.log("program was executed for file: " + fileName);
    }
}
