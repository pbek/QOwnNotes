import QtQml 2.0
import com.qownnotes.noteapi 1.0

/*  This script shows current note statistics in a "label":
    Char(+s) = characters including spaces
    Char(−s) = characters excluding spaces
    Words = character groups divided by spaces
    Paras = paragraphs - character groups divided by line breaks 
*/

QtObject {
    
    function init() {script.registerLabel("note stats")}
    
    function noteStats(note) {
        script.setLabelText("note stats", 
            "<table align=center width=90%>
                <tr>
                    <td align=center>Char(+s) <b>" + note.noteText.length + "</b></th>
                    <td align=center>Char(−s) <b>" + note.noteText.match(/[^ ]/gi).length + "</b></th>
                    <td align=center>Words <b>" + note.noteText.split(/\s+/).length + "</b></th>
                    <td align=center>Paras <b>" + (note.noteText.match(/^.*?\S/gm) || "").length + "</b></th>
                </tr>
            </table>")
    }
    
    function noteOpenedHook(note) {noteStats(note)}
    function onNoteStored(note) {noteStats(note)}
} 
