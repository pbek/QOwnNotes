import QtQml 2.0
import QOwnNotesTypes 1.0

/**
 * This example script with input dialogs as custom actions
 */
Script {
    /**
     * Initializes the custom actions
     */
    function init() {
        script.registerCustomAction("inputDialogGetItem", "Combo box");
    }

    /**
     * This function is invoked when a custom action is triggered
     * in the menu or via button
     * 
     * @param identifier string the identifier defined in registerCustomAction
     */
    function customActionInvoked(identifier) {
        script.log("customActionInvoked - " + identifier);

        switch (identifier) {
            // open an input dialog with a select box
            case "inputDialogGetItem":
                var result = script.inputDialogGetItem(
                    "combo box", "Please select an item", ["Item 1", "Item 2", "Item 3"]);
                script.log(result);
                break;
        }
    }
}
