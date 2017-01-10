import QtQml 2.0

/**
 * This is an example for custom styling of html in the note preview
 */
QtObject {
    /**
     * This function is called when the markdown html of a note is generated
     * 
     * It allows you to modify this html
     * This is for example called before by the note preview
     * 
     * @param {NoteApi} note - the note object
     * @param {string} html - the html that is about to being rendered
     * @return {string} the modfied html or an empty string if nothing should be modified
     */
    function noteToMarkdownHtmlHook(note, html) {
        // see http://doc.qt.io/qt-5/richtext-html-subset.html for a list of
        // supported css styles
        var stylesheet = "h2 {margin-left: 20px;} h3 {margin-left: 40px;} h4 {margin-left: 60px;}";
        html = html.replace("</style>", stylesheet + "</style>");
        return html;
    }
}
