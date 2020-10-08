import QtQml 2.0
import QOwnNotesTypes 1.0

/**
 * This script allows you to configure an external editor (or an other application) that should open
 * a note when it is double clicked in the note list
 */
Script {
    property string executablePath;

    // register your settings variables so the user can set them in the script settings
    property variant settingsVariables: [
        {
            "identifier": "executablePath",
            "name": "Path of external editor",
            "description": "Please select the path to the executable:",
            "type": "file",
            "default": "kate",
        }
    ];

    /**
     * This function is called after a note was double clicked
     *
     * @param {NoteApi} note - the note object that was clicked
     */
    function noteDoubleClickedHook(note) {
        // start the editor with the note path as parameters
        script.startDetachedProcess(executablePath, [note.fullNoteFilePath]);
    }
}
