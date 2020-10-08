import QtQml 2.0
import QOwnNotesTypes 1.0

/**
 * This script creates a menu item and a button that parses the text that was copied from the Skype Electron client
 * to the clipboard and tries to extract and paste text from it
 */
QtObject {
    /**
     * Initializes the custom action
     */
    function init() {
        // create a menu entry with a button and a freedesktop theme icon
        script.registerCustomAction("pasteFromSkypeHistory", "Paste text from Skype history", "Paste Skype Text", "edit-paste");
    }

    /**
     * This function is invoked when a custom action is triggered
     * in the menu or via button
     * 
     * @param identifier string the identifier defined in registerCustomAction
     */
    function customActionInvoked(identifier) {
        if (identifier != "pasteFromSkypeHistory") {
            return;
        }
        // get the text that is currently in the clipboard
        var html = script.clipboard(true);

//        script.log(html);

        var textRegExp = /<p.*?>(.+?)<\/p>/gim;
        var match;
        
        while ((match = textRegExp.exec(html)) !== null) {
            script.noteTextEditWrite(match[1].replace(/<.+?>/gim, "") + "\n");
        }
    }
}
