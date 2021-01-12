import QtQml 2.0
import QOwnNotesTypes 1.0
import com.qownnotes.noteapi 1.0

/**
 * This script shows a log entry when the window state is changed
 */
Script {
    function windowStateChangedHook(windowState) {
        script.log('Window state changed: ' + windowState);
    }
}
