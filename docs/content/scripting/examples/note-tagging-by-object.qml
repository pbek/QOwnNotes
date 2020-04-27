import QtQml 2.0
import QOwnNotesTypes 1.0

/**
 * This script handles tagging in a note for tags in the note text like:
 * @tag1 @tag2 @tag3
 */
Script {
    /**
     * Handles note tagging for a note
     *
     * This function is called when tags are added to, removed from or renamed in
     * a note or the tags of a note should be listed
     *
     * @param note
     * @param action can be "add", "remove", "rename" or "list"
     * @param tag to be added, removed or renamed
     * @param newTagName tag name to be renamed to if action = "rename"
     * @return note text string or string-list of tag ids (if action = "list")
     */
    function noteTaggingByObjectHook(note, action, tag, newTagName) {
        var tagName = tag.name;
        var noteText = note.noteText;
        var tagRegExp = RegExp("\\B@" + escapeRegExp(tagName) + "\\b");
        script.log("action: " + action);

        switch (action) {
            // adds the tag "tagName" to the note
            // the new note text has to be returned so that the note can be updated
            // returning an empty string indicates that nothing has to be changed
            case "add":
                // check if tag already exists
                if (noteText.search(tagRegExp) > 0) {
                    return "";
                }

                // add the tag at the end of the note
                noteText += "\n@" + tagName;
                script.log("Adding tag '" + tagName + "' to note");
                return noteText;
                break;

            // removes the tag "tagName" from the note
            // the new note text has to be returned so that the note can be updated
            // returning an empty string indicates that nothing has to be changed
            case "remove":
                noteText = noteText.replace(tagRegExp, "");
                script.log("Removing tag '" + tagName + "' from note");
                return noteText;
                break;

            // renames the tag "tagName" in the note to "newTagName"
            // the new note text has to be returned so that the note can be updated
            // returning an empty string indicates that nothing has to be changed
            case "rename":
                noteText = noteText.replace(tagRegExp, "@" + newTagName);
                script.log("Renaming tag '" + tagName + "' in note");
                return noteText;
                break;

            // returns a list of all tag names of the note
            case "list":
                var tag = script.getTagByNameBreadcrumbList(["level1a", "level2b", "level3b"]);
                script.log(tag.id);
                script.log(tag.name);

                var re = new RegExp("\\B@([^\\s,]+)", "gi"),
                    result, tagIdList = [];

                while ((result = re.exec(noteText)) !== null) {
                    var tagName = result[1];
                    tag.fetchByName(tagName);
                    var tagId = tag.id;

                    if (tagId == 0) {
                        // TODO: create tag and set tagId
                        continue;
                    }

                    // add the tag if it wasn't in the list
                    if (tagIdList.indexOf(tagId) == -1) {
                        tagIdList.push(tagId);
                        script.log(tagId);
                    }
                }

                if (tagIdList.length > 0) {
                    script.log("Found tags '" + tagIdList + "' in note");
                }

                return tagIdList;
                break;
        }
    
        return "";
    }
    
    /**
     * Escapes a string for regular expressions
     */
    function escapeRegExp(str) {
        return str.replace(/[\-\[\]\/\{\}\(\)\*\+\?\.\\\^\$\|]/g, "\\$&");
    }
}
