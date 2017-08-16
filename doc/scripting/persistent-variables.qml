import QtQml 2.0
import QOwnNotesTypes 1.0

/**
 * This is an example on how to use persistent variables.
 */
Script {

    function init() {
        script.registerCustomAction("persistentVariablesSetTest", "Persistent variables set test", "Persistent var set test", "edit-copy");
        script.registerCustomAction("persistentVariablesGetTest", "Persistent variables get test", "Persistent var get test", "edit-paste");
    }

    /**
     * This function is invoked when a custom action is triggered
     * in the menu or via button
     * 
     * @param identifier string the identifier defined in registerCustomAction
     */
    function customActionInvoked(identifier) {
        if (identifier == "persistentVariablesSetTest") {
            var result = script.inputDialogGetItem(
                "combo box", "Please select an item", ["Item 1", "Item 2", "Item 3"]);
            
            // store persistent variable
            script.setPersistentVariable("PersistentVariablesTest/myVar", result);
        }
        
        if (identifier == "persistentVariablesGetTest") {
            // load and log persistent variable
            script.log(script.getPersistentVariable("PersistentVariablesTest/myVar", "nothing here yet"));
        }
    }
}
