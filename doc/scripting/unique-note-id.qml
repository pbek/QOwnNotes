import QtQml 2.0
import QOwnNotesTypes 1.0
import com.qownnotes.noteapi 1.0

/**
 * This script generates a 10 character alphanumeric id for the current note and also allows to jump to the note by it
 */
Script {
    /**
     * Initializes the custom actions
     */
    function init() {
        script.registerCustomAction("generateId", "Generate unique id for current note",
            "Unique id", "xml-element-new", false, false, true);
        script.registerCustomAction("jumpById", "Jump to note with unique id",
            "Jump to id", "edit-find", true, false, false);
    }

    /**
     * This function is invoked when a custom action is triggered
     * in the menu or via button
     * 
     * @param identifier string the identifier defined in registerCustomAction
     */
    function customActionInvoked(identifier) {
        switch (identifier) {
            case "generateId":
                var uniqueId = uniqueid();
                var noteText = script.currentNote().noteText;

                // check if we already have generated an id for the note
                if (noteText.search("id:\\[") != -1) {
                    script.log("Note already has a unique id");
                    return;
                }

                var noteText = script.currentNote().noteText;
                noteText += "\nid:[" + uniqueId + "]";
                
                script.noteTextEditSelectAll();
                script.noteTextEditWrite(noteText);
                break;
            case "jumpById":
                var uniqueId = script.inputDialogGetText("Unique id", "Please enter the id of the note");
                var noteIds = script.fetchNoteIdsByNoteTextPart("id:[" + uniqueId + "]");

                if (noteIds.length == 0) {
                    return;
                }

                var note = script.fetchNoteById(noteIds[0]);
                script.setCurrentNote(note);
                break;
        }
    }
    
    function uniqueid() {
        // always start with a letter
        var idstr=String.fromCharCode(Math.floor((Math.random()*25)+65));

        do {                
            // between numbers and characters (48 is 0 and 90 is Z (42-48 = 90)
            var ascicode=Math.floor((Math.random()*42)+48);
            
            if (ascicode<58 || ascicode>64) {
                // exclude all chars between : (58) and @ (64)
                idstr+=String.fromCharCode(ascicode);    
            }                
        } while (idstr.length<10);

        return idstr.toLowerCase();
    }
}
