import QtQml 2.0
import QOwnNotesTypes 1.0

/**
 * This is an example on how to register variables that can be set by the user in
 * the script settings.
 */
Script {
    // you have to define your registered variables so you can access them later
    property string myString;
    property bool myBoolean;
    property string myText;
    property int myInt;
    property string myFile;
    property string myDirectory;
    property string mySelection;

    // register your settings variables so the user can set them in the script settings
    //
    // unfortunately there is no QVariantHash in Qt, we only can use
    // QVariantMap (that has no arbitrary ordering) or QVariantList (which at
    // least can be ordered arbitrarily)
    property variant settingsVariables: [
        {
            "identifier": "myString",
            "name": "I am a line edit",
            "description": "Please enter a valid string:",
            "type": "string",
            "default": "My default value",
        },
        {
            "identifier": "myBoolean",
            "name": "I am a checkbox",
            "description": "Some description",
            "text": "Check this checkbox",
            "type": "boolean",
            "default": true,
        },
        {
            "identifier": "myText",
            "name": "I am a textbox",
            "description": "Please enter your text:",
            "type": "text",
            "default": "This can be a really long text\nwith multiple lines.",
        },
        {
            "identifier": "myInt",
            "name": "I am a number selector",
            "description": "Please enter a number:",
            "type": "integer",
            "default": 42,
        },
        {
            "identifier": "myFile",
            "name": "I am a file selector",
            "description": "Please select the file:",
            "type": "file",
            "default": "pandoc",
        },
        {
            "identifier": "myDirectory",
            "name": "I am a directory selector",
            "description": "Please select the directory:",
            "type": "directory",
            "default": "/home",
        },
        {
            "identifier": "mySelection",
            "name": "I am an item selector",
            "description": "Please select an item:",
            "type": "selection",
            "default": "option2",
            "items": {"option1": "Text for option 1", "option2": "Text for option 2", "option3": "Text for option 3"},
        }
    ];
    
    
    function init() {
        script.registerCustomAction("variablesTest", "Variables test", "Var test", "edit-copy");
        doSomethingWithYourVariables();
    }
    
    function doSomethingWithYourVariables() {
        // these variables will be set by QOwnNotes from the settings
        // if the user didn't set a variable your default value will be set
        script.log(myString);
        script.log(myBoolean);
        script.log(myText);
        script.log(myInt);
        script.log(myFile);
        script.log(myDirectory);
        script.log(mySelection);
    }
    
    /**
     * This function is invoked when a custom action is triggered
     * in the menu or via button
     * 
     * @param identifier string the identifier defined in registerCustomAction
     */
    function customActionInvoked(identifier) {
        if (identifier != "variablesTest") {
            return;
        }

        doSomethingWithYourVariables();
    }

    /**
     * Registers the settings variables again
     * 
     * Use this method if you want to use code to override your variables, like setting
     * default values depended on the operating system.
     */
    function registerSettingsVariables() {
        if (script.platformIsWindows()) {
            // override the myFile default value
            settingsVariables[3].default = "pandoc.exe"
        }
    }
}
