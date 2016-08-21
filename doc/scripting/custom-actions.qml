import QtQml 2.0
import com.qownnotes.noteapi 1.0

/**
 * This script is an example for custom actions that will be called by QOwnNotes
 */
QtObject {
    /**
     * Initializes the custom actions
     */
    function init() {
        // create a menu entry "Open with KWrite" with a button "KWrite"
        script.registerCustomAction("openWithKWrite", "Open with KWrite", "KWrite");

        // create a menu entry "No button here", but no button for it
        script.registerCustomAction("noButtonAction", "No button here");
    }

    /**
     * This function is invoked when a custom action is triggered
     * in the menu or via button
     * 
     * @param identifier string the identifier defined in registerCustomAction
     */
    function customActionInvoked(identifier) {
        script.log("customActionInvoked - " + identifier);

        if (identifier == "openWithKWrite") {
            var note = script.currentNote();
            var fileName = note.fullNoteFilePath;
            script.startDetachedProcess("kwrite", [fileName]);
        }
    }
}
