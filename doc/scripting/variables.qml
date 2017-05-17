import QtQml 2.0

/**
 */
QtObject {
    function init() {
    }
    
    /**
     * Registeres the variables
     * 
     * @returns a map with the information about the variables that should be registered
     *          to be shown in the script settings
     */
    function registerVariables() {
        var variables = {
            "myString": {
                "name": "My string variable",
                "type": "string",
                "default": "My default value",
            },
            "myText": {
                "name": "My text variable",
                "type": "text",
                "default": "This can be a really long text\nwith multiple lines.",
            },
            "myInt": {
                "name": "My integer variable",
                "type": "integer",
                "default": 42,
            }
        };
        
        return variables;
    }
}
