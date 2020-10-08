import QtQml 2.0
import QOwnNotesTypes 1.0

Script {
    /**
     * Writes the selected text on the webpage that was sent to QOwnNotes from
     * the browser extension into the current note
     */
    function websocketRawDataHook(requestType, pageUrl, pageTitle, rawData,
                                  screenshotDataUrl) {
        // we only want to handle selection requests, page requests are ignored
        if (requestType != "selection") {
            return false;
        }

        // write selected text from the page to the current note
        script.noteTextEditWrite(rawData);

        return true;
    }
}
