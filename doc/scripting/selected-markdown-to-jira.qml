import QtQml 2.0

/**
 * This script creates a menu item and a button that converts the selected Markdown
 * text to Jira code in the clipboard
 * 
 * Dependencies:
 * NodeJS
 * https://github.com/kylefarris/J2M
 * https://github.com/pbek/QOwnNotes/tree/develop/doc/scripting/md2jira/md2jira.js
 * 
 * first you have to install jira2md:
 * npm install jira2md
 * 
 * then you have to copy https://github.com/pbek/QOwnNotes/tree/develop/doc/scripting/md2jira/md2jira.js
 * to /usr/local/bin/md2jira,js
 */
QtObject {
    /**
     * Initializes the custom action
     */
    function init() {
        script.registerCustomAction("markdownToJira", "Markdown to Jira", "Jira", "edit-copy", true);
    }

    /**
     * This function is invoked when a custom action is triggered
     * in the menu or via button
     * 
     * @param identifier string the identifier defined in registerCustomAction
     */
    function customActionInvoked(identifier) {
        if (identifier != "markdownToJira") {
            return;
        }

        // get the selected text from the note text edit
        var text = script.noteTextEditSelectedText();
        
        // fix unordered lists
        text = text.replace(/^-/g, "*");
        text = text.replace(/\t-/g, "\t*");
        
        // you need pandoc and the Jira writer from https://github.com/2ion/pandoc-bbcode
        // to convert Markdown to Jira
        var params = [];
        var result = script.startSynchronousProcess("/usr/local/bin/md2jira.js", params, text);

        // put the result into the clipboard
        script.setClipboardText(result);
    }
}
