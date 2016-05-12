import QtQml 2.0

/**
 * This script parses the text that was copied from a task email of the webservice 5pm 
 * and creates a note text with a headline, the link to the task and the description
 */
QtObject {
    /**
     * This function is called when html or a media file is pasted to a note with `Ctrl + Shift + V`
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

        var re3 = /^^Projekt: .+\nBeschreibung: ((.|\n)+)^Priorität: /igm;
        var result3 = re3.exec(text);
        
        // return an empty string if we didn't find anything
        if ((result1 === null) || (result2 === null)) {
            return "";
        }
        
        var headline = result1[1];
        var url = result2[1];
        
        // add the headline of the task
        var text = headline + "\n";
        
        // add "=" charactes so that the headline is really a headline
        for (var i = 0; i < headline.length; i++) {
            text += "=";
        }
        
        // add the url to the task
        text += "\n\n- <" + url + ">";
        
        // add the description of the task
        if (result3 !== null) {
            var description = result3[1];
            text += "\n\n" + description
        }
        
        // return the text that should be pasted
        return text;
    }
}
