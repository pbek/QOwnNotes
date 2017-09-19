import QtQml 2.0
import QOwnNotesTypes 1.0

/**
 * This script handles tagging in a note for tags in the note text like:
 * @tag1 @tag2 @tag3
 */
Script {
    function noteTaggingHook(note, action, tagName) {
        var noteText = note.noteText;

        switch (action) {
            // adds a tag "tagName" to the note
            case "add":
                noteText += "\n@" + tagName;
                script.log("Adding tag '" + tagName + "' to note");
                return noteText;
                break;
            // removes a tag "tagName" from the note
            case "remove":
                noteText = noteText.replace(RegExp("\\B@" + escapeRegExp(tagName)), "");
                script.log("Removing tag '" + tagName + "' from note");
                return noteText;
                break;
            // returns a list of all tag names of the note
            case "list":
                var re = new RegExp("\\B@(\\w+)", "gi"),
                    result, tagNameList = [];

                while ((result = re.exec(noteText)) !== null) {
                    tagNameList.push(result[1]);
                }

                script.log("Found tags '" + tagNameList + "' in note");
                return tagNameList;
                break;
        }
    
        return "";
    }
    
    function escapeRegExp(str) {
        return str.replace(/[\-\[\]\/\{\}\(\)\*\+\?\.\\\^\$\|]/g, "\\$&");
    }
}
