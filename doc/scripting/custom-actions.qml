import QtQml 2.0
import com.qownnotes.noteapi 1.0

/**
 * This script is a generic example of some functions that will be called by QOwnNotes
 */
QtObject {
    /**
     * Initializes the custom actions
     */
    function init() {
        script.registerCustomAction("custom1", "Test menu entry 1", "Button 1");
        script.registerCustomAction("custom2", "Test 2", "B2");
        script.registerCustomAction("custom3", "Test 3");
    }
    
    /**
     * This function is invoked when a custom action is triggered
     * in the menu or via button
     * 
     * @param identifier string the identifier defined in registerCustomAction
     */
    function customActionInvoked(identifier) {
        script.log("customActionInvoked - " + identifier);
        
        if (identifier == "custom1") {
            var note = script.currentNote();
//             var fileName = "\"" + note.fullNoteFilePath + "\"";
            var fileName = note.fullNoteFilePath;
            script.log("fileName - " + fileName);
            
            script.startDetachedProcess("kwrite", [fileName]);
        }
    }
}
