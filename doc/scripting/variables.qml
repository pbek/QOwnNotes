import QtQml 2.0

/**
 * This is an example on how to register variables that can be set by the user in
 * the script settings.
 */
QtObject {
    // you have to define your registered variables so you can access them later
    property string myString;
    property string myText;
    property int myInt;
    property string myFile;
    
    // register your settings variables so the user can set them in the script settings
    // use this property if you don't need 
    property variant settingsVariables: [
        {
            "identifier": "myString",
            "name": "I am a line edit",
            "description": "Please enter a valid string:",
            "type": "string",
            "default": "My default value",
        },
        {
            "identifier": "myText",
            "name": "I am textbox",
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
        }
    ];

    
    function init() {
        script.registerCustomAction("variablesTest", "Variables test", "Var test", "edit-copy");
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

        // these variables will be set by QOwnNotes from the settings
        script.log(myString);
        script.log(myText);
        script.log(myInt);
        script.log(myFile);
        
        script.log(settingsVariables[0].identifier);
    }

    /**
     * Registeres the settings variables
     * 
     * This is the 2nd way to register settings variables.
     * Use this method if you want to use code to set your variables, like setting
     * default values dependend on the operating system.
     * 
     * registerSettingsVariables will override the settingsVariables property
     * 
     * @returns a map with the information about the variables that should be registered
     *          to be shown in the script settings
     */
    function registerSettingsVariables() {
        // don't forget to define these variables also as property
        var variables = [
            {
                "identifier": "myString",
                "name": "I am a line edit",
                "description": "Please enter a valid string:",
                "type": "string",
                "default": "My default value",
            },
            {
                "identifier": "myText",
                "name": "I am textbox",
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
            }
        ];
        
        return variables;
    }
}
