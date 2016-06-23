import QtQml 2.0
import com.qownnotes.noteapi 1.0

/**
 * This script is an example how to modify the headline of new notes
 */
QtObject {
    /**
     * This function is called before a note note is created
     * 
     * It allows you to modify the headline of the note before it is created
     * Note that you have to take care about a unique note name, otherwise
     * the new note will not be created, it will just be found in the note list
     * 
     * @return {string} the headline of the note
     */
    function handleNewNoteHeadlineHook() {
        var date = new Date();
        return "My custom note name " + date.toISOString();
    }
}
