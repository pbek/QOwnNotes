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
        // create a menu entry "Open with KWrite" with a button "KWrite"
        script.registerCustomAction("openWithKWrite", "Open with KWrite", "KWrite");

        // create a menu entry "Create meeting note" with a button and a freedesktop theme icon
        script.registerCustomAction("createMeetingNote", "Create meeting note", "Meeting note", "task-new");

        // create a menu entry "No button here", but no button for it
        script.registerCustomAction("noButtonAction", "No button here");

        // create a menu entry to transform text with rot13
        script.registerCustomAction("transformTextRot13", "Transform selected text with rot13", "rot13", "text-wrap", true);

        script.registerCustomAction("noteSubFolder", "Show active note subfolder information", "Subfolder");

        script.registerCustomAction("setActiveTag", "Set active tag", "Active tag");
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

            // transform selected text with rot13
            case "transformTextRot13":
                var text = script.noteTextEditSelectedText();

                // do the rot13 transformation
                text = text.replace(/[a-zA-Z]/g, function(c){
                    return String.fromCharCode((c <= "Z" ? 90 : 122) >= (c = c.charCodeAt(0) + 13) ? c : c - 26);
                });

                script.noteTextEditWrite(text);
                break;

            case "noteSubFolder":
                var noteSubFolderQmlObj = Qt.createQmlObject("import QOwnNotesTypes 1.0; NoteSubFolder{}", mainWindow, "noteSubFolder");
                var subFolder = noteSubFolderQmlObj.activeNoteSubFolder();
                script.log(subFolder.fullPath());
                script.log(subFolder.relativePath());
                break;

            // jump to the tag "test" in the tag tree
            case "setActiveTag":
                var tag = script.getTagByNameBreadcrumbList(["test"]);
                mainWindow.jumpToTag(tag.id);
                break;
        }
    }
}
