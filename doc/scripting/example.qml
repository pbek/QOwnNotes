import QtQml 2.0
import com.qownnotes.noteapi 1.0

/**
 * This script is a generic example of some functions that will be called by QOwnNotes
 */
QtObject {
    /**
     * This is called when the script is loaded by QOwnNotes
     */
    function init() {
        console.log("Script init");
    }
    
    /**
     * This function is called when a note gets stored to disk
     * You cannot modify stored notes, that would be a mess since 
     * you are most likely editing them by hand at the same time
     * 
     * @param {NoteApi} note - the note object of the stored note
     */
    function onNoteStored(note) {
        console.log("fileName was stored!");
        console.log(note.id);
        console.log(note.name);
        console.log(note.fileName);
        
        // tagNames() returns a list of all tag names
        var tagNameList = note.tagNames();
        console.log(tagNameList);
    }
    
    /**
     * This function is called when media file is inserted into the note
     * If this function is defined in multiple scripts, then the first script that returns a non-empty string wins
     * 
     * @param fileName string the file path of the source media file before it was copied to the media folder
     * @param mediaMarkdownText string the markdown text of the media file, e.g. ![my-image](file://media/505671508.jpg)
     * @return string the new markdown text of the media file
     */
    function insertMediaHook(fileName, mediaMarkdownText) {
        console.log("mediafile was inserted: " + fileName);
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
        console.log("text was pasted: " + text);
        return "";
    }
    
    /**
     * This starts a timer that triggers two seconds
     */
    property QtObject timer: Timer {
        interval: 2000
        repeat: true
        running: true
        
        property int count: 0
        
        onTriggered: {
            count++;
            console.log("count = " + count);
            console.log(script.currentNote().id);
            console.log(script.currentNote().name);
            console.log(script.currentNote().fileName);
            
            // tagNames() returns a list of all tag names
            var tagNameList = script.currentNote().tagNames();
            console.log(tagNameList);
        }
    }
}
