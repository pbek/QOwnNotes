import QtQml 2.0
import com.qownnotes.noteapi 1.0

/**
 * This script creates a menu item and a button to create or jump to the current date's journal entry
 */
QtObject {
    /**
     * Initializes the custom action
     */
    function init() {
        script.registerCustomAction("journalEntry", "Create or open a journal entry", "Journal", "document-new");
    }

    /**
     * This function is invoked when a custom action is triggered
     * in the menu or via button
     * 
     * @param identifier string the identifier defined in registerCustomAction
     */
    function customActionInvoked(identifier) {
        if (identifier != "journalEntry") {
            return;
        }

        // get the date headline
        var m = new Date();
        var headline = m.getFullYear() + ("0" + (m.getMonth()+1)).slice(-2) + ("0" + m.getDate()).slice(-2);

        var fileName = headline + ".md";
        var note = script.fetchNoteByFileName(fileName);
        
        // check if note was found
        if (note.id > 0) {
            // jump to the note if it was found
            script.log("found journal entry: " + headline);
            script.setCurrentNote(note);
        } else {
            // create a new journal entry note if it wasn't found
            // keep in mind that the note will not be created instantly on the disk
            script.log("creating new journal entry: " + headline);
            script.createNote(headline + "\n========\n\n");
        }
    }
}
