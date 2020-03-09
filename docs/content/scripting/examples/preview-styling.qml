import QtQml 2.0

/**
 * This is an example for custom styling of html in the note preview
 */
QtObject {
    /**
     * This function is called before the markdown html of a note is generated
     *
     * It allows you to modify what is passed to the markdown to html converter 
     *
     * The method can for example be used in multiple scripts to render code (like LaTeX math or mermaid)
     * to its graphical representation for the preview
     *
     * The note will not be changed in this process
     *
     * @param {NoteApi} note - the note object
     * @param {string} markdown - the markdown that is about to being converted to html
     * @return {string} the modified markdown or an empty string if nothing should be modified
     */
    function preNoteToMarkdownHtmlHook(note, markdown) {
        markdown = markdown.replace("- special text", "- something else");
        return markdown;
    }

    /**
     * This function is called when the markdown html of a note is generated
     * 
     * It allows you to modify this html
     * This is for example called before by the note preview
     * 
     * @param {Note} note - the note object
     * @param {string} html - the html that is about to being rendered
     * @return {string} the modified html or an empty string if nothing should be modified
     */
    function noteToMarkdownHtmlHook(note, html) {
        // see http://doc.qt.io/qt-5/richtext-html-subset.html for a list of
        // supported css styles
        var stylesheet = "h2 {margin-left: 20px; color: red;} h3 {margin-left: 40px;} h4 {margin-left: 60px;}";
        html = html.replace("</style>", stylesheet + "</style>");
        return html;
    }
}
