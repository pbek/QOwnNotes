import QtQml 2.0

/**
 * This script uses kanboard-cli to fetch the title of Kanboard task URLs.
 *
 * Dependency:
 * https://github.com/digital-blueprint/kanboard-cli
 */
QtObject {
    property string kanboardCliPath;
    property string kanbanUrlPrefixes;

    property variant settingsVariables: [
        {
            "identifier": "kanboardCliPath",
            "name": "kanboard-cli path",
            "description": "Please select the path to your kanboard-cli executable:",
            "type": "file",
            "default": "kanboard-cli",
        },
        {
            "identifier": "kanbanUrlPrefixes",
            "name": "Kanban URL prefixes",
            "description": "Enter Kanboard base URL prefixes, one per line. Matching task URLs below those prefixes will be resolved with kanboard-cli.",
            "type": "text",
            "default": "https://kanboard.example.com",
        },
    ];

    /**
     * This function is called before QOwnNotes tries to fetch a webpage title
     * in the link dialog.
     *
     * @param url string the URL whose title should be fetched
     * @return string the title for the URL, or an empty string to let QOwnNotes fetch it
     */
    function fetchUrlTitleHook(url) {
        var taskId = taskIdFromConfiguredPrefix(url);

        if (taskId === "") {
            return "";
        }

        var result = script.startSynchronousProcess(
            kanboardCliPath, ["--json", "task", "get", taskId]);

        try {
            var task = JSON.parse(String(result));
            return task.title || "";
        } catch (error) {
            script.log("Could not fetch Kanboard task title: " + error);
        }

        return "";
    }

    function taskIdFromConfiguredPrefix(url) {
        var prefixes = kanbanUrlPrefixes.split(/\r?\n/);

        for (var i = 0; i < prefixes.length; i++) {
            var prefix = prefixes[i].trim();

            if (prefix === "" || url.indexOf(prefix) !== 0) {
                continue;
            }

            var rest = url.substring(prefix.length);
            var taskIdMatch = /^(?:\/)?task\/(\d+)(?:[/?#]|$)/.exec(rest);

            if (taskIdMatch !== null) {
                return taskIdMatch[1];
            }
        }

        return "";
    }
}
