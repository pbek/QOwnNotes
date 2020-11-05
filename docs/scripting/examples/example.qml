import QtQml 2.0
import QOwnNotesTypes 1.0

/**
 * This script is a generic example of some functions that will be called by QOwnNotes
 */
QtObject {
    /**
     * This is called when the script is loaded by QOwnNotes
     */
    function init() {
        script.log("Script init");
    }
    
    /**
     * This function is called when a note gets stored to disk
     * You cannot modify stored notes, that would be a mess since 
     * you are most likely editing them by hand at the same time
     * 
     * @param {Note} note - the note object of the stored note
     */
    function onNoteStored(note) {
        script.log("fileName was stored!");
        script.log(note.id);
        script.log(note.name);
        script.log(note.fileName);
        
        // tagNames() returns a list of all tag names
        var tagNameList = note.tagNames();
        script.log(tagNameList);
    }
    
    /**
     * This function is called when a note gets stored to disk if
     * "Allow note file name to be different from headline" is enabled 
     * in the settings
     * 
     * It allows you to modify the name of the note file
     * Return an empty string if the file name of the note should 
     * not be modified
     * 
     * @param {Note} note - the note object of the stored note
     * @return {string} the file name of the note
     */
    function handleNoteTextFileNameHook(note) {
        script.log(note.id);
        script.log(note.name);
        script.log(note.fileName);
        
        // tagNames() returns a list of all tag names
        var tagNameList = note.tagNames();
        script.log(tagNameList);
        
        // don't actually change anything
        return "";
    }

    /**
     * This function is called when the note name is determined for a note
     *
     * It allows you to modify the name of the note that is viewed
     *
     * Return an empty string if the name of the note should not be modified
     *
     * @param {NoteApi} note - the note object of the stored note
     * @return {string} the name of the note
     */
    function handleNoteNameHook(note) {
        return note.name + " some text";
    }

    /**
     * This function is called when the markdown html of a note is generated
     * 
     * It allows you to modify this html
     * This is for example called before by the note preview
     * 
     * @param {Note} note - the note object
     * @param {string} html - the html that is about to being rendered
     * @param {string} forExport - true if the html is used for an export, false for the preview
     * @return {string} the modified html or an empty string if nothing should be modified
     */
    function noteToMarkdownHtmlHook(note, html, forExport) {
        script.log("noteToMarkdownHtmlHook was called");
        
        // show a custom headline and a horizontal ruler on top of the preview
        return html.replace("body>", "body><h1>Custom preview for note " + note.name + "</h1><hr />");
    }
    
    /**
     * This function is called when a media file is inserted into the current note
     * If this function is defined in multiple scripts, then the first script that returns a non-empty string wins
     * 
     * @param fileName string the file path of the source media file before it was copied to the media folder
     * @param markdownText string the markdown text of the media file, e.g. ![my-image](media/my-image-4101461585.jpg)
     * @return string the new markdown text of the media file
     */
    function insertMediaHook(fileName, markdownText) {
        script.log("mediafile was inserted: " + fileName);
        return "";
    }

    /**
     * This function is called when an attachment file is inserted into the current note
     * If this function is defined in multiple scripts, then the first script that returns a non-empty string wins
     *
     * @param fileName string the file path of the source attachment file before it was copied to the attachment folder
     * @param markdownText string the markdown text of the attachment file, e.g. [my-file.txt](attachments/my-file-4245650967.txt)
     * @return string the new markdown text of the attachment file
     */
    function insertAttachmentHook(fileName, markdownText) {
        script.log("attachment was inserted: " + fileName);
        return "";
    }

    /**
     * This function is called when html or a media file is pasted to a note with `Ctrl + Shift + V`
     * 
     * @param text text of the QMimeData object
     * @param html html of the QMimeData object
     * @returns the string that should be inserted instead of the text from the QMimeData object
     */
    function insertingFromMimeDataHook(text, html) {
        script.log("text was pasted: " + text);
        return "";
    }
    
    /**
     * This starts a timer that triggers every 30 seconds
     */
    property QtObject timer: Timer {
        interval: 30000
        repeat: true
        running: true
        
        property int count: 0
        
        onTriggered: {
            count++;
            script.log("count = " + count);
            
            var note = script.currentNote();
            if (note) {
                script.log(note.id);
                script.log(note.name);
                script.log(note.fileName);
                
                // the tags property returns a list of assigned tag objects
                var tags = note.tags;
                
                // print out all tag names
                for (var i in tags) {
                    var tag = tags[i];
                    script.log(tag.name);
                }
                
                // tagNames() returns a list of all tag names
                var tagNameList = note.tagNames();
                script.log(tagNameList);
            }
        }
    }
}
