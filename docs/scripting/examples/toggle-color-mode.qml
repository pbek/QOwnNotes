import QtQml 2.0
import QOwnNotesTypes 1.0

/**
 * This script adds custom actions to toggle between the light and dark color mode
 * and to switch to any of the configured color modes (including their editor color schema)
 */
Script {
    property var colorModes: []

    /**
     * Initializes the custom actions
     */
    function init() {
        script.registerCustomAction("toggleColorMode", "Toggle light/dark color mode", "Color mode", "weather-clear-night");

        // Add a menu entry for every color mode
        colorModes = script.getColorModes();
        colorModes.forEach(function (mode) {
            script.registerCustomAction("colorMode-" + mode.id, "Switch to color mode: " + mode.name);
        });
    }

    /**
     * This function is invoked when a custom action is triggered
     * in the menu or via button
     *
     * @param identifier string the identifier defined in registerCustomAction
     */
    function customActionInvoked(identifier) {
        if (identifier === "toggleColorMode") {
            const isDark = script.getCurrentColorModeId() === "ColorMode-dark";
            script.switchToColorMode(isDark ? "ColorMode-light" : "ColorMode-dark");
            return;
        }

        if (identifier.indexOf("colorMode-") === 0) {
            script.switchToColorMode(identifier.substring("colorMode-".length));
        }
    }
}
