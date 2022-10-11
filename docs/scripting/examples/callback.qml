import QtQml 2.0
import QOwnNotesTypes 1.0

/**
 * This script is a short example of how to use detached processes and callbacks
 * to relieve the UI thread.
 * Visit http://docs.qownnotes.org/ for more information about scripting
 */
Script {

    function log(txt) {
        script.log("[callback-example] " + txt);
    }

    function init() {
        log("init")
        for (var i = 0; i < 100; i++) {
            var dur = Math.floor(Math.random() * 10) + 1;
            script.startDetachedProcess("sleep", [`${dur}s`], "callback-example", i);
        }
    }

    /**
    * This function is called when a script thread is done executing.
    * Hint: thread[1]==0 helps to determine if a bulk of started processes for a certain identifier is done.
    *
    * @param {QString} callBackIdentifier - the provided id when calling startDetachedProcess()
    * @param {QString} resultSet - the result of the process
    * @param {QVariantList} cmd - the entire command array [0-executablePath, 1-parameters, 2-exitCode]
    * @param {QVariantList} thread - the thread information array [0-passed callbackParameter, 1-remaining threads for this identifier]
    */
    function onDetachedProcessCallback(callBackIdentifier, resultSet, cmd, thread) {
        if (callBackIdentifier == "callback-example") {
            log(`#${thread[1]} i[${thread[0]}] t${cmd[1]}`);
        }
    }
}
