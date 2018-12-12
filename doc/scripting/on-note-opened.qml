import QtQml 2.0
import QOwnNotesTypes 1.0

/**
 * This script is an example how to get a notification when a note was opened
 */
QtObject {
    /**
     * This function is called after a note was opened
     * 
     * @param {Note} note - the note object that was opened
     */
    function noteOpenedHook(note) {
        script.log(note.fullNoteFilePath);
    }
}
