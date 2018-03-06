import QtQml 2.0
import QOwnNotesTypes 1.0

/**
 * This script adds a menu entry to the context menu of the note list to diff selected notes in an external diff program
 * The path of the diff program can be selected in the script settings of the script
 */
Script {
    property string executablePath;
    property string imagePath;

    // the path to the script's directory will be set here
    property string scriptDirPath;

    // register your settings variables so the user can set them in the script settings
    property variant settingsVariables: [
        {
            "identifier": "executablePath",
            "name": "Path of external image manipulation application",
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
        var mediaFile = script.insertMedia(imagePath, true);
        var mediaFilePath = mediaFile.replace("file://media", script.currentNoteFolderPath() + "/media");

        // write the scribble image to the note
        script.noteTextEditWrite("![scribble](" + mediaFile + ")");
        
        // edit the scribble image
        var params = [mediaFilePath];
        script.startDetachedProcess(executablePath, params);
    }
}
