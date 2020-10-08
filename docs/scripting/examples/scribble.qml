import QtQml 2.0
import QOwnNotesTypes 1.0

/**
 * This script adds a menu entry to the context menu of the note edit to insert a
 * scribble image to the media-folder, that will be edited by an external image editor
 * 
 * The path of the image editor and the image template can be selected in the script settings of the script
 */
Script {
    property string executablePath;
    property string imagePath;
    property bool executeInBackground;

    // the path to the script's directory will be set here
    property string scriptDirPath;

    // register your settings variables so the user can set them in the script settings
    property variant settingsVariables: [
        {
            "identifier": "executablePath",
            "name": "Path of external image editor",
            "description": "Please select the path of the executable:",
            "type": "file",
            "default": "gimp",
        },
        {
            "identifier": "imagePath",
            "name": "Path of template image",
            "description": "Please select the path of the template image:",
            "type": "file",
            "default": scriptDirPath + "/scribble.png",
        },
        {
            "identifier": "executeInBackground",
            "name": "Execute image editor in background",
            "description": "If the image editor is executed in the background you will be able to work with QOwnNotes while you are editing the scribble, but the note preview will not be refreshed automatically after you close the image editor.",
            "text": "Execute in background",
            "type": "boolean",
            "default": false,
        }
    ];

    /**
     * Initializes the custom actions
     */
    function init() {
        script.registerCustomAction("scribble", "Add scribble", "Scribble", "view-preview", true, false, false);
    }

    /**
     * This function is invoked when a custom action is triggered
     * in the menu or via button
     * 
     * @param identifier string the identifier defined in registerCustomAction
     */
    function customActionInvoked(identifier) {
        if (identifier != "scribble") {
            return;
        }
        
        // insert the scribble template image as media file
        var mediaFile = script.insertMediaFile(imagePath, true);
        var mediaFilePath = mediaFile.replace("file://media", script.currentNoteFolderPath() + "/media");

        // write the scribble image to the note
        script.noteTextEditWrite("![scribble](" + mediaFile + ")\n");

        var params = [mediaFilePath];
        
        // edit the scribble image
        if (executeInBackground) {
            script.startDetachedProcess(executablePath, params);
        } else {
            script.startSynchronousProcess(executablePath, params);
            script.regenerateNotePreview();
        }
    }
}
