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
        script.registerCustomAction("questionMessageBox", "Question message box");
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
            case "questionMessageBox":
                // show a question message box with an apply and a help button
                // see: https://doc.qt.io/qt-5/qmessagebox.html#StandardButton-enum
                var result = script.questionMessageBox(
                    "The text I want to show", "Some optional title", 0x01000000|0x02000000, 0x02000000);
                script.log(result);
                break;
        }
    }
}
