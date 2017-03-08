import QtQml 2.0
import com.qownnotes.noteapi 1.0
import QtQuick 2.3
import QtQuick.Dialogs 1.2

/* This script will add custom action with a toolbar button that will show 
 * current note statistics in a pop-up window. */

MessageDialog {
    
    function init() {
        script.registerCustomAction("statNote", "Show note statistics", "Stats", "view-statistics.svg")
    }
    
    function customActionInvoked(action) {
        if (action == "statNote") {
            
            var note = script.currentNote()
            
            title = "Note statistics"
            text = "Characters (inc. spaces): " + note.noteText.length + "\n" +
                   "Characters (w/o spaces): " + note.noteText.match(/[^ ]/gi).length + "\n" + 
                   "Words: " + note.noteText.split(/\s+/).length + "\n" + 
                   "Paragraphs: " + (note.noteText.match(/^.*?\S/gm) || "").length
            visible = true
        }
    }
} 
