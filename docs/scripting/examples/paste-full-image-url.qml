import QtQml 2.0
import QOwnNotesTypes 1.0

/**
 * This script is an example of pasting an image link with the full file url if a file is dropped into QOwnNotes
 * It was created for this question: https://github.com/pbek/QOwnNotes/issues/623
 */
Script {
    /**
     * This function is called when html or a media file is pasted to a note with `Ctrl + Shift + V`
     * 
     * @param text text of the QMimeData object
     * @param html html of the QMimeData object
     * @returns the string that should be inserted instead of the text from the QMimeData object
     */
    function insertingFromMimeDataHook(text, html) {
        return "![My image name](" + text + ")";
    }
}
