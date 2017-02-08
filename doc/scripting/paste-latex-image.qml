import QtQml 2.0
import com.qownnotes.noteapi 1.0

/**
 * This script creates a menu item and a button that generates and pastes an image from
 * Latex code in the clipboard
 */
QtObject {
    /**
     * Initializes the custom action
     */
    function init() {
        // create a menu entry to paste Latex code as an image
        script.registerCustomAction("pasteLatexImage", "Paste Latex image", "Latex", "insert-image", true);
    }

    /**
     * This function is invoked when a custom action is triggered
     * in the menu or via button
     * 
     * @param identifier string the identifier defined in registerCustomAction
     */
    function customActionInvoked(identifier) {
        switch (identifier) {
            case "pasteLatexImage":
                // get the text that is currently in the clipboard
                var text = script.clipboard();

                // use the webservice to generate and download the image
                var url = "http://latex.codecogs.com/gif.latex?" + text;
                var markdown = script.downloadUrlToMedia(url);
                
                // write the image markdown to the note
                script.noteTextEditWrite(markdown);
                break;
        }
    }
}
