import QtQml 2.0

/**
 * This script detects if a GitHub issue url is in the clipboard and downloads the page 
 * so it can parse the headline from it to paste a markdown headline and a link
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
        text = text.trim();

        // http://www.regexpal.com is your friend
        // check if the pasted text is a GitHub issue url
        var re = /^http[s]?:\/\/github\.com.+\/issues\//igm;
        if (re.test(text)) {
            script.log("found GitHub issue url in clipboard: " + text);

            // download matched GitHub url
            var output = script.downloadUrlToString(text);

            // parse the headline
            var re2 = /<span class="js-issue-title">([^>]+)<\/span>/igm
            var result2 = re2.exec(output);

            if (result2 !== null) {
                var headline = result2[1];
                script.log("found headline: " + headline);

                // generate markdown to paste
                var result = "## " + headline + "\n- <" + text + ">\n";
                return result;
            }

            script.log("no headline was found!");
        }

        script.log("no GitHub issue url was found");
        return "";
    }
}
