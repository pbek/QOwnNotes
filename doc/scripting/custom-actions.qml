import QtQml 2.0
import com.qownnotes.noteapi 1.0

/**
 * This script is an example for creating custom actions, that can be invoked 
 * via menu entries and buttons
 */
QtObject {
    /**
     * Initializes the custom actions
     */
    function init() {
        // create a menu entry "Open with KWrite" with a button "KWrite"
        script.registerCustomAction("openWithKWrite", "Open with KWrite", "KWrite");

        // create a menu entry "Create meeting note" with a button "Meeting note"
        script.registerCustomAction("createMeetingNote", "Create meeting note", "Meeting note");

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

        switch (identifier) {
            // open the current note with KWrite
            case "openWithKWrite":
                var note = script.currentNote();
                var fileName = note.fullNoteFilePath;
                script.startDetachedProcess("kwrite", [fileName]);
                break;

            // create a new note with a custom content
            case "createMeetingNote":
                var date = new Date();
                // create the headline
                // we are removing the ":", because they should not be in the filename of the note
                var text = "Meeting note " + date.toISOString().replace(/:/g, ".") + "\n";
                var underline = "";

                // add the underline for the headline
                for (var i = 0; i < (text.length - 1); i++) {
                    underline += "=";
                }

                // add some text to our meeting note
                text += underline + "\n\n## Important points\n\n\n## Things to remember\n\n";

                // add the text that is currently in the clipboard
                text += script.clipboard();

                // create a new note
                script.createNote(text);
                break;
        }
    }
}
