import QtQml 2.0
import QOwnNotesTypes 1.0

Script {
    /**
     * This function is called when workspaces are switched
     *
     * @param oldUuid old uuid of workspace
     * @param newUuid new uuid of workspace
     */
    function workspaceSwitchedHook(oldUuid, newUuid) {
        script.log(oldUuid);
        script.log(newUuid);
    }
}
