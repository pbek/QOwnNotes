import QtQml 2.0

/**
 */
QtObject {
    function init() {
    }
    
    /**
     * Registeres the settings variables
     * 
     * @returns a map with the information about the variables that should be registered
     *          to be shown in the script settings
     */
    function registerSettingsVariables() {
        var variables = [
            {
                "identifier": "myString",
                "name": "My apple string variable",
                "description": "Please enter a valid string.",
                "type": "string",
                "default": "My default value",
            },
            {
                "identifier": "myText",
                "name": "My apple text variable",
                "type": "text",
                "default": "This can be a really long text\nwith multiple lines.",
            },
            {
                "identifier": "myInt",
                "name": "My apple integer variable",
                "type": "integer",
                "default": 42,
            },
            {
                "identifier": "myInt2",
                "name": "My 2nd apple integer variable",
                "type": "integer",
                "default": 42,
            }
        ];
        
        return variables;
    }
}
