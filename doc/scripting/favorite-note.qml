import QtQml 2.0

/**
 * This script creates a menu item and a button that adds a "favorite" tag to the current note
 */
QtObject {
    /**
     * Initializes the custom action
     */
    function init() {
        // create the menu entry
         script.registerCustomAction("favoriteNote", "Favorite note", "fav", "bookmark-new");
    }

    /**
     * This function is invoked when a custom action is triggered
     * in the menu or via button
     * 
     * @param identifier string the identifier defined in registerCustomAction
     */
    function customActionInvoked(identifier) {
        switch (identifier) {
            // add a "favorite" tag to the current note
            case "favoriteNote":
                script.tagCurrentNote("favorite");
                break;
        }
    }
}
