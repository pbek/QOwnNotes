import QtQml 2.0
import QOwnNotesTypes 1.0

/**
 * This script will disable the read-only mode if a note is double clicked
 */
Script {
    /**
     * This function is called after a note was double clicked
     *
     * @param {NoteApi} note - the note object that was clicked
     */
    function noteDoubleClickedHook(note) {
        // disable the read-only mode
        script.triggerMenuAction("actionAllow_note_editing", 1);
        
        // focus the note text edit
        mainWindow.focusNoteTextEdit();
    }
}
