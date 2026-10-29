import QtQml 2.0
import QOwnNotesTypes 1.0

/**
 * Creates a dated journal entry from a small template
 */
Script {
    property string journalFolder
    property string headingFormat
    property bool addWeekday
    property string template

    property variant settingsVariables: [
        {
            "identifier": "journalFolder",
            "name": "Journal folder",
            "description": "Subfolder for new journal entries:",
            "type": "string",
            "default": "Journal",
        },
        {
            "identifier": "headingFormat",
            "name": "Heading format",
            "description": "Date format of the entry heading:",
            "type": "string",
            "default": "yyyy-MM-dd",
        },
        {
            "identifier": "addWeekday",
            "name": "Weekday",
            "text": "Add the weekday to the heading",
            "type": "boolean",
            "default": true,
        },
        {
            "identifier": "template",
            "name": "Template",
            "description": "What every entry starts with:",
            "type": "selection",
            "default": "goodThings",
            "items": {
                "goodThings": "Three good things",
                "plan": "A plan for the day",
                "blank": "A blank page",
            },
        },
    ];

    function init() {
        script.registerCustomAction("journalEntry", "New journal entry", "Journal",
                                    "document-new");
    }

    function customActionInvoked(identifier) {
        if (identifier !== "journalEntry") {
            return;
        }

        var date = new Date();
        var heading = Qt.formatDate(date, headingFormat);
        if (addWeekday) {
            heading += " " + Qt.formatDate(date, "dddd");
        }

        var body = {
            "goodThings": "- \n- \n- \n",
            "plan": "## Today\n\n- [ ] \n",
            "blank": "",
        }[template];

        script.jumpToNoteSubFolder(journalFolder);
        script.createNote(heading + "\n" + "=".repeat(heading.length) + "\n\n" + body);
    }
}
