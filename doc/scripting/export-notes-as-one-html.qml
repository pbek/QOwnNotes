import QtQml 2.0
import QOwnNotesTypes 1.0
import com.qownnotes.noteapi 1.0

/**
 * This script exports multiple notes as one HTML file
 */
Script {
    /**
     * Initializes the custom actions
     */
    function init() {
        script.registerCustomAction("exportHTML", "Export notes as html file",
            "", "", false, true, true);
    }

    /**
     * This function is invoked when a custom action is triggered
     * in the menu or via button
     * 
     * @param identifier string the identifier defined in registerCustomAction
     */
    function customActionInvoked(identifier) {
        if (identifier != "exportHTML") {
            return;
        }

        var noteIds = script.selectedNotesIds();

        if (noteIds.length == 0) {
            return;
        }

        var html = "";

        noteIds.forEach(function (noteId){
            var note = script.fetchNoteById(noteId);
            var noteHtml = note.toMarkdownHtml();

            // TODO: multiple html headers have to be removed
            html += noteHtml;
        });

        var filePath = script.getSaveFileName("Please select where to store the HTML",
            "Notes.html", "HTML (*.html)");

        if (filePath == "") {
            return;
        }

        script.writeToFile(filePath, html);
    }
}
