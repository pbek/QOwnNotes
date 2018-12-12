import QtQml 2.0
import QOwnNotesTypes 1.0

/**
 * This script is an example how to add the tag names to the file names of notes 
 * when a note file gets stored in QOwnNotes
 */
QtObject {
    /**
     * This function is called when a note gets stored to disk if
     * "Allow note file name to be different from headline" is enabled 
     * in the settings
     * 
     * It allows you to modify the name of the note file
     * Return an empty string if the file name of the note should 
     * not be modified
     * 
     * @param {Note} note - the note object of the stored note
     * @return {string} the file name of the note without file extension
     */
    function handleNoteTextFileNameHook(note) {
        // the current note name is the fallback
        var fileName = note.name;

        // get a list of note text lines
        var noteLines = note.noteText.split("\n");

        // set the first line of the note as base for the file name
        if (noteLines.length > 0) {
            // you maybe also want to exclude some disallowed characters here
            fileName = noteLines[0];
        }
        
        // get a list of all assigned tag names
        var tagNameList = note.tagNames();
        
        // add the tag names to the filename
        if (tagNameList.length > 0) {
            fileName += " [" + tagNameList.join("] [") + "]";
        }

        script.log("note file name: " + fileName);

        var noteExists = script.noteExistsByFileName(fileName + ".md", note.id);
        script.log("note " + (noteExists ? "exists" : "doesn't exist"));

        return fileName;
    }
}
