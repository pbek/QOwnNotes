import QtQml 2.0

/**
 * This script will make the paths of inserted media files absolute in the markdown link (tested on Linux)
 */
QtObject {
    /**
     * This function is called when media file is inserted into the note
     * 
     * @param fileName string the file path of the source media file before it was copied to the media folder
     * @param mediaMarkdownText string the markdown text of the media file, e.g. ![my-image](file://media/505671508.jpg)
     * @return string the new markdown text of the media file
     */
    function callInsertMediaHook(fileName, mediaMarkdownText) {
        // get the path of the current note folder
        var path = script.currentNoteFolderPath();

        // Windows users might want to add an additional slash in front of the path
        //path = "/" + path;

        // make the path of inserted media files absolute
        return mediaMarkdownText.replace(new RegExp("media", "g"), path + "/media");
    }
}
