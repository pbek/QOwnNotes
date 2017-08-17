import QtQml 2.0
import QOwnNotesTypes 1.0

/**
 * This script is an example for creating custom actions, that can be invoked 
 * via menu entries and buttons
 */
QtObject {
    /**
     * Initializes the custom actions
     */
    function init() {
        // create a menu entry "Create Zettelkasten note" with a button and a freedesktop theme icon
        script.registerCustomAction("createZettelkastenNote", "Create Zettelkasten note", "Zettelkasten note", "task-new");
    }

    /**
     * This function is invoked when a custom action is triggered
     * in the menu or via button
     * 
     * @param identifier string the identifier defined in registerCustomAction
     */
    function customActionInvoked(identifier) {
        script.log("customActionInvoked - " + identifier);

        switch (identifier) {
            // create a new note with a custom content
            case "createZettelkastenNote":
                var m = new Date();
                // create the headline
                var text = m.getFullYear() + ("0" + (m.getMonth()+1)).slice(-2) + ("0" + m.getDate()).slice(-2) + ("0" + m.getHours()).slice(-2) + ("0" + m.getMinutes()).slice(-2) + ("0" + m.getSeconds()).slice(-2) + " Title\n";

                var underline = "";

                // add the underline for the headline
                for (var i = 0; i < (text.length - 1); i++) {
                    underline += "-";
                }

                // add some text to our Zettelkasten note
                text += underline + "\n\n Note\n\n\n# keywords\n\n### Cf\n\n---\n\n### References\n\n";

                // create a new note
                script.createNote(text);
                break;
        }
    }
}
