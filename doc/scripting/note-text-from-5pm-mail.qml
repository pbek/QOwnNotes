import QtQml 2.0

/**
 * This script parses the text that was copied from a task email of the webservice 5pm 
 * and creates a note text with a headline and the link to the task
 */
QtObject {
    /**
     * This function is called when html or media file are pasted to a note with `Ctrl + Shift + V`
     * 
     * @param text text of the QMimeData object
     * @param html html of the QMimeData object
     * @returns the string that should be inserted instead of the text from the QMimeData object
     */
    function insertingFromMimeDataHook(text, html) {
        var re1 = /^Neue Aufgabe von [\w\s]+$\s\s^([\w\s\.:]+)$/igm;
        var result1 = re1.exec(text);

        var re2 = /^Direkter Zugriff hier: (http.+\d+)$/igm;
        var result2 = re2.exec(text);
        
        // return an empty string if we didn't find anything
        if ((result1 === null) || (result2 === null)) {
            return "";
        }
        
        var headline = result1[1];
        var url = result2[1];
        
        var text = headline + "\n";
        
        for (var i = 0; i < headline.length; i++) {
            text += "=";
        }
        
        text += "\n\n- <" + url + ">";
        
        return text;
    }
}
