import QtQml 2.0

/**
 * This script creates a menu item and a button that converts the selected Markdown
 * text to Jira code in the clipboard
 * 
 * Dependencies:
 * NodeJS
 * https://github.com/kylefarris/J2M
 * 
 * first you have to install jira2md:
 * npm install jira2md
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

        // you need NodeJs and jira2md (https://github.com/kylefarris/J2M) to convert Markdown to Jira
        var params = ["-e", "console.log(require('jira2md').to_jira(require('fs').readFileSync('/dev/stdin').toString()))"];
        var result = script.startSynchronousProcess("nodejs", params, text);

        // replace some names
        result = String(result).replace(/\@Georg/ig, "[~g.franz]");

        // put the result into the clipboard
        script.setClipboardText(result);
    }
}
